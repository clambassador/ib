#include <string>
#include <vector>
#include <unistd.h>

#include "ib/logger.h"
#include "ib/scanner.h"

using namespace std;
using namespace ib;

int main() {
	Scanner scanner(true);
	scanner.add_token("GREETING", "hello");
	scanner.add_token("GREETING", "greetings");
	scanner.add_token("FAREWELL", "byebye");
	scanner.add_token("FAREWELL", "good day");

	string test = "hello there good sir and greetings! i wish you a good day.  byebye";
	auto x = scanner.tokenize(test);

	for (auto& y : x) {
		assert(get<1>(y) == test.substr(get<2>(y), get<1>(y).length()));
		cout << get<0>(y) << " => " << get<1>(y) << endl;
	}
}
