#include <cassert>

#include "ib/logger.h"
#include "ib/re.h"

using namespace std;
using namespace ib;

int main(int argc, char** argv) {
	if (argc == 2) {
		RE r(argv[1]);
		cout << r.describe();
		cout << endl;
		return 0;
	}
	Logger::info("aaa+bbb+ccc 3 %", RE::get_left("aaa+bbb+ccc", 3));
	Logger::info("aaa+bbb+ccc 3 %", RE::get_right("aaa+bbb+ccc", 3));
	Logger::info("aaa+bbb+ccc 7 %", RE::get_left("aaa+bbb+ccc", 7));
	Logger::info("aaa+bbb+ccc 7 %", RE::get_right("aaa+bbb+ccc", 7));
	Logger::info("a+b* 3 %", RE::get_left("a+b*", 3));
	Logger::info("a+b* 1 %", RE::get_left("a+b*", 1));
	Logger::info("a+b* 1 %", RE::get_right("a+b*", 1));
	string test = "aa(b+(d+c)*)*";
	Logger::info("% 4 %", test, RE::get_left(test, 4));
	Logger::info("% 4 %", test, RE::get_right(test, 4));
	Logger::info("% 7 %", test, RE::get_left(test, 7));
	Logger::info("% 7 %", test, RE::get_right(test, 7));
	Logger::info("% 10  %", test, RE::get_left(test, 10));
	Logger::info("% 12  %", test, RE::get_left(test, 12));
	test = "aa((a+b)+((b+(d+c)*)*))";
	Logger::info("% 8 %", test, RE::get_left(test, 8));
	Logger::info("% 8 %", test, RE::get_right(test, 8));
	Logger::info("a+b* 1 %", RE::get_right("a+b*", 1));

	vector<string> tests;
	tests.push_back("aaa+bbb+ccc");
	tests.push_back("ab(bb+cc)+cc+bb(dd(d)+c)+c");
	tests.push_back("a*b*c*");
	tests.push_back("a*+(b*c*)");
	tests.push_back("aa((a+b)+((b+(d+c)*)*))");
	tests.push_back("(a(aa)*b)*");
	for (auto& x : tests) {
		Logger::info("GENERATING FOR %", x);
		RE y(x);
		Logger::info("%", y.describe());
		for (size_t i = 0; i < 100; ++i) {
			Logger::info("generate: %", RE::generate(x));
		}
	}
	assert(RE::min_string("a*b*c*") == "");
	assert(RE::min_string("aa*b*c*") == "a");
	assert(RE::min_string("a+b*") == "");
	assert(RE::min_string("(aaa*)+(b(b*)b*)") == "b");
	assert(RE::min_string("(aa(aa)*(b+c))+(bbbbb*)") == "aab");

	string re = "a*";
	Logger::info("% -> %", re, RE::enumerate(re, 5));
	re = "aa((a+b)+((b+(d+c))))";
	Logger::info("% -> %", re, RE::enumerate(re, 5));
	re = "(a(aa)*b)*";
	Logger::info("% -> %", re, RE::enumerate(re, 5));
	re = "a*b*c*";
	Logger::info("% -> %", re, RE::enumerate(re, 5));
	re = "a*+(b*c*)";
	Logger::info("% -> %", re, RE::enumerate(re, 5));
}
