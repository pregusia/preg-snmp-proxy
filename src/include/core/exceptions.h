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


#ifndef INCLUDE_CORE_EXCEPTIONS_H_
#define INCLUDE_CORE_EXCEPTIONS_H_

#include <base.h>
#include <exception>

namespace core {

	class Exception: public std::exception {
		public:
			Exception();
			Exception(const std::string& msg);
			virtual ~Exception() throw() { };

			virtual const char* what() const throw() { return m_message.c_str(); }
			const std::string& message() const { return m_message; }
			const std::string& trace() const { return m_trace; }

		protected:
			std::string m_message;
			std::string m_trace;
	};

	class CoreException: public Exception {
		public:
			CoreException(const std::string& msg) : Exception(msg) { }
			virtual ~CoreException() throw() { }
	};


} /* namespace core */

#endif /* INCLUDE_CORE_EXCEPTIONS_H_ */
