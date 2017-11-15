#ifndef __IB__LIMITER__H__
#define __IB__LIMITER__H__

#include <cassert>
#include <cstdint>
#include <chrono>
#include <thread>
#include <vector>

#include "logger.h"

using namespace std;

namespace ib {

class Limiter {
public:
	template<typename... ARGS>
	Limiter(ARGS... cdr) {
		_cur = 0;
		_count = 0;

		_durations.push_back(1);
		_durations.push_back(10);
		_durations.push_back(100);
		_durations.push_back(1000);
		_durations.push_back(10000);
		_durations.push_back(100000);
		set_stops(cdr...);
	}

	template<typename... CDR>
	void set_stops(size_t car, CDR... cdr) {
		_steps.push_back(car + (_steps.size() ? _steps.back() : 0));
		set_stops(cdr...);
	}

	void set_stops() {
		assert(_steps.size() <= _durations.size());
	}

	bool operator()() {
		++_count;
		while (_count > _steps[_cur]) {
			++_cur;
			if (_cur >= _steps.size()) return true;
		}
		this_thread::sleep_for(chrono::milliseconds(_durations[_cur]));
		return false;
	}

protected:
	size_t _count;
	size_t _cur;
	vector<size_t> _steps;
	vector<size_t> _durations;
};

}  // namespace ib

#endif  // __IB__LIMITER__H__
