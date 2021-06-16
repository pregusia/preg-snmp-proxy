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


#ifndef LOGGER_H
#define LOGGER_H

#include <base.h>
#include <fstream>
#include <mutex>
#include "exceptions.h"

namespace core {

	ENUM_DEFINE(LogLevel,
		Debug = 1,
		Info = 2,
		Script = 3,
		Warning = 4,
		Error = 5,
		Fatal = 6,
	);


	class Logger
	{
	    enum {
	    	MAX_LOG_HISTORY = 1000
	    };

	public:
	    void log(LogLevel::Enum level, const std::string& message, bool dumpTrace);

	    void debug(const std::string& what, bool dumpTrace=false) { log(LogLevel::Debug, what, dumpTrace); }
	    void info(const std::string& what, bool dumpTrace=false) { log(LogLevel::Info, what, dumpTrace); }
	    void script(const std::string& what, bool dumpTrace=false) { log(LogLevel::Script, what, dumpTrace); }
	    void warning(const std::string& what, bool dumpTrace=false) { log(LogLevel::Warning, what, dumpTrace); }
	    void error(const std::string& what, bool dumpTrace=false) { log(LogLevel::Error, what, dumpTrace); }
	    void fatal(const std::string& what, bool dumpTrace=false) { log(LogLevel::Fatal, what, dumpTrace); }
	    void exception(const std::string& msg, const Exception& e);

	    void setLogFile(const std::string& file);

	private:
	    std::ofstream m_outFile;
	    std::recursive_mutex m_mutex;
	};

}

extern core::Logger g_logger;

#endif
