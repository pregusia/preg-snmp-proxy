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

#ifndef INCLUDE_APPLICATION_SNMP_SOCKET_H_
#define INCLUDE_APPLICATION_SNMP_SOCKET_H_

#include "base.h"
#include <arpa/inet.h>
#include <io/buffers.h>
#include <io/FileDescriptor.h>
#include <io/InetEndpoint.h>

namespace application { namespace snmp {

	class SocketSendRequest {
		public:
			SocketSendRequest(const sockaddr_in& addr, const io::DataBuffer& buf) : m_buf(buf), m_addr(addr) { }

			const io::DataBuffer& buf() const { return m_buf; }
			const sockaddr_in& addr() const { return m_addr; }

		private:
			io::DataBuffer m_buf;
			sockaddr_in m_addr;
	};


	class Socket: public stdext::object {
		public:
			Socket(const io::InetEndpoint& spec);
			virtual ~Socket();

			bool send(const io::InetEndpoint& to, const io::DataBuffer& buf);
			bool read(io::DataBuffer& out);
			bool read(io::InetEndpoint& source, io::DataBuffer& out);

			bool inactive() const;
			void close();

			void onRead(io::FileDescriptorPtr fd);
			void onWrite(io::FileDescriptorPtr fd);



		private:
			io::FileDescriptorPtr m_socket;
			io::InetEndpoint m_endpoint;
			ticks_t m_lastUseTime;

			class SocketReadEntry {
				public:
					io::InetEndpoint from;
					io::DataBuffer buf;

					SocketReadEntry() { }
					SocketReadEntry(const io::InetEndpoint& from, const io::DataBuffer& buf) : from(from), buf(buf) { }
			};

			std::list<SocketSendRequest> m_toSend;
			std::list<SocketReadEntry> m_recvVec;
	};


} }


#endif /* INCLUDE_APPLICATION_SNMP_SOCKET_H_ */
