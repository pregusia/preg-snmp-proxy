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

#ifndef INCLUDE_IO_SIGNALS_H_
#define INCLUDE_IO_SIGNALS_H_

#include <base.h>
#include <functional>

namespace io {

	class Signals {
		public:
			Signals();

			typedef std::function<void()> SignalHandlerCallback;
			typedef std::function<void(int32_t, int32_t)> ChildEndedCallback;

			void start();
			void poll();

			void registerSignalHandler(int32_t sig, const SignalHandlerCallback& callback);
			void registerChildEndCallback(int32_t pid, const ChildEndedCallback& callback);

		private:
			static const int32_t SIGS_NUM = 64;
			static const int32_t CHILD_SLOTS = 64;

			volatile bool m_anyRaised;
			volatile bool m_raisedSignals[SIGS_NUM];
			volatile int32_t m_childPids[CHILD_SLOTS];
			volatile int32_t m_childExitCodes[CHILD_SLOTS];

			std::map<int32_t,SignalHandlerCallback> m_signalHandlers;
			std::map<int32_t,ChildEndedCallback> m_childrenHandlers;

			void clear();

			void onSignal(int32_t sig);
			static void onSignalStatic(int32_t sig);
			static void registerSignals();
	};

}

extern io::Signals g_unixSignals;


#endif /* INCLUDE_IO_SIGNALS_H_ */
