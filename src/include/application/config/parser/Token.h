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

#ifndef INCLUDE_APPLICATION_CONFIG_TOKEN_H_
#define INCLUDE_APPLICATION_CONFIG_TOKEN_H_

#include <base.h>

namespace application { namespace config { namespace parser {

	class TokenType {
		public:

			typedef enum {
				TOKEN_ID = 1,

				TOKEN_STRING = 10,
				TOKEN_NUMBER = 11,

				TOKEN_BLOCK_OPEN = 20, // {
				TOKEN_BLOCK_CLOSE = 21, // }

				TOKEN_SEMICOLON = 70, // ;
				TOKEN_COLON = 71, // :
				TOKEN_COMMA = 72, // ,


				TOKEN_END = 255
			} Enum;

			static std::string toString(Enum type);

		private:
			TokenType() { }

	};


	class Token {
		public:
			Token(int line, TokenType::Enum type) : m_line(line), m_type(type), m_data("") { };
			Token(int line, TokenType::Enum type, const std::string& data) : m_line(line), m_type(type), m_data(data) { };

			~Token() { }
			std::string toString() const;


			int32_t line() const { return m_line; }
			TokenType::Enum type() const { return m_type; }

			const std::string& asString() const { return m_data; }
			const int32_t asInt() const { return atoi(m_data.c_str()); }
			const float asFloat() const { return atof(m_data.c_str()); }

		private:
			int32_t m_line;
			TokenType::Enum m_type;
			std::string m_data;
	};

} } }


#endif /* INCLUDE_APPLICATION_CONFIG_TOKEN_H_ */
