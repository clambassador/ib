#include <string>
#include <vector>
#include <unistd.h>

#include "ib/logger.h"
#include "ib/fsm.h"

using namespace std;
using namespace ib;

int main() {
	vector<string> rules;
	rules.push_back("START,a -> AS");
	rules.push_back("START,b -> BS");
	rules.push_back("AS,a -> AS");
	rules.push_back("AS,b -> BS");
	rules.push_back("BS,b -> BS");
	FSM fsm(rules);
	fsm.set_accept("BS");

	vector<pair<string, string>> tests;
	tests.push_back(make_pair("a a b b a", ""));
	tests.push_back(make_pair("a a a", "AS"));
	tests.push_back(make_pair("a a b b", "BS"));
	tests.push_back(make_pair("", "START"));
	tests.push_back(make_pair("a", "AS"));
	tests.push_back(make_pair("b b b", "BS"));

	for (auto &x : tests) {
		fsm.reset();
		vector<string> symbols;
		Tokenizer::split(x.first, " ", &symbols);
		for (auto &y : symbols) {
			Logger::info("state after % is %",
				     y, fsm.process(y));
		}
		assert(fsm.state() == x.second);
	}
}
