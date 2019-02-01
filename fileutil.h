#ifndef __IB__FILEUTIL__H__
#define __IB__FILEUTIL__H__

#include <sys/types.h>
#include <dirent.h>

#include <fstream>
#include <string>
#include <vector>

#include "ib/logger.h"

using namespace ib;
using namespace std;

namespace ib {

class Fileutil {
public:
	static int read_binary_file(const string& file_name,
			     string* output) {
		if (file_name.empty()) return -1;
		ifstream fin(file_name, ios::in | ios::binary);
		assert(fin.good());
		stringstream buffer;
		buffer << fin.rdbuf();
		assert(fin.good());
		*output = buffer.str();
		return 0;
	}

	static int read_file(const string& file_name,
			     string* output) {
		if (file_name.empty()) return -1;
		ifstream fin(file_name);
		if (!fin.good()) {
			*output = "";
			return -1;
		}
		stringstream buffer;
		buffer << fin.rdbuf();
		if (!fin.good()) Logger::error("failed to read: %", file_name);
		assert(fin.good());
		*output = buffer.str();
		return 0;
	}

	static string maybe_strip_extension(const string& extension,
					    const string& filename) {
		assert(!extension.empty());
		assert(extension[0] != '.');
		assert(!filename.empty());
		if (filename.length() > extension.length() &&
		    filename.substr(filename.length() - extension.length() - 1)
		    == extension) {
			return filename.substr(0, filename.length() -
					       extension.length() - 1);
		}

		return filename;
	}

	static bool exists(const string& file_name) {
		if (file_name.empty()) return false;
		if (safety() && !safe(file_name)) {
			throw "nice try";
		}
		ifstream fin(file_name);
		return fin.good();
	}

	static int read_file(const string& file_name,
			     set<string>* output) {
		if (file_name.empty()) return -1;
		if (safety() && !safe(file_name)) {
			throw "nice try";
		}
		ifstream fin(file_name);
		assert(fin.good());
		while (fin.good()) {
			string s;
			getline(fin, s);
			if (s.empty()) continue;
			output->insert(s);
		}
		return 0;
	}

	static int read_file(const string& file_name,
			     vector<string>* output) {
		if (file_name.empty()) return -1;
		if (safety() && !safe(file_name)) {
			throw "nice try";
		}
		ifstream fin(file_name);
		if (!fin.good()) Logger::error("error reading %", file_name);
		assert(fin.good());
		while (fin.good()) {
			string s;
			getline(fin, s);
			output->push_back(s);
		}
		if (output->size() && output->back().empty()) {
			output->pop_back();
		}
		return 0;
	}

	static int list_directory(const string& directory,
				  const string& extension,
				  vector<string>* files) {
		if (safety() && !safe(directory)) {
			throw "nice try";
		}
		DIR* dir = opendir(directory.c_str());
		if (!dir) return -1;

		struct dirent* result;
		while (true) {
			result = readdir(dir);
			if (!result) break;
			string name = result->d_name;
			if (name.length() < extension.length()) continue;
			if (name.substr(name.length() - extension.length())
			    == extension)
				files->push_back(name);
		}
		return 0;
	}

	static int list_directory(const string& directory,
				  vector<string>* files) {
		if (safety() && !safe(directory)) {
			throw "nice try";
		}
		DIR* dir = opendir(directory.c_str());
		if (!dir) return -1;

		struct dirent* result;
		while (true) {
			result = readdir(dir);
			if (!result) break;
			files->push_back(result->d_name);
		}
		return 0;
	}

	static int largest_filenumber(const vector<string>& files,
				      const string& extension) {
		int curmax = -1;
		for (auto &x : files) {
			if (x.length() < extension.length()) continue;
			if (x.substr(x.length() - extension.length())
			    == extension) {
				stringstream ss;
				ss << x.substr(0, extension.length() - 1);
				int num;
				ss >> num;
				if (num > curmax) {
					curmax = num;
				}
			}
		}
		return curmax;
	}

	static string file_dir(const string& file) {
		for (size_t i = file.size() - 1; i != 0; --i) {
			if (file[i] == '/') return file.substr(0, i);
		}
		return file;
	}

	static string realpath(const string& path) {
		unique_ptr<char> result;
		result.reset(::realpath(path.c_str(), nullptr));
		if (result.get()) return string(result.get());
		return "";
	}

	static bool safety() {
		return _safe;
	}

	static void unsafe() {
		_safe = false;
	}

	static void safe() {
		_safe = true;
	}

	static bool safe(const string& filename) {
		string file = realpath(file_dir(filename));
		string pwd = realpath(".");
		if (file.substr(0, pwd.length()) != pwd) return false;
		return true;
	}

	static bool _safe;


};

}   // namespace ib

#endif  // __IB__FILEUTIL__H__
