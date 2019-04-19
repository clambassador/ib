#ifndef __IB__FIND_REPLACE__H__
#define __IB__FIND_REPLACE__H__

#include <set>
#include <string>
#include <vector>

#include "ib/containers.h"
#include "ib/tokenizer.h"

using namespace std;

namespace ib {

class FindReplace {
public:
	static string generate(const string& seed, const string& grammar) {
		vector<pair<string, string>> rules = parse(grammar);
		string retval = seed;
		set<size_t> indices = Containers::setrange(rules.size());
		while (indices.size()) {
			size_t pos = rand() % indices.size();
			pos = Containers::get_set(indices, pos);
			assert(pos < rules.size());
			pair<string, string> rule = rules[pos];
			if (retval.find(rule.first) != string::npos) {
				retval = Tokenizer::replace_first(
					retval, rule.first, rule.second);
				indices = Containers::setrange(rules.size());
			} else {
				indices.erase(pos);
			}
		}
		return retval;
	}

	static vector<pair<string, string>> parse(const string& grammar) {
		vector<pair<string, string>> retval;
		vector<string> lines;
		Tokenizer::split(grammar, "\n", &lines);
		for (auto &x : lines) {
			string left, right;
			if (Tokenizer::extract("% -> %", x, &left, &right) == 2) {
				retval.push_back(make_pair(left, right));
			}
		}
		return retval;
	}

};

}  // namespace ib

#endif  // __IB__FIND_REPLACE__H__
