#ifndef __IB__CSV_TABLE__H__
#define __IB__CSV_TABLE__H__

#include <cassert>
#include <cstdint>
#include <chrono>
#include <thread>
#include <vector>

#include "ib/fileutil.h"
#include "ib/formatting.h"
#include "ib/logger.h"

using namespace std;

namespace ib {

class CSVTable {
public:
	CSVTable() {}

	virtual ~CSVTable() {}

	virtual void load(const string& csv_file) {
		vector<string> lines;
		Fileutil::read_file(csv_file, &lines);

		int i = 1;
		try {
			while (true) {
				string s = Formatting::csv_read(lines[0], i++);
				_headers.push_back(s);
				_columns.push_back(vector<string>());
			}
		} catch (string s) {
		}

		for (int line = 1; line < lines.size(); ++line) {
			string s = lines[line];
			for (int i = 0; i < _columns.size(); ++i) {
				_columns[i].push_back(
				    Formatting::csv_read(s, i + 1));
			}
		}
	}

	virtual void save(const string& csv_file) {
		start_write();
		ofstream fout(csv_file);
		if (_columns.empty()) return;

		for (size_t i = 0; i < _headers.size(); ++i) {
			csv_write(&fout, _headers[i]);
		}
		write_newline(&fout);
		for (size_t i = 0; i < _columns[0].size(); ++i) {
			for (size_t j = 0; j < _columns.size(); ++j) {
				csv_write(&fout, _columns[j][i]);
			}
			write_newline(&fout);
		}
	}


	virtual const vector<string>& project(size_t column) {
		assert(column < _columns.size());
		return _columns[column];
	}

	virtual void project(size_t column, const vector<string>& data) {
		assert(column < _columns.size());
		_columns[column] = data;
	}

	virtual string get_header(size_t column) {
		assert(column < _headers.size());
		return _headers[column];
	}

protected:
	virtual void start_write() {
		_newline = true;
	}
	virtual void write_newline(ofstream* fout) {
		assert(fout);
		*fout << endl;
		_newline = true;
	}

	virtual void csv_write(ofstream* fout, const string& data) {
		if (!_newline) *fout << ",";
		*fout << Formatting::csv_escape(data);
		_newline = false;
	}

	bool _newline;
	vector<string> _headers;
	vector<vector<string>> _columns;
};

}  // namespace ib

#endif  // __IB__CSV_TABLE__H__
