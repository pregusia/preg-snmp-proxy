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

#include "io.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fstream>
#include <sys/socket.h>
#include <unistd.h>

#include "FileDescriptor.h"

io::IO g_io;

namespace io {

	// ************************************************************************************
	bool IO::isDirectory(const std::string& path) {
		struct stat buf = { 0 };
		int32_t res = ::stat(path.c_str(), &buf);
		if (res == 0) {
			return S_ISDIR(buf.st_mode);
		}
		return false;
	}

	// ************************************************************************************
	bool IO::isFile(const std::string& path) {
		struct stat buf = { 0 };
		int32_t res = ::stat(path.c_str(), &buf);
		if (res == 0) {
			return S_ISREG(buf.st_mode);
		}
		return false;
	}

	// ************************************************************************************
	void IO::listFiles(const std::string& path, StringVector& files) {
		DIR* d = opendir(path.c_str());
		if (d != NULL) {
			while(true) {
				dirent* e = readdir(d);
				if (e == NULL) break;

				if (e->d_type == DT_REG) {
					files.push_back(e->d_name);
				}
			}
			closedir(d);
		}
	}

	// ************************************************************************************
	std::string IO::readFileContents(const std::string& path) {
		std::ifstream fs(path, std::ios::binary);
		if (fs.good()) {
			std::string str((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
			return str;
		} else {
			return "";
		}
	}

	// ************************************************************************************
	void IO::ensureDir(const std::string& p, bool omnitLast) {
		if (p.empty()) return;
		StringVector path = stdext::split<char>(p,"/");
		if (omnitLast) path.pop_back();
		if (path.empty()) return;

		std::stringstream ss;
		for(auto& e: path) {
			ss << e << "/";
			if (ss.str() == "/") continue;
			if (!isDirectory(ss.str())) {
				mkdir(ss.str().c_str(), 0777);
			}
		}
	}

	// ************************************************************************************
	bool IO::select(int32_t timeout) {
		fd_set readSet, writeSet;
		int32_t readMax, writeMax;

		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);
		readMax = writeMax = -1;

		for(auto& fd: m_fileDescriptors) {
			if (fd->valid() && fd->waitForRead()) {
				FD_SET(fd->fd(), &readSet);
				readMax = std::max(readMax, fd->fd());
			}
			if (fd->valid() && fd->waitForWrite()) {
				FD_SET(fd->fd(), &writeSet);
				writeMax = std::max(writeMax, fd->fd());
			}
		}

		if (readMax != -1 || writeMax != -1) {
			struct timeval tv;
			tv.tv_sec = timeout / 1000;
			tv.tv_usec = (timeout % 1000) * 1000;

			int32_t res = ::select(std::max(readMax, writeMax) + 1, &readSet, &writeSet, nullptr, &tv);
			if (res > 0) {
				std::vector<FileDescriptorPtr> toCallRead, toCallWrite;

				for(auto& fd: m_fileDescriptors) {
					if (FD_ISSET(fd->fd(), &readSet)) {
						toCallRead.push_back(fd->dynamic_self_cast<FileDescriptor>());
					}
					if (FD_ISSET(fd->fd(), &writeSet)) {
						toCallWrite.push_back(fd->dynamic_self_cast<FileDescriptor>());
					}
				}

				for(auto& fd: toCallRead) {
					fd->callReadyRead();
				}
				for(auto& fd: toCallWrite) {
					fd->callReadyWrite();
				}
				return true;
			}
		} else {
			usleep(timeout * 1000);
		}
		return false;
	}

	// ************************************************************************************
	void IO::onFork() {
		for(auto& fd: m_fileDescriptors) {
			if (fd->fd() >= 0) {
				::close(fd->fd());
			}
		}
	}

	// ************************************************************************************
	void IO::registerFileDescriptor(FileDescriptor* fd) {
		if (fd == nullptr) return;
		m_fileDescriptors.push_back(fd);
	}

	// ************************************************************************************
	void IO::unregisterFileDescriptor(FileDescriptor* fd) {
		if (fd == nullptr) return;

		auto it = std::find(m_fileDescriptors.begin(), m_fileDescriptors.end(), fd);
		if (it != m_fileDescriptors.end()) {
			m_fileDescriptors.erase(it);
		}
	}


}
