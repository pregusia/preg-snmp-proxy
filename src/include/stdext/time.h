/*
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
 * From OTClient OTClient <https://github.com/edubart/otclient>
 * Modded by pregusia
 *
 */

#ifndef INCLUDE_STDEXT_TIME_H_
#define INCLUDE_STDEXT_TIME_H_

#include "types.h"
#include <string>

namespace stdext {

	class Time {
		private:
			Time() { }

		public:
			static ticks_t seconds();
			static ticks_t millis();
			static ticks_t micros();

			static void millisleep(size_t ms);
			static void microsleep(size_t us);

			static std::string asString(const char* format=nullptr);
			static std::string asStringWithMilis();
	};

	class RawTimer {
		public:
			RawTimer() { restart(); }

			ticks_t millisElapsed() const { return (Time::micros() - m_start) / 1000; }
			ticks_t microsElapsed() const { return Time::micros() - m_start; }
			float secondsElapsed() const { return (float)(Time::micros() - m_start) / 1000000.0f; }

			void restart() { m_start = Time::micros(); }

		private:
			ticks_t m_start;
	};

} /* namespace core */

#endif /* INCLUDE_CORE_TIME_H_ */
