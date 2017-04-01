#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "output.h"

#ifndef _WIN32
#define EOL "\n"
#else
#define EOL "\r\n"
#endif

#define ESC		"\033["
#define SEP		";"
#define SGR_MOD		"m"

#define RESET		"0"
#define BOLD		"1"
#define FAINT		"2"
#define ITALIC		"3"
#define UNDERLINE	"4"
#define BLINK_SLOW	"5"
#define BLINK_RAPID	"6"

#define DEFAULT_FONT	"10"

#define BLINK_OFF	"25"

#define FG_BLACK	"30"
#define FG_RED		"31"
#define FG_GREEN	"32"
#define FG_YELLOW	"33"
#define FG_BLUE		"34"
#define FG_MAGENTA	"35"
#define FG_CYAN		"36"
#define FG_WHITE	"37"
#define FG_DEFAULT	"39"

#define BG_BLACK	"40"
#define BG_RED		"41"
#define BG_GREEN	"42"
#define BG_YELLOW	"43"
#define BG_BLUE		"44"
#define BG_MAGENTA	"45"
#define BG_CYAN		"46"
#define BG_WHITE	"47"
#define BG_DEFAULT	"49"

#define STRING_SIZE 1024
#define CSI_SIZE 128


extern void
say (const char *fmt, ...)
{
	va_list args;
	char str[STRING_SIZE];
	size_t offset = 0;
	char *end = EOL;
	uv_write_t req;
	uv_buf_t buf;


	if (colorized) {
		SGR opt[] = {
			SGR_COLOR_YELLOW,
			SGR_UNDERLINE
		};
		char *csi = create_csi (opt, sizeof (opt) / sizeof (opt[0]));
		offset = strlen (csi);
		snprintf (str, STRING_SIZE, csi);
		end = EOL ESC RESET SGR_MOD;
		free (csi);
	}

	va_start (args, fmt);
	vsnprintf (str + offset, STRING_SIZE - offset, fmt, args);
	offset = strlen (str) - strlen (end);
	snprintf (str + offset, STRING_SIZE - offset, end);
	va_end (args);
	
	buf = uv_buf_init (str, strlen (str));
	uv_write (&req, (uv_stream_t *) &tty_out, &buf, 1, NULL);
}


static const char const *csi[SGR_MAX] = {
	RESET,
	BOLD,
	FAINT,
	ITALIC,
	UNDERLINE,
	BLINK_SLOW,
	BLINK_RAPID,

	DEFAULT_FONT,

	BLINK_OFF,

	FG_BLACK,
	FG_RED,
	FG_GREEN,
	FG_YELLOW,
	FG_BLUE,
	FG_MAGENTA,
	FG_CYAN,
	FG_WHITE,
	FG_DEFAULT,

	BG_BLACK,
	BG_RED,
	BG_GREEN,
	BG_YELLOW,
	BG_BLUE,
	BG_MAGENTA,
	BG_CYAN,
	BG_WHITE,
	BG_DEFAULT
};


extern char *
create_csi (SGR options[], int count)
{
	static char result[CSI_SIZE] = ESC; /* length 2 */
	int len = strlen (ESC);
	char *dst = result;
	char *csi_str;
	size_t csi_len;
	int *o = (int *) options;
	int i;


	if (options == NULL)
		count = 0;

	for (i = 0; i < count; i++, o++) {
		if ((*o >= 0) && (*o < SGR_MAX)) {
			csi_str = (char *) csi[*o];
			csi_len = strlen (csi_str);
			snprintf (dst + len, CSI_SIZE - len, "%s;", csi_str);
			len += csi_len + 1;
		}
	}

	if (len >= 3) /* ESC + 'n;' */
		snprintf (dst + len - 1, CSI_SIZE, SGR_MOD);

	return strndup (result, CSI_SIZE);
}
