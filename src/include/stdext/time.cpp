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

#include "time.h"
#include <chrono>

#ifdef _MSC_VER
#include <thread>
#else
#include <unistd.h>
#endif


namespace stdext {

	const static auto startup_time = std::chrono::high_resolution_clock::now();

	ticks_t Time::seconds() { return std::time(NULL); }
	ticks_t Time::millis() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startup_time).count(); }
	ticks_t Time::micros() { return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startup_time).count(); }

	// ************************************************************************************
	void Time::millisleep(size_t ms) {
#ifdef _MSC_VER
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#else
		usleep(ms * 1000);
#endif
	};

	// ************************************************************************************
	void Time::microsleep(size_t us) {
#ifdef _MSC_VER
		std::this_thread::sleep_for(std::chrono::microseconds(us));
#else
		usleep(us);
#endif
	};

	// ************************************************************************************
	std::string Time::asString(const char* format) {
		if (format == nullptr) format = "%b %d %Y %H:%M:%S";
		char date[1024];
		std::time_t tnow;
		std::time(&tnow);
		std::tm *ts = std::localtime(&tnow);
		std::strftime(date, 1024, format, ts);
		return date;
	}

	// ************************************************************************************
	std::string Time::asStringWithMilis() {
		time_t rawtime;
		struct tm *info;
		char buffer[256] = { 0 };

		std::time(&rawtime);
		info = localtime( &rawtime );

		size_t n = strftime(buffer,256,"%d-%m-%Y %H:%M:%S", info);
		sprintf(buffer + n, " +%04d ms", (int)(millis() % 1000));
		return buffer;
	}

} /* namespace core */
