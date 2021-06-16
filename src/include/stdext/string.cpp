/*
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
 * From OTClient OTClient <https://github.com/edubart/otclient>
 * Modded by pregusia
 *
 */

#include <stdext/string.h>

namespace stdext {

	// ************************************************************************************
	int32_t toInt(const std::string& v) {
		return atoi(v.c_str());
	}

	// ************************************************************************************
	float toFloat(const std::string& v) {
		return atof(v.c_str());
	}

	// ************************************************************************************
	bool toBoolean(std::string v) {
		v = toLowerCase(v);
		if (v == "yes" || v == "on" || v == "true" || v == "1") return true;
		return false;
	}

	// ************************************************************************************
	bool isNumeric(const std::string& v) {
		if (v.empty()) return false;
		for(std::string::size_type i=0;i<v.length();++i) {
			if (i == 0 && v[i] == '-') continue;
			if (!isdigit(v[i])) return false;
		}
		return true;
	}

	// ************************************************************************************
	std::string repeatString(const std::string& str, int32_t n) {
		std::stringstream ss;
		for(int32_t i=0;i<n;++i) {
			ss << str;
		}
		return ss.str();
	}

	// ************************************************************************************
	std::string pathDirectory(const std::string& filePath) {
		if (filePath.empty()) return "";

		// lecimy od konca az znajdziemy jakies /
		for(std::string::size_type n=filePath.length() - 1;n>=0;--n) {
			if (filePath[n] == '/') {
				return filePath.substr(0, n + 1);
			}
		}
		return "";
	}

	// ************************************************************************************
	std::string pathResolve(const std::string& filePath, std::string sourcePath) {
		if(startsWith<char>(filePath, "/")) return filePath;
		if(!endsWith<char>(sourcePath, "/")) {
			std::size_t slashPos = sourcePath.find_last_of("/");
			if(slashPos == std::string::npos) {
				//throw_exception(format("invalid source path '%s', for file '%s'", sourcePath, filePath));
				return "";
				// TODO: throw sth
			}
			sourcePath = sourcePath.substr(0, slashPos + 1);
		}
		return sourcePath + filePath;
	}

	// ************************************************************************************
	uint64_t hexToDec(const std::string& str) {
		uint64_t num = 0;
		std::istringstream i(str);
		i >> std::hex >> num;
		return num;
	}

	// ************************************************************************************
	char upperCharacter(char c) {
		if((c >= 97 && c <= 122) || (unsigned)c >= 224)
			c -= 32;
		return c;
	}

	// ************************************************************************************
	char16_t upperCharacter(char16_t c) {
		if((c >= 97 && c <= 122) || (unsigned)c >= 224)
			c -= 32;
		return c;
	}

	// ************************************************************************************
	char lowerCharacter(char c) {
		if((c >= 65 && c <= 90) || ((unsigned char)c >= 192 && (unsigned char)c <= 223))
			c += 32;
		return c;
	}

	// ************************************************************************************
	char16_t lowerCharacter(char16_t c) {
		if((c >= 65 && c <= 90) || ((unsigned)c >= 192 && (unsigned)c <= 223))
			c += 32;
		return c;
	}

	// ************************************************************************************
	std::string tabLines(const std::string& s, int32_t spaces) {
		auto arr = split<char>(s,"\n");
		std::stringstream ss;

		for(auto s: arr) {
			for(int32_t i=0;i<spaces;++i) ss << " ";
			ss << s << std::endl;
		}
		return ss.str();
	}

	// ************************************************************************************
	int hex_to_dec(char c) {
		switch(c) {
			case '0': return 0;
			case '1': return 1;
			case '2': return 2;
			case '3': return 3;
			case '4': return 4;
			case '5': return 5;
			case '6': return 6;
			case '7': return 7;
			case '8': return 8;
			case '9': return 9;

			case 'a':
			case 'A': return 10;

			case 'b':
			case 'B': return 11;

			case 'c':
			case 'C': return 12;

			case 'd':
			case 'D': return 13;

			case 'e':
			case 'E': return 14;

			case 'f':
			case 'F': return 15;

			default: return 0;
		}
	}

	// ************************************************************************************
	std::string simpleEncode(const std::string& str) {
		if (str.empty()) return "";

		char res[4096] = { 0 };

		for(uint i=0;i<str.size();++i) {
			char b[16];
			sprintf(b,"%02X", str[i]);
			res[2*i + 0] = b[0];
			res[2*i + 1] = b[1];
		}

		for(uint i=0;i<str.size() * 2;++i) {
			int shift = -7 + (i % 14);
			int v = hex_to_dec(res[i]);
			v += 'h';
			v += shift;
			res[i] = v;
		}
		return std::string(res);
	}

	// ************************************************************************************
	std::string simpleDecode(const std::string& str) {
		if (str.empty()) return "";
		if (str.size() % 2 != 0) return "";

		char buf[4096] = { 0 };

		for(uint i=0;i<str.size();i += 2) {
			int shift0 = -7 + (i % 14);
			int shift1 = -7 + ((i + 1) % 14);

			unsigned char c1 = str[i + 0] - 'h' - shift0;
			unsigned char c2 = str[i + 1] - 'h' - shift1;

			buf[i / 2] = c1 * 16 + c2;
		}

		return std::string(buf);
	}

	// ************************************************************************************
	std::string convertCP1250ToUTF8(const std::string& str) {
		if (str.empty()) return "";

		char buf[409600];

		int idx = 0;

		for(uint32_t i=0;i<str.length();++i) {
			if (idx + 3 >= 409600) break;

			switch((unsigned char)str[i]) {

				case 0xA5: buf[idx++] = 0xC4; buf[idx++] = 0x84; break; // Ą
				case 0xB9: buf[idx++] = 0xC4; buf[idx++] = 0x85; break; // ą

				case 0xC6: buf[idx++] = 0xC4; buf[idx++] = 0x86; break; // Ć
				case 0xE6: buf[idx++] = 0xC4; buf[idx++] = 0x87; break; // ć

				case 0xCA: buf[idx++] = 0xC4; buf[idx++] = 0x98; break; // Ę
				case 0xEA: buf[idx++] = 0xC4; buf[idx++] = 0x99; break; // ę

				case 0xA3: buf[idx++] = 0xC5; buf[idx++] = 0x81; break; // Ł
				case 0xB3: buf[idx++] = 0xC5; buf[idx++] = 0x82; break; // ł

				case 0xD1: buf[idx++] = 0xC5; buf[idx++] = 0x83; break; // Ń
				case 0xF1: buf[idx++] = 0xC5; buf[idx++] = 0x84; break; // ń

				case 0x8C: buf[idx++] = 0xC5; buf[idx++] = 0x9A; break; // Ś
				case 0x9C: buf[idx++] = 0xC5; buf[idx++] = 0x9B; break; // ś

				case 0x8F: buf[idx++] = 0xC5; buf[idx++] = 0xB9; break; // Ź
				case 0x9F: buf[idx++] = 0xC5; buf[idx++] = 0xBA; break; // ź

				case 0xAF: buf[idx++] = 0xC5; buf[idx++] = 0xBB; break; // Ż
				case 0xBF: buf[idx++] = 0xC5; buf[idx++] = 0xBC; break; // ż

				case 0xD3: buf[idx++] = 0xC3; buf[idx++] = 0x93; break; // Ó
				case 0xF3: buf[idx++] = 0xC3; buf[idx++] = 0xB3; break; // ó

				default: buf[idx++] = str[i]; break;
			}
		}
		buf[std::min(409600 - 1, idx)] = 0;

		return buf;
	}

	// ************************************************************************************
	std::string convertUTF8ToCP1250(const std::string& str) {
		if (str.empty()) return "";

		char buf[409600];

		int idx = 0;

		for(uint32_t i=0;i<str.length();++i) {
			if (idx + 3 >= 409600) break;

			uint16_t ch;

			unsigned char ch0 = str[i];
			unsigned char ch1 = (i + 1 < str.length() ? str[i + 1] : 0);
			unsigned char ch2 = (i + 2 < str.length() ? str[i + 2] : 0);

			((unsigned char*)&ch)[0] = ch1;
			((unsigned char*)&ch)[1] = ch0;

			// znaczek strzalki
			if (ch0 == 0xE2 && ch1 == 0x80 && ch2 == 0xA6) {
				i += 2;
				buf[idx++] = 0x85;
				continue;
			}

			// nutka pojedyncza
			if (ch0 == 0xE2 && ch1 == 0x99 && ch2 == 0xAA) {
				i += 2;
				buf[idx++] = 0x87;
				continue;
			}

			// nutka podwojna
			if (ch0 == 0xE2 && ch1 == 0x99 && ch2 == 0xAB) {
				i += 2;
				buf[idx++] = 0x86;
				continue;
			}

			switch(ch) {
				case 0xC484: buf[idx++] = 0xA5; ++i; break; // Ą
				case 0xC485: buf[idx++] = 0xB9; ++i; break; // ą

				case 0xC486: buf[idx++] = 0xC6; ++i; break; // Ć
				case 0xC487: buf[idx++] = 0xE6; ++i; break; // ć

				case 0xC498: buf[idx++] = 0xCA; ++i; break; // Ę
				case 0xC499: buf[idx++] = 0xEA; ++i; break; // ę

				case 0xC581: buf[idx++] = 0xA3; ++i; break; // Ł
				case 0xC582: buf[idx++] = 0xB3; ++i; break; // ł

				case 0xC583: buf[idx++] = 0xD1; ++i; break; // Ń
				case 0xC584: buf[idx++] = 0xF1; ++i; break; // ń

				case 0xC59A: buf[idx++] = 0x8C; ++i; break; // Ś
				case 0xC59B: buf[idx++] = 0x9C; ++i; break; // ś

				case 0xC5B9: buf[idx++] = 0x8F; ++i; break; // Ź
				case 0xC5BA: buf[idx++] = 0x9F; ++i; break; // ź

				case 0xC5BB: buf[idx++] = 0xAF; ++i; break; // Ż
				case 0xC5BC: buf[idx++] = 0xBF; ++i; break; // ż

				case 0xC393: buf[idx++] = 0xD3; ++i; break; // Ó
				case 0xC3B3: buf[idx++] = 0xF3; ++i; break; // ó

				default: buf[idx++] = str[i]; break;
			}
		}

		size_t len = std::min(409600 - 1, idx);

		return std::string(buf, len);
	}


} /* namespace core */
