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


#ifndef CLOCK_H
#define CLOCK_H

#include <base.h>

namespace core {

	class WallTime {
		public:
			int32_t seconds;
			int32_t minutes;
			int32_t hours;
			int32_t day;
			int32_t month;
			int32_t year;

			WallTime() {
				seconds = 0;
				minutes = 0;
				hours = 0;
				day = 0;
				month = 0;
				year = 0;
			}
	};

	class Clock {
		public:
			Clock();

			void update();

			ticks_t micros() { return m_currentMicros; }
			ticks_t millis() { return m_currentMillis; }
			float seconds() { return m_currentSeconds; }
			ticks_t time() { return m_unixTime; }
			int32_t normalizedTime() { return m_unixTime - 1468589110UL; }

			WallTime utcTime();
			WallTime localTime();

		private:
			ticks_t m_currentMicros;
			ticks_t m_currentMillis;
			float m_currentSeconds;
			ticks_t m_unixTime;
	};

	class ClockTimer {
		public:
			ClockTimer() { restart(); }

			ticks_t millisElapsed() const;
			ticks_t microsElapsed() const;
			float secondsElapsed() const;

			void restart();

		private:
			ticks_t m_start;
	};

}

extern core::Clock g_clock;

#endif

