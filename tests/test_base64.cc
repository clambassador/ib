#include "ib/base64.h"

#include <cassert>

using namespace ib;
using namespace std;

int main() {
	assert(Base64::decode("aGVsbG8=") == "hello");
	assert(Base64::decode("dG9vdA==") == "toot");
	assert(Base64::decode("") == "");
	assert(Base64::decode("dHJlYXRz") == "treats");
	assert(Base64::decode("dHJ\nlYXRz") == "treats");
	assert(Base64::reduce("123!@#qwe") == "123qwe");
	assert(!(Base64::decode("dHJlYXRz") == "treets"));

}


