#ifndef __IB__RE__H__
#define __IB__RE__H__

#include <deque>
#include <set>
#include <string>
#include <vector>

#include "ib/tokenizer.h"

using namespace std;

namespace ib {

class RE {
public:
	RE() {}

	RE(const string& re) : _re(re) {
		_re = tidy(_re);
	}

	virtual ~RE() {}

	virtual string generate() {
		return generate(_re);
	}

	virtual string describe() {
		stringstream ss;

		ss << "with more parenthesis: " << _re << endl;
		ss << "shortest word: " << min_string(_re) << endl;
		ss << "shortest word length: " << min_length(_re) << endl;
		ss << "example words: " << endl;
		set<string> words;
		for (size_t i = 0; i < 50; ++i) {
			words.insert(generate());
		}
		for (auto &x : words) {
			ss << "- " << x << endl;
		}
		return ss.str();
	}

	virtual vector<string> enumerate(size_t length) {
		assert(length);
		return enumerate(_re, length);
	}

	static vector<string> enumerate(const string& re,
					size_t length) {
		set<string> seen;
		vector<string> retval;
		deque<string> q;
		q.push_back(tidy(re));
		seen.insert(q.back());
		while (!q.empty()) {
			string item = q.front();
			q.pop_front();
			size_t N = num_ops(item);
			if (!N) {
				item = remove_parens(item);
				assert(item.length() <= length);
				retval.push_back(item);
			}
			for (size_t i = 0; i < N; ++i) {
				size_t pos = op_pos(item, i);
				vector<string> next = resolves(item, pos);
				for (auto &x : next) {
					if (x.length() < 10 * length &&
					   min_length(x) <= length &&
					    !seen.count(x)) {
						q.push_back(x);
						seen.insert(x);
					}
				}
			}
		}
		return retval;
	}

	static string tidy(const string& re) {
		string seed = Tokenizer::trimout(re, " ");
		for (size_t i = 0; i < seed.length(); ++i) {
			if (seed[i] == '*') {
				stringstream ss;
				string left = get_left(seed, i);
				if (!parened(left)) {
					ss << seed.substr(0, i - left.length())
					   << "(" << left << ")*"
					   << seed.substr(i + 1);
					seed = ss.str();
					i += 2;
				}
			}
		}
		for (size_t i = 0; i < seed.length(); ++i) {

			if (seed[i] == '+') {
				string left = get_left(seed, i);
				string right = get_right(seed, i);
				stringstream ss;
	                        ss << seed.substr(0, i - left.length());
				int add = 0;
				if (!parened(left)) {
					add = 2;
					ss << "(" << left << ")";
				} else {
					ss << left;
				}
				ss << "+";
				if (!parened(right)) {
					ss << "(" << right << ")";
				} else {
					ss << right;
				}
		                ss << seed.substr(i + right.length() + 1);
				seed = ss.str();
				i += add;
			}
		}
		return seed;
	}

	static bool parened(const string& s) {
		if (s.size() < 2) return false;
		if (s[0] != '(') return false;
		if (s[s.size() - 1] != ')') return false;
		return true;
	}

	static string generate(const string& re) {
		string seed = tidy(re);
		while (has_ops(seed)) {
			size_t pos = random_op_pos(seed);
			if (pos != string::npos) {
				seed = resolve(seed, pos);
			}
		}
		return remove_parens(seed);
	}

	static string remove_parens(const string& seed) {
		stringstream ss;
		for (auto &x : seed) {
			if (x == '(' || x == ')') continue;
			ss << x;
		}
		return ss.str();
	}

	static size_t min_length(const string& re) {
		return min_string(re).length();
	}

	static string min_string(const string& re) {
		string val = re;
		while (true) {
			size_t pos = val.find("*");
			if (pos == string::npos) break;
			val = resolve(val, pos, EXIT);
		}
		while (true) {
			size_t pos = val.find("+");
			if (pos == string::npos) break;
			string left = resolve(val, pos, LEFT);
			string right = resolve(val, pos, RIGHT);
			bool which = remove_parens(left).length() <=
				     remove_parens(right).length();
			val = resolve(val, pos, which ? LEFT : RIGHT);
		}
		return remove_parens(val);
	}

	static int num_ops(const string& re) {
		int ops = 0;
		for (auto &x : re) {
			if (x == '+') ++ops;
			if (x == '*') ++ops;
		}
		return ops;
	}

	static size_t random_op_pos(const string& re) {
		int ops = num_ops(re);
		if (!ops) return string::npos;
		return op_pos(re, rand() % ops);
	}

	static size_t op_pos(const string& re, int ops) {
		++ops;
		size_t pos = 0;
		for (auto &x : re) {
			if (x == '+') --ops;
			if (x == '*') --ops;
			if (!ops) return pos;
			++pos;
		}
		assert(0);
	}

	static bool has_ops(const string& re) {
		if (re.find("*") != string::npos) return true;
		return re.find("+") != string::npos;
	}

	static vector<string> resolves(const string& re,
				       size_t pos) {
		vector<string> retval;
		retval.push_back(resolve(re, pos, false));
		retval.push_back(resolve(re, pos, true));
		return retval;
	}

	static string resolve(const string& re,
			      size_t pos) {
		int how = rand() % 2;
		return resolve(re, pos, how);
	}

	static string resolve(const string& re,
			      size_t pos, bool how) {
		stringstream ss;
		if (re[pos] == '*') {
			string component = get_left(re, pos);
			if (how) {
				ss << re.substr(0, pos)
				   << component
				   << re.substr(pos);
			} else {
				ss << re.substr(0, pos - component.length())
				   << "()"
				   << re.substr(pos + 1);
			}
		} else {
			assert(re[pos] == '+');
			string left = get_left(re, pos);
			string right = get_right(re, pos);
			ss << re.substr(0, pos - left.length());
			if (how == LEFT) ss << left;
			else ss << right;
			ss << re.substr(pos + right.length() + 1);
		}
		return ss.str();
	}

	static string get_left(const string& re,
			       size_t pos) {
		int count = 0;
		assert(pos);
		size_t move = pos - 1;
		if (re[pos] == '+') {
			while (move > 0) {
				if (re[move] == ')') ++count;
				if (re[move] == '(') --count;
				if (count < 0) {
					++move;
					break;
				}
				if (!count && re[move] == '+') {
					++move;
					break;
				}
				--move;
			}
		} else {
			while (move > 0 && re[move] == '*') --move;
			while (move > 0) {
				if (re[move] == ')') ++count;
				if (re[move] == '(') --count;
				if (!count) break;
				--move;
			}
		}

		assert(move < re.length());
		return re.substr(move, pos - move);
	}

	static string get_right(const string& re,
			        size_t pos) {
		int count = 0;
		size_t move = pos + 1;
		assert(re[pos] == '+');
		while (move < re.length()) {
			if (re[move] == '(') ++count;
			if (re[move] == ')') --count;
			if (count < 0) {
				--move;
				break;
			}
			if (!count && re[move] == '+') {
				--move;
				break;
			}
			++move;
		}
		while (move + 1 < re.length() && re[move + 1] == '*') ++move;
		assert(move > pos);
		return re.substr(pos + 1, move - pos);
	}

	enum {
		LEFT = 0,
		RIGHT
	};
	enum {
		EXIT = 0,
		LOOP
	};

protected:
	string _re;
};

}  // namespace ib

#endif  // __IB__RE__H__
