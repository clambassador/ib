#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "ib/csv_table.h"
#include "ib/fileutil.h"
#include "ib/formatting.h"
#include "ib/logger.h"
#include "ib/set.h"
#include "ib/tokenizer.h"
#include "ib/json.h"

using namespace std;
using namespace ib;

//LCOV_EXCL_START
int main(int argc, char** argv) {
	string s;
	if (argc > 2) {
		Logger::error("usage: % jsonfile", argv[0]);
		return -1;
	}
	if (argc == 1) {
		Fileutil::read_stream(cin, &s);
	} else {
		Fileutil::read_file(argv[1], &s);
	}
        JSON json;
        json.parse(s);
	json.trace(cout);
	return 0;
}
//LCOV_EXCL_STOP
