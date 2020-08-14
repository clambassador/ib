#ifndef __IB__JSON__H__
#define __IB__JSON__H__

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "ib/json_scanner.h"
#include "ib/logger.h"
#include "ib/tokenizer.h"

using namespace std;

namespace ib {

class JSON {
public:
	enum type {
		NONE,
		ARRAY,
		OBJECT,
		STRING
	};

	operator string() const {
		if (_type == STRING) return _val;
		stringstream ss;
		trace(ss);
		return ss.str();
	}

	bool operator==(const string& val) {
		return ((string) *this) == val;
	}

	bool operator!=(const string& val) {
		return !(*this == val);
	}

	static string type_name(type t) {
		if (t == NONE) return "none";
		if (t == ARRAY) return "array";
		if (t == OBJECT) return "object";
		if (t == STRING) return "string";
		assert(0);
	}

	JSON() : _type(NONE), _name(""), _fqname(""), _depth(0) {
	}

	JSON(const string& name, const string& fqname, size_t depth)
		: _type(NONE), _name(name), _fqname(fqname), _depth(depth) {
	}

	JSON& operator=(const string& other) {
		check_type(STRING, "assign to " + other);

		_val = other;
		return *this;
	}

	JSON& operator[](size_t idx) {
		check_type(ARRAY, Logger::stringify("index at %", idx));
		if (_array.count(idx)) return *_array[idx].get();

		string name = Logger::stringify("[%]", idx);
		_array[idx].reset(new JSON("", _fqname + name, _depth + 1));
		_iter.push_back(*_array[idx].get());
		return *_array[idx].get();
	}

	void parse(const string& data) {
		check_type(NONE, "parse data on known type");

		vector<tuple<string, string, size_t>> tokens;
		JSONScanner::scan(data, &tokens);
		parse(data, tokens, 0);
	}

	size_t parse_arrayitem(
			const string& data,
			const vector<tuple<string, string, size_t>>& tokens,
			size_t pos) {
		pos = (*this)[length()].parse(data, tokens, pos);
		if (get<0>(tokens[pos]) == "comma") ++pos;
		return pos;
	}

	size_t parse_objectitem(
			const string& data,
			const vector<tuple<string, string, size_t>>& tokens,
			size_t pos) {
		string name = Tokenizer::trim(get<1>(tokens[pos++]), "\"");
		if (get<0>(tokens[pos]) != "colon") {
			Logger::error("parse error for % at token % expected colon: % %",
				      name, pos, get<0>(tokens[pos]),
				      get<1>(tokens[pos]));
			return pos;
		}

		pos = (*this)[name].parse(data, tokens, pos + 1);
		if (get<0>(tokens[pos]) == "comma") ++pos;
		return pos;
	}

	size_t parse(const string& data,
	 	     const vector<tuple<string, string, size_t>>& tokens,
		     size_t pos) {
		if (get<0>(tokens[pos]) == "begin_array") {
			_type = ARRAY;
			++pos;
			while (get<0>(tokens[pos]) != "end_array") {
				pos = parse_arrayitem(data, tokens, pos);
			}
			return pos + 1;
		} else if (get<0>(tokens[pos]) == "begin_object") {
			_type = OBJECT;
			++pos;
			while (get<0>(tokens[pos]) != "end_object") {
				pos = parse_objectitem(data, tokens, pos);
			}
			return pos + 1;
		} else if (get<0>(tokens[pos]) == "expression" ||
			   get<0>(tokens[pos]) == "quote_expression") {
			_type = STRING;
			_val = Tokenizer::trim(get<1>(tokens[pos]), "\"");
			return pos + 1;
		}
		Logger::error("parse error at token %. expected expression "
			      "or beginning of array or object. got % %",
			      pos, get<0>(tokens[pos]),
			      get<1>(tokens[pos]));
		return pos + 1;
	}

	void erase(size_t idx) {
		check_type(ARRAY, Logger::stringify("erase %", idx));
		_array.erase(idx);
	}

	JSON& operator[](const string& key) {
		check_type(OBJECT, "get field " + key);

		if (_fields.count(key)) return *_fields[key].get();
		_fields[key].reset(new JSON(key, _fqname + "." + key, _depth + 1));
		_iter.push_back(*_fields[key].get());
		return *_fields[key].get();
	}

	void check_type(type expected, const string& error) {
		if (_type == NONE) _type = expected;
		if (_type != expected) {
			throw "json type error: is " + type_name(_type)
			    + " wanted " + type_name(expected) + " extra: " + error;
		}
	}

	void check_type_nocoerce(type expected, const string& error) const {
		if (_type == NONE && _type != expected) {
			throw "json type error: is " + type_name(_type)
			    + " wanted " + type_name(expected) + " extra: " + error;
		}
	}

	void trace_depth(ostream& os) const {
		for (size_t i = 0; i < _depth; ++i) {
			os << "  ";
		}
	}

	size_t length() const {
		check_type_nocoerce(ARRAY, "length on non-array");

		if (_array.size() == 0) return 0;
		auto x = _array.end();
		--x;
		return 1 + x->first;
	}

	void csvify(const string& prefix, ostream& os) const {
		if (_type == NONE) {
			os << prefix << ",{}" << endl;
		}
		else if (_type == STRING) {
			os << prefix << "," << _val << endl;
		} else if (_type == ARRAY) {
			if (!length()) {
				os << prefix << ",[]" << endl;
			} else {
				for (size_t i = 0; i < length(); ++i) {
					_array.at(i)->csvify(Logger::stringify(
						"%_[%]", prefix, i), os);
				}
			}
		} else if (_type == OBJECT) {
			if (_fields.size() == 0) {
				os << prefix << ",{}" << endl;
			} else {
				for (const auto &x : _fields) {
					x.second->csvify(Logger::stringify("%_%", prefix,
								 x.first), os);
				}
			}
		}
	}

	void trace(ostream& os) const {
		trace_depth(os);
		if (!_name.empty())
			os << Logger::stringify("\"%\": ", _name);

		if (_type == NONE) {
			os << "{},";
		}
		else if (_type == STRING) {
			os << "\"" << escape(_val) << "\"," << endl;
		} else if (_type == ARRAY) {
			if (!length()) {
				os << "[]," << endl;
			} else {
				os << "[" << endl;
				for (size_t i = 0; i < length(); ++i) {
					if (!_array.count(i)) {
						trace_depth(os);
						os << "  {}," << endl;
					} else {
						_array.at(i)->trace(os);
					}
				}
				trace_depth(os);
				os << "]," << endl;
			}
		} else if (_type == OBJECT) {
			os << "{" << endl;
			for (const auto &x : _fields) {
				x.second->trace(os);
			}
			trace_depth(os);
			os << "}," << endl;
		}
	}

	string escape(const string& input) const {
		return input;
	}

	vector<reference_wrapper<JSON>>::iterator begin() {
		return _iter.begin();
	}

	vector<reference_wrapper<JSON>>::iterator end() {
		return _iter.end();
	}

	bool has(const string& field) const {
		if (_type != OBJECT) return false;
		if (_fields.count(field) == 0) return false;
		return true;
	}

	friend ostream& operator<<(ostream &out, const JSON& o);
protected:
	type _type;
	string _name;
	string _fqname;
	string _val;
	size_t _depth;

	map<size_t, unique_ptr<JSON> > _array;
	map<string, unique_ptr<JSON> > _fields;

	vector<reference_wrapper<JSON>> _iter;

};

std::ostream& operator<<(std::ostream& os, const JSON& o) {
	if (o._type == JSON::STRING) os << o._val;
	else o.trace(os);
	return os;
}

}  // namespace ib

#endif  // __IB__JSON__H__
