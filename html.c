/*
    academic-cli -- a command line utility for ACADEMIC website.
    Copyright (C) 2017  Vitaliy V. Tokarev

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
			myhtml_tree_node_t *dt = myhtml_node_child
				(dt_coll->list[dt_coll->length - 1]);

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
			myhtml_tree_node_t *ddch = myhtml_node_child
				(dd_coll->list[dd_coll->length - 1]);

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
	html_data_t *res;
	html_el_t *els = NULL;
	html_tag_t *tags;
	size_t elsnum = 0;
	myhtml_tree_node_t *child;
	myhtml_tree_node_t *next = NULL;
	int tag_count = 0;
	int level = 0;
	char *text;


	NULL_CHECK(res = malloc (sizeof (*res)));
	res->els = els;
	res->el_count = elsnum;
	res->size = sizeof (char);
	res->length = 0;
	NULL_CHECK(res->text = malloc (res->size));

#define get_tagid(n) myhtml_token_node_tag_id (myhtml_node_token ((n)))

	while (node) {
		child = node;
		next = myhtml_node_next (node);

		/* SKIP text from this tags */
		switch (get_tagid (node)) {
			case MyHTML_TAG_IFRAME:
			case MyHTML_TAG_SCRIPT:
				node = next;
				continue;
		}

		/* go deep until end */
		while ((child = myhtml_node_child (child))) {
			tag_count++;
			node = child;
			/* look for closest sibling */
			if (myhtml_node_next (child)) {
				next = myhtml_node_next (child);
				level = tag_count;
			}
		}

		vlog (VLOG_TRACE, "tag count %d level %d", tag_count, level);

		if (get_tagid (node) == MyHTML_TAG__TEXT) {
			const char *t = myhtml_node_text (node, 0);
			vlog (VLOG_TRACE, "'%s' tags: %d", t, tag_count);

			size_t len = strlen (t);
			res->size += len;
			NULL_CHECK(text = realloc (res->text, res->size));
			memcpy (text + res->length, t, len);

			if (tag_count > 0) {
				if (els == NULL)
					els = malloc (sizeof (*els));
				else
					els = realloc (res->els, sizeof (*els) * (elsnum + 1));
				NULL_CHECK (els);

				NULL_CHECK(tags = malloc (sizeof (*tags) * tag_count));
				els[elsnum].tag_count = tag_count;

				myhtml_tree_node_t *p = node;
				for (int i = tag_count - 1; i >= 0; i--) {
					p = myhtml_node_parent (p);
					tags[i].tag_id = get_tagid (p);
					vlog (VLOG_TRACE, "  tag id: %#x", tags[i].tag_id);

					if (tags[i].tag_id == MyHTML_TAG_A) {
						tags[i].href = get_href (p);
						vlog (VLOG_TRACE, "    href: %s", tags[i].href);
					}
					else {
						tags[i].href = NULL;
					}
				}

				els[elsnum].tags = tags;
				els[elsnum].start = res->length;
				els[elsnum].end = res->length + len;
				res->el_count = ++elsnum;
				res->els = els;
			}

			res->text = text;
			res->length += len;
		}
		else if (get_tagid (node) == MyHTML_TAG_BR) {
			vlog (VLOG_TRACE, "line break");
			res->size += 1;
			NULL_CHECK(text = realloc (res->text, res->size));
			text[res->length] = '\n';
			res->text = text;
			res->length += 1;
		}

		if (next) {
			tag_count = level;
			node = next;
		}
		else {
			while (tag_count-- > 0)
				node = myhtml_node_parent (node);
			node = myhtml_node_next (node);
			level = 0;
		}
	}

	vlog (VLOG_TRACE, "level %d count %d", level, tag_count);

#undef get_tagid

	res->text[res->length] = '\0';
	vlog (VLOG_TRACE, "parsing completed");

	return res;
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
