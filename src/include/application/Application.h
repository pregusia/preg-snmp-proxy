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

#ifndef INCLUDE_APPLICATION_APPLICATION_H_
#define INCLUDE_APPLICATION_APPLICATION_H_

#include "base.h"

namespace application {

	class Application {
		public:

			class StartConfig {
				public:
					std::string logFile;
					std::string configFile;
					std::string pidFile;
			};

			Application();
			~Application();

			bool running() const { return m_running; }

			int32_t daemonize(const StartConfig& config);
			int32_t run(const StartConfig& config);

		private:
			volatile bool m_running;

			std::vector<snmp::ProxyServerPtr> m_servers;
			std::vector<snmp::ClientPtr> m_clients;

			bool processConfig(const std::string& path);
			bool processProxyConfig(const config::parser::ConfigEntriesCollection& config);

	};

}

extern application::Application g_app;

#endif /* INCLUDE_APPLICATION_APPLICATION_H_ */
