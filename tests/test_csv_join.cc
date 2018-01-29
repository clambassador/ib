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
	ofstream fout("/tmp/test_csv_table1");
	fout << "foo,bar" << endl;
	fout << "1,2" << endl;
	fout << "2,3" << endl;
	fout << "3,3" << endl;
	fout << "4,3" << endl;
	fout.close();
	fout.open("/tmp/test_csv_table2");
	fout << "doop,baz" << endl;
	fout << "1,a" << endl;
	fout << "2,a" << endl;
	fout << "3,b" << endl;
	fout << "4,b" << endl;
	fout.close();
	fout.open("/tmp/test_csv_table3");
	fout << "loo,doop,baz" << endl;
	fout << "aa,2,aa" << endl;
	fout << "aaa,3,bbb" << endl;
	fout << "aaaaa,5,ccccc" << endl;
	fout.close();

	CSVTable t1;
	t1.load("/tmp/test_csv_table1");
	CSVTable t2;
	t2.load("/tmp/test_csv_table2");
	CSVTable t3;
	t3.load("/tmp/test_csv_table3");

	t1.join(0, t2, 0);
	t1.save("/tmp/test_csv_table_out");
	t1.join(0, t3, 1);
	t1.save("/tmp/test_csv_table_out2");
}
