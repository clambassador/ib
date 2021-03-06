#ifndef __IB__LOGGER__H__
#define __IB__LOGGER__H__

#include <cassert>
#include <cstdio>
//#include <execinfo.h>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <stdio.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "ib/sensible_time.h"

using namespace std;

namespace ib {

class Logger {
public:
	template<typename... Args>
	static void info(const char* format, const Args&... args) {
		unique_lock<mutex> lock(*_mutex.get());
		cout << "\033[1;36m" << runtime() << " INFO: \033[0m "
		     << stringify(format, args...) << endl;
	}

	static void error() {}
	template<typename... Args>
	static void error(const char* format, const Args&... args) {
		if (_error_to_file) {
			flog(format, args...);
			return;
		}
		unique_lock<mutex> lock(*_mutex.get());
		cerr << "\033[1;31m" << runtime() << " ERROR: \033[0m"
		     << stringify(format, args...) << endl;
	}

	template<typename... Args>
	static void alert(const char* format, const Args&... args) {
		unique_lock<mutex> lock(*_mutex.get());
		cout << "\033[5;33m" << runtime() << " ALERT: \033[0m"
		     << stringify(format, args...) << endl;
	}

	template<typename... Args>
	static void debug(const char* format, const Args&... args) {
		unique_lock<mutex> lock(*_mutex.get());
		cout << "\033[1;35m" << runtime() << " DEBUG: \033[0m"
		     << stringify(format, args...) << endl;
	}

	template<typename... Args>
	static void kernel(const char* format, const Args&... args) {
		unique_lock<mutex> lock(*_mutex.get());
		FILE *f = fopen("/dev/kmsg", "a");
		if (f) {
			fprintf(f, "(logger) %s %s\n", runtime(),
				stringify(format, args...).c_str());
			fclose(f);
		} else {
			Logger::error("Unable to write % to kernel",
				      Logger::stringify(format, args...));
		}
	}

	static void error_to_file() {
		_error_to_file = true;
	}

	template<typename... Args>
	static void flog(const char* format, const Args&... args) {
		assert(_fd_to_file.size() < 2);
		if (_fd_to_file.size() == 0) {
			open_file("flog.out");
		}
		for (auto &x : _fd_to_file) {
			file(x.first, format, args...);
		}
	}

	template<typename... Args>
	static void file(int fd, const char* format, const Args&... args) {
		if (!fd) {
			return;
		}
		FILE* f;
		{
			unique_lock<mutex> lock(*_mutex.get());
			assert(_fd_to_file.count(fd));
			f = _fd_to_file[fd];
			fprintf(f, "%s\n", stringify(format, args...).c_str());
			fflush(f);
		}
	}

	static int open_file(const string& filename) {
		unique_lock<mutex> lock(*_mutex.get());
		int fd = ++_cur_fd;
		FILE* f = fopen(filename.c_str(), "r+");
		if (!f) {
			f = fopen(filename.c_str(), "w");
			fclose(f);
			f = fopen(filename.c_str(), "r+");
		}
		_fd_to_file[fd] = f;
		return fd;
	}

	static int delete_file(const string& filename) {
		unique_lock<mutex> lock(*_mutex.get());
		remove(filename.c_str());
		return 0;
	}

	static void close_file(int fd) {
		unique_lock<mutex> lock(*_mutex.get());
		if (!_fd_to_file.count(fd)) return;
		fclose(_fd_to_file[fd]);
		_fd_to_file.erase(fd);
	}

	static string boolify(bool b) {
		if (b) return "true";
		return "false";
	}

	template<typename... Args>
	static string stringify(const char* format, const Args&... args) {
		stringstream ss;
		stringify(ss, format, args...);
		return ss.str();
	}

	template<typename T, typename... Args>
	static void stringify(stringstream& ss, const char* format, T& value,
			      const Args&... args) {
		while (*format) {
			if (*format == '%') {
				if (*(format + 1) == '%') {
					++format;
				} else {
					ss << stringify(value);
					stringify(ss, format + 1, args...);
					return;
				}
			}
			ss << *format++;
		}
	}

	static void stringify(stringstream& ss, const char* format) {
		while (*format) {
			if (*format == '%') {
				if (*(format + 1) == '%') {
					++format;
				} else {
					cerr << "BAD FORMAT in " << ss.str() << " " << format
					     << endl;
					assert(0);
				}
			}
			ss << *format++;
		}
	}

	static bool is_binary(const string& data) {
		for (int i = 0; i < data.length(); ++i) {
			if ((unsigned char) data[i] > 127) {
				return true;
			}
		}
		return false;
	}

	template<typename T>
	static string stringify(const list<T>& val) {
		stringstream ss;
		auto x = val.begin();
		ss << "[";
		if (x != val.end()) {
			ss << *x++;
		}
		while (x != val.end()) {
			ss << ", " << *x++;
		}
		ss << "]";
		return ss.str();
	}

	template<typename T, typename R>
	static string stringify(const pair<T, R>& val) {
		stringstream ss;
		ss << "(" << val.first << ", " << val.second << ")";
		return ss.str();
	}

	template<typename T, typename R>
	static string stringify(const unordered_map<T, R>& val) {
		stringstream ss;
		ss << "{ ";
		if (val.size()) {
			for (auto &x : val) {
				ss << stringify("\t% --> %", x.first, x.second);
			}
		}
		ss << "}";
		return ss.str();
	}
	template<typename T, typename R>
	static string stringify(const map<T, R>& val) {
		stringstream ss;
		ss << "{ ";
		if (val.size()) {
			for (auto &x : val) {
				ss << stringify("\t% --> %", x.first, x.second);
			}
		}
		ss << "}";
		return ss.str();
	}

	template<typename T>
	static string stringify(const set<T>& val) {
		stringstream ss;
		ss << "{";
			for (auto &x: val) {
				ss << x << ", ";
		}
		ss << "}";
		return ss.str();
	}

	template<typename T>
	static string stringify(const vector<T>& val) {
		stringstream ss;
		ss << "<";
		if (val.size()) {
			for (int i = 0; i < val.size() - 1; ++i) {
				ss << stringify(val[i]) << ", ";
			}
			ss << stringify(val.back());
		}
		ss << ">";
		return ss.str();
	}

	static string stringify() { return ""; }

	template<typename T>
	static string stringify(const unique_ptr<T>& val) {
		if (val.get()) return val->trace();
		return "";
	}

	template<typename T>
	static string stringify(const T& val) {
		stringstream ss;
		ss << val;
		return ss.str();
	}

	static string nicebinary(const string& val) {
		string retval = val;
		for (size_t i = 0; i < retval.size(); ++i) {
			if (!isprint(retval[i])) retval[i] = '.';
		}
		return retval;
	}

	static string stringify(const string& val) {
//		for (auto &x : val) {
//			if (!isprint(x) && x != '\n') {
//				return hexify((uint8_t*) val.c_str(), val.length());
//			}
//		}
		return val;
	}

	static string dehexify(const string& data) {
		assert(!(data.length() % 2));

		stringstream ssout;
		for (size_t i = 0; i < data.length(); i += 2) {
			stringstream ss;
			int value;
			ss << hex << data.substr(i, 2);
			ss >> value;
			ssout << (char) value;
		}
		return ssout.str();
	}

	static string hexify(const string& data) {
		return hexify((uint8_t*) data.c_str(), data.length());
	}

	static string hexify(uint8_t* data, uint32_t len) {
                char hv[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
                stringstream ss;
                for (size_t i = 0; i < len; ++i) {
                        ss << hv[((data[i] >> 4) & 0x0F)] << hv[data[i] & 0x0F];
                }
                return ss.str();
        }

	static string lower_hexify(const string& data) {
		return lower_hexify((uint8_t*) data.c_str(), data.length());
	}

	static string lower_hexify(uint8_t* data, uint32_t len) {
                char hv[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
                stringstream ss;
                for (size_t i = 0; i < len; ++i) {
                        ss << hv[((data[i] >> 4) & 0x0F)] << hv[data[i] & 0x0F];
                }
                return ss.str();
        }

	static string runtime() {
		stringstream ss;
		ss << setw(5);
		ss << sensible_time::runtime();
		return ss.str();
	}

protected:
	static unique_ptr<mutex> _mutex;
	static int _cur_fd;
	static unordered_map<int, FILE *> _fd_to_file;
	static bool _error_to_file;
};

}  // namespace ib

#endif  // __IB__LOGGER__H__
