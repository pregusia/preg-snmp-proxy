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

#include "ProxyServer.h"
#include "Client.h"
#include "Socket.h"
#include "SocketsManager.h"
#include "streams.h"

#include <cstdio>

#include <core/clock.h>
#include <application/config/parser/ConfigEntry.h>

namespace application { namespace snmp {

// ##############################################################################################################################
// ProxyServerCacheEntry
// ##############################################################################################################################

	// ************************************************************************************
	ProxyServerCacheEntry::ProxyServerCacheEntry() {
		m_updateInterval = 0;
		m_nextUpdateTime = 0;
		m_updating = false;
		m_initialized = false;
	}

	// ************************************************************************************
	ProxyServerCacheEntry::~ProxyServerCacheEntry() {

	}

	// ************************************************************************************
	bool ProxyServerCacheEntry::loadFromConfig(const std::string& oid, const config::parser::ConfigEntriesCollection& entries) {
		if (oid.empty()) {
			g_logger.warning("[ProxyServerOIDSpec::loadFromConfig] Empty oid");
			return false;
		}

		if (oid[0] == '.' && stdext::endsWith<char>(oid,".*")) {
			m_baseOID = OID(oid);
		} else {
			g_logger.warning(stdext::format("[ProxyServerCacheEntry::loadFromConfig] Invalid oid format '%s'", oid));
			return false;
		}

		for(auto& e: entries) {
			if (e->name() == "update-interval" && e->hasValueInt(0)) {
				m_updateInterval = e->valueInt(0);
				continue;
			}

			g_logger.warning(stdext::format("[ProxyServerCacheEntry::loadFromConfig] Unknown config entry '%s'", e->name()));
		}

		if (m_updateInterval < 30) {
			g_logger.warning("[ProxyServerOIDSpec::loadFromConfig] Invalid update interval");
			return false;
		}

		return true;
	}

	// ************************************************************************************
	void ProxyServerCacheEntry::doGetAll(const Callback& func) {
		if (m_updating || !m_initialized) {
			auto self = dynamic_self_cast<ProxyServerCacheEntry>();
			m_waitingCalls.push_back([=](){
				self->doGetAll(func);
			});
			doUpdate();
			return;
		}

		func(m_values);
	}

	// ************************************************************************************
	void ProxyServerCacheEntry::doGetOne(const OID& oid, const Callback& func) {
		if (m_updating || !m_initialized) {
			auto self = dynamic_self_cast<ProxyServerCacheEntry>();
			m_waitingCalls.push_back([=](){
				self->doGetOne(oid, func);
			});
			doUpdate();
			return;
		}

		std::vector<VarBinding> res;
		for(auto& e: m_values) {
			if (e.name == oid) {
				res.push_back(e);
			}
		}

		func(res);
	}

	// ************************************************************************************
	void ProxyServerCacheEntry::doGetFrom(const OID& start, int32_t num, const Callback& func) {
		if (m_updating || !m_initialized) {
			auto self = dynamic_self_cast<ProxyServerCacheEntry>();
			m_waitingCalls.push_back([=](){
				self->doGetFrom(start, num, func);
			});
			doUpdate();
			return;
		}

		std::vector<VarBinding> res;
		for(auto& e: m_values) {
			if (e.name < start) continue;
			res.push_back(e);
			if (static_cast<int32_t>(res.size()) >= num) break;
		}

		func(res);
	}

	// ************************************************************************************
	void ProxyServerCacheEntry::doGetNext(const OID& oid, const Callback& func) {
		if (m_updating || !m_initialized) {
			auto self = dynamic_self_cast<ProxyServerCacheEntry>();
			m_waitingCalls.push_back([=](){
				self->doGetNext(oid, func);
			});
			doUpdate();
			return;
		}

		std::vector<VarBinding> res;
		for(auto& e: m_values) {
			if (e.name <= oid) continue;
			res.push_back(e);
			break;
		}

		func(res);
	}

	// ************************************************************************************
	void ProxyServerCacheEntry::setClient(const ClientPtr& client, const io::InetEndpoint& dest) {
		m_client = client;
		m_destEndpoint = dest;
	}

	// ************************************************************************************
	bool ProxyServerCacheEntry::matches(const OID& oid) const {
		return oid.startsWith(m_baseOID);
	}

	// ************************************************************************************
	void ProxyServerCacheEntry::poll() {
		if (m_nextUpdateTime < g_clock.time()) {
			m_nextUpdateTime = g_clock.time() + m_updateInterval;
			doUpdate();
		}
	}

	// ************************************************************************************
	void ProxyServerCacheEntry::doUpdate() {
		if (m_updating) return;

		g_logger.info(stdext::format("[ProxyServerCacheEntry::processUpdateResult] [Cache %s] Starting update...",
			m_baseOID.toString()
		));


		m_updating = true;
		auto self = dynamic_self_cast<ProxyServerCacheEntry>();
		m_client->doGetBulk(m_baseOID, std::bind(&ProxyServerCacheEntry::processUpdateResult, self, std::placeholders::_1, std::placeholders::_2));
	}

	// ************************************************************************************
	void ProxyServerCacheEntry::processUpdateResult(const std::vector<VarBinding>& values, const SNMPError& error) {
		m_updating = false;

		if (error.hasError()) {
			g_logger.warning(stdext::format("[ProxyServerCacheEntry::processUpdateResult] [Cache %s] Got error %s. Values not changed.",
				m_baseOID.toString(),
				error.toString()
			));
		} else {
			m_values = values;
			std::sort(m_values.begin(), m_values.end());
			m_initialized = true;

			/*
			for(auto& e: m_values) {
				g_logger.debug(stdext::format("[ProxyServerCacheEntry::processUpdateResult] oid=%s val=%s",
					e.name.toString(),
					e.value.toString()
				));
			}
			*/

			g_logger.info(stdext::format("[ProxyServerCacheEntry::processUpdateResult] [Cache %s] Updated. Got %d values.",
				m_baseOID.toString(),
				m_values.size()
			));
		}

		auto waitingCallsCopy = m_waitingCalls;
		m_waitingCalls.clear();

		for(auto& cb: waitingCallsCopy) {
			cb();
		}
	}

// ##############################################################################################################################
// ProxyServerStatEntry
// ##############################################################################################################################

	// ************************************************************************************
	ProxyServerStatEntry::ProxyServerStatEntry() : m_lastTimes(100) {
		m_num = 0;
	}

	// ************************************************************************************
	void ProxyServerStatEntry::tick(const std::string& name) {
		if (m_name.empty()) m_name = name;
		m_num += 1;
		m_lastTimes.push_back(g_clock.time());
	}

	// ************************************************************************************
	float ProxyServerStatEntry::getPerSecond() const {
		if (m_lastTimes.empty()) return 0.0f;

		ticks_t minTime = g_clock.time();
		ticks_t maxTime = 0;

		for(auto& e: m_lastTimes) {
			if (e < minTime) minTime = e;
			if (e > maxTime) maxTime = e;
		}

		float seconds = maxTime - minTime;
		return static_cast<float>(m_lastTimes.size()) / seconds;
	}

	// ************************************************************************************
	std::string ProxyServerStatEntry::toString() const {
		return stdext::format("%s  num=%d  perSec=%.2f",
			m_name, m_num,
			getPerSecond()
		);
	}


// ##############################################################################################################################
// ProxyServer
// ##############################################################################################################################

	// ************************************************************************************
	ProxyServer::ProxyServer() {
		m_statsWriteInterval = 0;
		m_statsSaveNextTime = 0;
	}

	// ************************************************************************************
	ProxyServer::~ProxyServer() {

	}

	// ************************************************************************************
	void ProxyServer::poll() {
		for(auto& e: m_cache) {
			e->poll();
		}

		if (m_statsWriteInterval > 0 && m_statsSaveNextTime < g_clock.time()) {
			m_statsSaveNextTime = g_clock.time() + m_statsWriteInterval;
			saveStats();
		}
	}

	// ************************************************************************************
	bool ProxyServer::handleMessage(const io::InetEndpoint& source, const Value& message) {
		// TODO: tutaj trzeba obslugiwac PDU od get/get-next/get-bulk/set
		// i ustalic jakie to jest OIDSpec i na podstaiwe tego, czy moze jest cache czy nie
		// + uzyc m_client do uzyskania tego co potrzeba

		auto community = message[1].valueString();
		auto& pdu = message[2];
		auto self = dynamic_self_cast<ProxyServer>();

		if (std::find(m_serverCommunities.begin(), m_serverCommunities.end(), community) == m_serverCommunities.end()) {
			// nie obslugujemy tego community
			return false;
		}

		if (pdu.type() == ValueType::PDU_GET && pdu.size() == 4) {
			processGet(source, message);
			return true;
		}

		if (pdu.type() == ValueType::PDU_SET && pdu.size() == 4) {
			processSet(source, message);
			return true;
		}

		if (pdu.type() == ValueType::PDU_GET_NEXT && pdu.size() == 4) {
			processGetNext(source, message);
			return true;
		}

		if (pdu.type() == ValueType::PDU_GET_BULK && pdu.size() == 4) {
			processGetBulk(source, message);
			return true;
		}

		return false;
	}


	// ************************************************************************************
	void ProxyServer::replyError(const io::InetEndpoint& dest, const Value& orginalMessage, const SNMPError& err) {
		auto message = orginalMessage;
		auto& pdu = message[2];

		pdu.setType(ValueType::PDU_RESPONSE);
		pdu[1] = Value::createInt(err.code);
		pdu[2] = Value::createInt(err.index);

		send(dest, message);
	}

	// ************************************************************************************
	void ProxyServer::send(const io::InetEndpoint& dest, const Value& message) {
		io::DataBuffer buf;
		io::DataBufferOutputStream os(buf, true);
		SNMPOutputStreamAdapter snmpOS(os);
		snmpOS.writeValue(message);
		m_serverSocket->send(dest, buf);
	}

	// ************************************************************************************
	bool ProxyServer::loadFromConfig(const config::parser::ConfigEntriesCollection& entries, std::vector<ClientPtr>& clients) {
		io::InetEndpoint socketSpec;

		for(auto& e: entries) {
			if (e->name() == "community") {
				for(auto& v: e->values()) {
					if (v->isPrimitive()) {
						m_serverCommunities.push_back(v->primitive());
					}
				}
				continue;
			}

			if (e->name() == "socket" && e->hasValuePrimitive(0)) {
				socketSpec = e->valuePrimitive(0);
				continue;
			}

			if (e->name() == "target" && e->hasValueBlock(0)) {
				for(auto& ee: e->valueBlock(0)) {
					if (ee->name() == "src-socket" && ee->hasValuePrimitive()) {
						m_targetSourceSocketSpec = ee->valuePrimitive();
						continue;
					}
					if (ee->name() == "dst-socket" && ee->hasValuePrimitive()) {
						m_targetDestSocketSpec = ee->valuePrimitive();
						continue;
					}
					if (ee->name() == "community" && ee->hasValuePrimitive()) {
						m_targetCommunity = ee->valuePrimitive();
						continue;
					}
					g_logger.warning(stdext::format("[ProxyServer::loadFromConfig] Unknown config entry '%s'", ee->name()));
				}
				continue;
			}

			if (e->name() == "statistics" && e->hasValueBlock(0)) {
				for(auto& ee: e->valueBlock(0)) {
					if (ee->name() == "file" && ee->hasValuePrimitive()) {
						m_statsFile = ee->valuePrimitive();
					}
					if (ee->name() == "write-interval" && ee->hasValuePrimitive()) {
						m_statsWriteInterval = ee->valueInt();
					}
				}
				continue;
			}

			if (e->name() == "cache-for" && e->hasValuePrimitive(0) && e->hasValueBlock(1)) {
				ProxyServerCacheEntryPtr ce(new ProxyServerCacheEntry());
				if (ce->loadFromConfig(e->valuePrimitive(0), e->valueBlock(1))) {
					m_cache.push_back(ce);
				} else {
					g_logger.warning(stdext::format("[ProxyServer::loadFromConfig] Cannot load cache-for '%s'", e->valuePrimitive(0)));
				}
				continue;
			}

			g_logger.warning(stdext::format("[ProxyServer::loadFromConfig] Unknown config entry '%s'", e->name()));
		}


		if (m_targetSourceSocketSpec.empty()) {
			g_logger.warning("[ProxyServer::loadFromConfig] Invalid target source socket");
			return false;
		}
		if (m_targetDestSocketSpec.empty()) {
			g_logger.warning("[ProxyServer::loadFromConfig] Invalid target dest socket");
			return false;
		}
		if (m_targetCommunity.empty()) {
			g_logger.warning("[ProxyServer::loadFromConfig] No target community given");
			return false;
		}

		m_client.reset(new Client(m_targetSourceSocketSpec, m_targetDestSocketSpec, m_targetCommunity));
		clients.push_back(m_client);

		if (socketSpec.empty()) {
			g_logger.warning("[ProxyServer::loadFromConfig] No server socket specified");
			return false;
		}
		if (m_serverCommunities.empty()) {
			g_logger.warning("[ProxyServer::loadFromConfig] No server community specified");
			return false;
		}

		for(auto& ce: m_cache) {
			ce->setClient(m_client, m_targetDestSocketSpec);
		}

		m_serverSocket = g_snmpSocketsManager.ensureServerSocket(socketSpec, dynamic_self_cast<ProxyServer>());
		return true;
	}

	// ************************************************************************************
	void ProxyServer::tickStat(const std::string& key) {
		m_stats[key].tick(key);
	}

	// ************************************************************************************
	void ProxyServer::saveStats() {
		FILE* fp = fopen(m_statsFile.c_str(), "w");
		if (fp != nullptr) {

			for(auto& e: m_stats) {
				fprintf(fp, "%s\n", e.second.toString().c_str());
			}

			fclose(fp);
		}
	}

	// ************************************************************************************
	ProxyServerCacheEntryPtr ProxyServer::findCacheFor(const OID& oid) {
		for(auto& e: m_cache) {
			if (e->matches(oid)) return e;
		}
		return nullptr;
	}

	// ************************************************************************************
	void ProxyServer::proxyRequest(const io::InetEndpoint& source, const Value& requestMessage) {
		auto self = dynamic_self_cast<ProxyServer>();
		m_client->doRequest(requestMessage[2], [=](const Value& responseMessage, const SNMPError& error){
			Value msg = requestMessage;
			if (PDUUtils::copyMaintainingRequestID(msg, responseMessage)) {
				// ok
			} else {
				PDUUtils::setError(msg, error);
			}
			self->send(source, msg);
		});
	}

	// ************************************************************************************
	void ProxyServer::processSet(const io::InetEndpoint& source, const Value& requestMessage) {
		// proxujemy 1:1
		// TODO: a jak updatujemy cos co jest w cache, to chyba powinnismy to tez zmienic?

		if (isStatsEnabled()) {
			auto varBindings = VarBindingRef::fromValue(requestMessage[2][3]);
			for(auto& e: varBindings) {
				tickStat(stdext::format("set %s", e.name.toString()));
			}
		}

		proxyRequest(source, requestMessage);
	}

	// ************************************************************************************
	void ProxyServer::processGet(const io::InetEndpoint& source, const Value& requestMessage) {
		auto varBindings = VarBindingRef::fromValue(requestMessage[2][3]);
		auto self = dynamic_self_cast<ProxyServer>();

		if (varBindings.size() == 0) {
			// TODO: a moze powinnismy odeslac puste?
			replyError(source, requestMessage, SNMPError(SNMPError::SNMP_NO_SUCH_NAME, 0));
			return;
		}

		if (isStatsEnabled()) {
			for(auto& e: varBindings) {
				tickStat(stdext::format("get %s", e.name.toString()));
			}
		}

		if (varBindings.size() > 1) {
			g_logger.warning("[ProxyServer::processGet] Cache for more than one VarBinding not supported. Proxing 1:1");
			proxyRequest(source, requestMessage);
			return;
		}

		auto ce = findCacheFor(varBindings[0].name);
		if (ce) {
			ce->doGetOne(varBindings[0].name, [=](const std::vector<VarBinding>& res){
				Value msg = requestMessage;

				PDUUtils::setPDUType(msg, ValueType::PDU_RESPONSE);

				if (res.empty()) {
					PDUUtils::setError(msg, SNMPError(SNMPError::SNMP_NO_SUCH_NAME, 0));
				} else {
					PDUUtils::setVarBindings(msg, res);
				}

				self->send(source, msg);
			});
		} else {
			proxyRequest(source, requestMessage);
		}
	}

	// ************************************************************************************
	void ProxyServer::processGetNext(const io::InetEndpoint& source, const Value& requestMessage) {
		auto varBindings = VarBindingRef::fromValue(requestMessage[2][3]);
		auto self = dynamic_self_cast<ProxyServer>();

		if (varBindings.size() == 0) {
			// TODO: a moze powinnismy odeslac puste?
			replyError(source, requestMessage, SNMPError(SNMPError::SNMP_NO_SUCH_NAME, 0));
			return;
		}

		if (isStatsEnabled()) {
			for(auto& e: varBindings) {
				tickStat(stdext::format("get-next %s", e.name.toString()));
			}
		}

		if (varBindings.size() > 1) {
			g_logger.warning("[ProxyServer::processGetNext] Cache for more than one VarBinding not supported. Proxing 1:1");
			proxyRequest(source, requestMessage);
			return;
		}

		auto ce = findCacheFor(varBindings[0].name);
		if (ce) {
			OID varName = varBindings[0].name;
			ce->doGetNext(varName, [=](const std::vector<VarBinding>& res){
				Value msg = requestMessage;
				PDUUtils::setPDUType(msg, ValueType::PDU_RESPONSE);

				if (res.empty()) {
					PDUUtils::setEndOfMIBView(msg, varName);
				} else {
					PDUUtils::setVarBindings(msg, res);
				}
				self->send(source, msg);
			});
		} else {
			proxyRequest(source, requestMessage);
		}
	}

	// ************************************************************************************
	void ProxyServer::processGetBulk(const io::InetEndpoint& source, const Value& requestMessage) {
		auto varBindings = VarBindingRef::fromValue(requestMessage[2][3]);
		auto self = dynamic_self_cast<ProxyServer>();
		int32_t maxRepetitions = requestMessage[2][2].valueInt();

		if (varBindings.size() == 0) {
			// TODO: a moze powinnismy odeslac puste?
			replyError(source, requestMessage, SNMPError(SNMPError::SNMP_NO_SUCH_NAME, 0));
			return;
		}

		if (isStatsEnabled()) {
			for(auto& e: varBindings) {
				tickStat(stdext::format("get-bulk %s", e.name.toString()));
			}
		}

		if (varBindings.size() > 1) {
			g_logger.warning("[ProxyServer::processGetBulk] Cache for more than one VarBinding not supported. Proxing 1:1");
			proxyRequest(source, requestMessage);
			return;
		}

		auto ce = findCacheFor(varBindings[0].name);
		if (ce) {
			OID varName = varBindings[0].name;
			ce->doGetFrom(varName, maxRepetitions, [=](const std::vector<VarBinding>& res){
				Value msg = requestMessage;
				PDUUtils::setPDUType(msg, ValueType::PDU_RESPONSE);

				if (res.empty()) {
					PDUUtils::setEndOfMIBView(msg, varName);
				} else {
					PDUUtils::setVarBindings(msg, res);
				}
				self->send(source, msg);
			});
		} else {
			proxyRequest(source, requestMessage);
		}
	}



} }
