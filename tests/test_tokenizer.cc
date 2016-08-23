#include <string>
#include <vector>
#include <unistd.h>

#include "ib/logger.h"
#include "ib/tokenizer.h"

using namespace std;
using namespace ib;

int main() {
	vector<string> tokens;
	Tokenizer::split("hello there \"good people\"", " ", &tokens);
	Logger::info("%", tokens);
	tokens.clear();
	Tokenizer::split_mind_quote("hello there \"good people\"", " ", &tokens);
	Logger::info("%", tokens);
	tokens.clear();

	int val = 0;
	int ret = Tokenizer::extract("(%)", "(42)", &val);
	assert(val == 42);
	assert(ret == 1);
	string str1, str2;
	ret = Tokenizer::extract("(%)%(%)", "(2)hello(toot)", &val, &str1, &str2);
	assert(ret == 3);
	assert(val == 2);
	assert(str1 == "hello");
	assert(str2 == "toot");

	ret = Tokenizer::extract("%--? %", "preamble--? 18", &str1, &val);
	assert(val == 18);
	assert(str1 == "preamble");
	assert(ret == 2);

	/*
TODO: compare format string to data string with a doubling of percents.
TODO: allow other symbols than percent to allow reading 5 from 5%
	ret = Tokenizer::extract("% %%", "5 %", &str1);
	assert(str1 == "5");
	assert(ret == 1);
	*/
}
