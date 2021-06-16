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

#include "streams.h"
#include <zlib.h>
#include "buffers.h"

namespace io {

	// ************************************************************************************
	std::string InputStream::readString() {
		int32_t len = readPrimitive<uint16_t>();
		return readString(len);
	}

	// ************************************************************************************
	std::string InputStream::readString(int32_t len) {
		if (len == 0) return "";
		char buf[65536] = { 0 };
		read(buf, len);
		return std::string(buf);
	}

	// ************************************************************************************
	void InputStream::readBufferRaw(DataBuffer& dest) {
		if (dest.size() == 0) return;
		read(&dest[0], dest.size());
	}

	// ************************************************************************************
	void InputStream::readBufferWithSize(DataBuffer& dest) {
		std::size_t size = readPrimitive<uint32_t>();
		if (size > 0) {
			dest.resize(size);
			read(&dest[0], dest.size());
		} else {
			dest.clear();
		}
	}

	// ************************************************************************************
	void InputStream::readFullTo(DataBuffer& dest) {
		dest.clear();
		io::DataBufferOutputStream os(dest, true);
		uint8_t buf[1024] = { 0 };

		while (!eof()) {
			std::size_t s = read(buf, 1024);
			if (s > 0) {
				os.write(buf, s);
			}
		}
	}



	// ************************************************************************************
	void OutputStream::writeString(const std::string& val) {
		writePrimitive<uint16_t>(val.size());
		if (!val.empty()) {
			write(val.c_str(), val.size());
		}
	}

	// ************************************************************************************
	void OutputStream::writeBufferRaw(const DataBuffer& buf) {
		if (buf.empty()) return;
		write(&buf[0], buf.size());
	}

	// ************************************************************************************
	void OutputStream::writeBufferWithSize(const DataBuffer& buf) {
		writePrimitive<uint32_t>(buf.size());
		if (!buf.empty()) {
			write(&buf[0], buf.size());
		}
	}

}
