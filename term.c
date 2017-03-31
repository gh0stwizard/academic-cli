#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <uv.h>
#include "vlog.h"
#include "check.h"
#include "term.h"
#include "curl_helpers.h"
#include "json.h"


/* ------------------------------------------------------------------ */


extern void
term_cb (uv_work_t *req)
{
	CURL *handle;
	term_t *w;
	char url[MAX_URL_SIZE];
	curl_mem_t storage;
	term_result_t *result;


	w = (term_t *) req->data;

	snprintf (url, MAX_URL_SIZE, ACADEMIC_URL_TERM_FMT,
		w->limit, academic_did_name[w->did], w->word);

	vlog (VLOG_DEBUG, "%s: %s", w->word, url);

	NULL_CHECK(storage.data = malloc (sizeof (char)));

	NULL_CHECK(handle = curl_easy_init ());

	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_URL, url));

#ifdef _DEBUG_CURL
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_VERBOSE, 1L));
#else
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_VERBOSE, 0L));
#endif

	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_LOW_SPEED_TIME, 3L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_LOW_SPEED_LIMIT, 1L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_CONNECTTIMEOUT, 30L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_FOLLOWLOCATION, 1L));
	CURL_CHECK(curl_easy_setopt
		(handle, CURLOPT_WRITEFUNCTION, curl_mem_write_cb));
	CURL_CHECK(curl_easy_setopt
		(handle, CURLOPT_WRITEDATA, (void *) &storage));
	
	CURL_CHECK(curl_easy_perform (handle));

	parse_json (storage.data, storage.size, &result);
	/* copy query data back that the main thread recogninise it */
	result->word = w->word;
	result->did = w->did;
	/* write result & send it to main thread */
	uv_rwlock_wrlock (w->lock);
	w->async->data = result;
	uv_rwlock_wrunlock (w->lock);
	/* notify main thread that we have finished */
	uv_async_send (w->async);

	curl_easy_cleanup (handle);
	free (storage.data);
}


extern void
term_after_cb (uv_work_t *req, int status)
{
	term_t *w = (term_t *) req->data;


	if (status != 0)
		vlog (VLOG_ERROR, "%p: status %d", req, status);

	if (w->lock != NULL) {
		uv_rwlock_destroy (w->lock);
		free (w->lock);
	}

	req->data = NULL;
	free (w);
	free (req);
}
