#ifndef TERM_H__
#define TERM_H__

#include <uv.h>
#include "academic.h"


typedef struct term_entry_s {
	char *id;
	char *value;
	char *info;
} term_entry_t;

typedef struct term_result_s {
	const char *word;
	int did;
	int limit;
	term_entry_t *list;
	int entries;
} term_result_t;

typedef void (*term_f) (term_result_t *terms);

typedef struct term_async_s {
	uv_async_t async;
	term_f cb;
} term_async_t;

typedef struct term_work_s {
	uv_work_t	req;
	uv_async_t *async;
	const char *word;
	int did;
	int limit;
} term_work_t;


extern void
w_term_cb (uv_work_t *req);

extern void
w_term_after_cb (uv_work_t *req, int status);

extern void
free_term_results (term_result_t *t);

#endif /* TERM_H__ */
