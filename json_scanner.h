#ifndef __IB__JSON_SCANNER__H__
#define __IB__JSON_SCANNER__H__

#include <string>
#include <tuple>
#include <vector>

#include "ib/logger.h"

using namespace std;

namespace ib {

class JSONScanner {
public:
	JSONScanner() {}

	virtual ~JSONScanner() {}

	static string what_type(const string& data, size_t pos) {
		if (isspace(data[pos])) return "whitespace";
		if (data[pos] == '{') return "begin_object";
		if (data[pos] == '}') return "end_object";
		if (data[pos] == '[') return "begin_array";
		if (data[pos] == ']') return "end_array";
		if (data[pos] == ',') return "comma";
		if (data[pos] == ':') return "colon";
		if (data[pos] == '"') return "quote_expression";
		return "expression";
	}

	static bool is_punctuation(char c) {
		if (c == '{') return true;
		if (c == '}') return true;
		if (c == '[') return true;
		if (c == ']') return true;
		if (c == ',') return true;
		if (c == ':') return true;
		if (c == '"') return true;
		return false;
	}

	static string consume(const string& data,
			      size_t pos,
			      const string& type) {
		if (type == "begin_object"
		    || type == "end_object"
		    || type == "begin_array"
		    || type == "end_array"
		    || type == "comma"
		    || type == "colon") return data.substr(pos, 1);
		size_t start = pos;
		if (type == "whitespace") {
			while (isspace(data[pos])) {
				++pos;
			}
		} else if (type == "quote_expression") {
			while (pos < data.length()) {
				++pos;
				if (data[pos] == '"' && data[pos - 1] != '\\')
					break;
			}
			return data.substr(start, pos + 1 - start);
		} else if (type == "expression") {
			while (++pos < data.length() && !isspace(data[pos]) &&
			       !is_punctuation(data[pos]));
		}
		return data.substr(start, pos - start);
	}

	static bool report(const string& type) {
		if (type == "whitespace") return false;
		return true;
	}

	static void scan(const string& data,
			  vector<tuple<string, string, size_t>> *tokens) {
		size_t pos = 0;
		while (pos < data.length()) {
			string type = what_type(data, pos);
			string token = consume(data, pos, type);
			size_t at = pos;
			pos += token.length();
			if (type == "expression" && token == "null") token = "";
			if (report(type)) tokens->push_back(
				make_tuple(type, token, at));
		}
	}

};

}  // namespace ib

#endif  // __IB__JSON_SCANNER__H__
