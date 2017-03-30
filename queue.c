#include <stdlib.h>
#include <uv.h>
#include "term.h"
#include "dic.h"
#include "check.h"
#include "loop.h"
#include "vlog.h"


static void
_term_async_cb (uv_async_t *handle);


void
queue_word (const char *word, academic_did did_list[])
{
	
}


void
queue_term (const char *word, academic_did did, unsigned int limit)
{
	uv_work_t *w;
	term_t *term;
	uv_async_t *async;
	uv_rwlock_t *lock;


	NULL_CHECK(w = (uv_work_t *) malloc (sizeof (*w)));
	NULL_CHECK(term = (term_t *) malloc (sizeof (*term)));
	NULL_CHECK(async = (uv_async_t *) malloc (sizeof (*async)));
	NULL_CHECK(lock = (uv_rwlock_t *) malloc (sizeof (*lock)));

	UV_CHECK(uv_async_init (loop, async, _term_async_cb));
	UV_CHECK(uv_rwlock_init (lock));

	term->limit = limit;
	term->did = did;
	term->query = word;
	term->async = async;
	term->lock = lock;
	w->data = term;

	UV_CHECK(uv_queue_work (loop, w, term_cb, term_after_cb));
}


static void
_term_async_cb (uv_async_t *handle)
{
	term_result_t *t = (term_result_t *) handle->data;
	term_data_t *list;
	int i, total;


	if (t != NULL) {
		vlog (VLOG_TRACE, "%s: cleanup", __func__);
		list = t->list;
		total = t->entries;
		for (i = 0; i < total; i++, list++)
			vlog (VLOG_TRACE, "id: %s value: %s %d/%d",
				list->id, list->value, i, total);
	}
	else {
		vlog (VLOG_TRACE, "%s: nothing to do", __func__);
	}

	uv_close ((uv_handle_t *) handle, NULL);
	free (handle);
}
