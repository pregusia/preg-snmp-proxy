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

#ifndef INCLUDE_APPLICATION_CONFIG_CONFIGENTRY_H_
#define INCLUDE_APPLICATION_CONFIG_CONFIGENTRY_H_

#include <base.h>
#include "TokensStream.h"

namespace application { namespace config { namespace parser {

	class ConfigEntriesCollection;
	class ConfigEntry;
	typedef stdext::object_ptr<ConfigEntry> ConfigEntryPtr;

	class ConfigEntryValue;
	typedef stdext::object_ptr<ConfigEntryValue> ConfigEntryValuePtr;

	class ConfigEntriesCollection {
		public:
			ConfigEntriesCollection() { }

			typedef std::vector<ConfigEntryPtr> container;
			typedef container::iterator iterator;
			typedef container::const_iterator const_iterator;

			iterator begin() { return m_entries.begin(); }
			iterator end() { return m_entries.end(); }

			const_iterator begin() const { return m_entries.begin(); }
			const_iterator end() const { return m_entries.end(); }

			bool empty() const { return m_entries.empty(); }

			const container& entries() const { return m_entries; }
			container& entries() { return m_entries; }

			ConfigEntryPtr entry(const std::string& name);
			container entries(const std::string& name);

			static ConfigEntriesCollection Parse(TokensStream& tokens);

		private:
			container m_entries;

			static ConfigEntryPtr parseEntry(TokensStream& tokens);
	};

	class ConfigEntryValue: public stdext::object {
		public:
			typedef enum {
				TYPE_NONE,
				TYPE_PRIMITIVE,
				TYPE_BLOCK
			} TypeEnum;

			ConfigEntryValue() : m_type(TYPE_NONE) { }
			ConfigEntryValue(const std::string& value) : m_type(TYPE_PRIMITIVE), m_valuePrimitive(value) { }
			ConfigEntryValue(const ConfigEntriesCollection& value) : m_type(TYPE_BLOCK), m_valueBlock(value) { }
			virtual ~ConfigEntryValue() { }

			TypeEnum type() const { return m_type; }
			bool isPrimitive() const { return m_type == TYPE_PRIMITIVE; }
			bool isBlock() const { return m_type == TYPE_BLOCK; }

			const std::string& primitive() const { return m_valuePrimitive; }
			const ConfigEntriesCollection& block() const { return m_valueBlock; }
			ConfigEntriesCollection& block() { return m_valueBlock; }



		private:
			TypeEnum m_type;
			std::string m_valuePrimitive;
			ConfigEntriesCollection m_valueBlock;
	};

	class ConfigEntry: public stdext::object {
		public:
			ConfigEntry(const std::string& name) : m_name(name) { }
			virtual ~ConfigEntry() { }

			const std::string& name() const { return m_name; }
			ConfigEntryValuePtr value(uint32_t nr) const { return nr >= 0 && nr < m_values.size() ? m_values[nr] : nullptr; }

			const std::vector<ConfigEntryValuePtr>& values() const { return m_values; }
			std::vector<ConfigEntryValuePtr>& values() { return m_values; }

			const std::string& valuePrimitive(int32_t nr=0);
			int32_t valueInt(int32_t nr=0);
			const ConfigEntriesCollection& valueBlock(int32_t nr=0);

			bool hasValue(int32_t nr=0) const { return nr < static_cast<int32_t>(m_values.size()); }
			bool hasValuePrimitive(int32_t nr=0) const { return hasValue(nr) && m_values[nr]->isPrimitive(); }
			bool hasValueInt(int32_t nr=0) const { return hasValuePrimitive(nr) && stdext::isNumeric(m_values[nr]->primitive()); }
			bool hasValueBlock(int32_t nr=0) const { return hasValue(nr) && m_values[nr]->isBlock(); }


		private:
			std::string m_name;
			std::vector<ConfigEntryValuePtr> m_values;

			friend class ConfigEntriesCollection;
	};

} } }

#endif /* INCLUDE_APPLICATION_CONFIG_CONFIGENTRY_H_ */
