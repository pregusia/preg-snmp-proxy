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


#include "clock.h"

core::Clock g_clock;

namespace core {

	// ************************************************************************************
	Clock::Clock() {
	    m_currentMicros = 0;
	    m_currentMillis = 0;
	    m_currentSeconds = 0;
	    m_unixTime = 0;
	}

	// ************************************************************************************
	void Clock::update() {
	    m_currentMicros = stdext::Time::micros();
	    m_currentMillis = m_currentMicros / 1000;
	    m_currentSeconds = m_currentMicros / 1000000.0f;
	    m_unixTime = stdext::Time::seconds();
	}
	
	// ************************************************************************************
	WallTime Clock::utcTime() {
		time_t now = ::time(nullptr);
		auto tm = gmtime(&now);

		WallTime res;
		res.day = tm->tm_mday;
		res.hours = tm->tm_hour;
		res.minutes = tm->tm_min;
		res.month = tm->tm_mon;
		res.seconds = tm->tm_sec;
		res.year = 1900 + tm->tm_year;
		return res;
	}

	// ************************************************************************************
	WallTime Clock::localTime() {
		time_t now = ::time(nullptr);
		auto tm = localtime(&now);

		WallTime res;
		res.day = tm->tm_mday;
		res.hours = tm->tm_hour;
		res.minutes = tm->tm_min;
		res.month = tm->tm_mon;
		res.seconds = tm->tm_sec;
		res.year = 1900 + tm->tm_year;
		return res;
	}




	// ************************************************************************************
	ticks_t ClockTimer::millisElapsed() const {
		return (g_clock.micros() - m_start) / 1000;
	}

	// ************************************************************************************
	ticks_t ClockTimer::microsElapsed() const {
		return g_clock.micros() - m_start;
	}

	// ************************************************************************************
	float ClockTimer::secondsElapsed() const {
		return (float)(g_clock.micros() - m_start) / 1000000.0f;
	}

	// ************************************************************************************
	void ClockTimer::restart() {
		m_start = g_clock.micros();
	}



}

