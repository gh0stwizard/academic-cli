#ifndef TERM_H__
#define TERM_H__

#include <uv.h>
#include "academic.h"


typedef struct term_s {
	int limit;
	int did;
	const char *word;
	uv_async_t *async;
	uv_rwlock_t *lock;
} term_t;

typedef struct term_data_s {
	char *id;
	char *value;
	char *info;
} term_data_t;

typedef struct term_result_s {
	term_data_t *list;
	int entries;
	const char *word;
	int did;
} term_result_t;


void
term_cb (uv_work_t *req);

void
term_after_cb (uv_work_t *req, int status);


#endif /* TERM_H__ */
