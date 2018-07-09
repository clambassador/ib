#ifndef __IB__CSV_TABLE__H__
#define __IB__CSV_TABLE__H__

#include <cassert>
#include <cstdint>
#include <chrono>
#include <fstream>
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

	template<typename... Args>
	static string cut(const vector<string>& row, Args... args) {
		return cut_impl(row, args...).substr(1);
	}

protected:
	template<typename... Args>
	static string cut_impl(const vector<string>& row, int field, Args... args) {
		return "," + row[field] + cut_impl(row, args...);
	}

	static string cut_impl(const vector<string>& row) {
		return "";
	}

public:
	virtual void stream(const string& csv_file) {
		_in.open(csv_file);
		_mode = STREAM;

		int i = 1;
		try {
			string line;
			getline(_in, line);
			while (true) {
				string s = Formatting::csv_read(line, i++);
				_headers.push_back(s);
			}
		} catch (string s) {
		}
	}

	static int load_map(const string& csv_file, map<string, string>* out) {
		CSVTable table;
		table.stream(csv_file);
		assert(table.cols() == 2);
		vector<string> data;
		while (table.get_next_row(&data)) {
			assert(data.size() == 2);
			(*out)[data[0]] = data[1];
			data.clear();
		}
		return true;
	}

	virtual void load(const string& csv_file) {
		vector<string> lines;
		Fileutil::read_file(csv_file, &lines);
		_mode = LOAD;

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
		assert(_headers.size() == _columns.size());
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

	virtual size_t cols() const {
		return _headers.size();
	}

	virtual void get_primary_keys(set<size_t> *cols) const {
		for (size_t i = 0; i < _columns.size(); ++i) {
			set<string> s;
			get_column(i, &s);
			if (s.size() == _columns[i].size()) cols->insert(i);
		}
	}

	virtual void join(size_t col, const CSVTable& other, size_t other_col) {
		int start_pos = _columns.size();
		for (int j = 0; j < other.cols(); ++j) {
			if (j == other_col) continue;
			_columns.push_back(vector<string>());
			_columns.back().resize(rows());
			_headers.push_back(other.get_header(j));
		}
		for (int i = 0; i < other.rows(); ++i) {
			vector<string> row;
			other.get_row(i, &row);
			size_t found = _columns[col].size();
			for (int j = 0; j < _columns[col].size(); ++j) {
				if (_columns[col][j] == row[other_col]) {
					found = j;
					break;
				}
			}
			if (found == _columns[col].size()) {
				for (auto &x : _columns) {
					x.push_back("");
				}
				_columns[col].back() = row[other_col];
			}
			int pos = start_pos;
			for (int j = 0; j < row.size(); ++j) {
				if (j == other_col) continue;
				Logger::info("pos % j % found %", pos, j,
					     found);
				_columns[pos][found] = row[j];
				++pos;
			}
		}
	}

	virtual void get_column(size_t col, set<string>* data) const {
		for (size_t i = 0; i < _columns[col].size(); ++i) {
			data->insert(_columns[col][i]);
		}
	}

	virtual void get_row(size_t row, vector<string> *data) const {
		assert(_mode == LOAD);
		for (size_t i = 0; i < _headers.size(); ++i) {
			data->push_back(_columns.at(i).at(row));
		}
	}

	virtual bool get_next_row(vector<string>* data) {
		assert(_mode == STREAM);
		assert(data);

		while (_in.good()) {
			string s;
			try {
				getline(_in, s);
				for (int i = 0; i < _headers.size(); ++i) {
					data->push_back(
					    Formatting::csv_read(s, i + 1));
				}
				return true;
			} catch (string ex) {
				data->clear();
			}
		}
		return false;
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

	virtual string get_header(size_t column) const {
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

	enum {
		STREAM,
		LOAD
	};
	int _mode;
	ifstream _in;
};

}  // namespace ib

#endif  // __IB__CSV_TABLE__H__
