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
_term_close_cb (uv_handle_t *handle);

static void
_word_id_cb (uv_async_t *handle);

static void
_word_id_close_cb (uv_handle_t *handle);


/* ------------------------------------------------------------------ */


extern void
queue_term (const char *word, int did[], int didnum, int limit, term_f cb)
{
	term_work_t *w;
	term_async_t *a;


	while (didnum-- > 0) {
		NULL_CHECK(a = malloc (sizeof (*a)));
		a->cb = cb;
		UV_CHECK(uv_async_init (loop, (uv_async_t *) a, _term_cb));

		NULL_CHECK(w = malloc (sizeof (*w)));
		w->word = word;
		w->did = did[didnum];
		w->limit = limit;
		w->async = &a->async;
		UV_CHECK(uv_queue_work (loop, (uv_work_t *) w, w_term_cb, w_term_after_cb));
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

	uv_close ((uv_handle_t *) req, _term_close_cb);
}


static void
_term_close_cb (uv_handle_t *handle)
{
	term_async_t *a = (term_async_t *) handle;
	free (a);
}


extern void
queue_word_id (int wid, int did, word_f cb)
{
	word_work_t *w;
	word_async_t *a;


	NULL_CHECK(a = malloc (sizeof (*a)));
	a->cb = cb;
	UV_CHECK(uv_async_init (loop, (uv_async_t *) a, _word_id_cb));

	NULL_CHECK(w = malloc (sizeof (*w)));
	w->async = &a->async;
	w->wid = wid;
	w->did = did;
	UV_CHECK(uv_queue_work (loop, (uv_work_t *) w, w_word_cb, w_word_after_cb));
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

	uv_close ((uv_handle_t *) req, _word_id_close_cb);
}


static void
_word_id_close_cb (uv_handle_t *handle)
{
	word_async_t *a = (word_async_t *) handle;
	free (a);
}
