#ifndef __IB__CONFIG__H__
#define __IB__CONFIG__H__

#include <cassert>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "ib/logger.h"
#include "ib/tokenizer.h"

using namespace std;

namespace ib {

class Config {
public:
	Config() {}
	virtual ~Config() {}

	static Config* _() {
		if (_impl.get() == nullptr) {
			_impl.reset(new Config());
		}
		return _impl.get();
	}

	int load(const string& config_filename) {
		assert(!config_filename.empty());
		if (config_filename[0] == '/') return load("/.", config_filename);

		return load(".", config_filename);
	}

	int load(const string& directory, const string& config_filename) {
		ifstream fin(directory + "/" + config_filename, ios::in);
		vector<string> prefices;
		prefices.push_back("");
		if (!fin.good()) return -1;
		while (fin.good()) {
			string name;
			uint64_t val;
			fin >> name;
			if (name.empty()) continue;

			if (name.substr(0, 2) == "//")
				continue;

			if (name.substr(0, 8) == "#include") {
				string file;
				fin >> file;
				load(directory, file);
				continue;
			}

			if (name.substr(0, 6) == "prefix") {
				string prefix;
				fin >> prefix;
				prefices.push_back(
					prefices.back() +
					prefix + "_");
				string temp;
				fin >> temp;
				assert(temp == "{");
				continue;
			}
			if (name[0] == '}') {
				prefices.pop_back();
				assert(prefices.size());
				continue;
			}
			if (name == "string") {
				string sval;
				fin >> name;
				getline(fin, sval);
				sval = Tokenizer::trim(sval);
				_name_to_string[prefices.back() + name] = sval;
			} else {
				fin >> val;
				_name_to_list[prefices.back() + name].push_back(val);
			}
		}
		return 0;
	}

	int save(const string& config_filename) {
		ofstream fout(config_filename, ios::out);
		if (!fout.good()) return -1;

		for (auto &x : _name_to_list) {
			for (uint64_t& val : x.second) {
				fout << x.first << " " << val << endl;
			}
		}

		for (auto &x : _name_to_string) {
			fout << "string " << x.first
			     << " " << x.second << endl;
		}
		fout.close();
		return 0;
	}

	template<typename T>
	string form_name(stringstream& ss, const T& value) {
		ss << value;
		return ss.str();
	}

	template<typename T, typename... Args>
	string form_name(stringstream& ss, const T& value, const Args&... args) {
		ss << value << "_";
		return form_name(ss, args...);
	}

	template<typename... Args>
	string form_name(const Args&... args) {
		stringstream ss;
		return form_name(ss, args...);
	}

	template<typename... Args>
	void set(const uint64_t& val, const Args&... args) {
		string name = form_name(args...);
		return set(name, val);
	}

	void set(const string& name, const uint64_t& val) {
		return _name_to_list[name].push_back(val);
	}

	template<typename... Args>
	string gets(const Args&... args) {
		string name = form_name(args...);
		return gets(name);
	}

	string gets(const string& name) {
		if (_name_to_string.count(name) == 0) {
			return "";
		}
		return _name_to_string[name];
	}

	template<typename... Args>
	uint64_t get(const Args&... args) {
		string name = form_name(args...);
		return get(name);
	}

	uint64_t get(const string& name) {
		if (_name_to_list.count(name) == 0) {
			return 0;
		}
		return _name_to_list[name].back();
	}

	template<typename... Args>
	const list<uint64_t>& get_list(const Args&... args) {
		string name = form_name(args...);
		return get_list(name);
	}

	const list<uint64_t>& get_list(const string& name) {
		return _name_to_list[name];
	}

	void get_all(const string& prefix, map<string, string>* name) {
		for (auto &x : _name_to_string) {
			if (x.first.length() <= prefix.length()) continue;
			if (x.first.at(prefix.length()) != '_') continue;
			if (x.first.substr(0, prefix.length()) == prefix) {
				(*name)[x.first.substr(prefix.length() + 1)] =
				    x.second;
			}
		}
	}

protected:


	Config(const Config&);

	map<string, list<uint64_t> > _name_to_list;
	map<string, string> _name_to_string;
	static unique_ptr<Config> _impl;

};

}  // namespace ib

#endif  // __IB__CONFIG__H__
