#ifndef __IB__TOKENIZER__H__
#define __IB__TOKENIZER__H__

#include <cassert>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <regex>
#include <thread>
#include <vector>

#include "ib/logger.h"

using namespace std;

namespace ib {

class Tokenizer {
public:
	static void printable_segments(const string& s,
				       vector<string>* out) {
		assert(out);
		size_t pos = 0;
		while (pos < s.length()) {
			while (pos < s.length() && !isprint(s[pos])) ++pos;
			if (pos == s.length()) break;
			size_t start = pos;
			while (pos < s.length() && isprint(s[pos])) ++pos;
			out->emplace_back(s.substr(start, pos - start));
		}
	}

	static string get_token(const string& s,
				size_t pos,
				const string& end) {
		set<char> terms;
		size_t cur = pos;
		for (auto &x : end) terms.insert(x);
		while (++cur != s.length()) {
			if (terms.count(s.at(cur))) {
				return s.substr(pos, cur - pos);
			}
		}
		return s.substr(pos);
	}

	static bool match_pairs(const string& s,
				vector<string>* components,
				vector<size_t>* matchings) {
		return match_pairs(s, components, matchings, false);
	}

	static bool match_pairs(const string& s,
				vector<string>* components,
				vector<size_t>* matchings,
				bool mind_quote) {
		assert(matchings);
		matchings->resize(s.length(), string::npos);
		components->resize(s.length());
		vector<pair<size_t, char>> stack;
		string annotated;
		if (mind_quote) annotate_quote(s, &annotated);
		for (size_t i = 0; i < s.length(); ++i) {
			if (mind_quote && annotated[i] != '_') continue;
			if (s[i] == '{' || s[i] == '[') {
				stack.push_back(make_pair(i, s[i]));
			}
			if (s[i] == '}' || s[i] == ']') {
				if (stack.empty()) throw string("parse error: empty stack");
				if ((stack.back().second == '{' && s[i] == ']') ||
			            (stack.back().second == '[' && s[i] == '}')) {
					throw string("parse error mismatch {}[]");
				}
				size_t start = stack.back().first;
				(*matchings)[i] = start;
				(*matchings)[start] = i;
				(*components)[start] = s.substr(start, i - start + 1);
				stack.pop_back();
			}
		}
		if (stack.size()) throw string("parse error: end with stack");
		return true;
	}

	template <typename... Args>
	static vector<string> tokenize(const string& s,
				       const Args&... rules) {
		vector<string> retval;
		set<string> tokens;
		form_tokens(&tokens, rules...);
		size_t i = 0;
		while (i < s.length()) {
			string rule;
			size_t pos = string::npos;
			for (auto& which : tokens) {
				size_t where = match_rule(s, i, which);
				if (where != string::npos && where < pos) {
					pos = where;
					rule = which;
				}
			}
			if (pos == string::npos) break;

			i = pos;
			retval.push_back(extract_rule(s, i, rule));
			assert(retval.back().length());
			i += retval.back().length();
			Logger::info("added % at % in %",
				     retval.back(),
				     pos, s);
		}
		return retval;
	}

	template <typename... Args>
	static void form_tokens(set<string>* out, const string& car, const Args&... cdr) {
		out->insert(car);
		form_tokens(out, cdr...);
	}

	static void form_tokens(set<string>* out) {}

	static size_t match_rule(const string& s, size_t pos, const string& rule) {
		// TODO generic rules
		return s.find(rule, pos);
	}

	static string extract_rule(const string& s, size_t pos, const string& rule) {
		// TODO generic rules
		return rule;
	}

	static void matching_brackets(const string& s, char open, char close,
				      vector<size_t>* out,
				      bool allow_escape, bool mind_quote) {
		vector<size_t> stack;
		string quote;
		assert(out->size() <= s.length());
		while (out->size() != s.length()) out->push_back(string::npos);

		if (mind_quote) {
			annotate_quote(s, &quote);
		} else {
			quote = string('_', s.length());
		}
		for (size_t i = 0; i < s.length(); ++i) {
			if (quote[i] == '_') {
				if (s[i] == open) {
					stack.push_back(i);
				} else if (s[i] == close) {
					if (stack.size()) {
						size_t start = stack.back();
						(*out)[start] = i;
						(*out)[i] = start;
						stack.pop_back();
					}
				}
			}
		}
	}

	static vector<size_t> matching_brackets(const string& s) {
		return matching_brackets(s, false, true);
	}

	static vector<size_t> matching_brackets(const string& s, bool
						allow_escape, bool mind_quote) {
		vector<size_t> ret;
		matching_brackets(s, '{', '}', &ret, allow_escape, mind_quote);
		matching_brackets(s, '[', ']', &ret, allow_escape, mind_quote);
		matching_brackets(s, '(', ')', &ret, allow_escape, mind_quote);
		return ret;
	}

	static string longest_prefix(const vector<string>& vals) {
		if (vals.empty()) return "";
		if (vals.size() == 1) return vals[0];
		string ref = vals[0];
		string retval = "";
		for (size_t i = 0; i < ref.length(); ++i) {
			for (size_t j = 1; j < vals.size(); ++j) {
				if (i == vals.at(j).length()) return retval;
				assert(i < vals.at(j).length());
				if (vals.at(j).at(i) != ref.at(i))
					return retval;
			}
			retval += ref[i];
		}
		return retval;
	}

	static void annotate_quote(const string& data, string* annotated) {
		int in_quote = 0;
		char chr[2] = {'_', '\''};
		size_t i = 0;

		annotated->resize(data.length());
		while (i < data.length()) {
			if (data[i] == '\\') {
				(*annotated)[i] += chr[!!in_quote];
				(*annotated)[i + 1] += chr[!!in_quote];
				i += 2;
				continue;
			}
			if (data[i] == '\"' || data[i] == '\'') {
				if (in_quote == data[i]) {
					(*annotated)[i] += '\"';
					++i;
					in_quote = 0;
					continue;
				} else if (!in_quote) {
					(*annotated)[i] += '\"';
                                        in_quote = data[i];
                                        ++i;
                                        continue;
				}
			}
			(*annotated)[i] += chr[!!in_quote];
			++i;
		}
	}

	static string collapse_quote(const string& data) {
		string annotate;
		string retval;
		stringstream ss;
		annotate_quote(data, &annotate);
		set<char> chr;
		chr.insert('_');
		chr.insert('\'');
		for (size_t i = 0; i < data.length(); ++i) {
			if (chr.count(annotate[i])) ss << data[i];
		}
		return ss.str();
	}

	static string hex_unescape(const string& data) {
		return hex_unescape(data, "%");
	}

	static string hex_unescape(const string& data, const string& delimiter) {
		vector<string> pieces;
		split(data, delimiter, &pieces);
		if (pieces.size() < 2) return data;
		stringstream ss;
		ss << pieces[0];
		for (size_t i = 1; i < pieces.size(); ++i) {
			if (pieces[i].length() < 2)
				ss << delimiter << pieces[i];
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

	static string join(const vector<string>& data,
			   const string& delimiter) {
		if (data.empty()) return "";
		stringstream ss;
		for (auto &x : data) {
			ss << x << delimiter;
		}
		return ss.str().substr(0, ss.str().length() -
				       delimiter.length());
	}

	static string join(const vector<string>& data, const string& delimiter,
			   const set<size_t> pos) {
		if (pos.size() == 0) return "";
		stringstream ss;
		for (auto &x : pos) {
			ss << data[x] << delimiter;
		}
		return ss.str().substr(0, ss.str().length() -
				       delimiter.length());
	}

	static void join(const vector<string>& data, const string& delimiter,
			   const set<size_t> pos, string* in, string* out) {
		if (pos.size() == 0) *in = "";
		if (pos.size() == data.size()) *out = "";
		stringstream ss_in;
		stringstream ss_out;

		for (size_t i = 0; i < data.size(); ++i) {
			if (pos.count(i + 1)) {
				ss_in << data[i] << delimiter;
			} else {
				ss_out << data[i] << delimiter;
			}
		}
		if (pos.size() > 0)
			*in = ss_in.str().substr(
				0, ss_in.str().length() - delimiter.length());
		if (pos.size() < data.size())
			*out = ss_out.str().substr(
				0, ss_out.str().length() - delimiter.length());
	}

	static string replace_first(const string& data, const string& find,
			      const string& replacement) {
		vector<string> pieces;
		split_with_empty(data, find, &pieces);
		if (pieces.size() < 2) return data;
		stringstream ss;
		ss << pieces[0];
		ss << replacement;
		ss << pieces[1];
		for (size_t i = 2; i < pieces.size(); ++i) {
		     ss << find << pieces[i];
		}
		return ss.str();
	}

	static string replace(const string& data, const string& find,
			      const string& replacement) {
		vector<string> pieces;
		split_with_empty(data, find, &pieces);
		if (pieces.size() < 2) return data;
		stringstream ss;
		ss << pieces[0];
		for (size_t i = 1; i < pieces.size(); ++i) {
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

	static string trim(const string& str, const string& chars) {
		set<char> skip;
		for (auto &x : chars) {
			skip.insert(x);
		}
                int s = 0;
                int e = str.length() - 1;
                while (skip.count(str[s])) ++s;
		while (skip.count(str[e])) --e;
                ++e;
                return str.substr(s, e - s);
        }

	static string trim(const string& str) {
                int s = 0;
                int e = str.length() - 1;
                while (whitespace(str[s])) ++s;
		while (whitespace(str[e])) --e;
                ++e;
                return str.substr(s, e - s);
        }

        static bool whitespace(const char& c) {
                return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
        }

	static bool pop_split(const string& data,
			       char delimiter,
			       int col,
			       string* in,
			       string* out) {
		int i = 1;
		size_t start_pos = 0;
		assert(out);
		assert(in);
		*in = "";
		for (size_t pos = 0; pos < data.length(); ++pos) {
			if (i == col) {
				start_pos = pos;
				if (start_pos) *in = data.substr(0, start_pos - 1);
				while (pos < data.length()) {
					if (data[pos] == delimiter) {
						*out = data.substr(
							start_pos,
							pos - start_pos);
						if (!in->empty()) *in +=
							delimiter;

						*in += data.substr(pos + 1);
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

	static void numset(const string& nums, set<size_t>* out, int shift) {
		set<size_t> tmp;
		numset(nums, &tmp);
		for (auto &x : tmp) {
			out->insert(x + shift);
		}
	}

	static void numset(const string& nums, set<size_t>* out) {
		if (nums == "-") return;
		assert(out);
		vector<string> cols;
		split(nums, ",", &cols);
		for (auto &x : cols) {
			size_t val;

			extract_one(x, &val);
			out->insert(val);
		}
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

	static size_t extract_outer_paired(const string& left,
					   const string& right,
				  	   const string& data,
					   vector<string>* results) {
		assert(results);
		assert(left != right);
		size_t curpos = 0;

		while (curpos < data.size()) {
			curpos = data.find(left, curpos);
			if (curpos == string::npos) return results->size();

			int depth = 1;
			size_t start = curpos++;
			while (curpos < data.length() && depth) {
	                        if (substreq(data, curpos, right)) {
        	                        --depth;
                	                curpos += right.length();
	                        } else if (substreq(data, curpos, left)) {
        	                        ++depth;
                	                curpos += left.length();
	                        } else ++curpos;
			}
			if (depth) return results->size();
			results->push_back(data.substr(start, curpos - start));
		}
		return results->size();
	}

	static bool substreq(const string& main, size_t pos,
			     const string& search) {
		return !strncmp(main.c_str() + pos, search.c_str(),
				search.length());
	}

	static size_t extract_all_paired(const string& left,
					 const string& right,
				  	 const string& data,
					 vector<string>* results) {
		assert(results);
		assert(left != right);
		int depth = 0;
		for (size_t i = 0; i < data.length(); ++i) {
			if (substreq(data, i, left)) {
				i += left.length();
				assert(!depth);
				depth = 1;
				for (size_t j = i; j < data.length(); ++j) {
					if (substreq(data, j, left)) {
						++depth;
					} else if (substreq(data, j, right)) {
						--depth;
					}
					if (!depth) {
						assert(i <= j);
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

	/* re_extract_all takes a regexp re, a string data,
	   and a position pos. It does a match all of the data using
	   re and puts the pos match into queries each go. 0th is full match,
	   1st is first parenthetic subexp, etc.
	 */
	static size_t re_extract_all(const regex& re,
				     const string& data,
				     size_t pos,
				     vector<string>* queries) {
		assert(queries);
		sregex_iterator it = sregex_iterator(
		    data.begin(), data.end(), re);
		while (it != sregex_iterator()) {
			assert(pos < it->size());
			queries->push_back(it->str(pos));
			++it;
		}
		return queries->size();
	}

	static size_t extract_all(const string& format,
				  const string& data,
				  vector<string>* results) {
		assert(results);
		int ret;
		string rest = data;
		assert(format.length());
		assert(format.at(format.length() - 1) != '%');
		while (true) {
			string tmp, result;
			ret = extract("%" + format + "%", rest, nullptr,
				      &result, &tmp);
			if (ret >= 2) results->push_back(result);
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
		size_t whitepos = token.find_last_of("\4");
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
				size_t candidate = data.find(testtoken, pos);
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
		if (!tokens->size()) {
			Logger::debug("about to crash: % % % % %",
				      pos_format, format, pos_data,
				      data, tokens, cnt);
		}
		assert(tokens->size());
		if (!check_token_match(
			data.substr(pos_data, token.length()),
			format.substr(pos_format, token.length()))) {
			return 0;
		}
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
		if (data.substr(pos_data) != tokens->front()) {
			Logger::info("extract failed: % % % % %", format, data,
				     *tokens, pos_data, data.length());
			assert(0);
		}
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
