#include <string>
#include <vector>
#include <unistd.h>

#include "ib/limiter.h"
#include "ib/logger.h"

using namespace std;
using namespace ib;

int main() {
	Limiter l5s1(0,0,0,5);
	Limiter l5s2(10,10,0,5,0,0);
	Limiter l1s1(0,0,10);
	Limiter l1s2(100,0,0,1);

	while (!l1s1());
	Logger::info("about a second");
	while (!l1s2());
	Logger::info("about a second");
	while (!l5s1());
	Logger::info("about five seconds");
	while (!l5s2());
	Logger::info("about five seconds");
}
