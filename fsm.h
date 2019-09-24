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
			if (3 == Tokenizer::extract("%,% -> %", rule,
						    &start_state, &symbol,
						    &end_state)) {
				start_state = Tokenizer::trim(start_state);
				symbol = Tokenizer::trim(symbol);
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

protected:
	string _start_state;
	set<string> _cur_states;
	set<string> _states;
	map<string, map<string, set<string>>> _delta;
	set<string> _accept;
	set<string> _reject;

};

}  // namespace ib

#endif  // __IB__FSM__H__
