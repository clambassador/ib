#include <string>
#include <vector>
#include <unistd.h>

#include "ib/logger.h"
#include "ib/tokenizer.h"

using namespace std;
using namespace ib;

int main() {
	vector<string> tokens;
	Tokenizer::split("hello there \"good people\"", " ", &tokens);
	Logger::info("%", tokens);
	tokens.clear();
	Tokenizer::split_mind_quote("hello there \"good people\"", " ", &tokens);
	Logger::info("%", tokens);
	tokens.clear();
}
