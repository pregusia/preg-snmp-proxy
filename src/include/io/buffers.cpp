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

#include "buffers.h"
#include <algorithm>

namespace io {

	// ************************************************************************************
	DataBuffer::DataBuffer(const void* source, std::size_t size) {
		resize(size);
		if (source) {
			memcpy(&m_data[0], source, size);
		}
	}

	// ************************************************************************************
	void DataBuffer::resize(std::size_t newSize) {
		m_data.resize(newSize, 0);
	}

	// ************************************************************************************
	void DataBuffer::clear() {
		m_data.clear();
	}

	// ************************************************************************************
	std::string DataBuffer::asString() const {
		if (m_data.empty()) return "";
		std::string str(m_data.begin(), m_data.end());
		return str;
	}

	// ************************************************************************************
	void DataBuffer::dumpHex(std::ostream& os, int32_t rowSize) const {
		int32_t inRow = 0;
		for(std::size_t i=0;i<m_data.size();++i) {

			os << stdext::format("%02X ", (int)m_data[i]);
			inRow += 1;
			if (inRow >= rowSize) {
				inRow = 0;
				os << std::endl;
			}

		}
	}

	// ************************************************************************************
	void DataBuffer::debugLog(int32_t rowSize) const {
		std::stringstream ss;
		int32_t inRow = 0;
		g_logger.debug(stdext::format("  size=%d", m_data.size()));

		for(std::size_t i=0;i<m_data.size();++i) {
			ss << stdext::format("%02X ", (int)m_data[i]);
			inRow += 1;
			if (inRow >= rowSize) {
				inRow = 0;
				g_logger.debug("  " + ss.str());
				ss.str("");
			}
		}

		if (inRow > 0) {
			g_logger.debug("  " + ss.str());
		}
	}


	// ************************************************************************************
	std::size_t DataBufferInputStream::read(void* dest, std::size_t size) {
		std:size_t n = std::min(size, m_buffer.size() - m_position);
		if (n > 0) {
			if (dest != nullptr) {
				memcpy(dest, &m_buffer[m_position], n);
			}
			m_position += n;
			return n;
		} else {
			return 0;
		}
	}

	// ************************************************************************************
	bool DataBufferInputStream::eof() const {
		return m_position >= m_buffer.size();
	}


	// ************************************************************************************
	std::size_t DataBufferOutputStream::write(const void* buf, std::size_t size) {
		if (size == 0) return 0;

		if (m_autoResize) {
			if (m_position + size > m_buffer.size()) {
				m_buffer.resize(m_position + size);
			}
			memcpy(&m_buffer[m_position], buf, size);
			m_position += size;
			return size;
		} else {
			std:size_t n = std::min(size, m_buffer.size() - m_position);
			if (n > 0) {
				memcpy(&m_buffer[m_position], buf, n);
				m_position += n;
				return n;
			} else {
				return 0;
			}
		}
	}

}

