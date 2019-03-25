#include "ib/base64.h"

#include <cassert>
#include <iostream>

using namespace ib;
using namespace std;

int main(int argc, char** argv) {
	assert(argc == 3);
	string alphabet = argv[1];
	string word = argv[2];
	cout << Base64::decode(word, alphabet) << endl;
}


