#include <string>
#include <vector>
#include <unistd.h>

#include "ib/logger.h"
#include "ib/formatting.h"

using namespace std;
using namespace ib;

int main() {
	vector<string> strs;
	vector<size_t> lens, cmdlens;
	strs.push_back("3343");
	lens.push_back(strs.back().length());
	cmdlens.push_back(strs.back().length());
	strs.push_back("hello there");
	lens.push_back(strs.back().length());
	cmdlens.push_back(strs.back().length() + 1);
	strs.push_back("what\nfollows");
	lens.push_back(strs.back().length() + 2);
	cmdlens.push_back(0);
	strs.push_back("once, twice, thrice, how nice");
	lens.push_back(strs.back().length() + 2);
	cmdlens.push_back(strs.back().length() + 4);
	strs.push_back("he said \"hello\"");
	lens.push_back(strs.back().length() + 4);
	cmdlens.push_back(strs.back().length() + 4);

	for (size_t i = 0; i < lens.size(); ++i) {
		string s;
		string t;
		Formatting::csv_escape(strs[i], &s);
		Formatting::cmd_escape(strs[i], &t);
		Logger::info("% % %", strs[i], s, t);
		assert(lens[i] == s.length());
		assert(cmdlens[i] == t.length());
	}

	string row1 = "343,hello,65";
	string row2 = "5435,\"343\"\"hello\"\"\",\"12\",3";

	assert(Formatting::csv_read(row1, 1) == "343");
	assert(Formatting::csv_read(row1, 2) == "hello");
	assert(Formatting::csv_read(row1, 3) == "65");
	assert(Formatting::csv_read(row2, 1) == "5435");
	assert(Formatting::csv_read(row2, 2) == "343\"hello\"");
	assert(Formatting::csv_read(row2, 3) == "12");
	assert(Formatting::csv_read(row2, 4) == "3");
}
