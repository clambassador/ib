#include <string>
#include <vector>
#include <unistd.h>

#include "ib/logger.h"
#include "ib/run.h"

using namespace std;
using namespace ib;

int main() {
	{
	Run run("/home/jreardon/a.out -l toot weee | /bin/grep _ | /usr/bin/wc", {"good"});
	run();
	Logger::info("result: %", run.read());
	}
	{
	Run run("no way");
	run();
	Logger::info("result: %", run.read());
	}
}
