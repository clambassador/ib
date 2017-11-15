#ifndef __IB__SET__H__
#define __IB__SET__H__

#include <set>

using namespace std;

namespace ib {

class Set {
public:
	template <typename T>
	static bool close(set<T> a, set<T> b, float percent) {
		set<T> set_intersect;
		intersect(a, b, &set_intersect);
		if ((set_intersect.size() > ((float) a.size() * percent)) &&
		    (set_intersect.size() > ((float) b.size() * percent)))
			return true;
		return false;
	}

	template <typename T>
	static void intersect(const set<T>& a, const set<T>& b, set<T>* result) {
		if (a.size() < b.size()) return intersect(b, a, result);
		for (const auto &x : a) {
			if (b.count(x)) result->insert(x);
		}
	}
};

}  // namespace ib

#endif  // __IB__SET__H__
