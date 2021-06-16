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

#include "FileDescriptor.h"
#include "io.h"

extern io::IO g_io;


namespace io {

	// ************************************************************************************
	FileDescriptor::FileDescriptor(int32_t fd): m_fd(fd) {
		g_io.registerFileDescriptor(this);
	}

	// ************************************************************************************
	FileDescriptor::~FileDescriptor() {
		close();
		g_io.unregisterFileDescriptor(this);
	}

	// ************************************************************************************
	void FileDescriptor::close() {
		if (m_fd >= 0) {
			::close(m_fd);
			m_fd = -1;
		}
	}

	// ************************************************************************************
	FileDescriptorPtr FileDescriptor::Adapt(int32_t fd) {
		if (fd < 0) return nullptr;
		return FileDescriptorPtr(new FileDescriptor(fd));
	}

	// ************************************************************************************
	void FileDescriptor::callReadyRead() {
		if (m_readyRead) {
			auto func = m_readyRead;
			m_readyRead = CallbackFunc();

			func(dynamic_self_cast<FileDescriptor>());
		}
	}

	// ************************************************************************************
	void FileDescriptor::callReadyWrite() {
		if (m_readyWrite) {
			auto func = m_readyWrite;
			m_readyWrite = CallbackFunc();

			func(dynamic_self_cast<FileDescriptor>());

		}
	}

}
