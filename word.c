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

#include <stdlib.h>
#include <curl/curl.h>
#include <uv.h>
#include "word.h"
#include "vlog.h"
#include "check.h"
#include "curl_helpers.h"
#include "html.h"


/* ------------------------------------------------------------------ */

static unsigned int curl_retries;
static struct timespec curl_sleep_ts;

/* ------------------------------------------------------------------ */


extern void
word_init (word_init_t *options)
{
	curl_retries = options->retries;
	curl_sleep_ts = options->sleep_ts;
}


extern void
w_word_cb (uv_work_t *req)
{
	CURLcode code;
	CURL *handle;
	char url[MAX_URL_SIZE];
	curl_mem_t storage;
	word_result_t *result = NULL;
	word_work_t *w = (word_work_t *) req;
	unsigned int retry = 0;


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

	do {
		code = curl_easy_perform (handle);

		if (code == CURLE_OK)
			break;

		if (code == CURLE_COULDNT_RESOLVE_HOST ||
			code == CURLE_OPERATION_TIMEDOUT)
		{
			if (retry++ < curl_retries) {
				vlog (VLOG_WARN, "%s [id: %d did: %d]: retry #%d",
					w->word, w->wid, w->did, retry);
				nanosleep (&curl_sleep_ts, NULL);
			}
			else
				break;
		}
	} while (code != CURLE_OK);

	if (code != CURLE_OK) {
#ifndef _DEBUG
		uvls_logf ("%s [id: %d did: %d]: curl error #%d: %s\n",
			w->word, w->wid, w->did, code, curl_easy_strerror (code));
#else
		vlog (VLOG_ERROR, "%s [id: %d did: %d]: curl error #%d: %s",
			w->word, w->wid, w->did, code, curl_easy_strerror (code));
#endif
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
