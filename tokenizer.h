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

	static string hex_unescape(const string& data) {
		vector<string> pieces;
		split(data, "%", &pieces);
		if (pieces.size() < 2) return data;
		stringstream ss;
		ss << pieces[0];
		for (int i = 1; i < pieces.size(); ++i) {
			if (pieces[i].length() < 2)
				ss << "%" << pieces[i];
			else {
				stringstream hexparse;
				int val;
				hexparse << hex << pieces[i].substr(0, 2);
				hexparse >> val;
				ss << (char) val << pieces[i].substr(2);
			}
		}
		return ss.str();
	}

	static string replace(const string& data, const string& find,
			      const string& replacement) {
		vector<string> pieces;
		split(data, find, &pieces);
		if (pieces.size() < 2) return data;
		stringstream ss;
		ss << pieces[0];
		for (int i = 1; i < pieces.size(); ++i) {
		     ss << replacement << pieces[i];
		}
		return ss.str();
	}

	template<typename T>
	static int last_token(const string& in, const string& delim, T out) {
		int pos = in.find_last_of(delim);
		if (pos != string::npos) {
			extract_one(in.substr(pos + delim.length()), out);
			return 1;
		}
		return 0;
	}

	static void add_token(const string& data, vector<string>* tokens) {
		if (data.length()) tokens->push_back(data);
	}

	static string trimout(const string &data, const string& delimitor) {
		stringstream ss;
		vector<string> pieces;
		split(data, delimitor, &pieces);
		for (const auto &x : pieces) {
			ss << x;
		}
		return ss.str();
	}

	static bool fast_split(const string& data,
			       char delimiter,
			       int col,
			       string* out) {
		int i = 1;
		size_t start_pos = 0;
		assert(out);
		for (size_t pos = 0; pos < data.length(); ++pos) {
			if (i == col) {
				start_pos = pos;
				while (pos < data.length()) {
					if (data[pos] == delimiter) {
						*out = data.substr(
							start_pos,
							pos - start_pos);
						return true;
					}
					++pos;
				}
				*out = data.substr(start_pos);
				return true;
			}
			if (data[pos] == delimiter) ++i;
		}
		*out = "";
		return false;
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

	static size_t extract_all_paired(const string& left,
					 const string& right,
				  	 const string& data,
					 vector<string>* results) {
		assert(results);
		assert(left != right);
		int depth = 0;
		for (int i = 0; i < data.length(); ++i) {
			if (data.substr(i, left.length()) == left) {
				i += left.length();
				assert(!depth);
				depth = 1;
				for (int j = i; j < data.length(); ++j) {
					if (data.substr(j, left.length()) ==
					    left) {
						++depth;
					} else if (data.substr(j, right.length())
						   == right) {
						--depth;
					}
					if (!depth) {
						results->push_back(data.substr(
							i, j - i));
						break;
					}
				}
				if (depth) {
					depth = 0;
				}
			}
		}
		return results->size();
	}

	static size_t extract_all(const string& format,
				  const string& data,
				  vector<string>* results) {
		assert(results);
		int ret;
		string rest = data;
		assert(format.at(format.length() - 1) != '%');
		while (true) {
			string tmp, result;
			ret = extract("%" + format + "%", rest, nullptr,
				      &result, &tmp);
			if (ret == 3) results->push_back(result);
			else break;
			rest = tmp;
		}
		return results->size();
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

	static size_t token_match(const string& data, const string& token,
				  int pos) {
		int whitepos = token.find_last_of("\4");
		if (whitepos == string::npos)
			return data.find(token, pos);
		else if (whitepos == 0) {
			size_t retval = string::npos;
			set<char> whitespace;
			whitespace.insert(' ');
			whitespace.insert('\n');
			whitespace.insert('\r');
			whitespace.insert('\t');
			for (char c : whitespace) {
				string testtoken = token;
				testtoken[0] = c;
				int candidate = data.find(testtoken, pos);
				if (candidate < retval) retval = candidate;
			}
			return retval;
		}
		Logger::debug("(tokenizer) error in match %. Put whitespace at"
			      " start", token);
		assert(0);
		return 0;
	}

	static bool check_token_match(const string& data, const string& token) {
		if (!token.length()) {
			if (!data.length()) return true;
			return false;
		}
		if (data.substr(1, data.length()) !=
		    token.substr(1, token.length())) return false;
		if (data[0] == token[0]) return true;
		set<char> whitespace;
		whitespace.insert(' ');
		whitespace.insert('\n');
		whitespace.insert('\r');
		whitespace.insert('\t');
		whitespace.insert('\4');
		if (whitespace.count(data[0]) &&
		    whitespace.count(token[0])) return true;
		return false;
	}

	template<typename T, typename... ARGS>
	static size_t extract_impl(size_t pos_format, const string& format,
				   size_t pos_data, const string& data,
				   list<string>* tokens, int cnt,
				   T car, ARGS... cdr) {
		string token = tokens->front();
		tokens->pop_front();
		assert(tokens->size());
		assert(check_token_match(
			data.substr(pos_data, token.length()),
			format.substr(pos_format, token.length())));
		pos_format += token.length();
		pos_data += token.length();

		size_t nextpos = token_match(data, tokens->front(), pos_data);

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

	template<typename... Args>
	static int get_split_matching(vector<string>* out,
				      const string& delimiter,
				      const string& data,
				      Args... args) {
		vector<string> tokens;
		split(data, delimiter, &tokens);
		return get_lines_matching_impl(out, tokens, args...);
	}

	template<typename... Args>
	static int get_lines_matching(vector<string>* out, const string& data,
				      Args... args) {
		vector<string> tokens;
		split(data, "\n", &tokens);
		return get_lines_matching_impl(out, tokens, args...);
	}

	template<typename... Args>
	static int get_lines_matching_impl(vector<string>* out,
					   const vector<string>& lines,
					   const string& car,
					   Args... cdr) {
		vector<string> result;
		get_lines_matching_impl(&result, lines, cdr...);
		for (auto &x : result) {
			if (x.find(car) != string::npos) {
				out->push_back(x);
			}
		}
		return out->size();
	}

	static int get_lines_matching_impl(vector<string>* out,
					   const vector<string>& lines) {
		assert(out);
		*out = lines;
		return out->size();
	}
};

}  // namespace ib

#endif  // __IB__TOKENIZER__H__
