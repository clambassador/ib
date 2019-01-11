#include <cassert>

#include "ib/logger.h"
#include "ib/containers.h"

using namespace std;
using namespace ib;

int main() {
	vector<string> a;
	a.push_back("hello");
	a.push_back("there");
	a.push_back("my");
	a.push_back("good");
	a.push_back("world");

	{
		vector<string> b;
		b.push_back("there");
		b.push_back("hello");
		b.push_back("world");

		vector<string> out;
		Containers::reorder_vector(a, b, &out);
		assert(out[0] == "hello");
		assert(out[1] == "there");
		assert(out[2] == "world");
	}
	{
		vector<string> b;
		b.push_back("world");

		vector<string> out;
		Containers::reorder_vector(a, b, &out);
		assert(out[0] == "world");
	}
	{
		vector<string> b;
		b.push_back("there");
		b.push_back("my");
		b.push_back("hello");
		b.push_back("hello");
		b.push_back("world");

		vector<string> out;
		Containers::reorder_vector(a, b, &out);
		Logger::info("% % %", a, b, out);
		assert(out[0] == "hello");
		assert(out[1] == "there");
		assert(out[2] == "my");
		assert(out[3] == "world");
	}
}
