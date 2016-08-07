#include <string>
#include <vector>
#include <unistd.h>

#include "ib/logger.h"
#include "ib/run.h"

using namespace std;
using namespace ib;

int main() {
	{
	Run run("/home/jreardon/a.out -l toot weee", {"good"});
	run();
	run.wait();
	Logger::info("result: %", run.read());
	assert(!run.error());
	}
	{
	Run run("no way");
	run();
	run.wait();
	Logger::info("result: %", run.read());
	assert(run.error());
	}
}
