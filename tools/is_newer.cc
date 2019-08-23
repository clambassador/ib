#include <ib/fileutil.h>
#include <string>
#include <iostream>

using namespace std;
using namespace ib;

int main(int argc, char** argv) {
	assert(argc == 3);
	cout << Fileutil::is_newer(argv[1], argv[2]) << endl;
}

