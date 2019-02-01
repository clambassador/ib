#ifndef __IB__ZCAT__H__
#define __IB__ZCAT__H__

#include <string>
#include <sstream>
#include <string.h>
#include <assert.h>
#include "zlib.h"

using namespace std;

namespace {

class ZCat {
public:
	static string zcat(const string& data) {
		return zcat(data, nullptr);
	}

	static string zcat(const string& data, size_t* leftover) {
		const int CHUNK = 1 << 12;
		int ret;
		unsigned have;
		z_stream strm;
		unsigned char in[CHUNK];
		unsigned char out[CHUNK];

		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;
		// garbage fire
		ret = inflateInit2(&strm, 9 + 16);
		assert(ret == Z_OK);

		size_t pos = 0;
		stringstream ss;
		do {
			size_t N = CHUNK;
			if (data.length() - pos < CHUNK)
				N = data.length() - pos;
			strm.avail_in = N;
			memcpy(in, data.c_str() + pos, N);
			pos += N;
			strm.next_in = in;
			do {
				strm.avail_out = CHUNK;
				strm.next_out = out;
				ret = inflate(&strm, Z_NO_FLUSH);
				assert(ret != Z_STREAM_ERROR);
				assert(ret != Z_NEED_DICT);
				have = CHUNK - strm.avail_out;
				if (ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
					assert(!have);
					inflateEnd(&strm);
					return ss.str();
				}
				ss << string((char*) out, have);
			} while (strm.avail_out == 0);
		} while (ret != Z_STREAM_END);
		inflateEnd(&strm);
		if (leftover) *leftover = strm.avail_in;
	        return ss.str();
	}
};

}  // namespace ib

#endif  // __IB__ZCAT__H__
