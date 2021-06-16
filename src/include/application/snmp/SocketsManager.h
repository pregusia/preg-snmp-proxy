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

#ifndef INCLUDE_APPLICATION_SNMP_SOCKETSMANAGER_H_
#define INCLUDE_APPLICATION_SNMP_SOCKETSMANAGER_H_

#include "base.h"

#include <io/buffers.h>
#include <io/InetEndpoint.h>

namespace application { namespace snmp {

	class SocketsManager {
		public:
			SocketsManager();
			~SocketsManager();

			SocketPtr ensureClientSocket(const io::InetEndpoint& endpoint, ClientPtr client);
			SocketPtr ensureServerSocket(const io::InetEndpoint& endpoint, ProxyServerPtr server);

			void poll();

		private:

			class Entry {
			public:
				SocketPtr socket;
				io::InetEndpoint endpoint;
				std::vector<ClientPtr> clients;
				std::vector<ProxyServerPtr> servers;

				bool handleMessage(const io::InetEndpoint& source, const io::DataBuffer& buf);
			};

			std::vector<Entry> m_sockets;
	};

} }

#endif /* INCLUDE_APPLICATION_SNMP_SOCKETSMANAGER_H_ */
