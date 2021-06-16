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

#include "TokensStream.h"
#include "ConfigParserException.h"

namespace application { namespace config { namespace parser {

	// ************************************************************************************
	TokensStream::TokensStream() {
		m_position = 0;
	}

	// ************************************************************************************
	int TokensStream::currentSourceLine() const {
		if (m_position >= 0 && m_position < static_cast<int32_t>(m_tokens.size())) {
			return m_tokens[m_position].line();
		} else {
			return 0;
		}
	}

	// ************************************************************************************
	Token TokensStream::nextToken() {
		if (m_position >= 0 && m_position < static_cast<int32_t>(m_tokens.size())) {
			return m_tokens[m_position++];
		} else {
			return Token(0, TokenType::TOKEN_END);
		}
	}

	// ************************************************************************************
	Token TokensStream::peekToken() {
		if (m_position >= 0 && m_position < static_cast<int32_t>(m_tokens.size())) {
			return m_tokens[m_position];
		} else {
			return Token(0, TokenType::TOKEN_END);
		}
	}

	// ************************************************************************************
	bool TokensStream::matchesArr(size_t n, TokenType::Enum types[]) {
		mark();
		for(size_t i=0;i<n;++i) {
			Token t = nextToken();
			if (t.type() != types[i]) {
				back();
				return false;
			}
		}
		back();
		return true;
	}


	// ************************************************************************************
	bool TokensStream::tryNumber(SourceStream& source) {
		std::stringstream ss;

		if (!SourceStream::isNum(source.peek())) return false;

		while(true) {
			char c = source.peek();

			if (c == '.') {
				source.mark();
				source.next();
				if (SourceStream::isNum(source.peek())) {
					ss << ".";
					continue;

				} else {
					source.back();
					m_tokens.push_back(Token(source.currentLine(), TokenType::TOKEN_NUMBER, ss.str()));
					return true;
				}
			}

			if (SourceStream::isNum(c)) {
				ss << c;
				source.next();
			} else {
				m_tokens.push_back(Token(source.currentLine(), TokenType::TOKEN_NUMBER, ss.str()));
				return true;
			}
		}

		return false;
	}

	// ************************************************************************************
	bool TokensStream::tryID(SourceStream& source) {
		std::stringstream ss;
		if (!SourceStream::isAlph(source.peek())) return false;

		while(true) {
			char c = source.peek();
			if (SourceStream::isAlph(c) || SourceStream::isNum(c) || c == '-' || c == '$' || c == '_') {
				ss << c;
				source.next();
			} else {
				m_tokens.push_back(Token(source.currentLine(), TokenType::TOKEN_ID,ss.str()));
				return true;
			}
		}

		return false;
	}

	// ************************************************************************************
	bool TokensStream::tryString(SourceStream& source, const std::string& begin, const std::string& end) {
		if (begin.empty()) return false;
		if (end.empty()) return false;

		std::stringstream ss;
		bool nextIgnore = false;

		if (source.peek(begin.size()) != begin) return false;
		source.next(begin.size());

		// going till next
		while(source.hasNext()) {
			if (!nextIgnore && source.peek(end.size()) == end) {
				source.next(end.size());
				m_tokens.push_back(Token(source.currentLine(), TokenType::TOKEN_STRING, ss.str()));
				return true;
			}


			char ch = source.peek();
			if (ch == '\\' && !nextIgnore) {
				nextIgnore = true;
				source.next();
				continue;
			}
			if (nextIgnore) {
				ss.put(ch);
				nextIgnore = false;
				source.next();
				continue;
			}

			ss.put(ch);
			source.next();
		}

		throw ConfigParserException(source.currentLine(),"String not ended");
	}

	// ************************************************************************************
	TokensStream TokensStream::Tokenize(SourceStream& source) {
		TokensStream tokens;

		while(source.hasNext()) {
			source.skipWhitespaces();
			if (!source.hasNext()) break;

			if (source.matches("//") || source.matches("#")) {
				source.skipToLineEnd();
				continue;
			}

			if (source.matches("{{")) {
				if (tokens.tryString(source, "{{", "}}")) continue;
			}

			if (source.matches("{")) { source.next(); tokens.add(Token(source.currentLine(), TokenType::TOKEN_BLOCK_OPEN)); continue; }
			if (source.matches("}")) { source.next(); tokens.add(Token(source.currentLine(), TokenType::TOKEN_BLOCK_CLOSE)); continue; }

			if (source.matches(":")) { source.next(); tokens.add(Token(source.currentLine(), TokenType::TOKEN_COLON)); continue; }
			if (source.matches(",")) { source.next(); tokens.add(Token(source.currentLine(), TokenType::TOKEN_COMMA)); continue; }
			if (source.matches(";")) { source.next(); tokens.add(Token(source.currentLine(), TokenType::TOKEN_SEMICOLON)); continue; }

			if (source.matches("\"")) {
				if (tokens.tryString(source, "\"", "\"")) continue;
			}

			if (source.matches("'")) {
				if (tokens.tryString(source, "'", "'")) continue;
			}

			if (tokens.tryNumber(source)) continue;
			if (tokens.tryID(source)) continue;

			throw ConfigParserException(source.currentLine(),stdext::format("Found not parsable char %c", source.peek()));
		}

		return tokens;
	}



} } }
