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

#ifndef INCLUDE_APPLICATION_SNMP_STREAMS_H_
#define INCLUDE_APPLICATION_SNMP_STREAMS_H_

#include "base.h"
#include <io/streams.h>
#include <functional>
#include "Value.h"

namespace application { namespace snmp {


	class SNMPInputStreamAdapter {
		public:

			static OID readOID(io::SeekableInputStream& is, int32_t len, bool& errorFlag);

			static int32_t readHeaderLength(io::SeekableInputStream& is, bool& errorFlag);
			static int32_t readInt32(io::SeekableInputStream& is, int32_t len, bool& errorFlag);
			static uint32_t readUInt32(io::SeekableInputStream& is, int32_t len, bool& errorFlag);
			static uint64_t readUInt64(io::SeekableInputStream& is, int32_t len, bool& errorFlag);
			static uint32_t readIPAddress(io::SeekableInputStream& is, int32_t len, bool& errorFlag);


			static Value read(io::SeekableInputStream& is, bool& errorFlag);

		private:
			SNMPInputStreamAdapter() { }

	};

	class SNMPOutputStreamAdapter {
		public:
			SNMPOutputStreamAdapter(io::SeekableOutputStream& os) : m_os(os) { }

			void writeEncodedInt(int32_t v);

			void writeString(const std::string& str);
			void writeInt8(int64_t v);
			void writeInt16(int64_t v);
			void writeInt32(int64_t v);
			void writeInt64(uint64_t v);
			bool writeOID(const OID& oid);
			void writeZeroLen(ValueType::Enum type);
			void writeNull() { writeZeroLen(ValueType::NULL_); }
			void writeSeq(ValueType::Enum type, const std::function<void()>& func);
			bool writeValue(const Value& value);

		private:
			io::SeekableOutputStream& m_os;
	};


} }

#endif /* INCLUDE_APPLICATION_SNMP_STREAMS_H_ */
