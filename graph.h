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

		set<Node<R> *> edges() { return _edges; }

	protected:
		unique_ptr<R> _r;
		set<Node<R> *> _edges;
	};

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

	void join_nodes(const T& one, const T& two) {
		assert(_nodes.count(one));
		assert(_nodes.count(two));
		assert(_nodes[one].get());
		assert(_nodes[two].get());
		_nodes[one]->add_edge(_nodes[two].get());
		_nodes[two]->add_edge(_nodes[one].get());
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

protected:
	map<T, unique_ptr<Node<T>>> _nodes;
	map<T, set<Node<T>*>> _reachable;
	size_t _depth;

};

}  // namespace ib

#endif  // __IB__GRAPH__H__
