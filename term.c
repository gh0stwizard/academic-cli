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
w_term_cb (uv_work_t *req)
{
	CURLcode code;
	CURL *handle;
	char url[MAX_URL_SIZE];
	curl_mem_t storage;
	term_result_t *result;
	term_work_t *w = (term_work_t *) req;


	snprintf (url, MAX_URL_SIZE, ACADEMIC_URL_TERM_FMT,
		w->limit, academic_did_name[w->did], w->word);

	NULL_CHECK(storage.data = malloc (sizeof (char)));
	NULL_CHECK(result = malloc (sizeof (*result)));
	result->word = w->word;
	result->did = w->did;
	result->limit = w->limit;
	result->list = NULL;
	result->entries = 0;

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
	vlog (VLOG_TRACE, "%s: %s", w->word, url);
	code = curl_easy_perform (handle);

	if (code != CURLE_OK) {
		/* TODO: CURLE_OPERATION_TIMEDOUT */
		uvls_logf ("%s: curl [%d]: %s\n",
			w->word, code, curl_easy_strerror (code));
		goto done;
	}

	vlog (VLOG_TRACE, "%s: parsing json data...", w->word);
	result->entries = parse_json (storage.data, storage.size, &result->list);

done:
	w->async->data = result;
	vlog (VLOG_TRACE, "%s: sending results...", w->word);
	UV_CHECK(uv_async_send (w->async));

	curl_easy_cleanup (handle);
	free (storage.data);
}


extern void
w_term_after_cb (uv_work_t *req, int status)
{
	term_work_t *w = (term_work_t *) req;
	vlog (VLOG_TRACE, "%s: done; status %d", w->word, status);
	free (w);
}


extern void
free_term_results (term_result_t *t)
{
	term_entry_t *e = e = t->list;
	term_entry_t *end = e + t->entries;


	vlog (VLOG_TRACE, "%s: cleanup", t->word);

	for (; e != end; e++) {
		free (e->id);
		free (e->value);
		free (e->info);
	}

	free (t->list);
	free (t);
}
