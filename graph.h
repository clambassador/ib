#ifndef __IB__GRAPH__H__
#define __IB__GRAPH__H__

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

	protected:
		unique_ptr<R> _r;
		set<Node<R>> _edges;
	};

	void add_node(const T& val) {
		_nodes[val].reset(new Node<T>(val));
	}

	void add_node(T* val) {
		_nodes[*val].reset(new Node<T>(val));
	}

	void join_nodes(const T& one, const T& two) {
		assert(_nodes.count(one));
		assert(_nodes.count(two));
		assert(_nodes[one].get());
		assert(_nodes[two].get());
		_nodes[one]->add_edge(_nodes[two].get());
		_nodes[two]->add_edge(_nodes[one].get());
	}


protected:
	map<T, unique_ptr<Node<T>>> _nodes;

};

}  // namespace ib

#endif  // __IB__GRAPH__H__
