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

	static void add_token(const string& data, vector<string>* tokens) {
		if (data.length()) tokens->push_back(data);
	}

	static void split(const string& data, const string& deliminator,
		          vector<string>* tokens) {
		size_t pos = 0;
		while (true) {
			size_t start = pos;
			pos = data.find(deliminator, start);
			if (pos == string::npos) {
				add_token(data.substr(start), tokens);
				break;
			}
			add_token(data.substr(start, pos - start), tokens);
			pos += deliminator.length();
		}
	}

	static void split_with_empty(const string& data, const string& deliminator,
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
				add_token(data.substr(start), tokens);
				break;
			}
			if (quote.at(pos) == '_') {
				add_token(data.substr(start, pos - start), tokens);
				start = pos + deliminator.length();
			}
			pos += deliminator.length();
		}
	}

	template<typename... ARGS>
	static size_t extract(const string& format, const string& data,
			      ARGS... args) {
		size_t pos_format = 0;
		size_t pos_data = 0;

		if (data.empty()) return 0;
		if (format.empty()) return 0;

		try {
		vector<string> tokens_with_empty;
		list<string> tokens;
		split_with_empty(format, "%", &tokens_with_empty);
		for (size_t i = 0; i < tokens_with_empty.size(); ++i) {
			if (tokens_with_empty[i].empty()) {
				if (tokens.empty()) {
					assert(!i);
					tokens.push_back("");
					if (format.length() > 1 &&
					    format[1] != '%')
						continue; // initial
				}
				if (i + 1 == tokens_with_empty.size()) {
					tokens.push_back("");
					continue;
				}
				string token = tokens.back();
				token += "%" + tokens_with_empty[++i];
				tokens.pop_back();
				tokens.push_back(token);

			} else {
				tokens.push_back(tokens_with_empty[i]);
			}
		}
		return Tokenizer::extract_impl(
			pos_format, format, pos_data, data,
			&tokens, 0, args...);
		} catch (int e) {
			return 0;  // TODO: replace assert with throws
		}
	}

	template<typename T, typename... ARGS>
	static size_t extract_impl(size_t pos_format, const string& format,
				   size_t pos_data, const string& data,
				   list<string>* tokens, int cnt,
				   T car, ARGS... cdr) {
		string token = tokens->front();
		tokens->pop_front();
		assert(tokens->size());
		assert(format.substr(pos_format, token.length()) ==
		       data.substr(pos_data, token.length()));
		pos_format += token.length();
		pos_data += token.length();

		size_t nextpos = data.find(tokens->front(), pos_data);

		if (nextpos == string::npos) return 0;
		if (tokens->front().empty()) nextpos = data.length();

		Tokenizer::extract_one(data.substr(pos_data, nextpos -
						   pos_data),
				       car);

		pos_data = nextpos;
		pos_format += 1;

		return Tokenizer::extract_impl(
			pos_format, format, pos_data, data,
			tokens, cnt + 1, cdr...);
	}

	/* string specialization ensures that whitespace is preserved. */
	static void extract_one(const string& in, string* out) {
		if (!out) return;
		*out = in;
	}

	static void extract_one(const string& in, nullptr_t out) {
		return;
	}

	template<typename T>
	static void extract_one(const string& in, T out) {
		if (!out) return;
		stringstream ss;
		ss << in;
		ss >> *out;
	}

	// base case
	static size_t extract_impl(size_t pos_format, const string& format,
				   size_t pos_data, const string& data,
				   list<string>* tokens, int cnt) {
		assert(tokens->size() == 1);
		if (tokens->empty()) return data.length();
		assert(data.substr(pos_data) == tokens->front());
		assert(format.substr(pos_format) == tokens->front());
		return cnt;
	}
};

}  // namespace ib

#endif  // __IB__TOKENIZER__H__
