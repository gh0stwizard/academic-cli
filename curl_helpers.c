#include <stdlib.h>
#include <string.h>
#include "curl_helpers.h"
#include "vlog.h"


extern size_t
curl_mem_write_cb (char *data, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	curl_mem_t *mem = (curl_mem_t *) userp;


	mem->data = realloc (mem->data, mem->size + realsize + 1);

	if (mem->data != NULL) {
		memcpy (&(mem->data[mem->size]), data, realsize);
		mem->size += realsize;
		mem->data[mem->size] = '\0';

		return realsize;
	}

	vlog (VLOG_ERROR, "%s: realloc: out of memory", __func__);

	return 0;
}
