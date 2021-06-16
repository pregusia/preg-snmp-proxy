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

#ifndef INCLUDE_APPLICATION_SNMP_CLIENT_H_
#define INCLUDE_APPLICATION_SNMP_CLIENT_H_

#include "base.h"
#include "Value.h"

#include <io/buffers.h>
#include <io/InetEndpoint.h>

namespace application { namespace snmp {


	class ClientRequestBase {
		public:
			ClientRequestBase(int32_t requestID);
			virtual ~ClientRequestBase();

			int32_t getRequestID() const { return m_requestID; }

			bool hasTimeouted() const;
			virtual bool parseResponse(const Value& message, Client* client) = 0;
			virtual void runCallbackError(const SNMPError& error) = 0;

		protected:
			int32_t m_requestID;
			ticks_t m_lastTime;
	};

	class ClientRequest_Raw: public ClientRequestBase {
		public:
			typedef std::function<void(const Value& responseMessage, const SNMPError& error)> Callback;

			ClientRequest_Raw(int32_t requestID, const Callback& callback);
			virtual ~ClientRequest_Raw();

			virtual bool parseResponse(const Value& message, Client* client);

			void runCallback(const Value& responseMessage, const SNMPError& error);
			virtual void runCallbackError(const SNMPError& error);

		private:
			Callback m_callback;
	};

	class ClientRequest_GetBulk: public ClientRequestBase {
		public:
			typedef std::function<void(const std::vector<VarBinding>& values, const SNMPError& error)> Callback;

			ClientRequest_GetBulk(int32_t requestID, const OID& baseOID, const Callback& callback);
			virtual ~ClientRequest_GetBulk();

			virtual bool parseResponse(const Value& message, Client* client);

			void runCallback(const std::vector<VarBinding>& values, const SNMPError& error);
			virtual void runCallbackError(const SNMPError& error);

		private:
			OID m_baseOID;
			Callback m_callback;
			std::vector<VarBinding> m_values;
	};





	class Client: public stdext::object {
		public:
			Client(const io::InetEndpoint& source, const io::InetEndpoint& dest, const std::string& community);
			virtual ~Client();

			const std::string& getCommunity() const { return m_community; }

			void poll();
			bool handleMessage(const io::InetEndpoint& source, const Value& message);
			void send(const io::DataBuffer& buf);

			bool doRequest(Value pdu, const ClientRequest_Raw::Callback& func);
			bool doGetBulk(const OID& start, const ClientRequest_GetBulk::Callback& func);

		private:
			SocketPtr m_socket;
			std::string m_community;
			io::InetEndpoint m_destEndpoint;

			std::map<int32_t, ClientRequestBase*> m_requests;
	};





} }

#endif /* INCLUDE_APPLICATION_SNMP_CLIENT_H_ */
