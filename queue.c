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
#include <string.h>
#include <uv.h>
#include "queue.h"
#include "check.h"
#include "loop.h"
#include "vlog.h"


static void
_term_cb (uv_async_t *handle);

static void
_word_id_cb (uv_async_t *handle);


/* ------------------------------------------------------------------ */

static term_async_t term_async;
static word_async_t word_async;

/* ------------------------------------------------------------------ */


extern void
queue_init (queue_init_t *options)
{
	term_init_t term_opts;
	word_init_t word_opts;


	UV_CHECK(uv_async_init (loop, (uv_async_t *) &term_async, _term_cb));
	UV_CHECK(uv_async_init (loop, (uv_async_t *) &word_async, _word_id_cb));
	term_async.cb = options->term_cb;
	word_async.cb = options->word_cb;
	uv_unref ((uv_handle_t*) &term_async.async);
	uv_unref ((uv_handle_t*) &word_async.async);

	term_opts.retries = word_opts.retries = options->curl.retries;
	term_opts.sleep_ts = word_opts.sleep_ts = options->curl.retry_sleep;

	term_init (&term_opts);
	word_init (&word_opts);
}


extern void
queue_term (const char *word, int did[], int didnum, int limit)
{
	term_work_t *t;


	while (didnum-- > 0) {
		NULL_CHECK(t = malloc (sizeof (*t)));
		t->async = &term_async.async;
		t->word = word;
		t->did = did[didnum];
		t->limit = limit;
		UV_CHECK(uv_queue_work
			(loop, (uv_work_t *) t, w_term_cb, w_term_after_cb));
	}
}


static void
_term_cb (uv_async_t *req)
{
	term_result_t *t = (term_result_t *) req->data;
	term_async_t *a = (term_async_t *) req;


	if (a->cb != NULL)
		a->cb (t);
	else
		free_term_results (t);
}


extern void
queue_word_id (const char *word, int wid, int did, word_f cb)
{
	word_work_t *w;


	NULL_CHECK(w = malloc (sizeof (*w)));
	w->async = &word_async.async;
	w->word = word;
	w->wid = wid;
	w->did = did;
	UV_CHECK(uv_queue_work
		(loop, (uv_work_t *) w, w_word_cb, w_word_after_cb));
}


static void
_word_id_cb (uv_async_t *req)
{
	word_result_t *w = (word_result_t *) req->data;
	word_async_t *a = (word_async_t *) req;


	if (a->cb != NULL)
		a->cb (w);
	else
		free_word_results (w);
}
