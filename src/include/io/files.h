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

#ifndef INCLUDE_IO_FILES_H_
#define INCLUDE_IO_FILES_H_

#include <base.h>
#include "streams.h"
#include <fstream>

namespace io {

	class FileInputStream: public SeekableInputStream {
		public:
			FileInputStream(const std::string& path);
			virtual ~FileInputStream() { }

			virtual std::size_t read(void* dest, std::size_t size);
			virtual void seek(std::size_t pos);
			virtual std::size_t tell() const;
			virtual bool eof() const;

			virtual std::string name() const { return m_path; }

		private:
			FileInputStream(const FileInputStream& from);
			FileInputStream& operator=(const FileInputStream& from);

			std::ifstream m_stream;
			std::string m_path;
	};

	class FileOutputStream: public SeekableOutputStream {
		public:
			FileOutputStream(const std::string& path);
			virtual ~FileOutputStream() { }

			virtual std::size_t write(const void* buf, std::size_t size);
			virtual void seek(std::size_t pos);
			virtual std::size_t tell() const;

		private:
			FileOutputStream(const FileOutputStream& from);
			FileOutputStream& operator=(const FileOutputStream& from);

			std::ofstream m_stream;
	};

}

#endif /* INCLUDE_IO_FILES_H_ */
