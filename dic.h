#ifndef DIC_H__
#define DIC_H__

#include <uv.h>
#include "academic.h"
#include "html.h"


typedef struct dic_s {
	uv_async_t *async;
	uv_rwlock_t *lock;
	int word_id;
	int did;
} dic_t;

typedef struct dic_result_s {
	int word_id;
	int did;
	char *term;
	html_data_t *data;
} dic_result_t;


void
dic_cb (uv_work_t *req);

void
dic_after_cb (uv_work_t *req, int status);


#endif /* DIC_H__ */
