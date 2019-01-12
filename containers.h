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

protected:
	Containers() {}
	~Containers() {}
};

}  // namespace ib

#endif  // __IB__TINY_TIMER__H__
