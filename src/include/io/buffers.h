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

#ifndef INCLUDE_IO_BUFFERS_H_
#define INCLUDE_IO_BUFFERS_H_

#include "base.h"
#include "streams.h"

namespace io {

	class DataBuffer {
		public:

			typedef std::vector<uint8_t>::iterator iterator;
			typedef std::vector<uint8_t>::const_iterator const_iterator;


			DataBuffer() { }
			DataBuffer(const void* source, std::size_t size);

			~DataBuffer() { }

			std::size_t size() const { return m_data.size(); }
			bool empty() const { return m_data.empty(); }

			void resize(std::size_t newSize);
			void clear();

			uint8_t& operator[](std::size_t idx) { return m_data[idx]; }
			const uint8_t& operator[](std::size_t idx) const { return m_data[idx]; }

			iterator begin() { return m_data.begin(); }
			iterator end() { return m_data.end(); }

			const_iterator begin() const { return m_data.begin(); }
			const_iterator end() const { return m_data.end(); }

			std::string asString() const;

			void dumpHex(std::ostream& os, int32_t rowSize=8) const;
			void debugLog(int32_t rowSize) const;

		private:
			std::vector<uint8_t> m_data;
	};

	class DataBufferInputStream: public SeekableInputStream {
		public:
			DataBufferInputStream(const DataBuffer& buffer) : m_buffer(buffer), m_position(0) { }
			virtual ~DataBufferInputStream() { }

			virtual std::string name() const { return "[DataBufferInputStream]"; }

			virtual std::size_t read(void* dest, std::size_t size);
			virtual bool eof() const;

			virtual void seek(std::size_t pos) { m_position = pos; }
			virtual std::size_t tell() const { return m_position; }

		private:
			DataBufferInputStream(const DataBufferInputStream& from);
			DataBufferInputStream& operator=(const DataBufferInputStream& os);

			const DataBuffer& m_buffer;
			std::size_t m_position;
	};

	class DataBufferOutputStream: public SeekableOutputStream {
		public:
			DataBufferOutputStream(DataBuffer& buffer, bool autoResize) : m_buffer(buffer), m_position(0), m_autoResize(autoResize) { }
			virtual ~DataBufferOutputStream() { }

			virtual void seek(std::size_t pos) { m_position = pos; }
			virtual std::size_t tell() const { return m_position; }
			virtual std::size_t write(const void* buf, std::size_t size);

		private:
			DataBufferOutputStream(const DataBufferOutputStream& from);
			DataBufferOutputStream& operator=(const DataBufferOutputStream& os);

			DataBuffer& m_buffer;
			std::size_t m_position;
			bool m_autoResize;
	};


}

#endif /* INCLUDE_IO_BUFFERS_H_ */
