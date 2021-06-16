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

#include "signals.h"
#include <core/eventdispatcher.h>

#include <signal.h>
#include <wait.h>

io::Signals g_unixSignals;

namespace io {

	// ************************************************************************************
	Signals::Signals() {
		clear();
		m_anyRaised = false;
	}

	// ************************************************************************************
	void Signals::start() {
		registerSignals();
	}

	// ************************************************************************************
	void Signals::clear() {
		for(int32_t i=0;i<SIGS_NUM;++i) {
			m_raisedSignals[i] = false;
		}
		for(int32_t i=0;i<CHILD_SLOTS;++i) {
			m_childExitCodes[i] = -1;
			m_childPids[i] = -1;
		}
	}

	// ************************************************************************************
	void Signals::poll() {
		if (m_anyRaised) {
			m_anyRaised = false;

			for(int32_t sig=0;sig<SIGS_NUM;++sig) {
				if (m_raisedSignals[sig]) {
					auto handlerIt = m_signalHandlers.find(sig);
					if (handlerIt != m_signalHandlers.end()) {
						g_dispatcher.addEvent(handlerIt->second);
					}
				}
			}

			for(int32_t i=0;i<CHILD_SLOTS;++i) {
				if (m_childPids[i] >= 0) {
					int32_t pid = m_childPids[i];
					int32_t exitCode = m_childExitCodes[i];

					auto handlerIt = m_childrenHandlers.find(pid);
					if (handlerIt != m_childrenHandlers.end()) {
						auto fn = handlerIt->second;
						g_dispatcher.addEvent([=](){
							fn(pid, exitCode);
						});
						m_childrenHandlers.erase(handlerIt);
					}
				}
			}

			clear();
		}
	}

	// ************************************************************************************
	void Signals::registerSignalHandler(int32_t sig, const SignalHandlerCallback& callback) {
		m_signalHandlers[sig] = callback;
	}

	// ************************************************************************************
	void Signals::registerChildEndCallback(int32_t pid, const ChildEndedCallback& callback) {
		m_childrenHandlers[pid] = callback;
	}

	// ************************************************************************************
	void Signals::onSignal(int32_t sig) {
		if (sig == SIGCHLD) {

			while(true) {
				int32_t status = 0;
				pid_t pid = ::waitpid(-1, &status, WNOHANG);
				if (pid <= 0) break;

				for(int32_t i=0;i<CHILD_SLOTS;++i) {
					if (m_childPids[i] < 0) {
						m_childPids[i] = pid;
						m_childExitCodes[i] = WEXITSTATUS(status);
						m_anyRaised = true;
						break;
					}
				}
			}

		} else {
			m_raisedSignals[sig] = true;
			m_anyRaised = true;
		}
	}

	// ************************************************************************************
	void Signals::onSignalStatic(int32_t sig) {
		g_unixSignals.onSignal(sig);
		registerSignals();
	}


	// ************************************************************************************
	void Signals::registerSignals() {
		signal(SIGUSR1, Signals::onSignalStatic);
		signal(SIGTERM, Signals::onSignalStatic);
		signal(SIGABRT, Signals::onSignalStatic);
		signal(SIGINT, Signals::onSignalStatic);
		signal(SIGPIPE, Signals::onSignalStatic);
		signal(SIGCHLD, Signals::onSignalStatic);
	}

}
