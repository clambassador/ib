#include <cassert>

#include "ib/logger.h"
#include "ib/set.h"

using namespace std;
using namespace ib;

int main() {
	set<int> a, b;
	for (int i = 0; i < 10; ++i) {
		a.insert(i);
	}
	for (int i = 0; i < 10; ++i) {
		b.insert(i + 2);
	}

	set<int> c;
	Set::intersect(a, b, &c);
	for (int i = 0; i < 8; ++i) {
		assert(c.count(i + 2));
	}

	assert(!Set::close(a, b, .81));
	assert(Set::close(a, b, .79));
}
