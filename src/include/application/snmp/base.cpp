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


#include "base.h"
#include "Value.h"

static int32_t g_snmpNextRequestID = 10;

namespace application { namespace snmp {

	// ************************************************************************************
	std::string SNMPError::toString() const {
		switch(code) {
			case NO_ERROR: return "NO_ERROR";
			case SNMP_TOO_BIG: return stdext::format("SNMP_TOO_BIG(index=%d)", index);
			case SNMP_NO_SUCH_NAME: return stdext::format("SNMP_NO_SUCH_NAME(index=%d)", index);
			case SNMP_BAD_VALUE: return stdext::format("SNMP_BAD_VALUE(index=%d)", index);
			case SNMP_READ_ONLY: return stdext::format("SNMP_READ_ONLY(index=%d)", index);
			case SNMP_GEN_ERROR: return stdext::format("SNMP_GEN_ERROR(index=%d)", index);

			case APP_TIMEOUT: return "APP_TIMEOUT";
			case APP_NOT_SEQUENCE: return "APP_NOT_SEQUENCE";

			default:
				return stdext::format("UNKNOWN(code=%d,index=%d)", static_cast<int32_t>(code), index);
		}
	}

	// ************************************************************************************
	SNMPError SNMPError::fromPDU(const Value& val) {
		if (val.size() == 4) {
			return SNMPError(val[1].valueInt(), val[2].valueInt());
		} else {
			return SNMPError();
		}
	}

	// ************************************************************************************
	int32_t nextRequestID() {
		int32_t next = ++g_snmpNextRequestID;
		if (g_snmpNextRequestID > 0xFFFFFF) g_snmpNextRequestID = 1;
		return next;
	}

	// ************************************************************************************
	bool PDUUtils::copyMaintainingRequestID(Value& destMessage, const Value& sourceMessage) {
		if (!sourceMessage.isMessage()) return false;
		if (!destMessage.isMessage()) return false;

		Value requestID = destMessage[2][0];
		destMessage[2] = sourceMessage[2];
		destMessage[2][0] = requestID;
		return true;
	}

	// ************************************************************************************
	bool PDUUtils::setError(Value& destMessage, const SNMPError& error) {
		if (!destMessage.isMessage()) return false;
		if (!error.hasError()) return false;

		int32_t code = static_cast<int32_t>(error.code);
		if (code == SNMPError::APP_TIMEOUT) {
			code = SNMPError::SNMP_RESOURCE_UNAVAILABLE;
		}

		destMessage[2][1] = Value::createInt(code);
		destMessage[2][2] = Value::createInt(error.index);
		return true;
	}

	// ************************************************************************************
	bool PDUUtils::setPDUType(Value& destMessage, ValueType::Enum type) {
		if (!destMessage.isMessage()) return false;
		if (!ValueType::isPDU(type)) return false;

		destMessage[2].setType(type);
		return true;
	}

	// ************************************************************************************
	bool PDUUtils::setVarBindings(Value& destMessage, const std::vector<VarBinding>& arr) {
		if (!destMessage.isMessage()) return false;

		Value seq = Value::createSequence();
		for(auto& e: arr) {
			seq.addItem(e.toValue());
		}

		destMessage[2][3] = seq;
		return true;
	}

	// ************************************************************************************
	bool PDUUtils::setEndOfMIBView(Value& destMessage, const OID& oid) {
		if (!destMessage.isMessage()) return false;

		Value seq2 = Value::createSequence();
		seq2.addItem(Value::createOID(oid));
		seq2.addItem(Value::createEndOfMIBView());

		Value seq1 = Value::createSequence();
		seq1.addItem(seq2);

		destMessage[2][3] = seq1;
		return true;
	}



} }
