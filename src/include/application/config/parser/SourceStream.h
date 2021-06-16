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

#ifndef INCLUDE_APPLICATION_CONFIG_SOURCESTREAM_H_
#define INCLUDE_APPLICATION_CONFIG_SOURCESTREAM_H_

#include <base.h>

namespace application { namespace config { namespace parser {

	class SourceStream {
		public:
			SourceStream(const std::string& data) : m_data(data), m_position(0) { }
			~SourceStream() { }

			bool hasNext() const { return m_position < m_data.length() - 1; }

			char next();
			std::string next(int32_t n);

			char peek() const;
			std::string peek(int32_t n) const;

			void skipToLineEnd();
			void skipWhitespaces();

			int32_t currentLine() const;
			int32_t position() const { return m_position; }

			bool matches(const std::string& val) const;

			void mark() { m_markers.push_back(m_position); }
			void back() { if (!m_markers.empty()) { m_position = m_markers.back(); m_markers.pop_back(); } }

			static bool isWhitespace(char c) { return c == '\t' || c == '\n' || c == ' ' || c == '\r'; }
			static bool isAlph(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
			static bool isNum(char c) { return c >= '0' && c <= '9'; }

		private:
			std::string m_data;
			size_t m_position;
			Int32Vector m_markers;
	};

} } }

#endif /* INCLUDE_APPLICATION_CONFIG_SOURCESTREAM_H_ */
