#include "ib/base64.h"
#include "ib/simple_config.h"
#include "ib/logger.h"

using namespace ib;
using namespace std;

int main(int argc, char** argv) {
	SimpleConfig cfg;
	cfg.load_base64("aGlnaCB0aGVyZQpiaWcgZGVhbApmdW4gb2theSBkb2tle"
			"SBnb29kIHNvIGJlIGl0CmEgMwpiIDMwNDkyMwplIDM0MjNlCg==");
	map<string, string> m;
	cfg.get_all(&m);
	Logger::info("%", m);
	assert(cfg.get("high") == "there");
	assert(cfg.get("a") == "3");
	assert(cfg.get("b") == "304923");
	assert(cfg.get("c") == "");
	assert(cfg.get("fun") == "okay dokey good so be it");
}


