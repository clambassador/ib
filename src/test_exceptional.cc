#include <string>
#include <vector>
#include <unistd.h>
#include <gtest/gtest.h>

#include "logger.h"
#include "exceptional.h"

using namespace std;
using namespace ib;

TEST(Exceptional, Exceptional_Main) {
    EXPECT_TRUE(1 < 2);
    EXPECT_TRUE(5 == 5) <<  "yes";

	try {
		THROW("hello %", "world");
	} catch (string s) {
        EXPECT_TRUE(s == "hello world");
	}

	try {
		THROW()
	} catch (string s) {
        EXPECT_TRUE(s == "");
	}

	LOG_IF(true, "hello");
	LOG_IF(5 < 3, "world");
	LOG_IF(5 > 3, "there");

    EXPECT_TRUE(5 < 3) << "Only panic if % is less than %", 5 , 3;
}
