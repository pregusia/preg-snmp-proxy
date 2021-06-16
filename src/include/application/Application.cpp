/*
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @author: pregusia
 * @part-of: preg-snmp-proxy
 *
 */

#include "Application.h"

#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <core/eventdispatcher.h>
#include <io/io.h>
#include <io/signals.h>

#include <application/config/parser/ConfigParserException.h>
#include <application/config/parser/ConfigEntry.h>
#include <application/config/parser/Token.h>

#include <application/snmp/SocketsManager.h>
#include <application/snmp/Client.h>
#include <application/snmp/ProxyServer.h>

#include <boost/filesystem.hpp>


application::Application g_app;

namespace application {

	// ************************************************************************************
	Application::Application() {
		m_running = false;
	}

	// ************************************************************************************
	Application::~Application() {

	}

	// ************************************************************************************
	int32_t Application::daemonize(const StartConfig& config) {
		pid_t res = fork();

		if (res < 0) {
			perror("fork");
			return 1;
		}

		if (res > 0) {
			return 0;
		}

		// file mask
		umask(0);

		// new session
		if (setsid() < 0) {
			perror("setsid");
			return 1;
		}

		if (!config.logFile.empty()) {
	        int fd = open(config.logFile.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0600);
	        if (fd != 1) {
	        	dup2(fd, STDOUT_FILENO);
	        	dup2(fd, STDERR_FILENO);
	        	close(fd);
	        }
		}

		// redirecting stdin
		if (1) {
			int fd = open("/dev/null", O_RDONLY);
			if (fd != 1) {
				dup2(fd, STDIN_FILENO);
				close(fd);
			}
		}

		// writing pid file
		if (!config.pidFile.empty()) {
	        FILE* f = fopen(config.pidFile.c_str(),"w");
	        fprintf(f,"%d", getpid());
	        fclose(f);
		}

		int32_t exitCode = run(config);

		if (!config.pidFile.empty()) {
			unlink(config.pidFile.c_str());
		}

		return exitCode;
	}

	// ************************************************************************************
	int32_t Application::run(const StartConfig& initialConfig) {
		g_unixSignals.start();
		g_unixSignals.registerSignalHandler(SIGTERM, [&](){ m_running = false; });
		g_unixSignals.registerSignalHandler(SIGABRT, [&](){ m_running = false; });
		g_unixSignals.registerSignalHandler(SIGINT, [&](){ m_running = false; });

	    try {
			// scripting
			if (!processConfig(initialConfig.configFile)) {
				g_logger.fatal("Cannot process config file");
				return 1;
			}

			if (m_servers.empty()) {
				g_logger.error("Empty ProxyServers. Exiting.");
				return 1;
			}
			if (m_clients.empty()) {
				g_logger.error("Empty Clients. Exiting.");
				return 1;
			}

			m_running = true;

			g_logger.info("Application started");

			while(m_running) {
				g_clock.update();
				g_unixSignals.poll();
				g_io.select(500);
				g_snmpSocketsManager.poll();

				for(auto& s: m_servers) {
					s->poll();
				}
				for(auto& c: m_clients) {
					c->poll();
				}

				g_dispatcher.poll(false);
			}

			g_dispatcher.shutdown();
			g_logger.info("Application stopped");

	    } catch(const core::Exception& e) {
			g_logger.warning(stdext::format("EXCEPTION:\n%s", e.what()));
		} catch(const std::exception& e) {
			g_logger.warning(stdext::format("EXCEPTION:\n%s", e.what()));
		}

		return 0;
	}


	// ************************************************************************************
	bool Application::processConfig(const std::string& path) {
		if (!g_io.isFile(path)) {
			g_logger.warning(stdext::format("Could not open config file %s", path));
			return false;
		}

		auto content = g_io.readFileContents(path);

		try {
			config::parser::SourceStream source(content);
			auto tokens = config::parser::TokensStream::Tokenize(source);
			auto config = config::parser::ConfigEntriesCollection::Parse(tokens);

			for(auto& e: config) {

				if (e->name() == "proxy" && e->hasValueBlock(0)) {
					if (processProxyConfig(e->valueBlock(0))) {
						// ok
					} else {
						return false;
					}
				}

			}

		}
		catch(const config::parser::ConfigParserException& e) {
			g_logger.error(stdext::format("ConfigParserException: %s at line %d\n", e.what(), e.line()));
			return false;
		}
		catch(const std::exception& e) {
			g_logger.error(stdext::format("Exception: %s\n", e.what()));
			return false;
		}

		return true;
	}

	// ************************************************************************************
	bool Application::processProxyConfig(const config::parser::ConfigEntriesCollection& config) {
		snmp::ProxyServerPtr server(new snmp::ProxyServer());
		if (server->loadFromConfig(config, m_clients)) {
			m_servers.push_back(server);
			return true;
		} else {
			return false;
		}
	}

}
