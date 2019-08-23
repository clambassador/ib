#include <cassert>
#include <string>
#include <vector>
#include <unistd.h>

#include "ib/tokenizer.h"

using namespace std;
using namespace ib;

class JSONMapper {
public:
	JSONMapper(const string& s) : _s(s) {}


	virtual void run() {
		vector<string> components;
	        vector<size_t> matchings;
	
        	Tokenizer::match_pairs(_s, &components, &matchings);

		vector<string> prefices;

		for (size_t i = 0; i < _s.length(); ++i) {
			if (_s[i] == ',') continue;
			if (components[i].empty()) {
				int state = 0;
				size_t next = _s.length();
				for (size_t j = i + 1; j < _s.length(); ++j) {
					if (state == 0 && _s[j] == ':') {
						state = 1;
						next = j;
					} else if (state == 1) {
						if (components[j].length()) {
							components[i] = _s.substr(i, j + components[j].length() - i);
							break;
						}
					}
					if (_s[j] == ',' || _s[j] == '}' || _s[j] == ']') {
						components[i] = _s.substr(i, j - i);
						if (next == _s.length()) next = j;
						break;
					}
				}
				i = next;
			}
		}
		process("", 0, components);
	}

	virtual void process(const string& prefix, size_t pos, const vector<string>& components) {
		if (_s[pos] == '{') {
			size_t cur = pos + 1;
			while (cur < components[pos].length() + pos) {
				process(prefix, cur, components);
				cur += components[cur].length() + 1;
			}
		}
		else if (_s[pos] == '[') {
			size_t cur = pos + 1;
			size_t idx = 0;
			while (cur < components[pos].length() + pos) {
				process(Logger::stringify("%[%]_", Tokenizer::trim(prefix), idx++),
					cur, components);
				cur += components[cur].length() + 1;
			}
		} else {
			vector<string> pieces;
			Tokenizer::split_mind_quote(components[pos], ":", &pieces);
			if (pieces.size() == 1) {
				observe(prefix, pieces[0]);
			} else {
				process(prefix + Tokenizer::trim(pieces[0]) + "_", pos + pieces[0].length() + 1, components);
			}
		}
	}

	virtual void observe(const string& key, const string& val) {
		cout << key.substr(0, key.length() - 1) << " ->\t " << val << endl;
	}

	virtual void trace(const vector<string>& components) {
		for (size_t i = 0; i < components.size(); ++i) {
			cout << i << "\t" << _s[i] << " -> "
				<< components[i] << endl;
		}
	}

protected:
	string _s;
};

int main(int argc, char** argv) {
	string s = "fair{a:b, bac:[0,1,2], lac:{b:boo, daq:[1,2,{b:b}],c:foo}}toggle{a:b}";
	vector<string> r;
	Tokenizer::extract_outer_paired("{", "}", s, &r);
	Logger::info("%", r);
	JSONMapper jm(s);

	try {
		jm.run();
	} catch (string s) {
		cerr << s << endl;
	}
}
