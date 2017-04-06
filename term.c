/*
    academic-cli -- a command line utility for ACADEMIC website.
    Copyright (C) 2017  Vitaliy V. Tokarev

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

static unsigned int curl_retries;
static struct timespec curl_sleep_ts;

/* ------------------------------------------------------------------ */


extern void
term_init (term_init_t *options)
{
	curl_retries = options->retries;
	curl_sleep_ts = options->sleep_ts;
}


extern void
w_term_cb (uv_work_t *req)
{
	CURLcode code;
	CURL *handle;
	char url[MAX_URL_SIZE];
	curl_mem_t storage;
	term_result_t *result;
	term_work_t *w = (term_work_t *) req;
	unsigned int retry = 0;


	snprintf (url, MAX_URL_SIZE, ACADEMIC_URL_TERM_FMT,
		w->limit, academic_did_name[w->did], w->word);

	NULL_CHECK(storage.data = malloc (sizeof (char)));
	storage.size = 0;
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

	do {
		code = curl_easy_perform (handle);

		if (code == CURLE_OK)
			break;

		if (code == CURLE_COULDNT_RESOLVE_HOST ||
			code == CURLE_OPERATION_TIMEDOUT)
		{
			if (retry++ < curl_retries) {
				vlog (VLOG_WARN, "%s: retry #%d", w->word, retry);
				storage.size = 0;
				(void) nanosleep (&curl_sleep_ts, NULL);
			}
			else
				break;
		}
	} while (code != CURLE_OK);

	if (code != CURLE_OK) {
#ifndef _DEBUG
		uvls_logf ("%s [%d: %s]: curl error #%d: %s\n",
			w->word, w->did, academic_dname_en[w->did],
			code, curl_easy_strerror (code));
#else
		vlog (VLOG_ERROR, "%s: curl error #%d: %s",
			w->word, code, curl_easy_strerror (code));
#endif
		goto done;
	}

	vlog (VLOG_TRACE, "%s: parsing json data...\n", w->word);
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
