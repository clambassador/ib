#ifndef __IB__CONTAINERS__H__
#define __IB__CONTAINERS__H__

#include <map>
#include <set>
#include <string>
#include <vector>

#include "ib/logger.h"

using namespace std;

namespace ib {

class Containers {
public:
	template <typename T>
	static vector<T> unique(const vector<T>& input) {
		vector<T> retval;
		set<T> tmp;
		for (const auto &x : input) {
			tmp.insert(x);
		}
		for (const auto &x : tmp) {
			retval.push_back(x);
		}
		return retval;
	}

	template <typename T>
	static int reorder_vector(const vector<T>& order,
				  const vector<T>& input,
				  vector<T>* output) {
		assert(order.size() >= input.size());
		map<T, size_t> reverse;
		map<size_t, T> result;
		lookup_vector(order, &reverse);
		for (const T& x: input) {
			assert(reverse.count(x));
			result[reverse[x]] = x;
		}
		output->clear();
		for (const auto& x: result) {
			output->push_back(x.second);
		}
		return 0;
	}

	template <typename T>
	static int lookup_vector(const vector<T>& input,
				 map<T, size_t>* output) {
		for (size_t i = 0; i < input.size(); ++i) {
			assert(output->count(input[i]) == 0);
			(*output)[input[i]] = i;
		}
		return 0;
	}

	template <typename T>
	static size_t index_of(const vector<T>& input,
			       const T& val) {
		size_t i = 0;
		for (; i < input.size(); ++i) {
			if (val == input[i]) return i;
		}
		return i;
	}

	template <typename T>
	static set<T> setrange(T stop) {
		return setrange((T) 0, stop);
	}

	template <typename T>
	static set<T> setrange(T start, T stop) {
		set<T> ret;
		for (T t = start; t < stop; ++t) {
			ret.insert(t);
		}
		return ret;
	}

	template <typename T>
	static T get_set(const set<T>& vals, size_t pos) {
		assert(pos < vals.size());
		auto x = vals.begin();
		size_t i = pos;
		while (i > 0) {
			++x;
			--i;
		}
		return *x;
	}

protected:
	Containers() {}
	~Containers() {}
};

}  // namespace ib

#endif  // __IB__TINY_TIMER__H__
