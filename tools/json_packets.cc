#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "ib/csv_table.h"
#include "ib/fileutil.h"
#include "ib/formatting.h"
#include "ib/logger.h"
#include "ib/set.h"
#include "ib/tokenizer.h"
#include "ib/json.h"

using namespace std;
using namespace ib;

string pii(const string& packet, JSON& main) {
	stringstream ss;
	set<string> found;
	for (JSON &x : main["result"]["pii_found"]) {
		if (packet == (string) x["packet_id"]) found.insert((string)
								    x["pii_type"]);
	}
	if (found.empty()) return "";
	for (auto &x : found) {
		ss << "+" << x;
	}
	return ss.str().substr(1);
}

//LCOV_EXCL_START
int main(int argc, char** argv) {
	string s;
	string seek;
	if (argc > 3 || argc == 1) {
		Logger::error("usage: % [jsonfile] search", argv[0]);
		return -1;
	}
	if (argc == 2) {
		Fileutil::read_stream(cin, &s);
		seek = argv[1];
	} else {
		assert(argc == 3);
		Fileutil::read_file(argv[1], &s);
		seek = argv[2];
	}
        JSON json;
        json.parse(s);

	for (JSON &x : json["result"]["transmissions"]) {
		if (((string)x["domain"]).find(seek) != string::npos) {
			cout << (string)json["app_package_name"] << "," <<  (string) x["domain"] << "," << (string)
			    (string) x["is_tls"] << "," << (string) x["packet_id"] << "," <<
			    pii((string)x["packet_id"], json) << endl;
		}
	}
	return 0;
}
//LCOV_EXCL_STOP
