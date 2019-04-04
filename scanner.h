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
		_token_to_match.insert(make_pair(token, regex(matcher)));

	}

	virtual void tokenize(const string& s) {
		smatch m;
		for (auto &x : _token_to_match) {
			regex_search(s, m, x.second);
			for (auto &y : m) {
				cout << x.first << "=" << y << endl;
			}
		}

		// TODO: conver this to a vector of pairs
		// <token_type, token_value>
		// include white space
	}

protected:
	bool _match_whitespace;

	multimap<string, regex> _token_to_match;

};

}  // namespace ib

#endif  // __IB__SCANNER__H__
