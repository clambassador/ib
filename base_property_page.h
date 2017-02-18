#ifndef __IB__BASE_PROPERTY_PAGE__H__
#define __IB__BASE_PROPERTY_PAGE__H__

#include <cassert>
#include <set>
#include <sstream>
#include <string>

#include "ib/abstract_property_page.h"

using namespace std;

namespace ib {

class BasePropertyPage : public AbstractPropertyPage {
public:
	BasePropertyPage() {}

	virtual ~BasePropertyPage() {}

	virtual const string& get(const string& key) const {
		auto x = _properties.find(key);
		assert(x != _properties.end());
		return x->second;
	}

	virtual bool has(const string& key) const {
		return _properties.count(key);
	}

	virtual const map<string, string>& get_all() const {
		return _properties;
	}

	template<typename T>
	int set(const string& key, const T& value) {
		stringstream ss;
		ss << value;

		int retval = _properties.count(key);
		_properties[key] = ss.str();
		return retval;
	}

	virtual void merge(const AbstractPropertyPage& page) {
                auto props = page.get_all();
                for (auto &x : props) {
                        set(x.first, x.second);
                }
        }

protected:
	map<string, string> _properties;
};

}  // namespace ib

#endif  // __IB__ABSTRACT_PROPERTY_PAGE__H__
