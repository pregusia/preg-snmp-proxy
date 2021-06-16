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

#ifdef OS_LINUX

#include "files.h"
#include "buffers.h"

namespace io {

	// ************************************************************************************
	FileInputStream::FileInputStream(const std::string& path) : m_path(path) {
		m_stream.open(path, std::ios::binary);
	}

	// ************************************************************************************
	std::size_t FileInputStream::read(void* dest, std::size_t size) {
		if (eof()) return 0;
		m_stream.read((char*)dest, size);
		return m_stream.gcount();
	}

	// ************************************************************************************
	void FileInputStream::seek(std::size_t pos) {
		m_stream.seekg(pos, std::ios::beg);
	}

	// ************************************************************************************
	std::size_t FileInputStream::tell() const {
		std::ifstream& fs = const_cast<std::ifstream&>(m_stream);
		return fs.tellg();
	}

	// ************************************************************************************
	bool FileInputStream::eof() const {
		return m_stream.eof() || m_stream.bad() || !m_stream.good();
	}




	// ************************************************************************************
	FileOutputStream::FileOutputStream(const std::string& path) {
		m_stream.open(path, std::ios::binary);
	}

	// ************************************************************************************
	std::size_t FileOutputStream::write(const void* buf, std::size_t size) {
		if (m_stream.bad()) return 0;

		m_stream.write((const char*)buf, size);
		if (m_stream.bad()) {
			return 0;
		} else {
			return size;
		}
	}

	// ************************************************************************************
	void FileOutputStream::seek(std::size_t pos) {
		m_stream.seekp(pos, std::ios::beg);
	}

	// ************************************************************************************
	std::size_t FileOutputStream::tell() const {
		std::ofstream& fs = const_cast<std::ofstream&>(m_stream);
		return fs.tellp();

	}

}

#endif
