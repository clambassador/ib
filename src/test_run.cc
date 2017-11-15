#include <string>
#include <vector>
#include <unistd.h>
#include <gtest/gtest.h>

#include "logger.h"
#include "run.h"

using namespace std;
using namespace ib;

TEST(Run, Run_Main) {
    {
    class Run run("head /usr/share/dict/words | grep A | wc -l");
	run();
	Logger::info("result: %", run.redirect("/tmp/garbage"));
    }
	{
    class Run run("cat /tmp/garbage");
	run();
	Logger::info("result: %", run.read());
	}
	{
    class Run run("wc", "hello there");
	run();
	Logger::info("result: %", run.read());
	}
	{
    class Run run("no working");
	try {
		run();
	} catch (string s) {
		Logger::info("caught %", s);
	}
	}

	{
    class Run run("cat /usr/share/dict/words | grep B | wc -l");
	run();
	Logger::info("result: %", run.read());
	}

	{
    class Run run("cat /usr/share/dict/words | cat | cat | cat");
	run();
	Logger::info("result (length): %", run.read().length());
	}
}
