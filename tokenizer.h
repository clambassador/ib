#ifndef __IB__TOKENIZER__H__
#define __IB__TOKENIZER__H__

#include <cassert>
#include <cstdint>
#include <chrono>
#include <thread>
#include <vector>

#include "ib/logger.h"

using namespace std;

namespace ib {

class Tokenizer {
public:
	static void split(const string& data, const string& deliminator,
		          vector<string>* tokens) {
		size_t pos = 0;
		while (true) {
			size_t start = pos;
			pos = data.find(deliminator, start);
			if (pos == string::npos) {
				tokens->push_back(data.substr(start));
				break;
			}
			tokens->push_back(data.substr(start, pos - start));
			pos += deliminator.length();
		}
	}
};

}  // namespace ib

#endif  // __IB__TOKENIZER__H__
