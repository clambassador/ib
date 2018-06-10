#include <ib/fileutil.h>
#include <string>
#include <iostream>

using namespace std;
using namespace ib;

int main(int argc, char** argv) {
	assert(argc == 3);
	string f1, f2;
	Fileutil::read_file(argv[1], &f1);
	Fileutil::read_file(argv[2], &f2);

	int n = f1.length();
	if (f2.length() > n) n = f2.length();
	for (int i = 0; i < n; ++i) {
		cout << (char) (f1[i] ^ f2[i]);
	}
	cout << endl;
}

