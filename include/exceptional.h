#ifndef __IB__EXCEPTIONAL__H__
#define __IB__EXCEPTIONAL__H__

#define ASSERT(a, ...) \
	do {\
		if (!(a)) {\
			Logger::error("%:%", __FILE__, __LINE__); \
			Logger::error(__VA_ARGS__);\
			assert(0);\
		}\
	} while (false);

#define THROW(...) \
	do {\
		throw Logger::stringify(__VA_ARGS__);\
	} while (false);

#define LOG_IF(a, ...) \
	do {\
		if (a) {\
			Logger::info(__VA_ARGS__);\
		}\
	} while (false);

#endif  // __IB__EXCEPTIONAL__H__
