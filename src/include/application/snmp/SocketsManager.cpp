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

#include "SocketsManager.h"
#include "Socket.h"
#include "streams.h"

#include "Client.h"
#include "ProxyServer.h"

#include <io/buffers.h>

application::snmp::SocketsManager g_snmpSocketsManager;

namespace application { namespace snmp {

	// ************************************************************************************
	SocketsManager::SocketsManager() {

	}

	// ************************************************************************************
	SocketsManager::~SocketsManager() {

	}

	// ************************************************************************************
	SocketPtr SocketsManager::ensureClientSocket(const io::InetEndpoint& endpoint, ClientPtr client) {
		for(auto& e: m_sockets) {
			if (e.endpoint == endpoint) {
				e.clients.push_back(client);
				return e.socket;
			}
		}

		Entry e;
		e.endpoint = endpoint;
		e.socket.reset(new Socket(endpoint));
		e.clients.push_back(client);
		m_sockets.push_back(e);

		return e.socket;
	}

	// ************************************************************************************
	SocketPtr SocketsManager::ensureServerSocket(const io::InetEndpoint& endpoint, ProxyServerPtr server) {
		for(auto& e: m_sockets) {
			if (e.endpoint == endpoint) {
				e.servers.push_back(server);
				return e.socket;
			}
		}

		Entry e;
		e.endpoint = endpoint;
		e.socket.reset(new Socket(endpoint));
		e.servers.push_back(server);
		m_sockets.push_back(e);

		return e.socket;
	}

	// ************************************************************************************
	void SocketsManager::poll() {
		io::DataBuffer buf;

		for(auto& e: m_sockets) {
			buf.clear();
			io::InetEndpoint from;

			while(e.socket->read(from, buf)) {
				e.handleMessage(from, buf);
			}
		}
	}

	// ************************************************************************************
	bool SocketsManager::Entry::handleMessage(const io::InetEndpoint& source, const io::DataBuffer& buf) {
		io::DataBufferInputStream is(buf);

		bool errorFlag = false;
		auto message = SNMPInputStreamAdapter::read(is, errorFlag);

		if (errorFlag) {
			std::stringstream ss;
			ss << "Readed data" << std::endl;
			buf.dumpHex(ss, 16);
			g_logger.debug(ss.str());
			return true;
		}

		if (message.type() != ValueType::SEQUENCE) {
			// invalid type, ignoring
			return false;
		}
		if (message.size() != 3) {
			// invalid size, ignoring
			return false;
		}

		/*
		g_logger.debug(stdext::format("[SocketsManager::Entry::handleMessage] source=%s requestID=%ld pdu=%02X",
			source.toString(),
			message[0].valueInt(),
			static_cast<int32_t>(message[2].type())
		));
		message.printDebug(2);
		*/


		bool handled = false;

		// no to teraz paczymy, czy moze to jest od servera czy klienta
		if (!handled) {
			for(auto& e: clients) {
				if (e->handleMessage(source, message)) {
					handled = true;
					break;
				}
			}
		}

		if (!handled) {
			for(auto& e: servers) {
				if (e->handleMessage(source, message)) {
					handled = true;
					break;
				}
			}
		}

		return handled;
	}



} }
