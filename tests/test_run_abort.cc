#include <string>
#include <vector>
#include <unistd.h>

#include "ib/logger.h"
#include "ib/run.h"

using namespace std;
using namespace ib;

int main() {
	{
	Run run("sleep 10");
	run();
	Logger::info("result: %", run.read(20));
	}
	{
	Run run("sleep 10");
	run();
	Logger::info("result: %", run.read(5));
	}
	{
	Run run("cat /usr/share/dict/words | sleep 5");
	run();
	Logger::info("result: %", run.read(1));
	}
}
