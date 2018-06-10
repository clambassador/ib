#include <ib/fileutil.h>
#include <string>
#include <iostream>

using namespace std;
using namespace ib;

int main(int argc, char** argv) {
	assert(argc == 3);
	string f1;
	Fileutil::read_file(argv[1], &f1);
	string mask = argv[2];
	Logger::info("mask % %", mask, mask.length());

	int n = f1.length();
	for (int i = 0; i < n - mask.length(); ++i) {
		stringstream ss;
		bool skip = false;
		ss << endl << "---" << i << "\t:";
		for (int j = 0; j < mask.length(); ++j) {
			char c = (char) (f1[i + j] ^ mask[j]);
			if (c >= 32) ss << c;
			else skip = true;
		}
		if (!skip) cout << ss.str();
	}
	cout << endl;
}

