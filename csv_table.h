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
			try {
				for (int i = 0; i < _columns.size(); ++i) {
					_columns[i].push_back(
					    Formatting::csv_read(s, i + 1));
				}
			} catch (string s) {
				size_t len = _columns[_columns.size() - 1].size();
				for (int i = 0; i < _columns.size(); ++i) {
					if (_columns[i].size() > len) {
						_columns[i].pop_back();
					}
				}
			}
		}
	}

	virtual bool save(const string& csv_file) {
		start_write();
		ofstream fout(csv_file);
		if (_columns.empty()) return false;

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
		return true;
	}

	virtual const vector<string>& project(size_t column) const {
		assert(column < _columns.size());
		return _columns.at(column);
	}

	virtual size_t rows() const {
		return _columns.at(0).size();
	}

	virtual void get_row(size_t row, vector<string> *data) const {
		for (size_t i = 0; i < _headers.size(); ++i) {
			data->push_back(_columns.at(i).at(row));
		}
	}

	virtual void project(size_t column, const vector<string>& data) {
		assert(column < _columns.size());
		_columns[column] = data;
	}

	virtual void insert_column(size_t column,
				   const string& name,
				   const vector<string>& data) {
		auto x = _headers.begin();
		auto y = _columns.begin();
		for (int i = 0; i < column; ++i, ++x, ++y);
		_headers.insert(x, name);
		_columns.insert(y, data);
	}

	virtual string get_header(size_t column) {
		assert(column < _headers.size());
		return _headers.at(column);
	}

	virtual const vector<string>& headers() const {
		return _headers;
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
