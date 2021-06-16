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

#include "ConfigEntry.h"
#include "ConfigParserException.h"

namespace application { namespace config { namespace parser {

	// ************************************************************************************
	const std::string& ConfigEntry::valuePrimitive(int32_t nr) {
		if (hasValuePrimitive(nr)) {
			return value(nr)->primitive();
		} else {
			static std::string empty;
			return empty;
		}
	}

	// ************************************************************************************
	const ConfigEntriesCollection& ConfigEntry::valueBlock(int32_t nr) {
		if (hasValueBlock(nr)) {
			return value(nr)->block();
		} else {
			static ConfigEntriesCollection empty;
			return empty;
		}
	}

	// ************************************************************************************
	int32_t ConfigEntry::valueInt(int32_t nr) {
		if (hasValueInt(nr)) {
			return stdext::unsafeCast<int32_t>(valuePrimitive(nr));
		} else {
			return 0;
		}
	}


	// ************************************************************************************
	ConfigEntryPtr ConfigEntriesCollection::entry(const std::string& name) {
		for(auto& entry: m_entries) {
			if (entry->name() == name) return entry;
		}
		return nullptr;
	}

	// ************************************************************************************
	ConfigEntriesCollection::container ConfigEntriesCollection::entries(const std::string& name) {
		container res;

		for(auto& entry: m_entries) {
			if (entry->name() == name) {
				res.push_back(entry);
			}
		}

		return res;
	}

	// ************************************************************************************
	ConfigEntryPtr ConfigEntriesCollection::parseEntry(TokensStream& tokens) {
		if (!tokens.matches(TokenType::TOKEN_ID)) return nullptr;
		tokens.mark();

		ConfigEntryPtr entry(new ConfigEntry(tokens.nextToken().asString()));

		while(true) {

			if (tokens.matches(TokenType::TOKEN_SEMICOLON)) {
				// znaczy ze koniec tego wpisu
				tokens.nextToken();
				tokens.cancelMark();
				break;
			}

			if (tokens.matches(TokenType::TOKEN_NUMBER)) {
				auto val = tokens.nextToken().asString();
				entry->values().push_back(ConfigEntryValuePtr(new ConfigEntryValue(val)));
				continue;
			}

			if (tokens.matches(TokenType::TOKEN_STRING)) {
				auto val = tokens.nextToken().asString();
				entry->values().push_back(ConfigEntryValuePtr(new ConfigEntryValue(val)));
				continue;
			}

			if (tokens.matches(TokenType::TOKEN_ID)) {
				auto val = tokens.nextToken().asString();
				entry->values().push_back(ConfigEntryValuePtr(new ConfigEntryValue(val)));
				continue;
			}

			if (tokens.matches(TokenType::TOKEN_BLOCK_OPEN)) {
				tokens.nextToken();

				auto val = ConfigEntriesCollection::Parse(tokens);

				if (tokens.matches(TokenType::TOKEN_BLOCK_CLOSE)) {
					tokens.nextToken();
					entry->values().push_back(ConfigEntryValuePtr(new ConfigEntryValue(val)));
					continue;
				} else {
					int32_t line = tokens.currentSourceLine();
					tokens.back();
					throw ConfigParserException(line,"Expecting }");
				}
			}

			throw ConfigParserException(tokens.peekToken().line(),"Invalid token");
		}

		return entry;
	}

	// ************************************************************************************
	ConfigEntriesCollection ConfigEntriesCollection::Parse(TokensStream& tokens) {
		ConfigEntriesCollection res;

		while(true) {
			auto entry = parseEntry(tokens);
			if (!entry.empty()) {
				res.m_entries.push_back(entry);
				continue;
			} else {
				break;
			}
		}

		return res;
	}

} } }
