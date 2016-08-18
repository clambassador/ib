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
	static void escape_cmd(const string& data, string* out) {
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
	static void escape_csv(const string& data, string* out) {
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
};

}  // namespace ib

#endif  // __IB__FORMATTING__H__