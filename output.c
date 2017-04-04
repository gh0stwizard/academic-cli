#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "cli.h"
#include "check.h"


#define ESC             "\033["
#define SEP             ";"
#define SGR_MOD         "m"


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
	return 0;
}
