#include <string>
#include <vector>
#include <unistd.h>

#include "ib/base64.h"
#include "ib/fileutil.h"
#include "ib/logger.h"
#include "ib/tokenizer.h"
#include "ib/fsm.h"

using namespace std;
using namespace ib;

int main(int argc, char** argv) {
	if (argc != 3) {
		Logger::error("usage % rulesb64 word", argv[0]);
		return -1;
	}

	string m = argv[1];
	string w = argv[2];
	m = Base64::decode(m);
	vector<string> lines;
	Tokenizer::split_with_empty(m, "\n", &lines);
	FSM fsm(lines);
	for (size_t i = 0; i < w.length(); ++i) {
		fsm.process(w.substr(i, 1));
	}
	if (fsm.is_accept(fsm.state())) {
		cout << "accept" << endl;
		return 1;
	} else {
		cout << "reject" << endl;
		return 0;
	}
}
