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

#ifndef INCLUDE_IO_IO_H_
#define INCLUDE_IO_IO_H_

#include <base.h>

namespace io {

	class FileDescriptor;

	class IO {

		public:
			void listFiles(const std::string& path, StringVector& files);
			bool isFile(const std::string& path);
			bool isDirectory(const std::string& path);
			std::string readFileContents(const std::string& path);
			void ensureDir(const std::string& path, bool omnitLast);

			bool select(int32_t timeout);
			void onFork();

		private:
			std::list<FileDescriptor*> m_fileDescriptors;

			void registerFileDescriptor(FileDescriptor* fd);
			void unregisterFileDescriptor(FileDescriptor* fd);


			friend class FileDescriptor;
	};

}

extern io::IO g_io;

#endif /* INCLUDE_IO_IO_H_ */
