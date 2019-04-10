#include <ib/logger.h>
#include <ib/base64.h>
#include <string>
#include <iostream>

using namespace std;
using namespace ib;

void doit(const string& input) {
	string d = Base64::decode(input);
	if (d.substr(0, 3) == "GET") return;
	string e;
	size_t i = 6;
	for (; i < d.length(); ++i) {
		e += (d[i] ^ d[((i-6)%4) + 2]);
	}

	cout << Logger::hexify(e) << "\t\t";
	for (size_t i = 0; i < e.length(); ++i) {
		if (isprint(e[i]) || isspace(e[i])) {
			cout << e[i];
		} else {
			cout << '.';
		}
	}
	cout << endl;
}

int main(int argc, char** argv) {
	if (argc == 2) {
		doit(argv[1]);
	} else {
		while (true) {
			string s;
			getline(cin, s);
			if (!cin.good()) return 0;
			if (s.empty()) continue;
			doit(s);
		}
	}
}

