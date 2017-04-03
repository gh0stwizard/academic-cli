#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "json.h"
#include "jsmn.h"
#include "vlog.h"
#include "check.h"


typedef struct token_node_s {
	jsmntok_t *id;
	jsmntok_t *value;
	jsmntok_t *info;
} token_node_t;


static int
get_nodes (const char *js, jsmntok_t *t, size_t count, token_node_t **out);

static term_result_t *
conv_nodes (const char *js, token_node_t *nodes, int count);

#ifdef _DEBUG_JSMN
static int
dump (const char *js, jsmntok_t *t, size_t count, int indent);
#endif


/* ------------------------------------------------------------------ */


extern int
parse_json (const char *data, size_t size, term_result_t **out)
{
	jsmn_parser p;
	jsmntok_t *tok;
	int r, count;
	token_node_t *nodes = NULL;


	jsmn_init (&p);
	r = jsmn_parse (&p, data, size, NULL, 0);
	*out = NULL;

	if (r > 0) {
		count = r;
		NULL_CHECK(tok = (jsmntok_t *) malloc (sizeof (*tok) * count));

		/* reset counters */
		jsmn_init (&p);
		r = jsmn_parse (&p, data, size, tok, count);

		if (r >= 0) {
			r = get_nodes (data, tok, p.toknext, &nodes);
			*out = conv_nodes (data, nodes, r);

			if (nodes != NULL)
				free (nodes);
		}

		free (tok);
	}

	return r;
}


static int
get_nodes (const char *js, jsmntok_t *tokens, size_t count, token_node_t **out)
{
	size_t i, j;
	jsmntok_t *t = tokens;
	int found = 0;
	token_node_t *current = NULL;


	if (t->type != JSMN_OBJECT) {
		vlog (VLOG_ERROR, "%s at %d: expected object", __func__, __LINE__);
		return -10;
	}

	if ((++t)->type != JSMN_STRING) {
		vlog (VLOG_ERROR, "%s at %d: expected string", __func__, __LINE__);
		return -11;
	}

	if (strncmp ("results", js+t->start, 7) != 0) {
		vlog (VLOG_ERROR, "%s at %d: expected 'results'", __func__, __LINE__);
		return -12;
	}

	if ((++t)->type != JSMN_ARRAY) {
		vlog (VLOG_ERROR, "%s at %d: expected array", __func__, __LINE__);
		return -13;
	}

	for (i = 3, t = tokens + 3; i < count; i++, t++) {
		if (t->type != JSMN_OBJECT)
			continue;

		jsmntok_t *id = NULL;
		jsmntok_t *value = NULL;
		jsmntok_t *info = NULL;

		for (j = 0, ++t; t->type == JSMN_STRING; j++, t++) {
			if (strncmp ("id", js+t->start, 2) == 0) {
				if ((++t)->type == JSMN_PRIMITIVE) {
					id = t;
				}
				j++;
			}
			else if (strncmp ("value", js+t->start, 2) == 0) {
				if ((++t)->type == JSMN_STRING) {
					value = t;
				}
				j++;
			}
			else if (strncmp ("info", js+t->start, 4) == 0) {
				if ((++t)->type == JSMN_STRING) {
					info = t;
				}
				j++;
			}
		}
		/* got back expecting next object */
		t--;
		i += j;
		

		if (! id || ! value || ! info) {
			vlog (VLOG_ERROR, "%s at %d: invalid object",
				__func__, __LINE__);
			continue;
		}

		found++;

		if (out == NULL)
			/* do count only */
			continue;

		/* TODO: better memory management */
		if (found == 1) {
			*out = (token_node_t *) malloc (sizeof (token_node_t));
			NULL_CHECK(*out);
		}
		else {
			*out = (token_node_t *) realloc 
				(*out, sizeof (token_node_t) * found);
			NULL_CHECK(*out);			
		}

		current = *out + found - 1;
		current->id = id;
		current->value = value;
		current->info = info;
	} /* for (i = 3, ++t; i < count; i++, t++) { */

	if ((found == 0) && (out != NULL))
		*out = NULL;

	return found;
}


static term_result_t *
conv_nodes (const char *js, token_node_t *nodes, int count)
{
	term_result_t *result;
	term_data_t *list;
	term_data_t *listp;
	token_node_t *n, *nend;
	int len;
	char *str;


	result = (term_result_t *) malloc (sizeof (*result));
	NULL_CHECK(result);
	result->entries = count;

	if (count <= 0) {
		result->list = NULL;
		return result;
	}

	NULL_CHECK(list = (term_data_t *) calloc (count, sizeof (*list)));
	listp = list;
	result->list = list;

	for (n = nodes, nend = nodes + count; n != nend; n++, listp++) {
		len = n->id->end - n->id->start;
		NULL_CHECK(str = malloc (sizeof (*str) * (len + 1)));
		memcpy (str, js+n->id->start, len);
		str[len] = '\0';
		listp->id = str;

		len = n->value->end - n->value->start;
		NULL_CHECK(str = malloc (sizeof (*str) * (len + 1)));
		memcpy (str, js+n->value->start, len);
		str[len] = '\0';
		listp->value = str;

		len = n->info->end - n->info->start;
		NULL_CHECK(str = malloc (sizeof (*str) * (len + 1)));
		memcpy (str, js+n->info->start, len);
		str[len] = '\0';
		listp->info = str;
	}

	return result;
}


/**
 * taken from: jsmn/example/jsondump.c
 */
#ifdef _DEBUG_JSMN
static int
dump (const char *js, jsmntok_t *t, size_t count, int indent)
{
	int i, j, k;


	if (count == 0) {
		return 0;
	}

	if (t->type == JSMN_PRIMITIVE) {
		printf("%.*s", t->end - t->start, js+t->start);
		return 1;
	} else if (t->type == JSMN_STRING) {
		printf("'%.*s'", t->end - t->start, js+t->start);
		return 1;
	} else if (t->type == JSMN_OBJECT) {
		printf("\n");
		j = 0;

		for (i = 0; i < t->size; i++) {
			for (k = 0; k < indent; k++) printf("  ");
			j += dump(js, t+1+j, count-j, indent+1);
			printf(": ");
			j += dump(js, t+1+j, count-j, indent+1);
			printf("\n");
		}

		return j+1;
	} else if (t->type == JSMN_ARRAY) {
		j = 0;
		printf("\n");

		for (i = 0; i < t->size; i++) {
			for (k = 0; k < indent-1; k++) printf("  ");
			printf("   - ");
			j += dump(js, t+1+j, count-j, indent+1);
			printf("\n");
		}

		return j+1;
	}

	return 0;
}
#endif
