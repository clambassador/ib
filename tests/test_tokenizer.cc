#include <string>
#include <vector>
#include <unistd.h>

#include "ib/logger.h"
#include "ib/tokenizer.h"

using namespace std;
using namespace ib;

int main() {
	Logger::info("3b0a3e4f % ", Logger::dehexify("3b0a3e4f"));
	{
	string out;
	assert(Tokenizer::fast_split("hello,there,good,people",
			      ',', 1, &out));
	assert(out == "hello");
	assert(Tokenizer::fast_split("hello,there,good,people",
			      ',', 2, &out));
	assert(out == "there");
	assert(Tokenizer::fast_split("hello,there,good,people",
			      ',', 3, &out));
	assert(out == "good");
	assert(Tokenizer::fast_split("hello,there,good,people",
			      ',', 4, &out));
	assert(out == "people");
	assert(!Tokenizer::fast_split("hello,there,good,people",
			      ',', 0, &out));
	assert(!Tokenizer::fast_split("hello,there,good,people",
			      ',', 5, &out));
	}
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

	ret = Tokenizer::extract("%=%;%=%", "unread=42;other=12",
				 nullptr, &val, &str1, nullptr);
	assert(val == 42);
	assert(str1 == "other");

	ret = Tokenizer::extract("test%\4%", "test42\thello",
				 &val, &str1);
	assert(val == 42);
	assert(str1 == "hello");
	ret = Tokenizer::extract("test%\4%", "test42 hello",
				 &val, &str1);
	assert(val == 42);
	assert(str1 == "hello");
	ret = Tokenizer::extract("test%\4%", "test42\nhello",
				 &val, &str1);
	assert(val == 42);
	assert(str1 == "hello");
	ret = Tokenizer::extract("test%\4%", "test42\rhello",
				 &val, &str1);
	assert(val == 42);
	assert(str1 == "hello");

	vector<string> inner;
	ret = Tokenizer::extract_all_paired(
		"{", "}", "{{hello}there}{great}{{}{}{}}", &inner);
	Logger::info("%", inner);

	assert(inner[0] == "{hello}there");
	assert(inner[1] == "great");
	assert(inner[2] == "{}{}{}");

	str1 = "hex%20is%20%41!";
	Logger::info("% to %", str1, Tokenizer::hex_unescape(str1));
	assert("hex is A!" == Tokenizer::hex_unescape(str1));



	string s = "b\na\naa\nab\nbb\nba\naabbba\naaaaaaaa no\nbbbba";
	vector<string> out;
	Tokenizer::get_lines_matching(&out, s, "a", "b");
	Logger::info("a and b  % ", out);
	out.clear();
	Tokenizer::get_lines_matching(&out, s, "b", "a");
	Logger::info("b and a  % ", out);
	out.clear();
	Tokenizer::get_lines_matching(&out, s, "a", "bb");
	Logger::info("a and bb  % ", out);
	out.clear();
	Tokenizer::get_lines_matching(&out, s, "a");
	Logger::info("a  % ", out);
	out.clear();
	Tokenizer::get_lines_matching(&out, s);
	Logger::info("   % ", out);

	{
	string in, out;
	Tokenizer::pop_split("one,two,three", ',', 2, &in, &out);
	Logger::info("% %", in, out);
	assert(in == "one,three");
	assert(out == "two");
	in = ""; out = "";
	Tokenizer::pop_split("one,two,three", ',', 1, &in, &out);
	Logger::info("% %", in, out);
	assert(in == "two,three");
	assert(out == "one");
	in = ""; out = "";
	Tokenizer::pop_split("one,two,three", ',', 3, &in, &out);
	Logger::info("% %", in, out);
	assert(in == "one,two");
	assert(out == "three");
	in = ""; out = "";
	Tokenizer::pop_split("one,two", ',', 2, &in, &out);
	Logger::info("% %", in, out);
	assert(in == "one");
	assert(out == "two");
	in = ""; out = "";
	Tokenizer::pop_split("one", ',', 1, &in, &out);
	Logger::info("% %", in, out);
	assert(in == "");
	assert(out == "one");
	}
	/*
TODO: compare format string to data string with a doubling of percents.
TODO: allow other symbols than percent to allow reading 5 from 5%
	ret = Tokenizer::extract("% %%", "5 %", &str1);
	assert(str1 == "5");
	assert(ret == 1);
	*/
}
