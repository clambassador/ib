#ifndef __IB__WAIT_QUEUE__H__
#define __IB__WAIT_QUEUE__H__

#include <condition_variable>
#include <queue>

using namespace std;

namespace ib {

template<typename T>
class WaitQueue {
public:
	virtual void push(const T& item) {
		unique_lock<mutex> ul(_l);
		_q.push(item);
		_c.notify_one();
	}

	virtual T pop() {
		unique_lock<mutex> ul(_l);
		while (_q.empty()) _c.wait(ul);
		assert(!_q.empty());
		T t = _q.front();
		_q.pop();
		return t;
	}

protected:
	queue<T> _q;
	mutex _l;
	condition_variable _c;
};

}  // namespace ib

#endif
