#include "ib/base64.h"
#include "ib/config.h"
#include "ib/logger.h"

using namespace ib;
using namespace std;

int main(int argc, char** argv) {
	if (argc == 2) {
		Config cfg;
		cfg.load_from_arg(Base64::decode(argv[1]));
	}

	Config::_()->load("tests/resources/", "test_config");

	assert(Config::_()->get("big", "1", "boaty") == 2);
	assert(Config::_()->get("regular", "cars") == 4);
	assert(Config::_()->gets("regular", "boots") == "boats");

	assert(Config::_()->get("outer", "prepop") == 4);
	assert(Config::_()->get("outer", "postpop") == 6);

	assert(Config::_()->get("big", "1", "boaty") == 2);

	auto x = Config::_()->get_list("list", "double", "numbers");
	auto y = x.begin();
	assert(*y++ == 2);
	assert(*y++ == 4);
	assert(*y++ == 6);
	assert(y == x.end());

	assert(Config::_()->get("outer", "inner", "compound_name") == 10);
	assert(Config::_()->get("outer", "inner", "compound", "name") == 10);

	assert(Config::_()->get("dropped", "item") == 1);
	assert(Config::_()->get("dropped", "subitem") == 6);
	assert(Config::_()->get("subitem") == 5);

	assert(Config::_()->gets("subconf", "item") == "good_news");
	assert(Config::_()->gets("subconf", "item", "split") == "good news");

	assert(Config::_()->get("working") == 1);

	Config::_()->save("tests/resources/test_config_out");
}


