#ifndef __IB__BASE64__H__
#define __IB__BASE64__H__

#include <cassert>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace ib {

class Base64 {
public:
	static size_t encoded_len(size_t len) {
		return 1 + ((len + 2) / 3 * 4);
	}

	static string encode(const char* str, int len) {
		stringstream ss;
		static const string base =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";
		size_t i;
		for (i = 0; i + 2 < len; i += 3) {
			ss << base[(str[i] >> 2) & 0x3f];
			ss << base[((str[i] & 0x3) << 4) |
				   ((int) (str[i + 1] & 0xf0) >> 4)];
			ss << base[((str[i + 1] & 0xf) << 2) |
				   ((int) (str[i + 2] & 0xc0) >> 6)];
			ss << base[str[i + 2] & 0x3f];
		}

		if (i < len) {
			ss << base[(str[i] >> 2) & 0x3f];
			if (i == (len - 1)) {
				ss << base[((str[i] & 0x3) << 4)];
				ss << '=';
			} else {
				ss << base[((str[i] & 0x3) << 4) |
				           ((int) (str[i + 1] & 0xf0) >> 4)];
				ss << base[((str[i + 1] & 0xF) << 2)];
			}
			ss << '=';
		}
		return ss.str();
	}

	static string encode(const string& str) {
		return encode(str.c_str(), str.length());
	}

	static string reduce(const string& s) {
		stringstream ss;
		for (auto& x : s) {
			if (x >= 'a' && x <= 'z') ss << x;
			if (x >= 'A' && x <= 'Z') ss << x;
			if (x >= '0' && x <= '9') ss << x;
			if (x == '+' || x == '/' || x == '=') ss << x;
		}
		return ss.str();
	}

	static string decode(const string& s) {
                if (s.empty()) return "";
                static const int _index [256] = {
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
0,  0,  0, 62, 63, 62, 62, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,  0,
0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6, 7,  8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0, 0,  0, 0, 63,  0, 26, 27, 28,
29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
49, 50, 51 };
		return decode(s, _index);
	}

	static string decode(const string& s, const string& alphabet) {
// "-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz"
		assert(alphabet.length() == 64);
		int index[256];
		map<char, int> where;
		for (size_t i = 0; i < 64; ++i) {
			assert(!where.count(alphabet[i]));
			where[alphabet[i]] = i;
		}
		for (size_t i = 0; i < 256; ++i) {
			index[i] = 0;
			if (where.count((char) i)) {
				index[i] = where[i];
			}
		}
		return decode(s, index);
	}

	static string decode(const string& s, const int _index[256]) {
                unsigned char* p = (unsigned char*) s.c_str();
                int len = s.length();

                int pad = len > 0 && (len % 4 || p[len - 1] == '=');
                const size_t l = ((len + 3) / 4 - pad) * 4;
                string str(l / 4 * 3 + pad, '\0');
                for (size_t i = 0, j = 0; i < l; i += 4) {
                        int n = (_index[p[i]] << 18) |
			        (_index[p[i + 1]] << 12) |
				(_index[p[i + 2]] << 6) |
				_index[p[i + 3]];
			if (p[i] == '\n'
			    || p[i + 1] == '\n'
			    || p[i + 2] == '\n'
			    || p[i + 3] == '\n') return decode(reduce(s));
                        str[j++] = n >> 16;
                        str[j++] = (n >> 8) & 0xFF;
                        str[j++] = n & 0xFF;
                }
                if (pad) {
                        int n = (_index[p[l]] << 18) |
				(_index[p[l + 1]] << 12);
                        str[str.size() - 1] = (n >> 16);
                        if (len > l + 2 && p[l + 2] != '=') {
                                 n |= (_index[p[l + 2]] << 6);
                                 str.push_back((n >> 8) & 0xFF);
                        }
                }
		return str;
	}
};

}  // namespace ib

#endif  // __IB__BASE64__H__
