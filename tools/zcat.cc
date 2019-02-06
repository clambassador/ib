#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"

#include "ib/fileutil.h"
#include "ib/logger.h"
#include "ib/zcat.h"

using namespace std;

int main(int argc, char** argv) {
	if (argc != 2) {
		Logger::error("usage: zcat file");
		return -1;
	}

	string s;
	assert(Fileutil::exists(argv[1]));
	Fileutil::read_file(argv[1], &s);
	cout << ZCat::huge_zcat(s);
}
