#include <iostream>
#include <sstream>
#include <string>

#include "ib/tokenizer.h"

using namespace std;
using namespace ib;

int main(int argc, char** argv) {
	stringstream ss;
	string s;
	while (!feof(stdin)) {
		s = "";
		cin >> s;
		ss << s;
	}
	cout << Tokenizer::hex_unescape(ss.str());
	return 0;
}
