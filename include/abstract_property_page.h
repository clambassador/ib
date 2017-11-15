#ifndef __IB__ABSTRACT_PROPERTY_PAGE__H__
#define __IB__ABSTRACT_PROPERTY_PAGE__H__

#include <map>
#include <set>
#include <string>

using namespace std;

namespace ib {

class AbstractPropertyPage {
public:
	virtual ~AbstractPropertyPage() {}

	virtual const string& get(const string&) const = 0;
	virtual bool has(const string&) const = 0;
	virtual const map<string, string>& get_all() const = 0;
};

}  // namespace ib

#endif  // __IB__ABSTRACT_PROPERTY_PAGE__H__
