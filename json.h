#ifndef JSON_H__
#define JSON_H__

#include "term.h"

extern int
parse_json (const char *data, size_t size, term_result_t **out);

#endif /* JSON_H__ */
