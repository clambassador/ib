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
	static int read_file(const string& file_name,
			     string* output) {
		if (file_name.empty()) return -1;
		ifstream fin(file_name);
		assert(fin.good());
		stringstream buffer;
		buffer << fin.rdbuf();
		assert(fin.good());
		*output = buffer.str();
		return 0;
	}

	static int read_file(const string& file_name,
			     vector<string>* output) {
		if (file_name.empty()) return -1;
		ifstream fin(file_name);
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
				  vector<string>* files) {
		DIR* dir = opendir(directory.c_str());
		if (!dir) return -1;

		struct dirent entry;
		struct dirent* result;
		while (!readdir_r(dir, &entry, &result)) {
			if (!result) break;
			Logger::info("file name %", entry.d_name);
			files->push_back(entry.d_name);
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
				Logger::info("file % num %", x, num);
				if (num > curmax) {
					curmax = num;
				}
			}
		}
		return curmax;
	}
};

}   // namespace ib

#endif  // __IB__FILEUTIL__H__
