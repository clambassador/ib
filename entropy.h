#ifndef __IB__ENTROPY__H__
#define __IB__ENTROPY__H__

#include <cstdint>
#include <mutex>
#include <random>

#include "ib/logger.h"

using namespace ib;
using namespace std;

class entropy {
public:
	template <typename T>
	static T _() {
		static unique_ptr<mutex> _lock(new mutex());
		unique_lock<mutex> lock(*_lock.get());
                random_device rd("/dev/urandom");
		T retval = 0;

		size_t sz = sizeof(rd());
		for (int i = 0; i < sizeof(T) / sz; ++i) {
			Logger::info("out %", i);
			retval <<= 32;
			retval += rd();
		}
                return retval;
	}
};

#endif
