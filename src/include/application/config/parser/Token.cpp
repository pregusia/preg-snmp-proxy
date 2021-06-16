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

#include "Token.h"

namespace application { namespace config { namespace parser {

	// ************************************************************************************
	std::string TokenType::toString(Enum type) {
		switch(type) {
			case TOKEN_BLOCK_CLOSE: return "BLOCK_CLOSE";
			case TOKEN_BLOCK_OPEN: return "BLOCK_OPEN";
			case TOKEN_COLON: return "COLON";
			case TOKEN_COMMA: return "COMMA";
			case TOKEN_ID: return "ID";
			case TOKEN_SEMICOLON: return "SEMICOLON";
			case TOKEN_STRING: return "STRING";
			case TOKEN_NUMBER: return "NUMBER";
			default: return stdext::format("Unknown type %d", type);
		}
	}

	// ************************************************************************************
	std::string Token::toString() const {
		if (m_data.empty()) {
			return stdext::format("[Token type=%s line=%d]", m_type, m_line);
		} else {
			return stdext::format("[Token type=%s line=%d data=\"%s\"]", m_type, m_line, m_data);
		}
	}



} } }
