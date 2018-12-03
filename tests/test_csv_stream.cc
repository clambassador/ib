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

	CSVTable<true> table;
	table.stream("/tmp/test_csv_table");

	vector<string> data;
	assert(table.get_next_row(&data));
	for (int i = 0; i < 5; ++i) {
		assert(data[i] == Logger::stringify("%", i + 1));
	}
	data.clear();

	assert(table.get_next_row(&data));
	for (int i = 0; i < 5; ++i) {
		assert(data[i] == Logger::stringify("%", i + 2));
	}
	data.clear();

	assert(table.get_next_row(&data) == false);

}
