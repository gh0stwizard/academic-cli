#ifndef CURL_HELPERS_H__
#define CURL_HELPERS_H__

#include <curl/curl.h>


typedef struct curl_mem_s {
	char *data;
	size_t size;
} curl_mem_t;


extern size_t
curl_mem_write_cb (char *data, size_t size, size_t nmemb, void *userp);


#endif /* CURL_HELPERS_H__ */
