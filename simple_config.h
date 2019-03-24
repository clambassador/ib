#ifndef __IB__SIMPLE_CONFIG__H__
#define __IB__SIMPLE_CONFIG__H__

#include <cassert>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "ib/base64.h"
#include "ib/logger.h"
#include "ib/tokenizer.h"

using namespace std;

namespace ib {

class SimpleConfig {
public:
	SimpleConfig() {}
	virtual ~SimpleConfig() {}

	int load_base64(const string& data) {
		stringstream ss;
		ss << Base64::decode(data);
		load(ss);
		return 0;
	}

	int load(istream& in) {
		if (!in.good()) return -1;
		while (in.good()) {
			string s;
			string name;
			string val;
			getline(in, s);
			if (s.empty()) continue;
			auto x = s.find(" ");

			if (x == string::npos) {
				Logger::error("(config) parse error %",
					      s);
				assert(0);
			}
			name = s.substr(0, x);
			val = s.substr(x + 1);
			if (name.empty()) continue;
			if (name.substr(0, 2) == "//")
				continue;
			if (_name_to_string.count(name)) {
				Logger::error("(config) duplicate config for %"
					      " was % and %",
					      name, _name_to_string[name],
					      val);
				assert(0);
			}
			_name_to_string[name] = val;
		}
		return 0;
	}

	template<typename T>
	string form_name(stringstream& ss, const T& value) const {
		ss << value;
		return ss.str();
	}

	template<typename T, typename... Args>
	string form_name(stringstream& ss, const T& value,
			 const Args&... args) const {
		ss << value << "_";
		return form_name(ss, args...);
	}

	template<typename... Args>
	string form_name(const Args&... args) const {
		stringstream ss;
		return form_name(ss, args...);
	}

	template<typename... Args>
	void set(const uint64_t& val, const Args&... args) {
		string name = form_name(args...);
		return set(name, val);
	}

	void set(const string& name, const uint64_t& val) {
		return _name_to_string[name].push_back(val);
	}

	template<typename... Args>
	string get(const Args&... args) const {
		string name = form_name(args...);
		return get(name);
	}

	string get(const string& name) const {
		if (_name_to_string.count(name) == 0) {
			return "";
		}
		return _name_to_string.at(name);
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

	void get_all(map<string, string>* name) {
		*name = _name_to_string;
	}

protected:
	SimpleConfig(const SimpleConfig&);
	map<string, string> _name_to_string;
};

}  // namespace ib

#endif  // __IB__CONFIG__H__
