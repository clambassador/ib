#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "ib/csv_table.h"
#include "ib/logger.h"
#include "ib/formatting.h"

using namespace std;
using namespace ib;

int main(int argc, char** argv) {
	assert(argc == 4);
	string file = argv[1];
	string name = argv[2];
	string val = argv[3];
	CSVTable table;
	table.load(argv[1]);

	vector<string> vs = table.project(0);
	for (int i = 0; i < vs.size(); ++i) {
		vs[i] = val;
	}
	table.insert_column(0, name, vs);
	table.save(file);
}
