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

#include "Client.h"
#include "Socket.h"
#include "streams.h"
#include "SocketsManager.h"

#include <core/clock.h>

namespace application { namespace snmp {

// ##############################################################################################################################
// ClientRequestBase
// ##############################################################################################################################

	// ************************************************************************************
	ClientRequestBase::ClientRequestBase(int32_t requestID) {
		m_requestID = requestID;
		m_lastTime = g_clock.time();
	}

	// ************************************************************************************
	ClientRequestBase::~ClientRequestBase() {

	}

	// ************************************************************************************
	bool ClientRequestBase::hasTimeouted() const {
		return g_clock.time() - m_lastTime > 10;
	}



// ##############################################################################################################################
// ClientRequest_Raw
// ##############################################################################################################################

	// ************************************************************************************
	ClientRequest_Raw::ClientRequest_Raw(int32_t requestID, const Callback& callback)
		: ClientRequestBase(requestID), m_callback(callback)
	{

	}

	// ************************************************************************************
	ClientRequest_Raw::~ClientRequest_Raw() {

	}

	// ************************************************************************************
	bool ClientRequest_Raw::parseResponse(const Value& message, Client* client) {
		m_lastTime = g_clock.time();

		auto& pdu = message[2];
		auto error = SNMPError::fromPDU(pdu);

		runCallback(message, error);
		return true;
	}

	// ************************************************************************************
	void ClientRequest_Raw::runCallback(const Value& responseMessage, const SNMPError& error) {
		if (m_callback) {
			m_callback(responseMessage, error);
		}
	}

	// ************************************************************************************
	void ClientRequest_Raw::runCallbackError(const SNMPError& error) {
		if (m_callback) {
			auto val = Value::createNull();
			m_callback(val, error);
		}
	}


// ##############################################################################################################################
// ClientRequest_GetBulk
// ##############################################################################################################################

	// ************************************************************************************
	ClientRequest_GetBulk::ClientRequest_GetBulk(int32_t requestID, const OID& baseOID, const Callback& callback)
		: ClientRequestBase(requestID), m_baseOID(baseOID), m_callback(callback)
	{

	}

	// ************************************************************************************
	ClientRequest_GetBulk::~ClientRequest_GetBulk() {

	}

	// ************************************************************************************
	bool ClientRequest_GetBulk::parseResponse(const Value& message, Client* client) {
		m_lastTime = g_clock.time();

		auto varBindings = VarBindingRef::fromValue(message[2][3]);
		OID oid;

		for(auto& b: varBindings) {
			oid = b.name;

			if (oid.startsWith(m_baseOID)) {
				m_values.push_back(b.toVarBinding());
			} else {
				// koniec przetwarzania
				runCallback(m_values, SNMPError());
				return true;
			}
		}

		if (!oid.empty()) {
			// trzeba wyslac pytanie o kolejne dane
			m_requestID = nextRequestID();

			io::DataBuffer buf;
			io::DataBufferOutputStream os(buf, true);
			SNMPOutputStreamAdapter snmpOS(os);

			snmpOS.writeSeq(ValueType::SEQUENCE,[&](){
				snmpOS.writeInt8(1);
				snmpOS.writeString(client->getCommunity());
				snmpOS.writeSeq(ValueType::PDU_GET_BULK,[&](){
					snmpOS.writeInt32(m_requestID);
					snmpOS.writeInt8(0); // non-repeaters
					snmpOS.writeInt8(10); // max repetitions
					snmpOS.writeSeq(ValueType::SEQUENCE,[&](){
						snmpOS.writeSeq(ValueType::SEQUENCE,[&](){
							snmpOS.writeOID(oid);
							snmpOS.writeNull();
						});
					});
				});
			});

			client->send(buf);
			return false;
		} else {
			// puste, znaczy ze nic nie dostalismy
			runCallback(m_values, SNMPError());
			return true;
		}
	}

	// ************************************************************************************
	void ClientRequest_GetBulk::runCallback(const std::vector<VarBinding>& values, const SNMPError& error) {
		if (m_callback) {
			m_callback(values, error);
		}
	}

	// ************************************************************************************
	void ClientRequest_GetBulk::runCallbackError(const SNMPError& error) {
		if (m_callback) {
			std::vector<VarBinding> val;
			m_callback(val, error);
		}
	}






// ##############################################################################################################################
// Client
// ##############################################################################################################################

	// ************************************************************************************
	Client::Client(const io::InetEndpoint& source, const io::InetEndpoint& dest, const std::string& community) {
		m_community = community;
		m_destEndpoint = dest;
		m_socket = g_snmpSocketsManager.ensureClientSocket(source, dynamic_self_cast<Client>());
	}

	// ************************************************************************************
	Client::~Client() {
		// TODO: wywalic oczekujace requesty z bledami
	}

	// ************************************************************************************
	void Client::poll() {
		// timeouts
		if (true) {
			for(auto it=m_requests.begin();it != m_requests.end();++it) {
				if (it->second->hasTimeouted()) {
					// za dlugo czeka, usuwmy
					it->second->runCallbackError(SNMPError(SNMPError::APP_TIMEOUT, 0));
					delete it->second;
					it = m_requests.erase(it);
				}
			}
		}
	}

	// ************************************************************************************
	bool Client::handleMessage(const io::InetEndpoint& source, const Value& message) {
		if (message.type() == ValueType::SEQUENCE && message.size() == 3) {
			auto& pdu = message[2];

			if (pdu.type() == ValueType::PDU_RESPONSE && pdu.size() == 4) {
				int32_t requestID = pdu[0].valueInt();

				auto requestIt = m_requests.find(requestID);
				if (requestIt != m_requests.end()) {
					ClientRequestBase* req = requestIt->second;

					int32_t oldRequestID = req->getRequestID();
					if (req->parseResponse(message, this)) {
						// trzeba usunac
						delete req;
						m_requests.erase(requestIt);
					} else {
						int32_t newRequestID = req->getRequestID();

						if (oldRequestID != newRequestID) {
							// zmienilo sie, zamieniamy w mapie
							m_requests.erase(requestIt);
							m_requests[newRequestID] = req;
						}
					}

				} else {
					g_logger.warning(stdext::format("[Client::handleResponse] Could not find request #%d", requestID));
				}

				return true;
			}
		}

		return false;
	}

	// ************************************************************************************
	void Client::send(const io::DataBuffer& buf) {
		m_socket->send(m_destEndpoint, buf);
	}

	// ************************************************************************************
	bool Client::doRequest(Value pdu, const ClientRequest_Raw::Callback& func) {
		if (!pdu.isPDU()) return false;

		int32_t requestID = nextRequestID();
		m_requests[requestID] = new ClientRequest_Raw(requestID, func);

		pdu[0] = Value::createInt(requestID);

		if (true) {
			io::DataBuffer buf;
			io::DataBufferOutputStream os(buf, true);
			SNMPOutputStreamAdapter snmpOS(os);

			snmpOS.writeSeq(ValueType::SEQUENCE,[&](){
				snmpOS.writeInt8(1);
				snmpOS.writeString(m_community);
				snmpOS.writeValue(pdu);
			});

			send(buf);
		}

		return true;
	}

	// ************************************************************************************
	bool Client::doGetBulk(const OID& baseOID, const ClientRequest_GetBulk::Callback& func) {
		int32_t requestID = nextRequestID();
		m_requests[requestID] = new ClientRequest_GetBulk(requestID, baseOID, func);

		if (true) {
			io::DataBuffer buf;
			io::DataBufferOutputStream os(buf, true);
			SNMPOutputStreamAdapter snmpOS(os);

			snmpOS.writeSeq(ValueType::SEQUENCE,[&](){

				snmpOS.writeInt8(1);
				snmpOS.writeString(m_community);
				snmpOS.writeSeq(ValueType::PDU_GET_BULK,[&](){
					snmpOS.writeInt32(requestID);
					snmpOS.writeInt8(0); // non-repeaters
					snmpOS.writeInt8(10); // max repetitions
					snmpOS.writeSeq(ValueType::SEQUENCE,[&](){
						snmpOS.writeSeq(ValueType::SEQUENCE,[&](){
							snmpOS.writeOID(baseOID);
							snmpOS.writeNull();
						});
					});
				});
			});

			send(buf);
		}

		return true;
	}




} }
