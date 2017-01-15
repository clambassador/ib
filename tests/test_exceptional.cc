#include <string>
#include <vector>
#include <unistd.h>

#include "ib/logger.h"
#include "ib/exceptional.h"

using namespace std;
using namespace ib;

int main() {
	ASSERT(1 < 2);
	ASSERT(5 == 5, "yes");

	try {
		THROW("hello %", "world");
	} catch (string s) {
		ASSERT(s == "hello world");
	}

	try {
		THROW()
	} catch (string s) {
		ASSERT(s == "");
	}

	LOG_IF(true, "hello");
	LOG_IF(5 < 3, "world");
	LOG_IF(5 > 3, "there");

	ASSERT(5 < 3, "Only panic if % is less than %", 5 , 3);
}
