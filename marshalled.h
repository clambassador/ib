#ifndef __IB__MARSHALLED__H__
#define __IB__MARSHALLED__H__

#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "ib/logger.h"

using namespace std;

namespace ib {

/* Marshalled is an object that stores data in a binary format. It exists either
 * in a read (pull) setup or a write (push) setup. It takes arbitary number of
 * parameters to construct and marshals all of them. Use str() to get the
 * representation out of it. To put it in the read configuration, call data()
 * and pass it a string created by a str() call. Then pull() takes arbitrary
 * parameters to remove data and puts it into pointers to the corresponding
 * type.
 *
 * I choose not to support endian-ness. I believe in big-endian.
 *
 * Type info is not included, but range checks are used. It throws on bad data.
 * It may be useful to add type info to the representation when debugging.
 */

class Marshalled {
 public:
	template<typename... Args>
	Marshalled(Args... args) {
		push(args...);
	}

	virtual ~Marshalled() {
	}

	void pull() {}
	void push() {}

	template<typename CAR, typename... CDR>
	void pull(CAR* car, CDR*... cdr) {
		pull(car);
		pull(cdr...);
	}

	virtual void data(const string& data) {
		_data = data;
		_pos = 0;
	}

	void pull(string* data) {
		size_t len;
		pull(&len);
		safety(len);
		*data = _data.substr(_pos, len);
		_pos += len;
	}

	template<typename T>
	void pull(vector<T>* items) {
		size_t len;
		pull(&len);
		for (size_t i = 0; i < len; ++i) {
			items->push_back(T());
			pull(&items->back());
		}
	}

	template<typename T>
	void pull(set<T>* items) {
		size_t len;
		pull(&len);
		for (size_t i = 0; i < len; ++i) {
			T t;
			pull(&t);
			items->insert(t);
		}
	}

	template<typename T, typename R>
	void pull(const map<T, R>& items) {
		size_t len;
		pull(&len);
		for (size_t i = 0; i < len; ++i) {
			T t;
			R r;
			pull(&t);
			pull(&r);
			items->insert(t, r);
		}
	}

	template<typename T>
	void pull(list<T>* items) {
		size_t len;
		pull(&len);
		for (size_t i = 0; i < len; ++i) {
			items->push_back(T());
			pull(&items->back());
		}
	}

	template<typename T>
	void pull(T* marshallable) {
		marshallable->demarshal(this);
	}

	/* I choose not to support endianness.
	   Only use this if you have big-endian architecture.
	 */
	void pull(uint64_t* value) {
		safety(sizeof(*value));
		*value = *reinterpret_cast<const uint64_t*>(_data.c_str() + _pos);
		_pos += sizeof(*value);
	}

	void pull(uint32_t* value) {
		safety(sizeof(*value));
		*value = *reinterpret_cast<const uint32_t*>(_data.c_str() + _pos);
		_pos += sizeof(*value);
	}

	void pull(uint16_t* value) {
		safety(sizeof(*value));
		*value = *reinterpret_cast<const uint16_t*>(_data.c_str() + _pos);
		_pos += sizeof(*value);
	}

	void pull(uint8_t* value) {
		safety(sizeof(*value));
		*value = *reinterpret_cast<const uint8_t*>(_data.c_str() + _pos);
		_pos += sizeof(*value);
	}

	void pull(int64_t* value) {
		safety(sizeof(*value));
		*value = *reinterpret_cast<const int64_t*>(_data.c_str() + _pos);
		_pos += sizeof(*value);
	}

	void pull(int32_t* value) {
		safety(sizeof(*value));
		*value = *reinterpret_cast<const int32_t*>(_data.c_str() + _pos);
		_pos += sizeof(*value);
	}

	void pull(int16_t* value) {
		safety(sizeof(*value));
		*value = *reinterpret_cast<const int16_t*>(_data.c_str() + _pos);
		_pos += sizeof(*value);
	}

	void pull(int8_t* value) {
		safety(sizeof(*value));
		*value = *reinterpret_cast<const int8_t*>(_data.c_str() + _pos);
		_pos += sizeof(*value);
	}

	template<typename CAR, typename... CDR>
	void push(const CAR& car, const CDR&... cdr) {
		push(car);
		push(cdr...);
	}

	void push(const string& data) {
		push(data.length());
		_ss << data;
	}

	template<typename T>
	void push(const vector<T>& items) {
		push(items.size());
		for (const auto &x : items) push(x);
	}

	template<typename T>
	void push(const set<T>& items) {
		push(items.size());
		for (const auto &x : items) push(x);
	}

	template<typename T, typename R>
	void push(const map<T, R>& items) {
		push<size_t>(items.size());
		for (const auto &x : items) {
			push(x.first);
			push(x.second);
		}
	}

	template<typename T>
	void push(const list<T>& items) {
		push<size_t>(items.size());
		for (const auto &x : items) push(x);
	}


	/* I choose not to support endianness.
	   Only use this if you have big-endian architecture.
	 */
	void push(const uint64_t& value) {
		_ss.write(reinterpret_cast<const char*>(&value), sizeof(value));
	}

	void push(const uint32_t& value) {
		_ss.write(reinterpret_cast<const char*>(&value), sizeof(value));
	}

	void push(const uint16_t& value) {
		_ss.write(reinterpret_cast<const char*>(&value), sizeof(value));
	}

	void push(const uint8_t& value) {
		_ss.write(reinterpret_cast<const char*>(&value), sizeof(value));
	}

	void push(const int64_t& value) {
		_ss.write(reinterpret_cast<const char*>(&value), sizeof(value));
	}

	void push(const int32_t& value) {
		_ss.write(reinterpret_cast<const char*>(&value), sizeof(value));
	}

	void push(const int16_t& value) {
		_ss.write(reinterpret_cast<const char*>(&value), sizeof(value));
	}

	void push(const int8_t& value) {
		_ss.write(reinterpret_cast<const char*>(&value), sizeof(value));
	}

	string str() { return _ss.str(); }

	template<class T>
	void push(const T& marshallable) {
		marshallable.marshal(this);
	}

 protected:
	void safety(size_t len) {
		if (len + _pos > _data.size()) {
			Logger::error("buffer overrun: at % of % read %",
				      _pos, _data.size(), len);
			throw 0;
		}
	}

	stringstream _ss;
	string _data;
	size_t _pos;
};

}  // namespace ib

#endif  // __IB__MARSHALLED__H__
