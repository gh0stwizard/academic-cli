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
	CURLcode code;
	CURL *handle;
	term_t *w;
	char url[MAX_URL_SIZE];
	curl_mem_t storage;
	term_result_t *result = NULL;


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

	/* perform request */
	code = curl_easy_perform (handle);

	if (code != CURLE_OK) {
		vlog (VLOG_WARN, "%s: curl [%d]: %s\n",
			code, curl_easy_strerror (code));
		goto done;
	}

	vlog (VLOG_TRACE, "%s: parsing json data...", w->word);
	parse_json (storage.data, storage.size, &result);

	/* copy query data back that the main thread recogninise it */
	if (result) {
		result->word = w->word;
		result->did = w->did;
	}

done:

	/* write result & send it to main thread */
	uv_rwlock_wrlock (w->lock);
	w->async->data = result;
	uv_rwlock_wrunlock (w->lock);

	/* notify main thread that we have finished */
	vlog (VLOG_TRACE, "%s: sending results...", w->word);
	UV_CHECK(uv_async_send (w->async));

	free (storage.data);
	curl_easy_cleanup (handle);
}


extern void
term_after_cb (uv_work_t *req, int status)
{
	term_t *w = (term_t *) req->data;


	vlog (VLOG_TRACE, "%s: done; status %d", w->word, status);

	if (status != 0)
		vlog (VLOG_ERROR, "%p: status %d", req, status);

	if (w->lock != NULL) {
		uv_rwlock_destroy (w->lock);
		free (w->lock);
	}

	free (w);
	free (req);
}
