#include <cassert>

#include "ib/logger.h"
#include "ib/graph.h"

using namespace std;
using namespace ib;

int main() {
	Graph<string> g;
	g.add_node("hello");
	g.add_node("there");
}
