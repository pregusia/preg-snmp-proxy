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


#include "eventdispatcher.h"
#include "clock.h"

core::EventDispatcher g_dispatcher;

namespace core {

	// ************************************************************************************
	EventDispatcher::EventDispatcher() {
		m_pollsWithoutExecuting = 0;
		m_pollEventsSize = 0;
		m_syncEventNum = 0;
	}

	// ************************************************************************************
	void EventDispatcher::shutdown() {
	    while(!m_eventList.empty()) poll(false);

	    while(!m_scheduledEventList.empty()) {
			ScheduledEventPtr scheduledEvent = m_scheduledEventList.top();
			scheduledEvent->cancel();
			m_scheduledEventList.pop();
	    }
	    m_disabled = true;
	}

	// ************************************************************************************
	void EventDispatcher::poll(bool useSleep) {
		// pump out synchronized events
		if (m_syncEventNum > 0) {
			m_syncEventsMutex.lock();
			for(auto& e: m_syncEvents) {
				addEvent(e);
			}
			m_syncEvents.clear();
			m_syncEventNum = 0;
			m_syncEventsMutex.unlock();
		}


	    for(int count = 0, max = m_scheduledEventList.size(); count < max && !m_scheduledEventList.empty(); ++count) {
	    	ScheduledEventPtr scheduledEvent = m_scheduledEventList.top();
	    	if(scheduledEvent->remainingTicks() > 0) break;

	    	m_scheduledEventList.pop();
	    	scheduledEvent->execute();

	    	if(scheduledEvent->nextCycle()) {
	    		m_scheduledEventList.push(scheduledEvent);
	    	}
	    }

	    // execute events list until all events are out
	    m_pollEventsSize = m_eventList.size();
	    if (m_pollEventsSize == 0) {
	    	m_pollsWithoutExecuting += 1;
	    } else {
	    	m_pollsWithoutExecuting = 0;
	    }

	    stdext::RawTimer reportTimer;
	    while(m_pollEventsSize > 0) {
			if(reportTimer.millisElapsed() > 300) {
				g_logger.error("ATTENTION the event list is not getting empty, this could be caused by some bad code");
				reportTimer.restart();
			}

			for(int i=0;i<m_pollEventsSize;++i) {
				EventPtr event = m_eventList.front();
				m_eventList.pop_front();
				event->execute();
			}
			m_pollEventsSize = m_eventList.size();
	    }

	    if (m_pollsWithoutExecuting > 100) {
	    	m_pollsWithoutExecuting = 200;
	    	if (useSleep) {
	    		stdext::Time::microsleep(1000);
	    	}
	    }
	}

	// ************************************************************************************
	ScheduledEventPtr EventDispatcher::scheduleEvent(const std::function<void()>& callback, int delay) {
	    if(m_disabled) return ScheduledEventPtr(new ScheduledEvent(nullptr, delay, 1));

	    assert(delay >= 0);
	    ScheduledEventPtr scheduledEvent(new ScheduledEvent(callback, delay, 1));
	    m_scheduledEventList.push(scheduledEvent);
	    return scheduledEvent;
	}

	// ************************************************************************************
	ScheduledEventPtr EventDispatcher::cycleEvent(const std::function<void()>& callback, int delay) {
	    if(m_disabled) return ScheduledEventPtr(new ScheduledEvent(nullptr, delay, 0));

	    assert(delay > 0);
	    ScheduledEventPtr scheduledEvent(new ScheduledEvent(callback, delay, 0));
	    m_scheduledEventList.push(scheduledEvent);
	    return scheduledEvent;
	}

	// ************************************************************************************
	EventPtr EventDispatcher::addEvent(const std::function<void()>& callback, bool pushFront) {
	    if(m_disabled) return EventPtr(new Event(nullptr));

	    EventPtr event(new Event(callback));
	    // front pushing is a way to execute an event before others
	    if(pushFront) {
			m_eventList.push_front(event);
			// the poll event list only grows when pushing into front
			m_pollEventsSize++;
	    } else {
	    	m_eventList.push_back(event);
	    }
	    return event;
	}

	// ************************************************************************************
	void EventDispatcher::pushEventSynchronized(const std::function<void()>& func) {
		m_syncEventsMutex.lock();
		m_syncEvents.push_back(func);
		m_syncEventNum += 1;
		m_syncEventsMutex.unlock();
	}


}

