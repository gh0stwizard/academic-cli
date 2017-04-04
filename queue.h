#ifndef QUEUE_H__
#define QUEUE_H__

#include "academic.h"
#include "term.h"
#include "word.h"


extern void
queue_term (const char *word, int did[], int didnum, int limit, term_f cb);

extern void
queue_word_id (int wid, int did, word_f cb);

#endif /* QUEUE_H__ */
