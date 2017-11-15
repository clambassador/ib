#ifndef __IB__SENSIBLE_TIME__H__
#define __IB__SENSIBLE_TIME__H__

#include <chrono>
#include <ctime>
#include <memory>
#include <mutex>
#include <string>
#include <sys/time.h>

using namespace std;

namespace ib {

class sensible_time {
public:
	static chrono::time_point<chrono::system_clock> now() {
		struct timeval tv;
		gettimeofday(&tv, nullptr);
		time_t tt = tv.tv_sec;
		chrono::time_point<chrono::system_clock> retval =
			chrono::system_clock::from_time_t(tt);
		retval += chrono::microseconds(tv.tv_usec);
		return retval;
	}

	static int runtime() {
		time_t zero = 0;
		chrono::time_point<chrono::system_clock> notime
			= chrono::system_clock::from_time_t(zero);
		uint64_t curtime = chrono::duration_cast<chrono::seconds>(
			now() - notime).count();

		if (_epoch == 0) _epoch = curtime;
		return curtime - _epoch;
	}

	static int runtime_us() {
		time_t zero = 0;
		chrono::time_point<chrono::system_clock> notime
			= chrono::system_clock::from_time_t(zero);
		uint64_t curtime = chrono::duration_cast<chrono::microseconds>(
			now() - notime).count();

		if (_epoch == 0) _epoch = curtime;
		return curtime - _epoch;
	}

	static void reset_runtime() {
		_epoch = 0;
		runtime();
	}

protected:
	static uint64_t _epoch;
};

}  // namespace ib

#endif  //  __IB__SENSIBLE_TIME__H__

