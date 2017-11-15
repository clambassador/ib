#include <string>
#include <vector>
#include <unistd.h>
#include <gtest/gtest.h>

#include "logger.h"
#include "run.h"

using namespace std;
using namespace ib;

TEST(RunAbort, RunAbort_Main) {
	{
    class Run run("sleep 10");
	run();
	Logger::info("result: %", run.read(20));
	}
	{
    class Run run("sleep 10");
	run();
	Logger::info("result: %", run.read(5));
	}
	{
    class Run run("cat /usr/share/dict/words | sleep 5");
	run();
	Logger::info("result: %", run.read(1));
	}
}
