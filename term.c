#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "vlog.h"
#include "check.h"
#include "term.h"
#include "curl_helpers.h"
#include "jsmn.h"


#define MAX_URL_SIZE 1024


typedef struct token_node_s {
	jsmntok_t *id;
	jsmntok_t *value;
	jsmntok_t *info;
} token_node_t;


#ifdef _DEBUG_JSMN
static int
dump (const char *js, jsmntok_t *t, size_t count, int indent);
#endif

static int
parse (const char *js, jsmntok_t *t, size_t count, token_node_t **out);

static int
get_token_nodes (const char *data, size_t size, token_node_t **out);

static void
dump_nodes (const char *js, token_node_t *nodes, int count);


/* ------------------------------------------------------------------ */


void
term_cb (uv_work_t *req)
{
	CURL *handle;
	term_t *w;
	char url[MAX_URL_SIZE];
	curl_mem_t storage;
	token_node_t *nodes;
	int nodes_num;


	w = (term_t *) req->data;

	snprintf (url, MAX_URL_SIZE, ACADEMIC_URL_TERM_FMT,
		w->limit, academic_did_name[w->did], w->query);

	storage.data = malloc (sizeof (char));

	vlog (VLOG_TRACE, "%s: %p: %s", __func__, req, url);

	NULL_CHECK(handle = curl_easy_init ());

	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_URL, url));

#ifdef _DEBUG_CURL
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_VERBOSE, 1L));
#else
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_VERBOSE, 0L));
#endif

	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_LOW_SPEED_TIME, 3L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_LOW_SPEED_LIMIT, 1L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_CONNECTTIMEOUT, 30L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_FOLLOWLOCATION, 1L));
	CURL_CHECK(curl_easy_setopt
		(handle, CURLOPT_WRITEFUNCTION, curl_mem_write_cb));
	CURL_CHECK(curl_easy_setopt
		(handle, CURLOPT_WRITEDATA, (void *) &storage));
	
	CURL_CHECK(curl_easy_perform (handle));

	nodes_num = get_token_nodes (storage.data, storage.size, &nodes);
	vlog (VLOG_DEBUG, "%s: %p: nodes = %d", __func__, req, nodes_num);
	dump_nodes (storage.data, nodes, nodes_num);

	if (nodes_num > 0)
		free (nodes);

	curl_easy_cleanup (handle);
	free (storage.data);
}


void
term_after_cb (uv_work_t *req, int status)
{
	term_t *w = (term_t *) req->data;


	vlog (VLOG_TRACE, "%s: %p: status %d", __func__, req, status);

	req->data = NULL;
	free (w);
	free (req);
}


static int
get_token_nodes (const char *data, size_t size, token_node_t **out)
{
	jsmn_parser p;
	jsmntok_t *tok;
	int r, count;


	jsmn_init (&p);
	r = jsmn_parse (&p, data, size, NULL, 0);

	if (r > 0) {
		count = r;
//		vlog (VLOG_TRACE, "%s: require tokens %d", __func__, count);
		NULL_CHECK(tok = (jsmntok_t *) malloc (sizeof (*tok) * count));

		/* reset counters */
		jsmn_init (&p);
		r = jsmn_parse (&p, data, size, tok, count);

		if (r >= 0) {
#ifdef _DEBUG_JSMN
			dump (data, tok, p.toknext, 0);
#endif
			r = parse (data, tok, p.toknext, out);
			
		}

		free (tok);
	}

	return r;
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


static int
parse (const char *js, jsmntok_t *tokens, size_t count, token_node_t **out)
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

		if (found == 1) {
//			vlog (VLOG_TRACE, "%s: allocate token nodes", __func__);
			*out = (token_node_t *) malloc (sizeof (token_node_t));
			NULL_CHECK(*out);
			current = *out;
		}
		else {
//			vlog (VLOG_TRACE, "%s: reallocate token nodes", __func__);
			*out = (token_node_t *) realloc 
			(*out, sizeof (token_node_t) * found);
			NULL_CHECK(*out);
			current = *out + found - 1;
		}

		current->id = id;
		current->value = value;
		current->info = info;
	} /* for (i = 3, ++t; i < count; i++, t++) { */

	if ((found == 0) && (out != NULL))
		*out = NULL;

	return found;
}


static void
dump_nodes (const char *js, token_node_t *nodes, int count)
{
	int i;
	token_node_t *n;


	for (i = 0, n = nodes; i < count; i++, n++)
		vlog (VLOG_DEBUG, "id: %.*s value: %.*s info: %.*s",
			n->id->end - n->id->start, js+n->id->start,
			n->value->end - n->value->start, js+n->value->start,
			n->info->end - n->info->start, js+n->info->start);
}
