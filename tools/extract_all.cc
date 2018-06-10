#include <ib/tokenizer.h>
#include <string>
#include <iostream>

using namespace std;
using namespace ib;

int main(int argc, char** argv) {
	assert(argc == 2);
	string pattern = argv[1];

	stringstream ss;
	while (cin.good()) {
		string s;
		cin >> s;
		ss << s;
	}

	vector<string> result;
	Tokenizer::extract_all(pattern, ss.str(), &result);
	for (auto &x : result) {
		cout << x << endl;
	}
}
