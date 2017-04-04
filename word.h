#ifndef DIC_H__
#define DIC_H__

#include <uv.h>
#include "academic.h"
#include "html.h"


typedef struct dic_result_s {
	const char *word;
	char *term;
	html_data_t *data;
	int wid;
	int did;
} word_result_t;

typedef void (*word_f) (word_result_t *data);

typedef struct word_async_s {
	uv_async_t async;
	word_f cb;
} word_async_t;

typedef struct word_work_s {
	uv_work_t	req;
	uv_async_t *async;
	const char *word;
	int wid;
	int did;
} word_work_t;


void
w_word_cb (uv_work_t *req);

void
w_word_after_cb (uv_work_t *req, int status);

extern void
free_word_results (word_result_t *r);

#endif /* DIC_H__ */
