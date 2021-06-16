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

#include "InetEndpoint.h"
#include "io.h"

#include <arpa/inet.h>
#include <netdb.h>


static std::map<std::string,std::string> g_dnsCache;

namespace io {

	// ************************************************************************************
	InetEndpoint::InetEndpoint() {
		m_port = 0;
	}

	// ************************************************************************************
	InetEndpoint::InetEndpoint(const std::string& host, int32_t port) {
		m_host = host;
		m_port = port;
		if (m_port < 0) m_port = 0;
		if (m_port > 65535) m_port = 0;
	}

	// ************************************************************************************
	InetEndpoint::InetEndpoint(const std::string& spec) {
		if (!spec.empty()) {
			auto arr = stdext::split<char>(spec,":");
			if (arr.size() == 2) {
				m_host = arr[0];
				m_port = stdext::unsafeCast<int32_t>(arr[1]);
			}
		}
	}

	// ************************************************************************************
	InetEndpoint::InetEndpoint(const sockaddr_in& addr) {
		m_port = ntohs(addr.sin_port);
		m_host = inet_ntoa(addr.sin_addr);
	}

	// ************************************************************************************
	std::string InetEndpoint::toString() const {
		if (empty()) {
			return "[empty]";
		} else {
			return stdext::format("%s:%d", m_host, m_port);
		}
	}

	// ************************************************************************************
	sockaddr_in InetEndpoint::toSockAddr() const {
		sockaddr_in res = { 0 };
		res.sin_family = AF_INET;
		res.sin_port = htons(m_port);

		if (inet_addr(m_host.c_str()) != INADDR_NONE) {
			res.sin_addr.s_addr = inet_addr(m_host.c_str());
			return res;
		}

		if (g_dnsCache.find(m_host) != g_dnsCache.end()) {
			res.sin_addr.s_addr = inet_addr(g_dnsCache[m_host].c_str());
			return res;
		}

		hostent* he = gethostbyname(m_host.c_str());
		if (he != NULL) {
			in_addr** addr_list = (in_addr**)he->h_addr_list;
			if (addr_list[0] != NULL) {
				res.sin_addr = *addr_list[0];
				g_dnsCache[m_host] = inet_ntoa(res.sin_addr);
				return res;
			}
		}

		res.sin_addr.s_addr = INADDR_NONE;
		return res;
	}

}
