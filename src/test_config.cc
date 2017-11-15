#include "config.h"
#include <gtest/gtest.h>

using namespace ib;
using namespace std;

TEST(Config, Config_Main) {

	Config::_()->load("tests/resources/", "test_config");

    EXPECT_TRUE(Config::_()->get("big", "1", "boaty") == 2);
    EXPECT_TRUE(Config::_()->get("regular", "cars") == 4);
    EXPECT_TRUE(Config::_()->gets("regular", "boots") == "boats");

    EXPECT_TRUE(Config::_()->get("outer", "prepop") == 4);
    EXPECT_TRUE(Config::_()->get("outer", "postpop") == 6);

    EXPECT_TRUE(Config::_()->get("big", "1", "boaty") == 2);

	auto x = Config::_()->get_list("list", "double", "numbers");
	auto y = x.begin();
    EXPECT_TRUE(*y++ == 2);
    EXPECT_TRUE(*y++ == 4);
    EXPECT_TRUE(*y++ == 6);
    EXPECT_TRUE(y == x.end());

    EXPECT_TRUE(Config::_()->get("outer", "inner", "compound_name") == 10);
    EXPECT_TRUE(Config::_()->get("outer", "inner", "compound", "name") == 10);

    EXPECT_TRUE(Config::_()->get("dropped", "item") == 1);
    EXPECT_TRUE(Config::_()->get("dropped", "subitem") == 6);
    EXPECT_TRUE(Config::_()->get("subitem") == 5);

    EXPECT_TRUE(Config::_()->gets("subconf", "item") == "good_news");
    EXPECT_TRUE(Config::_()->gets("subconf", "item", "split") == "good news");

    EXPECT_TRUE(Config::_()->get("working") == 1);

	Config::_()->save("tests/resources/test_config_out");
}


