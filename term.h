#ifndef TERM_H__
#define TERM_H__

#include <uv.h>
#include "academic.h"


typedef struct term_s {
	unsigned int limit;
	academic_did did;
	const char *query;
	uv_async_t *async;
	uv_rwlock_t *lock;
} term_t;

typedef struct term_data_s {
	char *id;
	char *value;
	char *info;
} term_data_t;

typedef struct term_result_s {
	uv_async_t *async; /* XXX: baton??? */
	term_data_t *list;
	int entries;
} term_result_t;


void
term_cb (uv_work_t *req);

void
term_after_cb (uv_work_t *req, int status);


#endif /* TERM_H__ */
