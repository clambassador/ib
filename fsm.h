#ifndef __IB__FSM__H__
#define __IB__FSM__H__

#include <set>
#include <string>
#include <vector>

#include "ib/tokenizer.h"

using namespace std;

namespace ib {

class FSM {
public:
	FSM() {}
	FSM(const string& rulelist) {
		vector<string> rules;
		Tokenizer::split(rulelist, "\n", &rules);
		init(rules);
	}
	FSM(const vector<string>& rules) {
		init(rules);
	}

	virtual ~FSM() {}

	virtual void init(const vector<string>& rules) {
		for (auto &rule : rules) {
			if (rule.empty()) continue;
			Logger::info("%", rule);
			string start_state, end_state, symbol;
			if (3 == Tokenizer::extract("{%},% -> {%}", rule,
						    &start_state, &symbol,
						    &end_state)) {
				start_state = "{" + Tokenizer::trim(start_state)
				    + "}";
				symbol = Tokenizer::trim(symbol);
				add_symbol(symbol);
				end_state = "{" + Tokenizer::trim(end_state) +
				    "}";
				add_state(start_state);
				add_state(end_state);
				add_transition(start_state, symbol, end_state);
			} else if (3 == Tokenizer::extract("%,% -> %", rule,
						    &start_state, &symbol,
						    &end_state)) {
				start_state = Tokenizer::trim(start_state);
				symbol = Tokenizer::trim(symbol);
				add_symbol(symbol);
				end_state = Tokenizer::trim(end_state);
				add_state(start_state);
				add_state(end_state);
				add_transition(start_state, symbol, end_state);
			} else if (1 == Tokenizer::extract("START %", rule,
							   &start_state)) {
				_start_state = Tokenizer::trim(start_state);
			} else if (1 == Tokenizer::extract("ACCEPT %", rule,
							   &start_state)) {
				set_accept(Tokenizer::trim(start_state));
				set_accept(start_state);
			} else {
				continue;
			}

		}
		for (auto & x : _states) {
			if (!_accept.count(x)) set_reject(x);
		}
		reset();
	}

	virtual void add_state(const string& state) {
		if (state.empty())
			throw string("cannot add special state");
		_states.insert(state);
	}

	virtual void reset() {
		_cur_states.clear();
		_cur_states.insert(_start_state);
		_cur_states = with_epsilon_moves(_cur_states);
	}

	virtual bool has_state(const string& state) const {
		return _states.count(state);
	}

	virtual void add_transition(const string& start_state,
				    const string& symbol,
				    const string& end_state) {
		assert(has_state(start_state));
		assert(has_state(end_state));
		_delta[start_state][symbol].insert(end_state);
	}

	virtual bool is_error() const {
		return _cur_states.empty();
	}

	virtual set<string> with_epsilon_moves(const set<string>& states) {
		set<string> retval = states;
		for (auto &x : process(states, "epsilon")) {
			retval.insert(x);
		}
		return retval;
	}

	virtual set<string> with_epsilon_moves(const string& state) {
		set<string> retval = process(state, "epsilon");
		retval.insert(state);
		return retval;
	}

	virtual set<string> process(const string& symbol) {
		_cur_states = process(_cur_states, symbol);
		return _cur_states;
	}

	virtual set<string> process(const set<string>& states, const string& symbol) {
		set<string> retval;
		for (auto &x : states) {
			set<string> result = process(x, symbol);
			for (const auto &y : result) {
				retval.insert(y);
			}
		}
		if (symbol != "epsilon") retval = with_epsilon_moves(retval);
		return retval;
	}

	virtual set<string> process(const string& cur_state, const string& symbol) {
		if (!_states.count(cur_state)) {
			return set<string>();
		}
		return _delta[cur_state][symbol];
	}

	virtual void trace(const vector<string>& symbols, vector<set<string>>* out) {
		trace("START", symbols, out);
	}

	virtual void trace(const string& state, const vector<string>& symbols,
			   vector<set<string>>* out) {
		set<string> cur_states;
		cur_states.insert(state);
		out->push_back(with_epsilon_moves(state));
		for (auto &x : symbols) {
			out->push_back(process(out->back(), x));
		}
	}

	virtual void state(const string& state) {
		_cur_states.clear();
		_cur_states.insert(state);
	}

	virtual set<string> state() const {
		return _cur_states;
	}

	virtual void set_accept(const string& state) {
		_accept.insert(state);
	}

	virtual void set_reject(const string& state) {
		_reject.insert(state);
	}

	virtual void add_symbol(const string& symbol) {
		_symbols.insert(symbol);
	}

	virtual string subset_construction() {
		stringstream ss;
		stringstream ss_accept;
		vector<set<string>> q;
		set<set<string>> considered;
		q.push_back(with_epsilon_moves(_start_state));
		ss << "START " << as_state(q.back()) << endl << endl;
		while (!q.empty()) {
			set<string> state = q.back();
			considered.insert(state);
			q.pop_back();
			if (is_accept(state))
				ss_accept << "ACCEPT " << as_state(state) << endl;
			for (auto &x : _symbols) {
				if (x == "epsilon") continue;
				set<string> next = process(state, x);
				if (!considered.count(next)) {
					considered.insert(next);
					q.push_back(next);
				}
				ss << as_state(state) << "," << x
				   << " -> " << as_state(next) << endl;
			}
		}
		ss << endl;
		ss << ss_accept.str();
		return ss.str();
	}

	static string as_state(const set<string>& state) {
		stringstream ss;
		ss << "{";

		for (auto x = state.begin(); x != state.end();) {
			ss << *x;
			++x;
			if (x != state.end()) ss << ",";
		}
		ss << "}";
		return ss.str();
	}

	virtual bool is_accept(set<string> state) {
		for (auto &x : state) {
			if (_accept.count(x)) return true;
		}
		return false;
	}

protected:
	string _start_state;
	set<string> _cur_states;
	set<string> _states;
	map<string, map<string, set<string>>> _delta;
	set<string> _accept;
	set<string> _reject;
	set<string> _symbols;

};

}  // namespace ib

#endif  // __IB__FSM__H__
