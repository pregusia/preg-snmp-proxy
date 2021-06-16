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

#include "streams.h"
#include "Value.h"

#include <arpa/inet.h>

namespace application { namespace snmp {

	// ************************************************************************************
	int32_t SNMPInputStreamAdapter::readHeaderLength(io::SeekableInputStream& is, bool& errorFlag) {
		uint8_t b = 0;

		b = is.readPrimitive<uint8_t>();
		if ((b & 0x80) == 0x00) {
			// simple
			return b;
		} else {
			// long size
			int32_t len = b & 0x7F;
			int32_t res = 0;

			if (len > 4) {
				g_logger.warning(stdext::format("[SNMPInputStreamAdapter::readHeaderLength] Overflow! len=%d", len));
				errorFlag = true;
				return 0;
			}

			for(int32_t i = 0; i < len; i++) {
				res <<= 8;
				res |= is.readPrimitive<uint8_t>();
			}

			return res;
		}
	}

	// ************************************************************************************
	int32_t SNMPInputStreamAdapter::readInt32(io::SeekableInputStream& is, int32_t len, bool& errorFlag) {
		if (len > 5) {
			g_logger.warning(stdext::format("[SNMPInputStreamAdapter::readInt32] Overflow! len=%d", len));
			errorFlag = true;
			return 0;
		}

		int32_t res = 0;
		uint8_t buf[128] = { 0 };
		int32_t offset = 0;

		is.read(buf, len);

		if ((buf[offset] & 0x80) != 0) {
			res = -1;
		}

		if (buf[offset] == 0x80 && len > 2 && (buf[offset + 1] == 0xFF && (buf[offset + 2] & 0x80) != 0)) {
			// this is a filler byte to comply with no 9 x consecutive 1s
			offset += 1;
			len -= 1; // we've used one byte of the encoded length
		}

		for(int32_t i = 0; i < len; i++) {
			res <<= 8;
			res |= buf[offset++];
		}

		return res;
	}

	// ************************************************************************************
	uint32_t SNMPInputStreamAdapter::readUInt32(io::SeekableInputStream& is, int32_t len, bool& errorFlag) {
		if (len > 5) {
			g_logger.warning(stdext::format("[SNMPInputStreamAdapter::readUInt32] Overflow! len=%d", len));
			errorFlag = true;
			return 0;
		}

		uint32_t res = 0;
		uint8_t buf[128] = { 0 };
		int32_t offset = 0;

		is.read(buf, len);

		for(int32_t i = 0; i < len; i++) {
			res <<= 8;
			res |= buf[offset++];
		}

		return res;
	}

	// ************************************************************************************
	uint64_t SNMPInputStreamAdapter::readUInt64(io::SeekableInputStream& is, int32_t len, bool& errorFlag) {
		if (len > 9) {
			g_logger.warning(stdext::format("[SNMPInputStreamAdapter::readUInt64] Overflow! len=%d", len));
			errorFlag = true;
			return 0;
		}

		if (len == 9) {
			// if length is 9 we have a padding byte added. Skip it
			is.readPrimitive<uint8_t>();
			len -= 1;
		}


		uint8_t buf[16] = { 0 };
		uint64_t res = 0;
		int32_t offset = 0;

		is.read(buf, len);

		for(int32_t i=0;i<len;++i) {
			res <<= 8;
			res |= buf[offset++];
		}

		return res;
	}

	// ************************************************************************************
	uint32_t SNMPInputStreamAdapter::readIPAddress(io::SeekableInputStream& is, int32_t len, bool& errorFlag) {
		if (len != 4) {
			g_logger.warning(stdext::format("[SNMPInputStreamAdapter::readIPAddress] Invalid IPAddress len (%d)", len));
			errorFlag = true;
			return 0;
		}

		return is.readPrimitive<uint32_t>();
	}

	// ************************************************************************************
	OID SNMPInputStreamAdapter::readOID(io::SeekableInputStream& is, int32_t len, bool& errorFlag) {
		uint8_t buf[1024] = { 0 };
		is.read(buf, len);

		int32_t offset = 0;
		Int32Vector oid;

		// pierwszy kodowany inaczej
		if (true) {
			uint8_t firstTwo = buf[offset++];
			oid.push_back(firstTwo / 40);
			oid.push_back(firstTwo % 40);
		}

		while(offset < len) {
			uint32_t e = 0;

			while(offset < len) {
				uint8_t b = buf[offset++];
				e <<= 7;
				e |= b & 0x7F;
				if ((b & 0x80) == 0x00) break;
			}

			oid.push_back(e);
		}

		std::stringstream ss;
		for(auto& e: oid) {
			ss << "." << e;
		}
		return OID(ss.str());
	}

	// ************************************************************************************
	Value SNMPInputStreamAdapter::read(io::SeekableInputStream& is, bool& errorFlag) {
		uint8_t type = is.readPrimitive<uint8_t>();
		uint32_t len = readHeaderLength(is, errorFlag);

		//g_logger.debug(stdext::format("[SNMPInputStreamAdapter::read] type=%02X len=%d", type, len));

		if (type == ValueType::NULL_) {
			if (len > 0) {
				is.read(nullptr, len);
			}
			return Value::createNull();
		}
		if (type == ValueType::INTEGER) {
			return Value::createInt(readInt32(is, len, errorFlag));
		}
		if (type == ValueType::COUNTER32) {
			return Value::createCounter32(readUInt32(is, len, errorFlag));
		}
		if (type == ValueType::COUNTER64) {
			return Value::createCounter64(readUInt64(is, len, errorFlag));
		}
		if (type == ValueType::GAUGE32) {
			return Value::createGauge32(readUInt32(is, len, errorFlag));
		}
		if (type == ValueType::TIMETICKS) {
			return Value::createTimeTicks(readUInt32(is, len, errorFlag));
		}
		if (type == ValueType::IPADDR) {
			in_addr addr = { 0 };
			addr.s_addr = readIPAddress(is, len, errorFlag);
			return Value::createIPAddr(inet_ntoa(addr));
		}
		if (type == ValueType::STRING) {
			return Value::createString(is.readString(len));
		}
		if (type == ValueType::OID) {
			return Value::createOID(readOID(is, len, errorFlag));
		}

		if (type == ValueType::SEQUENCE || ValueType::isPDU((ValueType::Enum)type)) {
			size_t end = is.tell() + len;
			std::vector<Value> vec;

			while(is.tell() < end) {
				auto val = read(is, errorFlag);
				vec.push_back(val);
			}

			return Value::createSequence(vec, (ValueType::Enum)type);
		}
		if (type == 0x80) {
			// no-such-object
			return Value::createNull();
		}
		if (type == 0x81) {
			// no-such-instance
			return Value::createNull();
		}
		if (type == 0x82) {
			// end of mib view
			return Value::createNull();
		}

		g_logger.warning(stdext::format("Unknown value type %02X", type));
		errorFlag = true;
		return Value::createNull();
	}



	// ************************************************************************************
	void SNMPOutputStreamAdapter::writeEncodedInt(int32_t v) {
		if (v > 127) {
			uint32_t val = v;
			uint8_t toWrite[32] = { 0 };
			int32_t toWriteLen = 0;

			while(val != 0) {
				uint8_t b = val & 0xFF;
				if ((b & 0x80) != 0) {
					b = b & 0x7F;
				}
				val >>= 7;
				toWrite[toWriteLen++] = b;
			}

			for(int32_t i=toWriteLen-1;i>=0;--i) {
				if (i > 0) {
					m_os.writePrimitive<uint8_t>(toWrite[i] | 0x80);
				} else {
					m_os.writePrimitive<uint8_t>(toWrite[i]);
				}
			}
		} else {
			m_os.writePrimitive<uint8_t>(v);
		}
	}


	// ************************************************************************************
	void SNMPOutputStreamAdapter::writeString(const std::string& str) {
		m_os.writePrimitive<uint8_t>(ValueType::STRING);
		m_os.writePrimitive<uint8_t>(str.length());
		m_os.write(str.c_str(), str.length());
	}

	// ************************************************************************************
	void SNMPOutputStreamAdapter::writeInt8(int64_t v) {
		m_os.writePrimitive<uint8_t>(ValueType::INTEGER);
		m_os.writePrimitive<uint8_t>(1);
		m_os.writePrimitive<uint8_t>(v);
	}

	// ************************************************************************************
	void SNMPOutputStreamAdapter::writeInt16(int64_t v) {
		uint8_t* data = (uint8_t*)&v;

		m_os.writePrimitive<uint8_t>(ValueType::INTEGER);
		m_os.writePrimitive<uint8_t>(2);

		m_os.writePrimitive<uint8_t>(data[1]);
		m_os.writePrimitive<uint8_t>(data[0]);
	}

	// ************************************************************************************
	void SNMPOutputStreamAdapter::writeInt32(int64_t v) {
		uint8_t* data = (uint8_t*)&v;

		m_os.writePrimitive<uint8_t>(ValueType::INTEGER);
		m_os.writePrimitive<uint8_t>(4);

		m_os.writePrimitive<uint8_t>(data[3]);
		m_os.writePrimitive<uint8_t>(data[2]);
		m_os.writePrimitive<uint8_t>(data[1]);
		m_os.writePrimitive<uint8_t>(data[0]);
	}

	// ************************************************************************************
	void SNMPOutputStreamAdapter::writeInt64(uint64_t v) {
		uint8_t* data = (uint8_t*)&v;

		m_os.writePrimitive<uint8_t>(ValueType::INTEGER);
		m_os.writePrimitive<uint8_t>(8);

		m_os.writePrimitive<uint8_t>(data[7]);
		m_os.writePrimitive<uint8_t>(data[6]);
		m_os.writePrimitive<uint8_t>(data[5]);
		m_os.writePrimitive<uint8_t>(data[4]);
		m_os.writePrimitive<uint8_t>(data[3]);
		m_os.writePrimitive<uint8_t>(data[2]);
		m_os.writePrimitive<uint8_t>(data[1]);
		m_os.writePrimitive<uint8_t>(data[0]);
	}


	// ************************************************************************************
	bool SNMPOutputStreamAdapter::writeOID(const OID& oid) {
		if (oid.empty()) return false;

		size_t idx = 0;
		uint8_t toWrite[64] = { 0 };
		int32_t toWriteLen = 0;

		if (true) {
			int32_t v1 = oid[idx++];
			int32_t v2 = oid[idx++];
			toWrite[toWriteLen++] = v1 * 40 + v2;
		}

		while(idx < oid.size()) {
			int32_t v = oid[idx++];

			if (v <= 127) {
				toWrite[toWriteLen++] = v;
			} else {
				uint8_t tmp[32] = { 0 };
				int32_t tmpLen = 0;

				while(v != 0) {
					tmp[tmpLen++] = v & 0x7F;
					v >>= 7;
				}

				for(int32_t i = tmpLen - 1; i >= 0; i--) {
					if (i > 0) {
						toWrite[toWriteLen++] = tmp[i] | 0x80;
					} else {
						toWrite[toWriteLen++] = tmp[i];
					}
				}
			}
		}

		m_os.writePrimitive<uint8_t>(ValueType::OID);
		m_os.writePrimitive<uint8_t>(toWriteLen);
		for(int32_t i=0;i<toWriteLen;++i) {
			m_os.writePrimitive<uint8_t>(toWrite[i]);
		}

		return true;
	}

	// ************************************************************************************
	void SNMPOutputStreamAdapter::writeZeroLen(ValueType::Enum type) {
		m_os.writePrimitive<uint8_t>(static_cast<uint8_t>(type));
		m_os.writePrimitive<uint8_t>(0);
	}

	// ************************************************************************************
	void SNMPOutputStreamAdapter::writeSeq(ValueType::Enum type, const std::function<void()>& func) {
		m_os.writePrimitive<uint8_t>(type);

		size_t lenPos = m_os.tell();

		// tutaj zapisujey dlugosc jako [len | 0x80, len[0], len[1]]
		// zakladamy, ze nigdy nie bedzie wiecej niz 65535 wiec raczej ok
		m_os.writePrimitive<uint8_t>(0);
		m_os.writePrimitive<uint8_t>(0);
		m_os.writePrimitive<uint8_t>(0);

		func();

		size_t pos = m_os.tell();
		m_os.seek(lenPos);

		size_t len = pos - lenPos - 3;
		m_os.writePrimitive<uint8_t>(0x80 | 2);
		m_os.writePrimitive<uint8_t>((len >> 8) & 0xFF);
		m_os.writePrimitive<uint8_t>(len & 0xFF);
		m_os.seek(pos);
	}

	// ************************************************************************************
	bool SNMPOutputStreamAdapter::writeValue(const Value& value) {
		if (value.type() == ValueType::COUNTER32) {
			writeInt32(value.valueInt());
			return true;
		}
		if (value.type() == ValueType::COUNTER64) {
			writeInt64(value.valueInt());
			return true;
		}
		if (value.type() == ValueType::GAUGE32) {
			writeInt32(value.valueInt());
			return true;
		}
		if (value.type() == ValueType::INTEGER) {
			writeInt32(value.valueInt());
			return true;
		}
		if (value.type() == ValueType::IPADDR) {
			// TODO
			return true;
		}
		if (value.type() == ValueType::NULL_) {
			writeNull();
			return true;
		}
		if (value.type() == ValueType::END_OF_MIB_VIEW) {
			writeZeroLen(ValueType::END_OF_MIB_VIEW);
			return true;
		}
		if (value.type() == ValueType::OID) {
			writeOID(value.valueOID());
			return true;
		}
		if (value.type() == ValueType::SEQUENCE || ValueType::isPDU(value.type())) {
			writeSeq(value.type(), [&]() {
				for(auto& e: value.valueVec()) {
					writeValue(e);
				}
			});
			return true;
		}
		if (value.type() == ValueType::STRING) {
			writeString(value.valueString());
			return true;
		}
		if (value.type() == ValueType::TIMETICKS) {
			writeInt32(value.valueInt());
			return true;
		}
		return false;
	}



} }
