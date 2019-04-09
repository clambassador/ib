#include <cassert>
#include <string>
#include <vector>
#include <unistd.h>

#include "ib/down_up_next_parser.h"
#include "ib/logger.h"
#include "ib/scanner.h"

using namespace std;
using namespace ib;

class testset {
public:
	virtual void consider(const string& token, const string& val) {
		_last = token;
		if (token == "VAR") {
			_term_children.insert(val);
		}
	}

	virtual bool is_down() {
		return _last == "DOWN";
	}

	virtual bool is_next() {
		return _last == "NEXT";
	}

	virtual bool is_up() {
		return _last == "UP";
	}

	virtual void add_children(vector<unique_ptr<testset>>* children) {
		for (auto &x : *children) {
			_nonterm_children.push_back(nullptr);
			_nonterm_children.back().reset(x.release());
		}
	}

	virtual void trace() {
		cout << "{";
		for (auto &x : _term_children) {
			cout << x << ",";
		}
		for (auto &x : _nonterm_children) {
			x->trace();
		}
		cout << "}";
	}

protected:
	string _last;

	set<string> _term_children;
	vector<unique_ptr<testset>> _nonterm_children;

};

int main() {
	Scanner scanner;
	scanner.add_token("DOWN", "\\{");
	scanner.add_token("VAR", "[0-9a-z]+");
	scanner.add_token("UP", "\\}");
	scanner.add_token("NEXT", ",");

	string test = "{a,b,{c,d}}";
	auto x = scanner.tokenize(test);
	DownUpNextParser<testset> dup;
	vector<unique_ptr<testset>> out;
	dup.parse(x, &out);
	for (auto& y : out) {
		y->trace();
	}
}
