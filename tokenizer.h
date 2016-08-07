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
	static void annotate_quote(const string& data, string* annotated) {
		bool in_quote = false;
		char chr[2] = {'_', '\''};
		size_t i = 0;
		annotated->resize(data.length());
		while (i < data.length()) {
			if (data[i] == '\\') {
				(*annotated)[i] += chr[in_quote];
				(*annotated)[i] += chr[in_quote];
				i += 2;
				continue;
			}
			if (data[i] == '\"') {
				(*annotated)[i] += '\"';
				++i;
				in_quote = ~in_quote;
				continue;
			}
			(*annotated)[i] += chr[in_quote];
			++i;
		}
	}

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

	static void split_mind_quote(const string& data, const string& deliminator,
				     vector<string>* tokens) {
		size_t pos = 0;
		string quote;
		annotate_quote(data, &quote);
		size_t start = pos;
		while (true) {
			pos = data.find(deliminator, pos);
			if (pos == string::npos) {
				tokens->push_back(data.substr(start));
				break;
			}
			Logger::info("quote % %", pos, quote[pos]);
			if (quote.at(pos) == '_') {
				tokens->push_back(data.substr(start, pos - start));
				start = pos;
			}
			pos += deliminator.length();
		}
	}
};

}  // namespace ib

#endif  // __IB__TOKENIZER__H__
