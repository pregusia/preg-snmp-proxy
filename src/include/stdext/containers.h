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
 *
 */

#ifndef INCLUDE_STDEXT_CONTAINERS_H_
#define INCLUDE_STDEXT_CONTAINERS_H_

#include <vector>
#include <list>
#include <cassert>
#include <string>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <set>
#include <algorithm>

namespace stdext {

	template<typename T,int SIZE>
	class static_vector {
		public:

			typedef T value_type;
			typedef std::size_t size_type;

			static_vector() : m_num(0) { }
			static_vector(const T& def) : m_num(0) {
				for(size_type i=0;i<SIZE;++i) m_data[i] = def;
			}
			~static_vector() { }

			void push_back(const T& v) { assert(m_num < SIZE); m_data[m_num++] = v; }
			size_type size() const { return m_num; }
			void clear() { m_num = 0; }

			T& operator[](size_type idx) { return m_data[idx]; }
			const T& operator[](size_type idx) const { return m_data[idx]; }

		private:

			T m_data[SIZE];
			size_type m_num;

	};

	template<typename T>
	class chance_container {
		public:
			class entry {
				public:
					T value;
					int32_t chance;
					entry(const T& value, int32_t chance) : value(value), chance(chance) { }
			};

			typedef T value_type;
			typedef std::size_t size_type;
			typedef typename std::vector<entry>::iterator iterator;
			typedef typename std::vector<entry>::const_iterator const_iterator;

			chance_container() { }
			~chance_container() { }

			bool empty() const { return m_entries.empty(); }
			void clear() { m_entries.clear(); }

			iterator begin() { return m_entries.begin(); }
			const_iterator begin() const { return m_entries.begin(); }
			iterator end() { return m_entries.end(); }
			const_iterator end() const { return m_entries.end(); }

			size_type size() const { return m_entries.size(); }

			void push(const T& value, int32_t chance) {
				m_entries.push_back(entry(value, chance));
			}

			bool contains(const T& val) const {
				for(auto& e: m_entries) {
					if (e.value == val) return true;
				}
				return false;
			}

			bool random(T& out) {
				if (empty()) return false;

				int32_t rnd = stdext::randomRange(0L, totalChance());
				int32_t v = 0;

				for(auto& e: m_entries) {
					if (rnd >= v && rnd <= v + e.chance) {
						out = e.value;
						return true;
					}
					v += e.chance;
				}
				return false;
			}

			int32_t totalChance() {
				int32_t v = 0;
				for(auto& e: m_entries) v += e.chance;
				return v;
			}

		private:
			std::vector<entry> m_entries;

	};


	template<typename C>
	struct container_utils;

	template<typename T>
	struct container_utils<std::set<T>> {
		static void insert(std::set<T>& arr, const T& e) { arr.insert(e); }
		static bool contains(const std::set<T>& arr, const T& e) { return arr.find(e) != arr.end(); }
	};

	template<typename T>
	struct container_utils<std::vector<T>> {
		static void insert(std::vector<T>& arr, const T& e) { arr.push_back(e); }
		static bool contains(const std::vector<T>& arr, const T& e) { return std::find(arr.begin(), arr.end(), e) != arr.end(); }
	};

	template<typename T>
	struct container_utils<std::list<T>> {
		static void insert(std::list<T>& arr, const T& e) { arr.push_back(e); }
		static bool contains(const std::list<T>& arr, const T& e) { return std::find(arr.begin(), arr.end(), e) != arr.end(); }
	};

	template<typename T>
	struct container_utils<boost::container::flat_set<T>> {
		static void insert(boost::container::flat_set<T>& arr, const T& e) { arr.insert(e); }
		static bool contains(const boost::container::flat_set<T>& arr, const T& e) { return arr.find(e) != arr.end(); }
	};


	template<typename C>
	class intersection_results {
		public:
			C removed;
			C added;
			C same;
	};

	template<typename C>
	intersection_results<C> intersect(const C& a, const C& b) {
		intersection_results<C> res;

		for(auto& e: a) {
			if (!container_utils<C>::contains(b, e)) {
				container_utils<C>::insert(res.removed, e);
			} else {
				container_utils<C>::insert(res.same, e);
			}
		}

		for(auto& e: b) {
			if (!container_utils<C>::contains(a, e)) {
				container_utils<C>::insert(res.added, e);
			}
		}

		return res;
	}

	template<typename C, typename T>
	void insert_if_not_exists(C& arr, const T& val) {
		if (!container_utils<C>::contains(arr, val)) {
			container_utils<C>::insert(arr, val);
		}
	}

	template<typename T, int N>
	class aggregation_container {
		public:
			aggregation_container() {
				m_addPos = 0;
				m_size = 0;
			}

			void add(const T& val) {
				m_values[m_addPos] = val;
				m_addPos += 1;
				if (m_addPos >= N) m_addPos = 0;
				m_size += 1;
				if (m_size >= N) m_size = N;
			}

			T sum() const {
				T res = T();
				for(int32_t i=0;i<m_size;++i) res += m_values[i];
				return res;
			}

			T min(T def=10000) const {
				T res = def;
				for(int32_t i=0;i<m_size;++i) {
					if (m_values[i] < res) res = m_values[i];
				}
				return res;
			}

			T max(T def=-10000) const {
				T res = def;
				for(int32_t i=0;i<m_size;++i) {
					if (m_values[i] > res) res = m_values[i];
				}
				return res;
			}

			float avg() const {
				if (m_size == 0) return 0.0f;
				return sum() / static_cast<float>(m_size);
			}

		private:
			T m_values[N];
			int32_t m_addPos;
			int32_t m_size;
	};

}

#endif /* INCLUDE_STDEXT_CONTAINERS_H_ */
