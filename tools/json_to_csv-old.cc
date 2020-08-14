#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "ib/csv_table.h"
#include "ib/fileutil.h"
#include "ib/formatting.h"
#include "ib/logger.h"
#include "ib/set.h"
#include "ib/tokenizer.h"

using namespace std;
using namespace ib;

void process_json(const string& blob, bool header) {
	vector<string> lines;
	Tokenizer::split(blob, "\n", &lines);
	stringstream ss;

	for (auto &x : lines) {
		vector<string> pieces;
		Tokenizer::split_mind_quote(x, ":", &pieces);
		if (pieces.size() < 2) continue;
		assert(pieces.size() == 2);
		if (header) {
			vector<string> inner;
			Tokenizer::split(pieces[0], "\"", &inner);
			ss << inner[1] << ", ";
		} else {
			if (pieces[1][1] == '"')
				ss << pieces[1].substr(2, pieces[1].length() - 4) << ", ";
			else
				ss << pieces[1].substr(1, pieces[1].length() - 2) << ", ";
		}
	}
	assert(ss.str().length() > 1);
	cout << ss.str().substr(0, ss.str().length() - 2) << endl;
	if (header) process_json(blob, false);
}

int main(int argc, char** argv) {
	if (argc != 2) {
		Logger::error("usage: % jsonfile", argv[0]);
		return -1;
	}
	string s;
	Fileutil::read_file(argv[1], &s);
	assert(s.length());
	vector<string> main;
	vector<string> results;

	Tokenizer::extract_all_paired("{", "}", s.substr(1, s.length() - 2), &results);
	bool header = true;
	for (auto &x : results) {
		process_json(x, header);
		header = false;
	}
	return 0;
}
