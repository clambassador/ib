#include <string>
#include <vector>
#include <unistd.h>

#include "ib/logger.h"
#include "ib/run.h"

using namespace std;
using namespace ib;

int main() {
	{
	Run run("head /usr/share/dict/words | grep A | wc -l");
	run();
	Logger::info("result: %", run.redirect("/tmp/garbage"));
	}
	{
	Run run("cat /tmp/garbage");
	run();
	Logger::info("result: %", run.read());
	}
	{
	Run run("wc", "hello there");
	run();
	Logger::info("result: %", run.read());
	}
	{
	Run run("no working");
	try {
		run();
	} catch (string s) {
		Logger::info("caught %", s);
	}
	}

	{
	Run run("cat /usr/share/dict/words | grep B | wc -l");
	run();
	Logger::info("result: %", run.read());
	}

	{
	Run run("cat /usr/share/dict/words | cat | cat | cat");
	run();
	Logger::info("result (length): %", run.read().length());
	}
}
