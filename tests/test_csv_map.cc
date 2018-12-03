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
	{
	ofstream fout("/tmp/test_csv_table");
	fout << "key,value" << endl;
	fout << "key1,val1" << endl;
	fout << "key2,val2" << endl;
	fout << "key3,val3" << endl;
	fout << "key4,val4" << endl;
	fout.close();
	}

	map<string, string> table;
	CSVTable<true>::load_map("/tmp/test_csv_table", &table);
	assert(table.size() == 4);
	assert(table["key1"] == "val1");
	assert(table["key2"] == "val2");
	assert(table["key3"] == "val3");
	assert(table["key4"] == "val4");

	{
	ofstream fout("/tmp/test_csv_table");
	fout << "key,value" << endl;
	fout.close();
	}

	table.clear();
	CSVTable<true>::load_map("/tmp/test_csv_table", &table);
	assert(table.size() == 0);
}
