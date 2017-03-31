#ifndef HTML_H__
#define HTML_H__

#include <myhtml/api.h>


typedef struct html_tag_s {
	int tag_id;
	char *href;
} html_tag_t;

typedef struct html_el_s {
	html_tag_t *tags;
	size_t tag_count;
	size_t start;
	size_t end;
} html_el_t;

typedef struct html_data_s {
	char *text;
	size_t size;
	size_t length;
	html_el_t *els;
	size_t el_count;
} html_data_t;


extern myhtml_t *myhtml;
extern myhtml_tree_t *myhtml_tree;


extern void
init_myhtml (void);

extern void
fini_myhtml (void);

extern char *
parse_html (const char *data, size_t size, html_data_t **out);

extern void
free_html_data (html_data_t *d);

#endif /* HTML_H__ */
