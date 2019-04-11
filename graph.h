#ifndef __IB__GRAPH__H__
#define __IB__GRAPH__H__

#include <deque>
#include <map>
#include <memory>

using namespace std;

namespace ib {

template <typename T>
class Graph {
public:
	Graph() {}
	virtual ~Graph() {}

	template <typename R>
	class Node {
	public:
		Node(const R& r) {
			_r.reset(new R(r));
		}

		Node(R* r) {
			_r.reset(r);
		}
		virtual ~Node() {}
		const R& data() { return *_r.get(); }

		void add_edge(Node<R>* other) {
			_edges.insert(other);
		}

		bool has_edge(const R& target) {
			for (auto& x: _edges) {
				if (x->data() == target) return true;
			}
			return false;
		}

		set<Node<R> *> edges() { return _edges; }

	protected:
		unique_ptr<R> _r;
		set<Node<R> *> _edges;
	};

	void maybe_add_node(const T& val) {
		if (!_nodes.count(val)) _nodes[val].reset(new Node<T>(val));
	}

	void add_node(const T& val) {
		assert(!_nodes.count(val));
		_nodes[val].reset(new Node<T>(val));
	}

	void add_node(T* val) {
		_nodes[*val].reset(new Node<T>(val));
	}

	void add_join_nodes(const T& one, const T& two) {
		if (!_nodes.count(one)) {
			add_node(one);
		}
		if (!_nodes.count(two)) {
			add_node(two);
		}
		join_nodes(one, two);
	}

	void add_directed_join_nodes(const T& one, const T& two) {
		if (!_nodes.count(one)) {
			add_node(one);
		}
		if (!_nodes.count(two)) {
			add_node(two);
		}
		directed_join_nodes(one, two);
	}

	void join_nodes(const T& one, const T& two) {
		assert(_nodes.count(one));
		assert(_nodes.count(two));
		assert(_nodes[one].get());
		assert(_nodes[two].get());
		_nodes[one]->add_edge(_nodes[two].get());
		_nodes[two]->add_edge(_nodes[one].get());
	}

	void directed_join_nodes(const T& one, const T& two) {
		assert(_nodes.count(one));
		assert(_nodes.count(two));
		assert(_nodes[one].get());
		assert(_nodes[two].get());
		_nodes[one]->add_edge(_nodes[two].get());
	}

	void build_reachability(size_t max_depth) {
		_reachable.clear();
		_depth = max_depth;
		for (auto &x : _nodes) {
			_reachable[x.first] = find_reachable(x.first,
							     max_depth);
		}
	}

	const set<Node<T>*> find_reachable(const T& node, size_t max_depth) {
		if (max_depth == 0) max_depth = static_cast<size_t>(-1);
		deque<pair<size_t, Node<T>*>> q;
		q.push_back(make_pair(max_depth, _nodes[node].get()));
		set<Node<T>*> reachable;
		while (q.size()) {
			pair<size_t, Node<T>*> node = q.front();
			q.pop_front();
			if (reachable.count(node.second)) continue;
			reachable.insert(node.second);
			if (!node.first) continue;
			for (auto &x: node.second->edges()) {
				q.push_back(make_pair(node.first - 1, x));
			}
		}
		return reachable;
	}

	bool is_connected(const T& one, const T& two) {
		return is_connected(one, two, 0);
	}

	bool is_connected(const T& one, const T& two, size_t depth) {
		assert(_nodes.count(one));
		assert(_nodes.count(two));
		if (_depth == depth && _reachable.count(one)) {
			return _reachable[one].count(_nodes[two].get());
		}
		return find_reachable(one, depth).count(_nodes[two].get());
	}

	vector<T> get_path(const T& one, const T& two) {
		vector<T> ret;
		ret.push_back(one);
		if (get_path_impl(two, &ret)) return ret;
		return vector<T>();
	}

	set<T> greedy_cover() {
		set<T> retval;
		map<T, bool> in;
		map<T, bool> on;
		for (auto &x : _nodes) {
			if (x.second->edges().size()) {
				in[x.first] = false;
			} else {
				on[x.first] = false;
			}
		}
		size_t totalcount = 0;
		size_t rounds = 0;
		while (true) {
			size_t maxcount = 0;
			T pos;
			bool posset = false;
			for (auto &x : in) {
				if (x.second) continue;
				size_t count = 0;
				for (auto &y : _nodes[x.first]->edges()) {
					if (!on[y->data()]) count++;
				}
				if (count > maxcount) {
					maxcount = count;
					pos = x.first;
					posset = true;
				}
			}
			if (!posset) return retval;
			in[pos] = true;
			retval.insert(pos);
			rounds++;
			totalcount += maxcount;
			for (auto &x : _nodes[pos]->edges()) {
				on[x->data()] = true;
			}
		}
	}

protected:
	bool get_path_impl(const T& two, vector<T>* path) {
		for (auto &x : _nodes[path->back()]->edges()) {
			if (x == _nodes[two].get()) {
				path->push_back(two);
				return true;
			}
		}
		for (auto &x : _nodes[path->back()]->edges()) {
			bool skip = false;
			for (auto &y : *path) {
				if (_nodes[y].get() == x) skip = true;
			}
			if (skip) continue;
			path->push_back(x->data());
			if (get_path_impl(two, path)) return true;
			path->pop_back();
		}
		return false;
	}

	map<T, unique_ptr<Node<T>>> _nodes;
	map<T, set<Node<T>*>> _reachable;
	size_t _depth;

};

}  // namespace ib

#endif  // __IB__GRAPH__H__
