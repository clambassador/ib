#include <string>
#include <vector>
#include <unistd.h>

#include "ib/fileutil.h"
#include "ib/logger.h"
#include "ib/fsm.h"

using namespace std;
using namespace ib;

int main(int argc, char** argv) {
	if (argc != 3) {
		Logger::error("usage % rulefile word", argv[0]);
		return -1;
	}

	string w = argv[2];
	vector<string> lines;
	Fileutil::read_file(argv[1], &lines);

	FSM fsm(lines);
	for (size_t i = 0; i < w.length(); ++i) {
		fsm.process(w.substr(i, 1));
	}
	Logger::info("state of machine after %: %", w, fsm.state());
}
