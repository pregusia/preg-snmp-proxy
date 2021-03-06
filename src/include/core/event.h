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


#ifndef EVENT_H
#define EVENT_H

#include <base.h>

namespace core {

	class Event : public stdext::object {
		public:
		    Event(const std::function<void()>& callback);
		    virtual ~Event();

		    virtual void execute();
		    void cancel();

		    bool isCanceled() { return m_canceled; }
		    bool isExecuted() { return m_executed; }

		protected:
		    std::function<void()> m_callback;
		    bool m_canceled;
		    bool m_executed;
	};

	typedef stdext::object_ptr<Event> EventPtr;

}	

#endif
