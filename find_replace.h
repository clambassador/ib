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
		size_t counter = 2000;
		while (indices.size()) {
			--counter;
			if (!counter) return "stuck in a loop; check grammar";
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

	static bool is_cfg(const string& grammar) {
		return is_cfg(parse(grammar));
	}

	static bool is_cfg(const vector<pair<string, string>>& grammar) {
		for (const auto &x : grammar) {
			if (x.first.length() != 1) return false;
		}
		return true;
	}

	static bool is_cnf(const string& grammar) {
		return is_cnf(parse(grammar));
	}

	static bool is_cnf(const vector<pair<string, string>>& grammar) {
		set<string> nonterminals;
		for (const auto &x : grammar) {
			nonterminals.insert(x.first);
		}
		for (const auto &x : grammar) {
			if (x.first.length() != 1) return false;
			if (x.second.length() == 2) {
				if (!nonterminals.count(x.second.substr(0,1))) return false;
				if (!nonterminals.count(x.second.substr(0,1))) return false;
			} else if (x.second.length() == 1) {
				if (nonterminals.count(x.second)) return false;
			} else if (!x.second.length() && x.first != "S") return false;
			else {
				if (x.first != "S" || x.second != "epsilon") return false;
			}
		}
		return true;
	}

	static vector<pair<string, string>> parse(const string& grammar) {
		vector<pair<string, string>> retval;
		vector<string> lines;
		Tokenizer::split(grammar, "\n", &lines);
		for (auto &x : lines) {
			string left, right;
			if (Tokenizer::extract("% -> %", x, &left, &right) == 2) {
				vector<string> pieces;
				Tokenizer::split_with_empty(right, "|", &pieces);
				for (auto &x : pieces) {
					retval.push_back(make_pair(
						Tokenizer::trim(left),
						Tokenizer::trim(x)));
				}
			}
		}
		return retval;
	}

};

}  // namespace ib

#endif  // __IB__FIND_REPLACE__H__
