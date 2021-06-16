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

#include "Socket.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <errno.h>
#include <functional>

#include <core/clock.h>
#include <io/io.h>

namespace application { namespace snmp {

	// ************************************************************************************
	Socket::Socket(const io::InetEndpoint& spec) {
		m_endpoint = spec;
		m_lastUseTime = g_clock.time();

		if (spec.empty()) {
			g_logger.fatal("[Socket::Socket] Empty endpoint given");
		}

		int32_t fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (fd >= 0) {
			sockaddr_in addrIn = spec.toSockAddr();

			int32_t res = ::bind(fd , (sockaddr*)&addrIn, sizeof(addrIn));
			if (res < 0) {
				g_logger.fatal(stdext::format("[Socket::Socket] could not bind socket to %s", spec.toString()));
				::close(fd);
			} else {
				auto self = dynamic_self_cast<Socket>();

				m_socket = io::FileDescriptor::Adapt(fd);
				m_socket->onReadReady(std::bind(&Socket::onRead, self, std::placeholders::_1));
			}
		} else {
			g_logger.fatal(stdext::format("[Socket::Socket] could not create socket,  %s", spec.toString()));
		}

	}

	// ************************************************************************************
	Socket::~Socket() {
		close();
	}

	// ************************************************************************************
	bool Socket::inactive() const {
		return m_socket.empty() || g_clock.time() - m_lastUseTime > 5 * 60;
	}

	// ************************************************************************************
	void Socket::close() {
		m_socket->close();
		m_socket.reset();
	}

	// ************************************************************************************
	bool Socket::send(const io::InetEndpoint& to, const io::DataBuffer& buf) {
		if (!m_socket) return false;
		if (to.empty()) return false;

		auto self = dynamic_self_cast<Socket>();
		auto sinAddr = to.toSockAddr();

		m_toSend.push_back(SocketSendRequest(sinAddr, buf));
		m_socket->onWriteReady(std::bind(&Socket::onWrite, self, std::placeholders::_1));

		return true;
	}

	// ************************************************************************************
	bool Socket::read(io::DataBuffer& out) {
		if (!m_socket) return false;

		if (!m_recvVec.empty()) {
			out = m_recvVec.front().buf;
			m_recvVec.pop_front();
			return true;
		} else {
			return false;
		}
	}

	// ************************************************************************************
	bool Socket::read(io::InetEndpoint& source, io::DataBuffer& out) {
		if (!m_socket) return false;

		if (!m_recvVec.empty()) {
			out = m_recvVec.front().buf;
			source = m_recvVec.front().from;
			m_recvVec.pop_front();
			return true;
		} else {
			return false;
		}
	}

	// ************************************************************************************
	void Socket::onRead(io::FileDescriptorPtr fd) {
		char buf[65536] = { 0 };
		sockaddr_in addr = { 0 };
		socklen_t addrLen = sizeof(sockaddr_in);

		if (m_socket.empty()) return;
		m_lastUseTime = g_clock.time();

		int32_t res = ::recvfrom(m_socket->fd(), buf, sizeof(buf), 0, (sockaddr*)&addr, &addrLen);
		if (res > 0) {
			auto self = dynamic_self_cast<Socket>();
			io::DataBuffer dataBuffer(buf, res);
			io::InetEndpoint endpoint(addr);
			m_recvVec.push_back(SocketReadEntry(endpoint, dataBuffer));

			//g_logger.debug(stdext::format("[Socket::onRead] socket=%s from=%s", m_endpoint.toString(), endpoint.toString()));
			//dataBuffer.debugLog(16);

			m_socket->onReadReady(std::bind(&Socket::onRead, self, std::placeholders::_1));
		}

		if (res == 0) {
			close();
		}

		if (res < 0) {
			g_logger.warning(stdext::format("Error recvfrom socket (%s) - %s", m_endpoint.toString(), strerror(errno)));
			close();
		}
	}

	// ************************************************************************************
	void Socket::onWrite(io::FileDescriptorPtr fd) {
		if (m_toSend.empty()) return;
		if (m_socket.empty()) return;

		auto& toSend = m_toSend.front();
		sockaddr_in addr = toSend.addr();
		socklen_t addrLen = sizeof(sockaddr_in);

		m_lastUseTime = g_clock.time();

		//g_logger.debug(stdext::format("[Socket::onWrite] socket=%s to=%s", m_endpoint.toString(), inet_ntoa(toSend.addr().sin_addr)));
		//toSend.buf().debugLog(16);


		int32_t res = sendto(m_socket->fd(), &toSend.buf()[0], toSend.buf().size(), 0, (sockaddr*)&addr, addrLen);

		m_toSend.pop_front();
		if (res < 0) {
			g_logger.warning(stdext::format("Error sendto socket (%s) - %s", m_endpoint.toString(), strerror(errno)));
			close();
		} else {

			if (!m_toSend.empty()) {
				auto self = dynamic_self_cast<Socket>();
				m_socket->onWriteReady(std::bind(&Socket::onWrite, self, std::placeholders::_1));
			}

		}
	}

} }
