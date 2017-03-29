#ifndef TERM_H__
#define TERM_H__

#include <uv.h>
#include "academic.h"


typedef struct term_s {
	unsigned int limit;
	academic_did did;
	const char *query;
} term_t;


void
term_cb (uv_work_t *req);

void
term_after_cb (uv_work_t *req, int status);


#endif /* TERM_H__ */
