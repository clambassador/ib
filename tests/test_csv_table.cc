#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "ib/csv_table.h"
#include "ib/logger.h"
#include "ib/formatting.h"

using namespace std;
using namespace ib;

int main() {
	ofstream fout("/tmp/test_csv_table");
	fout << "foo,bar,baz,biff,borked" << endl;
	fout << "1,2,3,4,5" << endl;
	fout << "2,3,4,5,6" << endl;
	fout.close();

	CSVTable table;
	table.load("/tmp/test_csv_table");

	vector<string> vs = table.project(2);
	vs[0] = "120";
	vs[1] = "121";
	table.project(2, vs);

	table.save("/tmp/test_csv_table_out");
}
