#include <ib/logger.h>
#include <ib/base64.h>
#include <string>
#include <iostream>

using namespace std;
using namespace ib;

void doit(const string& input) {
	string dd = Base64::decode(input);
	if (dd.substr(0, 3) == "GET") return;
	for (size_t j = 0; j < dd.length() - 10; ++j) {
		string d = dd.substr(j);
	string e;
	size_t i = 4 + j;
	for (; i < d.length(); ++i) {
		e += (d[i] ^ d[((i-6)%4) + j]);

	}
	cout << j << " " << e << endl;
	//cout << Logger::hexify(e.substr(0, 10)) << endl;
	if (e[5] == 'E') { //if ((e[0] == 0x1f && e[1] == (char) 0x91) || (e[0] == 0  && e[1] == 0))
	//	{

	for (size_t i = 0; i < e.length(); ++i) {
		if (isprint(e[i]) || isspace(e[i])) {
			cout << e[i];
		} else {
			cout << '.';
		}
	}
	cout << endl;
	return;
	}
	}
}

int main(int argc, char** argv) {
	if (argc == 2) {
		doit(argv[1]);
	} else if (argc == 1) {
		while (true) {
			string s;
			getline(cin, s);
			if (!cin.good()) return 0;
			if (s.empty()) continue;
			doit(s);
		}
	}
}

