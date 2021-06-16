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


#ifndef INCLUDE_STDEXT_HOLDERS_H_
#define INCLUDE_STDEXT_HOLDERS_H_

namespace stdext {

	class tag {
		public:
			static const int SIZE = 64;

			tag() {
				for(int i=0;i<SIZE;++i) m_data[i] = 0;
				m_value = 0;
			}

			int32_t value() const { return m_value; }
			void value(int32_t val) { m_value = val; }

			template<typename T>
			T get() {
				static_assert(sizeof(T) <= sizeof(m_data), "Invalid tag size");
				void* ptr = &m_data;
				T* tmp = static_cast<T*>(ptr);
				return *tmp;
			}

			template<typename T>
			void set(const T& val) {
				static_assert(sizeof(T) <= sizeof(m_data), "Invalid tag size");
				void* ptr = &m_data;
				T* tmp = static_cast<T*>(ptr);
				*tmp = val;
			}

		private:

			int32_t m_value;
			uint8_t m_data[SIZE];

	};

	template<typename T>
	class nullable {
		public:

			nullable() : m_value(), m_exists(false) { }
			nullable(const T& val) : m_value(val), m_exists(true) { }

			void reset() { m_value = T(); m_exists = false; }
			void set(const T& val) { m_value = val; m_exists = true; }

			bool hasValue() const { return m_exists; }
			bool empty() const { return !m_exists; }

			const T& value() const { return m_value; }
			T& value() { return m_value; }

		private:
			T m_value;
			bool m_exists;
	};

	template<typename T1, typename T2>
	class comparable_holder1 {
		public:
			T1 v1;
			T2 v2;

			comparable_holder1() : v1(), v2() { }
			comparable_holder1(const T1& v1, const T2& v2) : v1(v1), v2(v2) { }

			bool operator<(const comparable_holder1<T1,T2>& rhs) const { return v2 < rhs.v2; }
			bool operator<=(const comparable_holder1<T1,T2>& rhs) const { return v2 <= rhs.v2; }
			bool operator>(const comparable_holder1<T1,T2>& rhs) const { return v2 > rhs.v2; }
			bool operator>=(const comparable_holder1<T1,T2>& rhs) const { return v2 >= rhs.v2; }
			bool operator==(const comparable_holder1<T1,T2>& rhs) const { return v2 == rhs.v2; }
			bool operator!=(const comparable_holder1<T1,T2>& rhs) const { return v2 != rhs.v2; }
	};

	template<typename T>
	class range {
		public:
			T min;
			T max;

			range() : min(0), max(-1) { }
			range(const T& min, const T& max) : min(min), max(max) { }
			bool contains(const T& val) const {
				return val >= min && val <= max;
			}

			T size() const { return max - min + 1; }

			T clamp(const T& val) const {
				if (val < min) return min;
				if (val > max) return max;
				return val;
			}

			void updateMinMax(const T& val) {
				if (val < min) min = val;
				if (val > max) max = val;
			}
	};

	template<typename T>
	class value_with_range {
		public:
			value_with_range(const T& min, const T& max) : m_range(min, max), m_current(min) { }

			const T& value() const { return m_current; }
			const stdext::range<T>& range() const { return m_range; }
			const T& min() const { return m_range.min; }
			const T& max() const { return m_range.max; }

			float factor() const {
				return (float)(m_current - m_range.min) / (float)m_range.size();
			}

			value_with_range<T>& set(const T& val) {
				m_current = val;
				if (m_current < m_range.min) m_current = m_range.min;
				if (m_current > m_range.max) m_current = m_range.max;
				return *this;
			}

			value_with_range<T>& change(const T& v) {
				return set(value() + v);
			}

			value_with_range<T> changeRange(const T& min, const T& max) {
				return value_with_range(min, max).set(m_current);
			}

			T operator+(const T& v) const { return m_range.clamp(value() + v); }
			T operator-(const T& v) const { return m_range.clamp(value() - v); }

			value_with_range<T>& operator+=(const T& val) { change(val); return *this; }
			value_with_range<T>& operator-=(const T& val) { change(-val); return *this; }

		private:
			stdext::range<T> m_range;
			T m_current;
	};

	template<typename T>
	class value_object: public object {
		public:
			value_object() : m_value() { }
			value_object(const T& val) : m_value(val) { }
			virtual ~value_object() { }

			const T& get() const { return m_value; }
			void set(const T& val) { m_value = val; }


		private:
			T m_value;
	};

}



#endif /* INCLUDE_STDEXT_HOLDERS_H_ */
