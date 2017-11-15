#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "csv_table.h"
#include "logger.h"
#include "formatting.h"

using namespace std;
using namespace ib;

#include <termios.h>
#include <unistd.h>
#include <stdio.h>

/* reads from keypress, doesn't echo */
int getch(void)
{
	    struct termios oldattr, newattr;
	        int ch;
		    tcgetattr( STDIN_FILENO, &oldattr );
		        newattr = oldattr;
			    newattr.c_lflag &= ~( ICANON | ECHO );
			        tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
				    ch = getchar();
				        tcsetattr( STDIN_FILENO, TCSANOW,
						  &oldattr );
					    return ch;
}

int main(int argc, char** argv) {
	assert(argc > 2);
	CSVTable table;
	table.load(argv[1]);
	int col = atoi(argv[2]) - 1;

	string colname = table.get_header(col);
	Logger::info("colname %", colname);
	map<string, string> replace;

	const vector<string>& data = table.project(col);
	vector<string> output;

	for (const auto &x : data) {
		if (replace.count(x)) {
			output.push_back(replace[x]);
		} else {
			char c;
			cout << "CODE:  " << x << " > " << flush;
			c = getch();
			cout << c << endl;
			string s;
			s += c;
			output.push_back(s);
			replace[x] = s;
		}
	}

	table.project(col, output);

	table.save("/tmp/test_csv_table_out");
}
