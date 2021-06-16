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

#ifndef INCLUDE_IO_STREAMS_H_
#define INCLUDE_IO_STREAMS_H_

#include "base.h"

namespace io {

	class DataBuffer;

	// base classes

	class InputStream {
		public:
			virtual ~InputStream() { }
			virtual std::size_t read(void* dest, std::size_t size) = 0;
			virtual bool eof() const = 0;
			virtual std::string name() const { return "[InputStream]"; }

			template<typename T>
			T readPrimitive() {
				T val = 0;
				read(&val, sizeof(T));
				return val;
			}

			std::string readString();
			std::string readString(int32_t len);

			void readBufferRaw(DataBuffer& dest);
			void readBufferWithSize(DataBuffer& dest);

			void readFullTo(DataBuffer& dest);

			template<typename T, typename F>
			void readContainer(const F& func) {
				T n = readPrimitive<T>();
				for(T i=0;i<n;++i) {
					func();
				}
			}

		protected:
			InputStream() { }

		private:
			InputStream(const InputStream& from);
			InputStream& operator=(const InputStream& from);
	};

	class OutputStream {
		public:
			virtual ~OutputStream() { }
			virtual std::size_t write(const void* buf, std::size_t size) = 0;

			template<typename T>
			void writePrimitive(const T& val) {
				write(&val, sizeof(T));
			}

			void writeString(const std::string& val);
			void writeBufferRaw(const DataBuffer& buf);
			void writeBufferWithSize(const DataBuffer& buf);

			template<typename T, typename E, typename F>
			void writeContainer(const std::vector<E>& vec, const F& func) {
				writePrimitive<T>(vec.size());
				for(auto it=vec.begin();it != vec.end();++it) func(*it);
			}

		protected:
			OutputStream() { }

		private:
			OutputStream(const OutputStream& from);
			OutputStream& operator=(const OutputStream& from);

	};

	class SeekableInputStream: public InputStream {
		public:
			virtual ~SeekableInputStream() { }
			virtual void seek(std::size_t pos) = 0;
			virtual std::size_t tell() const = 0;
			virtual std::string name() const { return "[SeekableInputStream]"; }

		protected:
			SeekableInputStream() { }

		private:
			SeekableInputStream(const SeekableInputStream& from);
			SeekableInputStream& operator=(const SeekableInputStream& from);
	};

	class SeekableOutputStream: public OutputStream {
		public:
			virtual ~SeekableOutputStream() { }
			virtual void seek(std::size_t pos) = 0;
			virtual std::size_t tell() const = 0;

		protected:
			SeekableOutputStream() { }

		private:
			SeekableOutputStream(const SeekableOutputStream& from);
			SeekableOutputStream& operator=(const SeekableOutputStream& from);
	};

}

#endif /* INCLUDE_IO_STREAMS_H_ */
