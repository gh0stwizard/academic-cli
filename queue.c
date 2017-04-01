#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include "queue.h"
#include "term.h"
#include "dic.h"
#include "check.h"
#include "loop.h"
#include "vlog.h"


static void
_term_cb (uv_async_t *handle);

static void
_word_id_cb (uv_async_t *handle);

static void
_queue_word_id (int word_id, int did, uv_async_cb cb);

static void
_free_term_results (term_result_t *t);

static void
_free_dic_results (dic_result_t *d);

static void
_close_cb (uv_handle_t *handle);


/* ------------------------------------------------------------------ */


extern void
queue_word (const char *word, int did[], int did_num)
{
	while (did_num-- > 0)
		queue_term (word, did[did_num], ACADEMIC_TERM_LIMIT, _term_cb);
}


extern void
query_word_by_id (int word_id, int did[], int did_num)
{
	while (did_num-- > 0)
		_queue_word_id (word_id, did[did_num], _word_id_cb);
}


static void
_queue_word_id (int word_id, int did, uv_async_cb cb)
{
	uv_work_t *w;
	uv_async_t *async;
	uv_rwlock_t *lock;
	dic_t *dic;


	NULL_CHECK(w = (uv_work_t *) malloc (sizeof (*w)));
	NULL_CHECK(async = (uv_async_t *) malloc (sizeof (*async)));
	NULL_CHECK(lock = (uv_rwlock_t *) malloc (sizeof (*lock)));
	NULL_CHECK(dic = (dic_t *) malloc (sizeof (*dic)));

	UV_CHECK(uv_async_init (loop, async, cb));
	UV_CHECK(uv_rwlock_init (lock));

	dic->async = async;
	dic->lock = lock;
	dic->word_id = word_id;
	dic->did = did;

	w->data = dic;
	UV_CHECK(uv_queue_work (loop, w, dic_cb, dic_after_cb));
}


extern void
queue_term (const char *word, int did, int limit, term_cb_t cb)
{
	uv_work_t *w;
	uv_async_t *async;
	uv_rwlock_t *lock;
	term_t *term;


	NULL_CHECK(w = (uv_work_t *) malloc (sizeof (*w)));
	NULL_CHECK(term = (term_t *) malloc (sizeof (*term)));
	NULL_CHECK(async = (uv_async_t *) malloc (sizeof (*async)));
	NULL_CHECK(lock = (uv_rwlock_t *) malloc (sizeof (*lock)));

	UV_CHECK(uv_async_init (loop, async, cb));
	UV_CHECK(uv_rwlock_init (lock));

	term->limit = limit;
	term->did = did;
	term->word = word;
	term->async = async;
	term->lock = lock;

	w->data = term;
	UV_CHECK(uv_queue_work (loop, w, term_cb, term_after_cb));
}


static void
_term_cb (uv_async_t *handle)
{
	term_result_t *t = (term_result_t *) handle->data;
	term_data_t *e, *end;
	const char *word;
	size_t len;
	int matched = 0;

	if (t != NULL) {
		word = t->word;
		len = strlen (word);

		for (e = t->list, end = e + t->entries; e < end; e++) {
			vlog (VLOG_TRACE, "id: %s value: '%s'", e->id, e->value);
			if (! matched && strncmp (word, e->value, len) == 0)
				matched = atoi (e->id);
		}

		if (matched > 0) {
			vlog (VLOG_TRACE, "%s: exact match by id %d", word, matched);
			_queue_word_id (matched, t->did, _word_id_cb);
		}
		else {
			/* TODO */
			vlog (VLOG_NOTE, "%s: no exact match, %d available",
				word, t->entries);
		}
		
		handle->data = NULL;
		_free_term_results (t);
	}
	else {
		vlog (VLOG_ERROR, "nothing to do");
	}

	uv_close ((uv_handle_t *) handle, _close_cb);
}


static void
_free_term_results (term_result_t *t)
{
	term_data_t *e, *end;


	vlog (VLOG_TRACE, t->word);

	for (e = t->list, end = e + t->entries; e != end; e++) {
		free (e->id);
		free (e->value);
		free (e->info);
	}

	free (t->list);
	free (t);
}


static void
_word_id_cb (uv_async_t *handle)
{
	dic_result_t *d = (dic_result_t *) handle->data;


	if (d != NULL) {
		vlog (VLOG_TRACE, "%s: %s", d->term, d->data->text);

		handle->data = NULL;
		_free_dic_results (d);
	}
	else {
		vlog (VLOG_ERROR, "nothing to do");
	}

	uv_close ((uv_handle_t *) handle, _close_cb);
}


static void
_free_dic_results (dic_result_t *d)
{
	vlog (VLOG_TRACE, "%d", d->word_id);

	if (d->term)
		free (d->term);

	if (d->data)
		free_html_data (d->data);

	free (d);
}


static void
_close_cb (uv_handle_t *handle)
{
	free (handle);
}
