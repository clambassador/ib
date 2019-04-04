#include <string>
#include <vector>
#include <unistd.h>

#include "ib/logger.h"
#include "ib/scanner.h"

using namespace std;
using namespace ib;

int main() {
	Scanner scanner;
	scanner.add_token("GREETING", "hello");
	scanner.add_token("GREETING", "greetings");
	scanner.add_token("FAREWELL", "byebye");
	scanner.add_token("FAREWELL", "good day");

	scanner.tokenize("hello there good sir and greetings! i wish you a good day. byebye");
}
