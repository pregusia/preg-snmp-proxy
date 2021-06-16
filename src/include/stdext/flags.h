/*
 * flags.h
 *
 *  Created on: 4 gru 2015
 *      Author: pregusia
 */

#ifndef INCLUDE_STDEXT_FLAGS_H_
#define INCLUDE_STDEXT_FLAGS_H_

namespace stdext {

	template<typename T, typename S=uint32_t>
	class flags {
		public:
			typedef typename T::Enum Enum;

			flags(S v=0) : value(v) { }

			bool has(Enum e) const { return (value & (S)e) != 0; }
			void set(Enum e) { value |= (S)e; }
			void unset(Enum e) { value &= ~(S)e; }
			void clear() { value = 0; }
			bool empty() const { return value == 0; }

			void toggle(Enum e) {
				if (has(e)) {
					unset(e);
				} else {
					set(e);
				}
			}

			S value;
	};

}

#endif /* INCLUDE_STDEXT_FLAGS_H_ */
