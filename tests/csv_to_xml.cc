#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "ib/csv_table.h"
#include "ib/fileutil.h"
#include "ib/formatting.h"
#include "ib/logger.h"
#include "ib/tokenizer.h"

using namespace std;
using namespace ib;

int main(int argc, char** argv) {
	if (argc < 2) {
		Logger::error("usage: % top-name", argv[0]);
		return -1;
	}
	vector<string> files, csvs;

	string filename = Fileutil::maybe_strip_extension("xml", argv[1]);
	ofstream fout(filename + ".xml");
	fout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	fout << "<" << filename << ">" << endl;

	Fileutil::list_directory(".", &files);
	for (const auto& x : files) {
		if (x.length() < 5) continue;
		if (x.substr(x.length() - 4) == ".csv") {
			csvs.push_back(x);
		}
	}

	for (const auto &x : csvs) {
		string name;
		Tokenizer::extract("%.csv", x, &name);
		Logger::info("found a csv: %", x);
		CSVTable table;
		table.load(x);

		vector<string> headers = table.headers();
		for (size_t i = 0; i < table.rows(); ++i) {
			fout << "\t<" << name << ">" << endl;
			vector<string> row;
			table.get_row(i, &row);
			for (size_t j = 0; j < row.size(); ++j) {
				if (headers[j].empty()) continue;
				fout << "\t\t<" << headers[j]
				     << ">" << row[j]
				     << "</" << headers[j]
				     << ">" << endl;
			}
			fout << "\t</" << name << ">" << endl;
		}
	}
	fout << "</" << filename << ">" << endl;
	Logger::info("Success. Created %.xml", filename);
}
