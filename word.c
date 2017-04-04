#include <stdlib.h>
#include <curl/curl.h>
#include <uv.h>
#include "word.h"
#include "vlog.h"
#include "check.h"
#include "curl_helpers.h"
#include "html.h"


/* ------------------------------------------------------------------ */


extern void
w_word_cb (uv_work_t *req)
{
	CURLcode code;
	CURL *handle;
	char url[MAX_URL_SIZE];
	curl_mem_t storage;
	word_result_t *result = NULL;
	word_work_t *w = (word_work_t *) req;


	snprintf (url, MAX_URL_SIZE, academic_durl_fmt[w->did], w->wid);

	NULL_CHECK(storage.data = malloc (sizeof (char)));
	NULL_CHECK(result = malloc (sizeof (*result)));
	result->word = w->word;
	result->term = NULL;
	result->data = NULL;
	result->wid = w->wid;
	result->did = w->did;

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
	vlog (VLOG_DEBUG, "%s [id: %d did: %d]: %s",
		w->word, w->wid, w->did, url);
	code = curl_easy_perform (handle);

	if (code != CURLE_OK) {
		uvls_logf ("%s [id: %d did: %d]: curl error #%d: %s\n",
			w->word, w->wid, w->did, code, curl_easy_strerror (code));
		goto done;
	}

	vlog (VLOG_TRACE, "%s [id: %d did: %d]: parsing html (size %zu)...",
		w->word, w->wid, w->did, storage.size);
	result->term = parse_html (storage.data, storage.size, &result->data);	

done:
	w->async->data = result;
	vlog (VLOG_TRACE, "%s [id: %d did: %d]: sending results...",
		w->word, w->wid, w->did);
	UV_CHECK(uv_async_send (w->async));

	curl_easy_cleanup (handle);
	free (storage.data);
}


extern void
w_word_after_cb (uv_work_t *req, int status)
{
	word_work_t *w = (word_work_t *) req;
	vlog (VLOG_TRACE, "%s [id: %d did: %d]: done; status %d",
		w->word, w->wid, w->did, status);
	free (w);
}


extern void
free_word_results (word_result_t *r)
{
	vlog (VLOG_TRACE, "%s [id: %d did: %d]: cleanup",
		r->word, r->wid, r->did);

	if (r->term)
		free (r->term);

	if (r->data)
		free_html_data (r->data);

	free (r);
}
