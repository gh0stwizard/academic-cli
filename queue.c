#include <stdlib.h>
#include <uv.h>
#include "term.h"
#include "dic.h"
#include "check.h"
#include "loop.h"


void
queue_word (const char *word)
{

}


void
queue_term (const char *word, academic_did did, unsigned int limit)
{
	uv_work_t *w;
	term_t *term;


	NULL_CHECK(w = (uv_work_t *) malloc (sizeof (*w)));
	NULL_CHECK(term = (term_t *) malloc (sizeof (*term)));

	term->limit = limit;
	term->did = did;
	term->query = word;
	w->data = term;

	UV_CHECK(uv_queue_work (loop, w, term_cb, term_after_cb));
}
