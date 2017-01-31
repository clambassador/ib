#ifndef __IB__FORMATTING__H__
#define __IB__FORMATTING__H__

#include <cassert>
#include <cstdint>
#include <chrono>
#include <thread>
#include <vector>

#include "ib/logger.h"

using namespace std;

namespace ib {

class Formatting {
public:
	static void cmd_escape(const string& data, string* out) {
		static set<char> cmd_chars;
		static set<char> unspeakable;
		if (cmd_chars.empty()) {
			cmd_chars.insert('\\');
			cmd_chars.insert(' ');
			cmd_chars.insert('"');

			unspeakable.insert('\n');
			unspeakable.insert('\r');
			unspeakable.insert(0);
		}

		for (auto & x : data) {
			if (cmd_chars.count(x)) goto clean;
			if (unspeakable.count(x)) {
				Logger::error("(ib::formatting) "
					      "unspeakable in %",
					      data);
				*out = "";
				return;
			}
		}
		*out = data;
		return;

clean:
		stringstream ss;
		for (auto & x: data) {
			if (cmd_chars.count(x)) ss << "\\";
			ss << x;
		}
		*out = ss.str();
	}

	/* Sets <out> to be equal to an escaped version of data
	   conforming to RFC4180
	 */
	static string csv_escape(const string& data) {
		string retval;
		csv_escape(data, &retval);
		return retval;
	}

	static void csv_escape(const string& data, string* out) {
		static set<char> csv_chars;
		if (csv_chars.empty()) {
			csv_chars.insert(',');
			csv_chars.insert('\n');
			csv_chars.insert('\r');
			csv_chars.insert('"');
		}
		for (auto & x : data) {
			if (csv_chars.count(x)) goto clean;
		}
		*out = data;
		return;

clean:
		stringstream ss;
		ss << "\"";
		for (auto & x: data) {
			if (x == '"') ss << "\"";
			ss << x;
		}
		ss << "\"";
		*out = ss.str();
	}

	static string csv_read(const string& data, size_t pos) {
		string retval = csv_read_escaped(data, pos);
		return csv_unescape(retval);
	}

	static string csv_unescape(const string& data) {
		stringstream ss;

		for (size_t i = 0; i < data.length(); ++i) {
			if (data[i] == '"') {
				++i;
				if (i < data.length()) {
					if (data[i] == '"') {
						ss << '"';
					} else {
						ss << data[i];
					}
				}
			} else {
				ss << data[i];
			}
		}
		return ss.str();
	}

	static string csv_read_escaped(const string& data, size_t pos) {
		size_t i = 0;
		size_t curcol = 0;
		size_t start = 0;
		bool inquote = false;
		--pos;  // Counting from 1 for csv. I'm sorry.
		while (i < data.size()) {
			if (!inquote) {
				if (data[i] == ',') {
					if (curcol == pos) {
						return data.substr(start,
								   i - start);
					}
					++curcol;
					start = i + 1;

				}
				if (data[i] == '"') {
					inquote = true;
				}
			}
			if (inquote) {
				if (data[i] == '"') {
					if (i + 1 < data.size() &&
					    data[i + 1] == '"') {
						++i;
					} else {
						inquote = false;
					}
				}
			}
			++i;
		}
		if (curcol == pos) return data.substr(start);
		throw string("No column.");
	}
};

}  // namespace ib

#endif  // __IB__FORMATTING__H__
