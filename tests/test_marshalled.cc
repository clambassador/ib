#include "ib/marshalled.h"

#include <string>
#include <vector>
#include <unistd.h>

using namespace std;
using namespace ib;

class bigone {
 public:
	bigone() {
		_a = 0;
		_d = 0;
	}

	void init() {
		_a = 3;
		_b = "super";
		_c = "well hold on a minute";
		_d = -1;
	}

	void marshal(Marshalled* ml) const {
		ml->push(_a, _b, _c, _d);
	}

	void demarshal(Marshalled* ml) {
		ml->pull(&_a, &_b, &_c, &_d);
	}

	bool operator==(const bigone& other) const {
		if (_a != other._a) return false;
		if (_b != other._b) return false;
		if (_c != other._c) return false;
		if (_d != other._d) return false;
		return true;
	}

 protected:
	uint8_t _a;
	string _b;
	string _c;
	int64_t _d;
};

int main() {
	uint64_t a, aa;
	uint32_t b, bb;
	vector<string> c, cc;
	uint16_t d, dd;

	a = 53;
	b = 2;
	c.push_back("hello");
	c.push_back("there");
	c.push_back("goood");
	c.push_back("sirs!");
	d = 42;

	Marshalled ml(a, b, c, d);

	Marshalled out;
	out.data(ml.str());

	out.pull(&aa, &bb, &cc, &dd);
	assert(aa == a);
	assert(bb == b);
	assert(cc == c);
	assert(dd == d);

	bb++;
	bigone e;
	e.init();
	Marshalled ml2(bb, a, e);
	Marshalled ml3(b, a, e);

	Marshalled out2, out3;
	out2.data(ml2.str());
	out3.data(ml3.str());

	uint32_t bbb2, bbb3;
	uint64_t aa2, aa3;
	bigone ee2, ee3;
	out2.pull(&bbb2);
	out3.pull(&bbb3);
	out2.pull(&aa2);
	out3.pull(&aa3);
	out2.pull(&ee2);
	out3.pull(&ee3);
	assert(bbb2 == bbb3 + 1);
	assert(aa2 == aa3);
	assert(a == aa2);
	assert(ee2 == ee3);
	assert(e == ee2);

	Marshalled bad((uint64_t) 16, (uint64_t) 4, 453, 4, 34, 654);
	Marshalled badout;
	badout.data(bad.str());
	string s;
	badout.pull(&s);
	try {
		badout.pull(&s);
	} catch (int e) {
		Logger::info("buffer overrun: at 32 of 32 read 2808908611618");
		Logger::info("^^ should match.");
	}


}
