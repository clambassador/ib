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

void cover_test() {
	Graph<string> g;

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

	Logger::info("%", g.get_path("n1", "n7"));
	Logger::info("%", g.get_path("n2", "n8"));
	Logger::info("%", g.get_path("n4", "n5"));

	g.add_node("n9");
	g.add_node("n10");
	g.directed_join_nodes("n9", "n10");
	Logger::info("%", g.get_path("n9", "n10"));
	Logger::info("%", g.get_path("n5", "n10"));
	Logger::info("%", g.get_path("n10", "n9"));

	cover_test();
}
