#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <gtest/gtest.h>

#include "csv_table.h"
#include "logger.h"
#include "formatting.h"

using namespace std;
using namespace ib;

TEST(CSVStream, CSVStream_Main){
	ofstream fout("/tmp/test_csv_table");
	fout << "foo,bar,baz,biff,borked" << endl;
	fout << "1,2,3,4,5" << endl;
	fout << "2,3,4,5,6" << endl;
	fout.close();

	CSVTable table;
	table.stream("/tmp/test_csv_table");

	vector<string> data;
    EXPECT_TRUE(table.get_next_row(&data));
	for (int i = 0; i < 5; ++i) {
        EXPECT_TRUE(data[i] == Logger::stringify("%", i + 1));
	}
	data.clear();

    EXPECT_TRUE(table.get_next_row(&data));
	for (int i = 0; i < 5; ++i) {
        EXPECT_TRUE(data[i] == Logger::stringify("%", i + 2));
	}
	data.clear();

    EXPECT_TRUE(table.get_next_row(&data) == false);

}
