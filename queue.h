#ifndef QUEUE_H__
#define QUEUE_H__

#include "academic.h"


typedef void (*term_cb_t) (uv_async_t *);


extern void
queue_word (const char *word, int did[], int did_num);

extern void
query_word_by_id (int word_id, int did[], int did_num);

extern void
queue_term (const char *word, int did, int limit, term_cb_t cb);


#endif /* QUEUE_H__ */
