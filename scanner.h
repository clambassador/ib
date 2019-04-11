#ifndef __IB__SCANNER__H__
#define __IB__SCANNER__H__

#include <map>
#include <regex>
#include <string>
#include <vector>

using namespace std;

namespace ib {

class Scanner {
public:
	Scanner() : Scanner(false) {}
	Scanner(bool match_whitespace) : _match_whitespace(match_whitespace) {}
	virtual ~Scanner() {}

	virtual void add_token(const string& token,
			       const string& matcher) {
		_token_to_match.insert(
			make_pair(token,
				  make_pair(smatch(), regex(matcher))));

	}

	static void trace(const vector<tuple<string, string, size_t>>& tokens) {
		for (const auto &x : tokens) {
			cout << get<0>(x) << " -> " << get<1>(x) << " at "
			     << get<2>(x) << endl;
		}
	}

	virtual vector<tuple<string, string, size_t>> tokenize(const string& s) {
		vector<tuple<string, string, size_t>> retval;
		string t = s;
		size_t last_pos = 0;
		while (last_pos < s.length()) {
			multimap<string, pair<smatch, regex>>::iterator it;
			size_t min_pos = (size_t) - 1;
			for (auto x = _token_to_match.begin();
			     x != _token_to_match.end(); ++x) {
				regex_search(t, x->second.first, x->second.second);
				if (x->second.first.position() < min_pos) {
					it = x;
					min_pos = x->second.first.position();
				}
			}
			if (_match_whitespace) {
				string val;
				if (it->second.first.prefix().length()) {
					val = it->second.first.prefix();
				} else if (t.length() &&
					   it->second.first.position() >= t.length()) {
					val = t;
				}
				retval.push_back(make_tuple(
					"WHITESPACE", val, last_pos));
			}

			if (it->second.first.position() < t.length()) {
				retval.push_back(make_tuple(
					it->first, it->second.first.str(),
					last_pos + it->second.first.position()));
			}
			t = it->second.first.suffix();
			last_pos = s.length() - t.length();
		}

		return retval;
	}

protected:
	bool _match_whitespace;

	multimap<string, pair<smatch, regex> > _token_to_match;

};

}  // namespace ib

#endif  // __IB__SCANNER__H__
