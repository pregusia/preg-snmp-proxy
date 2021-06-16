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

#include "Value.h"

#include <arpa/inet.h>

namespace application { namespace snmp {

	// ************************************************************************************
	Value::Value() {
		m_type = ValueType::NULL_;
		m_valueInt = 0;
		m_valueString = "null";
	}

	// ************************************************************************************
	std::string Value::toString() const {
		switch(m_type) {
			case ValueType::COUNTER32: return stdext::format("[Value::COUNTER32 val=%ld]", m_valueInt);
			case ValueType::COUNTER64: return stdext::format("[Value::COUNTER64 val=%ld]", m_valueInt);
			case ValueType::GAUGE32: return stdext::format("[Value::GAUGE32 val=%ld]", m_valueInt);
			case ValueType::INTEGER: return stdext::format("[Value::INTEGER val=%ld]", m_valueInt);
			case ValueType::IPADDR: return stdext::format("[Value::IPADDR val=\"%s\"]", m_valueString);
			case ValueType::NULL_: return "[Value::NULL]";
			case ValueType::OID: return stdext::format("[Value::OID val=%s]", m_valueOID.toString());
			case ValueType::SEQUENCE: return stdext::format("[Value::SEQUENCE size=%d]", m_valueVec.size());
			case ValueType::STRING: return stdext::format("[Value::STRING val=\"%s\"]", m_valueString);
			case ValueType::TIMETICKS: return stdext::format("[Value::TIMETICKS val=%ld]", m_valueInt);

			case ValueType::PDU_GET: return stdext::format("[Value::PDU_GET size=%d]", m_valueVec.size());
			case ValueType::PDU_GET_BULK: return stdext::format("[Value::PDU_GET_BULK size=%d]", m_valueVec.size());
			case ValueType::PDU_GET_NEXT: return stdext::format("[Value::PDU_GET_NEXT size=%d]", m_valueVec.size());
			case ValueType::PDU_RESPONSE: return stdext::format("[Value::PDU_RESPONSE size=%d]", m_valueVec.size());
			case ValueType::PDU_SET: return stdext::format("[Value::PDU_SET size=%d]", m_valueVec.size());

			default: return "[Value::UNKNOWN_TYPE]";
		}
	}

	// ************************************************************************************
	std::string Value::toStringDeep() const {
		switch(m_type) {
			case ValueType::COUNTER32: return stdext::format("[Value::COUNTER32 val=%ld]", m_valueInt);
			case ValueType::COUNTER64: return stdext::format("[Value::COUNTER64 val=%ld]", m_valueInt);
			case ValueType::GAUGE32: return stdext::format("[Value::GAUGE32 val=%ld]", m_valueInt);
			case ValueType::INTEGER: return stdext::format("[Value::INTEGER val=%ld]", m_valueInt);
			case ValueType::IPADDR: return stdext::format("[Value::IPADDR val=\"%s\"]", m_valueString);
			case ValueType::NULL_: return "[Value::NULL]";
			case ValueType::OID: return stdext::format("[Value::OID val=%s]", m_valueOID.toString());
			case ValueType::STRING: return stdext::format("[Value::STRING val=\"%s\"]", m_valueString);
			case ValueType::TIMETICKS: return stdext::format("[Value::TIMETICKS val=%ld]", m_valueInt);
		}

		if (m_type == ValueType::SEQUENCE || ValueType::isPDU(m_type)) {
			std::stringstream ss;
			switch(m_type) {
				case ValueType::SEQUENCE: ss << "[Value::SEQUENCE inner={"; break;
				case ValueType::PDU_GET: ss << "[Value::PDU_GET inner={"; break;
				case ValueType::PDU_GET_BULK: ss << "[Value::PDU_GET_BULK inner={"; break;
				case ValueType::PDU_GET_NEXT: ss << "[Value::PDU_GET_NEXT inner={"; break;
				case ValueType::PDU_RESPONSE: ss << "[Value::PDU_RESPONSE inner={"; break;
				case ValueType::PDU_SET: ss << "[Value::PDU_SET inner={"; break;
			}

			for(auto& e: m_valueVec) {
				ss << e.toStringDeep() << ",";
			}

			ss << "}";
			return ss.str();
		}

		return "[Value::UNKNOWN_TYPE]";
	}

	// ************************************************************************************
	void Value::printDebug(int32_t indent) const {
		switch(m_type) {
			case ValueType::COUNTER32:
				g_logger.debug(stdext::repeatString(" ", indent) + stdext::format("COUNTER32(%ld)", m_valueInt));
				return;

			case ValueType::COUNTER64:
				g_logger.debug(stdext::repeatString(" ", indent) + stdext::format("COUNTER64(%ld)", m_valueInt));
				return;

			case ValueType::GAUGE32:
				g_logger.debug(stdext::repeatString(" ", indent) + stdext::format("GAUGE32(%ld)", m_valueInt));
				return;

			case ValueType::INTEGER:
				g_logger.debug(stdext::repeatString(" ", indent) + stdext::format("INTEGER(%ld)", m_valueInt));
				return;

			case ValueType::IPADDR:
				g_logger.debug(stdext::repeatString(" ", indent) + stdext::format("IPADDR(\"%s\")", m_valueString));
				return;

			case ValueType::NULL_:
				g_logger.debug(stdext::repeatString(" ", indent) + "NULL");
				return;

			case ValueType::OID:
				g_logger.debug(stdext::repeatString(" ", indent) + stdext::format("OID(%s)", m_valueOID.toString()));
				return;

			case ValueType::STRING:
				g_logger.debug(stdext::repeatString(" ", indent) + stdext::format("STRING(\"%s\")", m_valueString));
				return;

			case ValueType::TIMETICKS:
				g_logger.debug(stdext::repeatString(" ", indent) + stdext::format("TIMETICKS(%ld)", m_valueInt));
				return;
		}

		if (m_type == ValueType::SEQUENCE || ValueType::isPDU(m_type)) {
			switch(m_type) {
				case ValueType::SEQUENCE:
					g_logger.debug(stdext::repeatString(" ", indent) + "SEQUENCE");
					break;

				case ValueType::PDU_GET:
					g_logger.debug(stdext::repeatString(" ", indent) + "PDU_GET");
					break;

				case ValueType::PDU_GET_BULK:
					g_logger.debug(stdext::repeatString(" ", indent) + "PDU_GET_BULK");
					break;

				case ValueType::PDU_GET_NEXT:
					g_logger.debug(stdext::repeatString(" ", indent) + "PDU_GET_NEXT");
					break;

				case ValueType::PDU_RESPONSE:
					g_logger.debug(stdext::repeatString(" ", indent) + "PDU_RESPONSE");
					break;

				case ValueType::PDU_SET:
					g_logger.debug(stdext::repeatString(" ", indent) + "PDU_SET");
					break;
			}

			for(auto& e: m_valueVec) {
				e.printDebug(indent + 2);
			}
		}
	}


	// ************************************************************************************
	Value Value::createInt(int64_t v) {
		Value res;
		res.m_type = ValueType::INTEGER;
		res.m_valueInt = v;
		res.m_valueString = stdext::format("%ld", v);
		return res;
	}

	// ************************************************************************************
	Value Value::createCounter32(int64_t v) {
		Value res;
		res.m_type = ValueType::COUNTER32;
		res.m_valueInt = v;
		res.m_valueString = stdext::format("%ld", v);
		return res;
	}

	// ************************************************************************************
	Value Value::createCounter64(int64_t v) {
		Value res;
		res.m_type = ValueType::COUNTER64;
		res.m_valueInt = v;
		res.m_valueString = stdext::format("%ld", v);
		return res;
	}

	// ************************************************************************************
	Value Value::createGauge32(int64_t v) {
		Value res;
		res.m_type = ValueType::GAUGE32;
		res.m_valueString = stdext::format("%ld", v);
		res.m_valueInt = v;
		return res;
	}

	// ************************************************************************************
	Value Value::createTimeTicks(int64_t v) {
		Value res;
		res.m_type = ValueType::TIMETICKS;
		res.m_valueString = stdext::format("%ld", v);
		res.m_valueInt = v;
		return res;
	}

	// ************************************************************************************
	Value Value::createIPAddr(const std::string& ip) {
		Value res;
		res.m_type = ValueType::IPADDR;
		res.m_valueInt = inet_addr(ip.c_str());
		res.m_valueString = ip;
		return res;
	}

	// ************************************************************************************
	Value Value::createString(const std::string& str) {
		Value res;
		res.m_type = ValueType::STRING;
		res.m_valueString = str;
		return res;
	}

	// ************************************************************************************
	Value Value::createOID(const OID& oid) {
		if (oid.empty()) return createNull();

		Value res;
		res.m_type = ValueType::OID;
		res.m_valueString = oid.toString();
		res.m_valueOID = oid;
		return res;
	}

	// ************************************************************************************
	Value Value::createNull() {
		Value res;
		res.m_type = ValueType::NULL_;
		return res;
	}

	// ************************************************************************************
	Value Value::createEndOfMIBView() {
		Value res;
		res.m_type = ValueType::END_OF_MIB_VIEW;
		return res;
	}

	// ************************************************************************************
	Value Value::createSequence(const std::vector<Value>& vec, ValueType::Enum type) {
		Value res;
		res.m_type = type;
		res.m_valueVec = vec;
		res.m_valueString = "seq";
		return res;
	}

	// ************************************************************************************
	Value Value::createSequence(ValueType::Enum type) {
		Value res;
		res.m_type = type;
		res.m_valueString = "seq";
		return res;
	}


	// ************************************************************************************
	OID::OID(const std::string& str) {
		if (!str.empty() && str[0] == '.') {
			auto arr = stdext::split<char>(str, ".");
			for(size_t i=1;i<arr.size();++i) {
				if (!arr[i].empty() && stdext::isNumeric(arr[i])) {
					m_id.push_back(stdext::toInt(arr[i]));
				} else {
					break;
				}
			}

			if (m_id.size() < 3) {
				m_id.clear();
			}
		}
	}

	// ************************************************************************************
	bool OID::startsWith(const OID& other) const {
		if (empty()) return false;
		if (other.empty()) return false;
		if (size() >= other.size()) {
			for(size_t i=0;i<other.size();++i) {
				if (m_id[i] != other[i]) return false;
			}
			return true;
		}
		return false;
	}

	// ************************************************************************************
	std::string OID::toString() const {
		if (empty()) {
			return "";
		} else {
			return "." + stdext::join<Int32Vector, char>(m_id,".");
		}
	}

	// ************************************************************************************
	bool OID::operator==(const OID& other) const {
		if (other.size() != size()) return false;
		for(size_t i=0;i<other.size();++i) {
			if (m_id[i] != other[i]) return false;
		}
		return true;
	}

	// ************************************************************************************
	bool OID::operator<(const OID& other) const {
		// TODO: sprawdzic logike tego
		for(size_t i=0;i<std::min(size(),other.size());++i) {
			if (m_id[i] > other[i]) return false;
		}
		return size() <= other.size();
	}

	// ************************************************************************************
	bool OID::operator>(const OID& other) const {
		// TODO: sprawdzic logike tego
		for(size_t i=0;i<std::min(size(),other.size());++i) {
			if (m_id[i] < other[i]) return false;
		}
		return size() >= other.size();
	}


	// ************************************************************************************
	Value VarBinding::toValue() const {
		Value res = Value::createSequence();
		res.addItem(Value::createOID(name));
		res.addItem(value);
		return res;
	}

	// ************************************************************************************
	std::vector<VarBinding> VarBinding::fromValue(const Value& value) {
		std::vector<VarBinding> res;
		if (value.type() == ValueType::SEQUENCE) {
			for(size_t i=0;i<value.size();++i) {
				if (value[i].type() == ValueType::SEQUENCE && value[i].size() == 2) {
					VarBinding bb;
					bb.name = value[i][0].valueOID();
					bb.value = value[i][1];
					res.push_back(bb);
				}
			}
		}
		return res;
	}

	// ************************************************************************************
	std::vector<VarBindingRef> VarBindingRef::fromValue(const Value& value) {
		std::vector<VarBindingRef> res;
		if (value.type() == ValueType::SEQUENCE) {
			for(size_t i=0;i<value.size();++i) {
				if (value[i].type() == ValueType::SEQUENCE && value[i].size() == 2) {
					res.push_back(VarBindingRef(value[i][0].valueOID(), value[i][1]));
				}
			}
		}
		return res;
	}


} }
