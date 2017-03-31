#include <myhtml/api.h>
#include "vlog.h"


extern void
parse_html (const char *data, size_t size)
{
	myhtml_t *myhtml = myhtml_create ();
	myhtml_init (myhtml, MyHTML_OPTIONS_DEFAULT, 1, 0);

	myhtml_tree_t *tree = myhtml_tree_create ();
	myhtml_tree_init (tree, myhtml);

	myhtml_parse(tree, MyENCODING_UTF_8, data, size);

	myhtml_collection_t *collection =
		myhtml_get_nodes_by_tag_id (tree, NULL, MyHTML_TAG_TITLE, NULL);

	if (collection && collection->list && collection->length) {
		myhtml_tree_node_t *text_node =
			myhtml_node_child (collection->list[0]);

		if (text_node) {
			const char *text = myhtml_node_text (text_node, NULL);

			if (text) {
				vlog (VLOG_DEBUG, "%s: %s", __func__, text);
			}
		}
	}

	myhtml_collection_destroy (collection);
	myhtml_tree_destroy (tree);
	myhtml_destroy (myhtml);
}
