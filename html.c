#include <stdlib.h>
#include <string.h>

#include "html.h"
#include "vlog.h"
#include "check.h"


/* ------------------------------------------------------------------ */

myhtml_t *myhtml = NULL;
myhtml_tree_t *myhtml_tree = NULL;

/* ------------------------------------------------------------------ */


static html_data_t *
get_dd_text (myhtml_tree_node_t *node);

static char *
get_href (myhtml_tree_node_t *node);

#ifdef _DEBUG_HTML_TREE
static void
dump_dd (myhtml_tree_t *tree, myhtml_tree_node_t *node);

static void
say_tag_id (myhtml_tree_node_t *node, const char *prepend);
#endif


/* ------------------------------------------------------------------ */


extern void
init_myhtml (void)
{
	NULL_CHECK(myhtml = myhtml_create ());
	MYHTML_CHECK(myhtml_init (myhtml, MyHTML_OPTIONS_DEFAULT, 1, 0));
}


extern void
fini_myhtml (void)
{
	assert (myhtml_destroy (myhtml) == NULL);
}


extern char *
parse_html (const char *data, size_t size, html_data_t **out)
{
	myhtml_tree_t *tree;
	myhtml_collection_t *div_coll, *dt_coll = NULL, *dd_coll = NULL;
	mystatus_t status;
	html_data_t *result = NULL;
	char *term = NULL; /* return back to compare */


	/* initialization */
	NULL_CHECK(tree = myhtml_tree_create ());
	MYHTML_CHECK(myhtml_tree_init (tree, myhtml));

	/* parse document */
	MYHTML_CHECK(myhtml_parse (tree, MyENCODING_DEFAULT, data, size));

	/**
	 * Expecting the next data structure:
	 * <div id="article">
	 *   <dl>
	 *     <div itemscope itemtype="http://webmaster.yandex.ru/vocabularies/term-def.xml">
	 *       <dt [...]> WORD </dt>
	 *       <dd [...]> TEXT </dd>
	 *       <!-- META (not needed) //-->
	 *     </div> <!-- <div itemscope [...]/>  //-->
	 *   </dl>
	 * </div> <!-- <div id="article"> //-->
	 */

	/* FIXME: many *_get_nodes_* functions does not set properly status */
	div_coll = myhtml_get_nodes_by_attribute_value (tree, NULL, NULL,
			true /* case-insensitive */,
			"id", 2,
			"article", 7,
			&status);

	if (div_coll && div_coll->list && div_coll->length) {
		vlog (VLOG_TRACE, "found divs: size = %zu length %zu",
			div_coll->size, div_coll->length);

		myhtml_tree_node_t *dl = myhtml_node_child (div_coll->list[0]);

		if (dl == NULL) {
			vlog (VLOG_ERROR, "empty div");
			goto done;
		}

		/* term definition */
		dt_coll = myhtml_get_nodes_by_tag_id_in_scope
			(tree, NULL, dl, MyHTML_TAG_DT, NULL);

		if (dt_coll && dt_coll->list && dt_coll->length) {
			myhtml_tree_node_t *dt = myhtml_node_child (dt_coll->list[0]);

			if (dt == NULL) {
				vlog (VLOG_ERROR, "empty dt");
				goto done;
			}

			size_t len = 0;
			const char *text = myhtml_node_text (dt, &len);
			NULL_CHECK(term = (char *) malloc (len + 1));
			memcpy (term, text, len);
			term[len] = '\0';
		}
		else {
			vlog (VLOG_ERROR, "missing dt");
			goto done;
		}

		dd_coll = myhtml_get_nodes_by_tag_id_in_scope
			(tree, NULL, dl, MyHTML_TAG_DD, NULL);

		if (dd_coll && dd_coll->list && dd_coll->length) {
			myhtml_tree_node_t *ddch = myhtml_node_child (dd_coll->list[0]);

			if (ddch == NULL) {
				vlog (VLOG_ERROR, "empty dd");
				goto done;
			}

#ifndef _DEBUG_HTML_TREE
			result = get_dd_text (ddch);

			/* done */
			if (out != NULL)
				*out = result;
#else
			dump_dd (tree, ddch);
#endif
		}
		else
			vlog (VLOG_ERROR, "missing dd");
	}
	else
		vlog (VLOG_ERROR, "invalid html");


done:

	if (data == NULL && term != NULL) {
		free (term);
		term = NULL; /* mark as error */
	}

	if (div_coll)
		myhtml_collection_destroy (div_coll);

	if (dt_coll)
		myhtml_collection_destroy (dt_coll);

	if (dd_coll)
		myhtml_collection_destroy (dd_coll);

	myhtml_tree_destroy (tree);

	return term;
}


static html_data_t *
get_dd_text (myhtml_tree_node_t *node)
{
	char *newtext;
	int tag_num;
	html_tag_t *tags;
	html_data_t *list;
	html_el_t *newels = NULL;
	size_t len;
	myhtml_tree_node_t *prev, *begin;
	myhtml_tree_node_t *div = NULL, *div_parent = NULL;
	int skip = 0;

	NULL_CHECK(list = malloc (sizeof (*list)));
	NULL_CHECK(list->text = malloc (sizeof (char)));

	list->els = NULL;
	list->el_count = 0;
	list->size = 1;
	list->length = 0;

#define get_tagid(n) myhtml_token_node_tag_id (myhtml_node_token ((n)))

	while (node) {
		skip = 1;
		switch (get_tagid (node)) {
			case MyHTML_TAG__COMMENT:
			case MyHTML_TAG_IFRAME:
			case MyHTML_TAG_SCRIPT:
				node = myhtml_node_next (node);
				break;
			case MyHTML_TAG_DIV:
				vlog (VLOG_TRACE, "div: entry point");

				if (div) /* TODO */ {
					vlog (VLOG_TRACE, "div: found inferor");
					node = myhtml_node_next (node);
				}
				else {
					div = node;
					node = myhtml_node_child (node);
				}
				break;
			default:
				skip = 0;
				break;
		}

		if (skip)
			continue;

		begin = node;
		prev = node;
		tag_num = 0;

		while (get_tagid (node) != MyHTML_TAG__TEXT) {
			vlog (VLOG_TRACE, "tag %d id %d", tag_num, get_tagid (node));
			tag_num++;

			prev = node;
			node = myhtml_node_child (node);

			if (node == NULL) {
				prev = myhtml_node_parent (prev);
				node = prev;
			}			
		}

		if (get_tagid (node) == MyHTML_TAG__TEXT) {
			const char *ntxt = myhtml_node_text (node, 0);
			vlog (VLOG_TRACE, "'%s' tags = %d", ntxt, tag_num);

			len = strlen (ntxt);
			list->size += len;
			NULL_CHECK(newtext = realloc (list->text, list->size));
			list->text = newtext;
			memcpy (newtext + list->length, ntxt, len);
			

			if (tag_num > 0) {
				if (list->els == NULL) {
					list->els = (html_el_t *) malloc (sizeof (html_el_t));
					NULL_CHECK(list->els);
				}
				else {
					newels = realloc (list->els,
								sizeof (html_el_t) * (list->el_count + 1));
					NULL_CHECK(newels);
					list->els = newels;
				}

				NULL_CHECK(tags = malloc (sizeof (*tags) * tag_num));
				list->els[list->el_count].tag_count = tag_num;

				while (--tag_num >= 0) {
					node = myhtml_node_parent (node);
					tags[tag_num].tag_id = get_tagid (node);

					if (tags[tag_num].tag_id == MyHTML_TAG_A) {
						tags[tag_num].href = get_href (node);
						vlog (VLOG_TRACE, "href: %s", tags[tag_num].href);
					}
					else {
						tags[tag_num].href = NULL;
					}
				}

				list->els[list->el_count].tags = tags;
				list->els[list->el_count].start = list->length;
				list->els[list->el_count].end = list->length + len;
				list->el_count++;

				/* get back to current node */
				node = myhtml_node_child (prev);
			}

			list->length += len;
		}

		if (div) {
			if (myhtml_node_child (node)) {
				vlog (VLOG_TRACE, "div: next child");
				node = myhtml_node_child (node);
			}
			else if (myhtml_node_next (node)) {
				vlog (VLOG_TRACE, "div: next sibling");
				node = myhtml_node_next (node);
			}
			else if (myhtml_node_next (begin)) {
				vlog (VLOG_TRACE, "div: next sibling from begin");
				node = myhtml_node_next (begin);
			}
			else if (div_parent) {
				/* TODO */
				abort ();
			}
			else {
				vlog (VLOG_TRACE, "div: exit");
				node = myhtml_node_next (div);
				div = NULL; /* TODO */
			}

			continue;
		}

		prev = node;
		node = myhtml_node_child (node);

		if (node) {
			vlog (VLOG_TRACE, "next child");
			continue;
		}

		node = myhtml_node_next (prev);

		if (node) {
			vlog (VLOG_TRACE, "next sibling");
		}
		else {
			vlog (VLOG_TRACE, "next sibling from begin");
			node = myhtml_node_next (begin);
		}
	}

#undef get_tagid

	list->text[list->length] = '\0';
	vlog (VLOG_TRACE, "parsing completed");

	return list;
}


extern void
free_html_data (html_data_t *d)
{
	size_t i, j, elnum, tagnum;
	html_el_t *el;


	if (d == NULL)
		return;

	if (d->text)
		free (d->text);

	if (d->els) {
		el = d->els;

		for (i = 0, elnum = d->el_count; i < elnum; i++, el++) {
			for (j = 0, tagnum = el->tag_count; j < tagnum; j++) {
				if (el->tags[j].href)
					free (el->tags[j].href);
			}
		}

		free (d->els);
	}
}


static char *
get_href (myhtml_tree_node_t *node)
{
	char *result = NULL;
	size_t len = 0;
	/*XXX: do I need to release it??? */
	myhtml_tree_attr_t *attr = myhtml_attribute_by_key (node, "href", 4);


	if (attr) {
		const char *href = myhtml_attribute_value (attr, &len);
		NULL_CHECK(result = malloc (len + 1));
		memcpy (result, href, len);
		result[len] = '\0';
	}


	return result;
}


#ifdef _DEBUG_HTML_TREE
static void
dump_dd (myhtml_tree_t *tree, myhtml_tree_node_t *node)
{
	myhtml_tree_node_t *child;


	if (node == NULL)
		return;

	do {
		say_tag_id (node, "next: ");

		child = myhtml_node_child (node);

		do {
			if (child == NULL)
				break;

			say_tag_id (child, "child: ");
			dump_dd (tree, myhtml_node_next (child));

		} while ((child = myhtml_node_child (child)));

	} while ((node = myhtml_node_next (node)));
}


static void
say_tag_id (myhtml_tree_node_t *node, const char *prepend)
{
	if (node == NULL)
		return;

	if (prepend == NULL)
		prepend = "";

	myhtml_token_node_t *token = myhtml_node_token (node);
	myhtml_tag_id_t tagid = myhtml_token_node_tag_id (token);

	if (tagid == MyHTML_TAG__TEXT) {
		vlog (VLOG_TRACE, "%stag id: %d text: %s",
			prepend,
			tagid, myhtml_token_node_text (token, 0));
	}
	else {
		vlog (VLOG_TRACE, "%stag id: %d", prepend, tagid);
	}
}
#endif
