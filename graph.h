#ifndef __IB__GRAPH__H__
#define __IB__GRAPH__H__

#include <deque>
#include <map>
#include <memory>

using namespace std;

namespace ib {

/* Graph of nodes and edges. Nodes have template type T and edges have template
 * type R. R can be nullptr_t, in which case only the fact there is an edge
 * matters and no information is stored with edges.
 * Otherwise R can be any type so as to include additional information about the
 * edges such as an annotation. The R type must support the copy constructor.
 *
 * The type for node must support equality, i.e., operator== and the copy
 * constructor. If two T types are equal according to ==, then they are the same
 * node in the graph. Nodes can thus be looked-up just by their T value.
 *
 * A simple instantiation of Graph is Graph<string>, where each unique string is
 * a node and there is no edge values.
 */
template <typename T, typename R = nullptr_t>
class Graph {
public:
	Graph() {}
	virtual ~Graph() {}

	// TODO: create a sanity / consistency checker

	/* Node subclass for the graph. Each node tracks a list its edges. */
	class Edge;
	class Node {
	public:
		Node(const T& r) {
			_r.reset(new T(r));
		}

		Node(T* r) {
			_r.reset(r);
		}

		virtual ~Node() = default;

		// returns a const reference to the data object associated
		const T& data() { return *_r.get(); }

		// returns the data object associated with node as pointer
		T* ptr_data() { return _r.get(); }

		// add an edge from this node to another
		void add_edge(Node* other, Edge* edge) {
			_edges[other] = edge;
		}

		// returns true iff this node has an edge to the target
		bool has_edge(const T& target) {
			for (auto& x: _edges) {
				if (x.first->data() == target) return true;
			}
			return false;
		}

		// returns the edge map
		map<Node*, Edge*> edges() { return _edges; }

	protected:
		unique_ptr<T> _r;
		map<Node*, Edge*> _edges;
	};

	class Edge {
	public:
		Edge(Node* one, Node* two) : Edge(one, two, false) {}
		Edge(Node* one, Node* two, bool directed)
			: Edge(one, two, directed, nullptr) {}
		Edge(Node* one, Node* two, bool directed, R* data)
			: _one(one), _two(two), _directed(directed) {
			_r.reset(data);
		}

		virtual ~Edge() = default;

		const R& data() { return *_r.get(); }

		R* ptr_data() { return _r.get(); }

	protected:
		Node* _one;
		Node* _two;
		bool _directed;
		unique_ptr<R> _r;
	};

	/* Add a new node val if it is not there */
	void maybe_add_node(const T& val) {
		if (!_nodes.count(val)) _nodes[val].reset(new Node(val));
	}

	/* Check val is not a node, then add it by copying val */
	void add_node(const T& val) {
		assert(!_nodes.count(val));
		_nodes[val].reset(new Node(val));
	}

	/* Check *val is not a node, then add it by taking ownership of val */
	void add_node(T* val) {
		assert(!_nodes.count(*val));
		_nodes[*val].reset(new Node(val));
	}

	/* Add nodes one and two and then an edge that connects them. Takes
	 * ownership of edge for the edge annotation */
	void add_join_nodes(const T& one, const T& two, R* edge) {
		maybe_add_node(one);
		maybe_add_node(two);
		_edges.emplace_back(nullptr);
		_edges.back().reset(new Edge(_nodes[one].get(),
					     _nodes[two].get(),
					     true, edge));
		join_nodes(one, two, _edges.back().get());
	}

	/* Add nodes one and two and then an edge that connects them. Copies the
	 * value of edge for the edge annotation */
	void add_join_nodes(const T& one, const T& two, const R& edge) {
		maybe_add_node(one);
		maybe_add_node(two);
		_edges.emplace_back(nullptr);
		_edges.back().reset(new Edge(_nodes[one].get(),
					     _nodes[two].get(),
					     true, new R(edge)));
		join_nodes(one, two, _edges.back().get());
	}

	/* Add nodes one and two and then an edge that connects them */
	void add_join_nodes(const T& one, const T& two) {
		add_join_nodes(one, two, nullptr);
	}

	void add_directed_join_nodes(const T& one, const T& two, R* edge) {
		maybe_add_node(one);
		maybe_add_node(two);
		_edges.emplace_back(nullptr);
		_edges.back().reset(new Edge(_nodes[one].get(),
					     _nodes[two].get(),
					     false, edge));
		directed_join_nodes(one, two, _edges.back().get());
	}

	void add_directed_join_nodes(const T& one, const T& two, const R& edge) {
		maybe_add_node(one);
		maybe_add_node(two);
		_edges.emplace_back(nullptr);
		_edges.back().reset(new Edge(_nodes[one].get(),
					     _nodes[two].get(),
					     false, new R(edge)));
		directed_join_nodes(one, two, _edges.back().get());
	}

	void add_directed_join_nodes(const T& one, const T& two) {
		add_directed_join_nodes(one, two, nullptr);
	}

	void check_nodes(const T& one, const T& two) {
		assert(_nodes.count(one));
		assert(_nodes.count(two));
		assert(_nodes[one].get());
		assert(_nodes[two].get());
	}

	void join_nodes(const T& one, const T& two) {
		_edges.emplace_back(nullptr);
		_edges.back().reset(new Edge(_nodes[one].get(),
					     _nodes[two].get(),
					     false, nullptr));
	}


	void join_nodes(const T& one, const T& two, Edge* edge) {
		check_nodes(one, two);

		_nodes[one]->add_edge(_nodes[two].get(), edge);
		_nodes[two]->add_edge(_nodes[one].get(), edge);
	}

	void directed_join_nodes(const T& one, const T& two) {
		check_nodes(one, two);
		Node* n_one = _nodes[one].get();
		Node* n_two = _nodes[two].get();
		_edges.emplace_back(nullptr);
		_edges.back().reset(new Edge(n_one, n_two, true, nullptr));
		n_one->add_edge(n_two, _edges.back().get());
	}

	void directed_join_nodes(const T& one, const T& two, Edge* edge) {
		check_nodes(one, two);

		_nodes[one]->add_edge(_nodes[two].get(), edge);
	}

	void build_reachability(size_t max_depth) {
		_reachable.clear();
		_depth = max_depth;
		for (auto &x : _nodes) {
			_reachable[x.first] = find_reachable(x.first,
							     max_depth);
		}
	}

	const set<Node*> find_reachable(const T& node, size_t max_depth) {
		if (max_depth == 0) max_depth = static_cast<size_t>(-1);
		deque<pair<size_t, Node*>> q;
		q.push_back(make_pair(max_depth, _nodes[node].get()));
		set<Node*> reachable;
		while (q.size()) {
			pair<size_t, Node*> node = q.front();
			q.pop_front();
			if (reachable.count(node.second)) continue;
			reachable.insert(node.second);
			if (!node.first) continue;
			for (auto &x: node.second->edges()) {
				q.push_back(make_pair(node.first - 1, x.first));
			}
		}
		return reachable;
	}

	// returns true if node one is connected to node two
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

	/* returns an empty vector if there is no path from node one to two.
	 * Returns a list of node/edge pairs for a path otherwise. The last
	 * element on the path has a null edge as it is the end of the walk. */
	// TODO: make path itself a class
	// TODO: is this depth first? make breadth first
	vector<pair<T*, R*>> get_path(const T& one, const T& two) {
		Node* n_one = _nodes[one].get();
		Node* n_two = _nodes[two].get();
		return get_path(n_one, n_two);
	}

	vector<pair<T*, R*>> get_path(Node* one, Node* two) {
		vector<pair<T*, R*>> ret;
		if (!one || !two) return ret;
		ret.emplace_back(make_pair(one->ptr_data(), nullptr));
		if (get_path_impl(two, one, &ret)) return ret;
		return move(vector<pair<T*, R*>>());
	}

	set<T> greedy_cover() {
		return greedy_cover(0);
	}

	set<T> greedy_cover(size_t min_benefit) {
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
					if (!on[y.first->data()]) count++;
				}
				if (count > maxcount) {
					maxcount = count;
					pos = x.first;
					posset = true;
				}
			}
			if (!posset) return retval;
			if (maxcount < min_benefit) return retval;
			in[pos] = true;
			retval.insert(pos);
			rounds++;
			totalcount += maxcount;
			for (auto &x : _nodes[pos]->edges()) {
				on[x.first->data()] = true;
			}
		}
	}

protected:
	bool get_path_impl(Node* to, Node* from, vector<pair<T*, R*>>* path) {
		for (auto &x : from->edges()) {
			if (x.first == to) {
				path->back().second = x.second->ptr_data();
				path->push_back(make_pair(to->ptr_data(), nullptr));
				return true;
			}
		}
		for (auto &x : from->edges()) {
			bool skip = false;
			for (auto &y : *path) {
				if (_nodes[*y.first].get() == x.first) skip = true;
			}
			if (skip) continue;
			path->back().second = x.second->ptr_data();
			path->push_back(make_pair(x.first->ptr_data(), nullptr));
			if (get_path_impl(to, x.first, path)) return true;
			path->pop_back();
		}
		return false;
	}

	map<T, unique_ptr<Node>> _nodes;
	list<unique_ptr<Edge>> _edges;
	map<T, set<Node*>> _reachable;
	size_t _depth;

};

}  // namespace ib

#endif  // __IB__GRAPH__H__
