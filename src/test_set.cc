#include <cassert>
#include<gtest/gtest.h>

#include "logger.h"
#include "set.h"


using namespace std;
using namespace ib;

TEST(Set, Set_Main){
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
        EXPECT_TRUE(c.count(i + 2));
	}

    EXPECT_TRUE(!Set::close(a, b, .81));
    EXPECT_TRUE(Set::close(a, b, .79));
}
