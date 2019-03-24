#include <cassert>

#include "ib/logger.h"
#include "ib/graph.h"

using namespace std;
using namespace ib;

void trace(const set<Graph<string>::Node<string>*>& nodes) {
	for (auto x : nodes) {
		cout << x->data() << " ";
	}
	cout << endl << endl;
}

int main() {
	Graph<string> g;
	g.add_node("n1");
	g.add_node("n2");
	g.add_node("n3");
	g.add_node("n4");
	g.add_node("n5");
	g.add_node("n6");
	g.add_node("n7");
	g.add_node("n8");
	g.join_nodes("n1", "n2");
	g.join_nodes("n2", "n3");
	g.join_nodes("n3", "n4");
	g.join_nodes("n4", "n5");
	g.join_nodes("n4", "n6");
	g.join_nodes("n4", "n7");
	g.join_nodes("n4", "n8");

	trace(g.find_reachable("n4", 1));
	trace(g.find_reachable("n4", 2));
	trace(g.find_reachable("n4", 3));

	trace(g.find_reachable("n2", 1));
	trace(g.find_reachable("n2", 2));
	trace(g.find_reachable("n2", 5));
}
