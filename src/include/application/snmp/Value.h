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

#ifndef INCLUDE_APPLICATION_SNMP_VALUE_H_
#define INCLUDE_APPLICATION_SNMP_VALUE_H_

#include "base.h"

namespace application { namespace snmp {

	class OID {
		public:
			OID() { }
			OID(const std::string& str);

			bool empty() const { return m_id.empty(); }
			size_t size() const { return m_id.size(); }
			int32_t last() const { return m_id.empty() ? 0 : m_id[m_id.size() - 1]; }

			int32_t operator[](int32_t idx) const { return m_id[idx]; }

			bool startsWith(const OID& other) const;
			std::string toString() const;

			bool operator==(const OID& other) const;
			bool operator!=(const OID& other) const { return !(*this == other); }
			bool operator<(const OID& other) const;
			bool operator>(const OID& other) const;
			bool operator<=(const OID& other) const { return *this == other || *this < other; }
			bool operator>=(const OID& other) const { return *this == other || *this > other; }

		private:
			Int32Vector m_id;
	};

	class Value {
		public:
			Value();

			const ValueType::Enum type() const { return m_type; }
			void setType(ValueType::Enum type) { m_type = type; }

			bool isNull() const { return m_type == ValueType::NULL_; }
			bool isSequence() const { return m_type == ValueType::SEQUENCE; }
			bool isPDU() const { return ValueType::isPDU(m_type); }
			bool isMessage() const { return isSequence() && size() == 3; }

			size_t size() const { return m_valueVec.size(); }
			int64_t valueInt() const { return m_valueInt; }
			const std::string& valueString() const { return m_valueString; }
			const std::vector<Value>& valueVec() const { return m_valueVec; }
			const OID& valueOID() const { return m_valueOID; }

			const Value& operator[](int32_t idx) const { return m_valueVec[idx]; }
			Value& operator[](int32_t idx) { return m_valueVec[idx]; }

			void addItem(const Value& val) { m_valueVec.push_back(val); }

			std::string toString() const;
			std::string toStringDeep() const;
			void printDebug(int32_t indent) const;

			static Value createInt(int64_t v);
			static Value createCounter32(int64_t v);
			static Value createCounter64(int64_t v);
			static Value createGauge32(int64_t v);
			static Value createTimeTicks(int64_t v);
			static Value createIPAddr(const std::string& ip);
			static Value createString(const std::string& str);
			static Value createOID(const OID& oid);
			static Value createNull();
			static Value createEndOfMIBView();

			static Value createSequence(const std::vector<Value>& vec, ValueType::Enum type = ValueType::SEQUENCE);
			static Value createSequence(ValueType::Enum type = ValueType::SEQUENCE);

		private:

			ValueType::Enum m_type;
			int64_t m_valueInt;
			std::string m_valueString;
			OID m_valueOID;
			std::vector<Value> m_valueVec;
	};

	class VarBinding {
		public:
			OID name;
			Value value;

			Value toValue() const;
			static std::vector<VarBinding> fromValue(const Value& value);

			bool operator<(const VarBinding& other) const { return name < other.name; }
			bool operator>(const VarBinding& other) const { return name > other.name; }
	};

	class VarBindingRef {
		public:
			const OID& name;
			const Value& value;

			VarBindingRef(const OID& name, const Value& value) : name(name), value(value) { }
			VarBinding toVarBinding() const {
				VarBinding res;
				res.name = name;
				res.value = value;
				return res;
			}
			static std::vector<VarBindingRef> fromValue(const Value& value);
	};

} }

namespace std {

	template<>
	struct hash<application::snmp::OID> {
		size_t operator()(const application::snmp::OID& p) const {
			return std::hash<std::string>{}(p.toString());
		}
	};

}

#endif /* INCLUDE_APPLICATION_SNMP_VALUE_H_ */
