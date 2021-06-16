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

#ifndef INCLUDE_APPLICATION_CONFIG_CONFIGEXCEPTION_H_
#define INCLUDE_APPLICATION_CONFIG_CONFIGEXCEPTION_H_

#include <base.h>

namespace application { namespace config { namespace parser {

	class ConfigParserException: public std::exception {
		public:
			ConfigParserException(int32_t line, const std::string& message) : m_line(line), m_message(message) { }
			ConfigParserException(const std::string& message) : m_line(0), m_message(message) { }

			virtual ~ConfigParserException() throw() { }

			virtual const char* what() const _GLIBCXX_USE_NOEXCEPT { return m_message.c_str(); }
			int32_t line() const { return m_line; }

		private:
			int32_t m_line;
			std::string m_message;
	};

} } }

#endif /* INCLUDE_APPLICATION_CONFIG_CONFIGEXCEPTION_H_ */
