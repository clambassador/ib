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
	table.load("/tmp/test_csv_table");

	vector<string> vs = table.project(2);
	vs[0] = "120";
	vs[1] = "121";
	table.project(2, vs);

	table.save("/tmp/test_csv_table_out");

	{
	ofstream fout("/tmp/test_csv_table");
	fout << "1,2,3,4,5" << endl;
	fout << "2,2,2,4,5" << endl;
	fout << "3,2,1,4,5" << endl;
	fout << "4,3,4,5,6" << endl;
	fout.close();

	CSVTable<false> table;
	table.stream("/tmp/test_csv_table");
	for (size_t i = 0; i < 4; ++i) {
		vector<string> data;
		table.get_next_row(&data);
		Logger::info("% %", i+1, data);
		assert(data[0] == Logger::stringify("%", i+1));

	}
	}
	{
	ofstream fout("/tmp/test_csv_table");
	fout << "1,2,3,4,5" << endl;
	fout << "2,2,2,4,5" << endl;
	fout << "3,2,1,4,5" << endl;
	fout << "4,3,4,5,6" << endl;
	fout.close();

	CSVTable<false> table;
	table.stream("/tmp/test_csv_table");
	for (size_t i = 0; i < 5; ++i) {
		vector<string> data;
		table.get_next_row(&data);
		if (i == 2) {
			table.set_next_row(data);
		}
		Logger::info("% %", i+1, data);
		assert(data[0] == Logger::stringify("%", i <= 2 ? i+1 : i));

	}
	}

}
