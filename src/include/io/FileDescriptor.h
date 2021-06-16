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

#ifndef INCLUDE_IO_FILEDESCRIPTOR_H_
#define INCLUDE_IO_FILEDESCRIPTOR_H_

#include <base.h>
#include <functional>

namespace io {

	class FileDescriptor;
	typedef stdext::object_ptr<FileDescriptor> FileDescriptorPtr;


	class FileDescriptor: public stdext::object {
		public:
			typedef std::function<void(FileDescriptorPtr)> CallbackFunc;

			virtual ~FileDescriptor();

			void close();
			bool valid() const { return m_fd >= 0; }
			int32_t fd() const { return m_fd; }

			bool waitForRead() const { return m_readyRead ? true : false; }
			bool waitForWrite() const { return m_readyWrite ? true : false; }

			template<typename T>
			void onReadReady(const T& func) { m_readyRead = func; }

			template<typename T>
			void onWriteReady(const T& func) { m_readyWrite = func; }

			static FileDescriptorPtr Adapt(int32_t fd);

		private:
			FileDescriptor(int32_t fd);

			int32_t m_fd;
			CallbackFunc m_readyRead;
			CallbackFunc m_readyWrite;

			void callReadyRead();
			void callReadyWrite();

			friend class IO;
	};

}

#endif /* INCLUDE_IO_FILEDESCRIPTOR_H_ */
