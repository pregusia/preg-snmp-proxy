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

#include <base.h>
#include <getopt.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>
#include <application/Application.h>
#include <io/io.h>

#include <application/snmp/streams.h>
#include <io/buffers.h>

// ************************************************************************************
void printUsage(const char* prgName) {
	printf("Usage: %s -c CONFIG_FILE [-l LOG_FILE] [-p PID_FILE] [-d]\n", prgName);
	exit(0);
}

// ************************************************************************************
int main(int argc, char** argv) {
	application::Application::StartConfig config;
	bool daemon = false;

    while (1) {
    	int c = getopt(argc, argv, "c:l:L:p:d");
    	if (c == -1) break;
		switch (c) {
			case 'c':
				config.configFile = optarg;
				break;

			case 'l':
				config.logFile = optarg;
				break;

			case 'p':
				config.pidFile = optarg;
				break;

			case 'd':
				daemon = true;
				break;

			case '?':
			default:
				printUsage(argv[0]);
				break;
		}
    }

    if (config.configFile.empty()) {
    	printf("Invalid config file\n");
    	printUsage(argv[0]);
    	return 1;
    }
    if (!g_io.isFile(config.configFile)) {
    	printf("Given configuration file not exists\n");
    	printUsage(argv[0]);
    	return 1;
    }


    if (daemon) {
    	return g_app.daemonize(config);
    } else {
    	return g_app.run(config);
    }

	return 0;
}
