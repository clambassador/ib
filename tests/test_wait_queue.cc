#include <string>
#include <thread>
#include <vector>
#include <unistd.h>

#include "ib/logger.h"
#include "ib/wait_queue.h"

using namespace std;
using namespace ib;

WaitQueue<int> q;
bool stop = false;

void read_thread() {
	while (!stop) {
		int val;
		val = q.pop();
		Logger::info("GOT %", val);
		sleep(1);
	}
}

int main() {
	thread t(read_thread);

	while (true) {
		int val = 0;
		cin >> val;
		if (val == 0) break;
		q.push(val);
		q.push(val);
		q.push(val);
		q.push(val);
	}
	stop = true;
	t.join();
}
