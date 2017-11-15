#ifndef __IB__TINY_TIMER__H__
#define __IB__TINY_TIMER__H__

#include <chrono>
#include <ctime>
#include <memory>
#include <mutex>
#include <string>
#include <sys/time.h>

#include "ib/logger.h"
#include "ib/sensible_time.h"

using namespace std;

namespace ib {

class TinyTimer {
public:
	TinyTimer() : TinyTimer("[undefined]") {}
	TinyTimer(const string& name) {
		_then = sensible_time::runtime_us();
		_name = name;
	}

	virtual ~TinyTimer() {
		Logger::info("%: % us", _name,
			     sensible_time::runtime_us() - _then);
	}

protected:
	int _then;
	string _name;
};

}  // namespace ib

#endif  // __IB__TINY_TIMER__H__
