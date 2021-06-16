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

#ifndef INCLUDE_APPLICATION_SNMP_PROXYSERVER_H_
#define INCLUDE_APPLICATION_SNMP_PROXYSERVER_H_

#include "base.h"
#include "Value.h"

#include <unordered_map>

#include <io/InetEndpoint.h>
#include <boost/circular_buffer.hpp>

namespace application { namespace snmp {

	class ProxyServerCacheEntry: public stdext::object {
		public:
			typedef std::function<void(const std::vector<VarBinding>& res)> Callback;

			ProxyServerCacheEntry();
			virtual ~ProxyServerCacheEntry();

			bool loadFromConfig(const std::string& oid, const config::parser::ConfigEntriesCollection& entries);

			void setClient(const ClientPtr& client, const io::InetEndpoint& dest);
			bool matches(const OID& oid) const;
			void poll();

			void doGetAll(const Callback& func);
			void doGetOne(const OID& oid, const Callback& func);
			void doGetFrom(const OID& start, int32_t num, const Callback& func);
			void doGetNext(const OID& oid, const Callback& func);


		private:
			OID m_baseOID;
			std::vector<VarBinding> m_values;
			int32_t m_updateInterval;
			ticks_t m_nextUpdateTime;
			bool m_updating;
			bool m_initialized;

			ClientPtr m_client;
			io::InetEndpoint m_destEndpoint;

			std::vector<std::function<void(void)>> m_waitingCalls;

			void doUpdate();
			void processUpdateResult(const std::vector<VarBinding>& values, const SNMPError& error);
	};

	class ProxyServerStatEntry {
		public:
			ProxyServerStatEntry();

			void tick(const std::string& name);
			std::string toString() const;

			float getPerSecond() const;

		private:
			std::string m_name;
			int32_t m_num;
			boost::circular_buffer<ticks_t> m_lastTimes;
	};

	class ProxyServer: public stdext::object {
		public:
			ProxyServer();
			virtual ~ProxyServer();

			void poll();
			bool handleMessage(const io::InetEndpoint& source, const Value& message);

			void replyError(const io::InetEndpoint& dest, const Value& orginalMessage, const SNMPError& err);
			void send(const io::InetEndpoint& dest, const Value& message);

			bool loadFromConfig(const config::parser::ConfigEntriesCollection& entries, std::vector<ClientPtr>& clients);

		private:
			SocketPtr m_serverSocket;
			StringVector m_serverCommunities;

			io::InetEndpoint m_targetSourceSocketSpec;
			io::InetEndpoint m_targetDestSocketSpec;
			std::string m_targetCommunity;

			ClientPtr m_client;

			std::string m_statsFile;
			int32_t m_statsWriteInterval;

			std::unordered_map<std::string,ProxyServerStatEntry> m_stats;
			ticks_t m_statsSaveNextTime;

			bool isStatsEnabled() const { return m_statsWriteInterval > 0; }
			void tickStat(const std::string& key);
			void saveStats();

			std::vector<ProxyServerCacheEntryPtr> m_cache;

			ProxyServerCacheEntryPtr findCacheFor(const OID& oid);

			void proxyRequest(const io::InetEndpoint& source, const Value& requestMessage);

			void processSet(const io::InetEndpoint& source, const Value& requestMessage);
			void processGet(const io::InetEndpoint& source, const Value& requestMessage);
			void processGetNext(const io::InetEndpoint& source, const Value& requestMessage);
			void processGetBulk(const io::InetEndpoint& source, const Value& requestMessage);
	};


} }

#endif /* INCLUDE_APPLICATION_SNMP_PROXYSERVER_H_ */
