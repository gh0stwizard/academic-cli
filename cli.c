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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "check.h"
#include "vlog.h"


#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define ESC             "\033["
#define SEP             ";"
#define SGR_MOD         "m"


static size_t
make_mycsi (char ***out, size_t **out_sizes);

static void
free_mycsi (char **csis, size_t *sizes, size_t count);


extern size_t
create_csi (SGR options[], size_t size, char **out)
{
	static char dst[CSI_MAX_SIZE] = ESC;
	size_t esc_len = strlen (ESC);
	size_t len = esc_len;
	char csi[4];
	int *o = (int *) options;


	for (int *end = o + size / sizeof (*o); o != end; o++) {
		if ((*o >= 0) && (*o < SGR_MAX)) {
			snprintf (csi, 4, "%d", *o);
			size_t csi_len = strlen (csi);
			snprintf (dst + len, CSI_MAX_SIZE - len, "%s;", csi);
			len += csi_len + 1;
		}
	}

	if (len >= esc_len + 1) /* ESC + 'n;' */
		snprintf (dst + len - 1, CSI_MAX_SIZE, SGR_MOD);

	*out = strndup (dst, CSI_MAX_SIZE);

	return len;
}


extern size_t
convert_html (html_data_t *html, char **out)
{
	char *dst;
	size_t len = 0, off = 0;
	char *txtp = html->text;
	size_t elements = html->el_count;
	html_el_t *el = html->els;
	html_el_t *elend = el + elements;
	extern size_t make_mycsi (char ***, size_t **);
	extern void free_mycsi (char **, size_t *, size_t);
	char **csis = NULL;
	size_t *csi_sizes;
	size_t csi_count = 0;
	char *reset;
	size_t reset_size;


	if (elements == 0) {
		dst = strndup (txtp, html->size);
		len = html->length;
		goto done;
	}

	csi_count = make_mycsi (&csis, &csi_sizes);
	reset = csis[0];
	reset_size = csi_sizes[0];

	/* XXX: allocate twice as much space */
	NULL_CHECK(dst = malloc (html->size * 2));

	for (; el != elend; el++) {
		html_tag_t *tag = el->tags;
		html_tag_t *tagend = tag + el->tag_count;
		size_t start = el->start;
		size_t end = el->end;
		size_t delta = end - start;

		if (len - off < start) {
			/* copy missing fragment */
			vlog (VLOG_TRACE, "copy missing fragment: len %zu off %zu start %zu",
				len, off, start);
			memcpy (dst + len, txtp + (len - off), start - (len - off));
			len += start - (len - off);
		}

		/* copy this fragment as is */
		vlog (VLOG_TRACE, "copy fragment: len %zu off %zu start %zu end %zu delta %zu",
			len, off, start, end, delta);
		memcpy (dst + len, txtp + start, delta);
		len += delta;

		vlog (VLOG_TRACE, "'%.*s' start %zu end %zu tags %zu",
			delta, txtp + start, start, end, el->tag_count);


		size_t total_csz = 0;
		for (; tag != tagend; tag++) {
			size_t csz = 0;
			char *csi;

			switch (tag->tag_id) {
				case MyHTML_TAG_A:
					vlog (VLOG_TRACE, "%.*s <a>", delta, txtp + start);
					csz = csi_sizes[3];
					csi = csis[3];
					break;
				case MyHTML_TAG_EM:
					vlog (VLOG_TRACE, "%.*s <em>", delta, txtp + start);
					csz = csi_sizes[2];
					csi = csis[2];
					break;
				case MyHTML_TAG_STRONG: {
					vlog (VLOG_TRACE, "%.*s <strong>", delta, txtp + start);
					csz = csi_sizes[1];
					csi = csis[1];
				}	break;
				default:
					vlog (VLOG_TRACE, "%.*s tag id %#x",
						delta, txtp + start, tag->tag_id);
					break;
			}

			if (csz > 0) {
				char *src = dst + len - delta;
				memmove (src + csz, src, delta);
				memcpy (src, csi, csz);
				off += csz;
				len += csz;
				total_csz += csz;
			}
		}

		/* append SGR RESET once */
		if (total_csz > 0) {
			memcpy (dst + len, reset, reset_size);
			off += reset_size;
			len += reset_size;
		}
	}

	if (len - off < html->length) {
		/* copy ending */
		vlog (VLOG_TRACE, "copy ending: len %zu off %zu document len %zu",
			len, off, html->length);
		memcpy (dst + len, txtp + (len - off), html->length - (len - off));
		len += html->length - (len - off);
	}

done:
	if (csi_count > 0)
		free_mycsi (csis, csi_sizes, csi_count);

	dst[len] = '\0';
	*out = dst;
	return len;
}


static size_t
make_mycsi (char ***out, size_t **out_sizes)
{
	char **csis = NULL;
	size_t *sizes;
	SGR reset[] = { SGR_RESET };
	SGR bold[] = { SGR_BOLD };
	SGR italic[] = { SGR_UNDERLINE };
	SGR blue[] = { SGR_FG_BLUE, SGR_BOLD };
	SGR *sgrs[] = {
		reset,
		bold,
		italic,
		blue
	};
	size_t sgr_sizes[] = {
		sizeof (reset),
		sizeof (bold),
		sizeof (italic),
		sizeof (blue)
	};

	NULL_CHECK(csis = malloc (ARRAY_SIZE(sgrs) * sizeof (*csis)));
	NULL_CHECK(sizes = malloc (ARRAY_SIZE(sgrs) * sizeof (*sizes)));

	for (size_t i = 0; i < ARRAY_SIZE(sgrs); i++)
		sizes[i] = create_csi (sgrs[i], sgr_sizes[i], &(csis[i]));

	*out = csis;
	*out_sizes = sizes;

	return ARRAY_SIZE(sgrs);
}


static void
free_mycsi (char **csis, size_t *sizes, size_t count)
{
	for (size_t i; i < count; i++)
		free (csis[i]);

	free (csis);
	free (sizes);
}
