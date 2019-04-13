#ifndef __IB__DOWN_UP_NEXT_PARSER__H__
#define __IB__DOWN_UP_NEXT_PARSER__H__

#include <map>
#include <regex>
#include <string>
#include <vector>

#include "ib/logger.h"

using namespace std;

namespace ib {

template <typename T>
class DownUpNextParser {
public:
	DownUpNextParser() {}
	virtual ~DownUpNextParser() {}

	virtual bool parse(const vector<tuple<string, string, size_t>>& tokens,
			   vector<unique_ptr<T>>* out) {
		size_t pos = 0;
		return parse(tokens, out, &pos);
	}

protected:
	virtual bool parse(const vector<tuple<string, string, size_t>>& tokens,
			   vector<unique_ptr<T>>* out, size_t* pos) {
		assert(out);
		assert(pos);
		unique_ptr<T> cur(new T());
		while (*pos < tokens.size()) {
			string token = get<0>(tokens[*pos]);
			cur->consider(token, get<1>(tokens[*pos]));
			++*pos;
			if (token == "DOWN") {
				vector<unique_ptr<T>> children;
				assert(parse(tokens, &children, pos));
				cur->add_children(&children);
			} else if (token == "NEXT") {
				out->push_back(unique_ptr<T>(cur.release()));
				cur.reset(new T());
			} else if (token == "UP") {
				out->push_back(unique_ptr<T>(cur.release()));
				cur.reset(new T());
				return true;
			}
		}
		out->push_back(unique_ptr<T>(cur.release()));
		return false;
	}
};

}  // namespace ib

#endif  // __IB__DOWN_UP_NEXT_PARSER__H__
