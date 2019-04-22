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
			string start_state, end_state, symbol;
			if (3 != Tokenizer::extract("%,% -> %", rule,
						    &start_state, &symbol,
						    &end_state)) {
				Logger::error("could not parse %", rule);
				continue;
			}
			add_state(start_state);
			add_state(end_state);
			add_transition(start_state, symbol, end_state);
		}
		reset();
	}

	virtual void add_state(const string& state) {
		if (state.empty())
			throw string("cannot add special state");
		_states.insert(state);
	}

	virtual void reset() {
		_cur_state = "START";
	}

	virtual bool has_state(const string& state) const {
		return _states.count(state);
	}

	virtual void add_transition(const string& start_state,
				    const string& symbol,
				    const string& end_state) {
		assert(has_state(start_state));
		assert(has_state(end_state));
		_delta[start_state][symbol] = end_state;
	}

	virtual bool is_error() const {
		return _cur_state.empty();
	}

	virtual string process(const string& symbol) {
		string retval = process(_cur_state, symbol);
		_cur_state = retval;
		return retval;
	}

	virtual string process(const string& cur_state, const string& symbol) {
		if (is_error()) return cur_state;
		if (!_states.count(cur_state)) {
			throw string("no state " + cur_state);
		}
		return _delta[cur_state][symbol];
	}

	virtual void trace(const vector<string>& symbols, vector<string>* out) {
		return trace("START", symbols, out);
	}

	virtual void trace(const string& state, const vector<string>& symbols,
			   vector<string>* out) {
		string cur_state = state;
		out->push_back(state);
		for (auto &x : symbols) {
			out->push_back(process(out->back(), x));
		}
	}

	virtual void state(const string& state) {
		_cur_state = state;
	}

	virtual string state() const {
		return _cur_state;
	}

	virtual void set_accept(const string& state) {
		_accept.insert(state);
	}

	virtual void set_reject(const string& state) {
		_reject.insert(state);
	}

protected:
	string _cur_state;
	set<string> _states;
	map<string, map<string, string>> _delta;
	set<string> _accept;
	set<string> _reject;

};

}  // namespace ib

#endif  // __IB__FSM__H__
