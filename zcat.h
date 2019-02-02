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
	static string huge_zcat(const string& data) {
		return huge_zcat(data, nullptr);
	}

	static string huge_zcat(const string& data, size_t* leftover) {
		static unique_ptr<uint8_t> out;
		const size_t CHUNK = 1 << 25;
		if (out.get() == nullptr) {
			out.reset(new uint8_t[CHUNK]);
		}

		return zcat(data, nullptr, out.get(), CHUNK);
	}

	static string zcat(const string& data) {
		uint8_t buf[1 << 17];
		return zcat(data, nullptr, buf, 1 << 17);
	}

	static string zcat(const string& data, size_t* leftover,
			   uint8_t *out, size_t CHUNK) {
		int ret;
		unsigned have;
		z_stream strm;

		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;
		// garbage fire
		ret = inflateInit2(&strm, 9 + 16);
		assert(ret == Z_OK);

		stringstream ss;
		strm.avail_in = data.length();
		strm.next_in = (uint8_t*) data.c_str();
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_FINISH);
			assert(ret != Z_STREAM_ERROR);
			assert(ret != Z_NEED_DICT);
			have = CHUNK - strm.avail_out;
			if (ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
				ss << string((char*) out, have);
				if (leftover) *leftover = strm.avail_in;
				inflateEnd(&strm);
				return ss.str();
			}
			ss << string((char*) out, have);
		} while (strm.avail_out == 0);
		if (leftover) *leftover = strm.avail_in;
		inflateEnd(&strm);
	        return ss.str();
	}
};

}  // namespace ib

#endif  // __IB__ZCAT__H__
