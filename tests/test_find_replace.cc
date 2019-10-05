#include <string>
#include <vector>
#include <unistd.h>

#include "ib/find_replace.h"

using namespace std;
using namespace ib;

int main() {
	string grammar = "A -> BC\nB -> bBb\nB -> b\nC -> \nC -> ,A";
	for (size_t i = 0; i < 10; ++i) {
		cout << FindReplace::generate("A", grammar) << endl;
	}
	cout << FindReplace::is_cfg(grammar) << endl;
	cout << FindReplace::is_cnf(grammar) << endl;

	grammar = "A -> BC|BB | a\nB -> b | CC\nC -> c";
	for (size_t i = 0; i < 10; ++i) {
		cout << FindReplace::generate("A", grammar) << endl;
	}
	cout << FindReplace::is_cfg(grammar) << endl;
	cout << FindReplace::is_cnf(grammar) << endl;
}
