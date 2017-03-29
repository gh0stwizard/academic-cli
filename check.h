#ifndef CHECK_H__
#define CHECK_H__

#include <assert.h>

#define CURL_CHECK(x)	do { assert ((x) == CURLE_OK); } while (0)
#define CURLM_CHECK(x)	do { assert ((x) == CURLM_OK); } while (0)
#define NULL_CHECK(x)	do { assert ((x) != NULL); } while (0)
#define UV_CHECK(x)	do { assert ((x) == 0); } while (0)

#endif /* CHECK_H__ */
