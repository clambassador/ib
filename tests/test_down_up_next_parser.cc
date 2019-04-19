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
		if (token == "VAR") {
			_me = val;
		}
		cout << _me << " " << token << endl;
	}

	virtual void add_children(vector<unique_ptr<testset>>* children) {
		for (auto &x : *children) {
			bool skip = false;
                                for (auto &y : _children) {
                                        if (*y.get() == *x.get()) skip = true;
                                }
                                if (skip) continue;

			_children.push_back(unique_ptr<testset>(x.release()));
		}
	}

                virtual string trace() const {
                        if (_children.empty()) {
                         //       assert(!_me.empty());
                                return _me;
                        } else {
                                stringstream ss;
                                ss << "{";
				set<string> children;
                                for (const auto &x : _children) {
					string child = x->trace();
					assert(!children.count(child));
					children.insert(child);
				}
				for (const auto &x : children) {
                                        ss << x << ",";
                                }
                                ss << "}";
				return ss.str();
                        }
                }

                virtual bool operator==(const testset& other) const {
                        return trace() == other.trace();
                }


protected:
	string _me;
	vector<unique_ptr<testset>> _children;

};

int main(int argc, char** argv) {
	Scanner scanner;
	scanner.add_token("DOWN", "\\{");
	scanner.add_token("VAR", "[0-9a-z]+");
	scanner.add_token("UP", "\\}");
	scanner.add_token("NEXT", ",");

	string test = "{a,b,{c,d}}";
	if (argc == 2) test = argv[1];
	auto x = scanner.tokenize(test);
	scanner.trace(x);
	DownUpNextParser<testset> dup;
	vector<unique_ptr<testset>> out;
	dup.parse(x, &out);
	for (auto& y : out) {
		cout << y->trace() << endl;
	}
}
