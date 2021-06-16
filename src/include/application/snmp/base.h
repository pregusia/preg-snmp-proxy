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

#ifndef INCLUDE_APPLICATION_SNMP_BASE_H_
#define INCLUDE_APPLICATION_SNMP_BASE_H_

#include <application/base.h>

namespace application { namespace snmp {

	class Value;
	class OID;
	class VarBinding;

	class Socket;
	typedef stdext::object_ptr<Socket> SocketPtr;

	class Client;
	typedef stdext::object_ptr<Client> ClientPtr;

	class ProxyServer;
	typedef stdext::object_ptr<ProxyServer> ProxyServerPtr;

	class ProxyServerCacheEntry;
	typedef stdext::object_ptr<ProxyServerCacheEntry> ProxyServerCacheEntryPtr;

	class SocketsManager;

	class ValueType {
		public:

			ENUM_DEFINE_INCLS(
				INTEGER = 0x02,
				STRING = 0x04,
				NULL_ = 0x05,
				OID = 0x06,
				SEQUENCE = 0x30,

				IPADDR = 0x40,
				COUNTER32 = 0x41,
				GAUGE32 = 0x42,
				TIMETICKS = 0x43,
				COUNTER64 = 0x46,

				END_OF_MIB_VIEW = 0x82,

				PDU_GET = 0xA0,
				PDU_GET_NEXT = 0xA1,
				PDU_RESPONSE = 0xA2,
				PDU_SET = 0xA3,
				PDU_GET_BULK = 0xA5,
			);

			static bool isPDU(Enum e) {
				switch(e) {
					case PDU_GET:
					case PDU_GET_BULK:
					case PDU_GET_NEXT:
					case PDU_SET:
					case PDU_RESPONSE:
						return true;
					default:
						return false;
				}
			}

		private:
			ValueType() { }
	};

	class SNMPError {
		public:
			typedef enum:int32_t {

				NO_ERROR = 0,
				SNMP_TOO_BIG = 1,
				SNMP_NO_SUCH_NAME = 2,
				SNMP_BAD_VALUE = 3,
				SNMP_READ_ONLY = 4,
				SNMP_GEN_ERROR = 5,
				SNMP_NO_ACCESS = 6,
				SNMP_WRONG_TYPE = 7,
				SNMP_WRONG_LEN = 8,
				SNMP_WRONG_ENCODING = 9,
				SNMP_WRONG_VALUE = 10,
				SNMP_NO_CREATION = 11,
				SNMP_INCONSISTENT_VALUE = 12,
				SNMP_RESOURCE_UNAVAILABLE = 13,
				SNMP_COMMIT_FAILED = 14,
				SNMP_UNDO_FAILED = 15,
				SNMP_AUTHORIZATION_ERROR = 16,
				SNMP_NOT_WRITABLE = 17,
				SNMP_INCONSISTENT_NAME = 18,

				APP_TIMEOUT = 201,
				APP_NOT_SEQUENCE = 202,
			} Enum;

			Enum code;
			int32_t index;

			SNMPError() : code(NO_ERROR), index(0) { }
			SNMPError(Enum code, int32_t index) : code(code), index(index) { }
			SNMPError(int32_t code, int32_t index) : code(static_cast<Enum>(code)), index(index) { }
			bool hasError() const { return code != NO_ERROR; }

			std::string toString() const;

			static SNMPError fromPDU(const Value& val);
	};

	class PDUUtils {
		public:

			static bool copyMaintainingRequestID(Value& destMessage, const Value& sourceMessage);
			static bool setError(Value& destMessage, const SNMPError& error);
			static bool setPDUType(Value& destMessage, ValueType::Enum type);
			static bool setVarBindings(Value& destMessage, const std::vector<VarBinding>& arr);
			static bool setEndOfMIBView(Value& destMessage, const OID& oid);

		private:
			PDUUtils() { }
	};


	int32_t nextRequestID();

} }

extern application::snmp::SocketsManager g_snmpSocketsManager;


#endif /* INCLUDE_APPLICATION_SNMP_BASE_H_ */
