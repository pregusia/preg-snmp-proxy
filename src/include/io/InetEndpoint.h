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
 *
 */

#ifndef INCLUDE_IO_INETENDPOINT_H_
#define INCLUDE_IO_INETENDPOINT_H_

#include <base.h>
#include <arpa/inet.h>

namespace io {

	class InetEndpoint {
		public:
			InetEndpoint();
			InetEndpoint(const std::string& host, int32_t port);
			InetEndpoint(const std::string& spec);
			InetEndpoint(const sockaddr_in& addr);

			const std::string& host() const { return m_host; }
			int32_t port() const { return m_port; }

			bool empty() const { return m_host.empty() || m_port == 0; }

			std::string toString() const;
			sockaddr_in toSockAddr() const;

			bool operator==(const InetEndpoint& o) const { return o.m_host == m_host && o.m_port == m_port; }
			bool operator!=(const InetEndpoint& o) const { return !(o.m_host == m_host && o.m_port == m_port); }

		private:
			std::string m_host;
			int32_t m_port;
	};

}

#endif /* INCLUDE_IO_INETENDPOINT_H_ */
