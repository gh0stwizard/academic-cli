#ifndef CLI_H__
#define CLI_H__

#include "html.h"

/* maximum size of the string returned by create_csi() */
#ifndef CSI_MAX_SIZE
#define CSI_MAX_SIZE 128
#endif

/* options passed to create_csi() */
typedef enum {
	SGR_RESET           = 0,
	SGR_NORMAL          = 0,
	SGR_BOLD            = 1,
/* Not widely supported. */
	SGR_FAINT           = 2,
/* Not widely supported. Sometimes treated as inverse. */
	SGR_ITALIC          = 3,
	SGR_UNDERLINE       = 4,
/* less than 150 per minute */
	SGR_BLINK_SLOW      = 5,
/* MS-DOS ANSI.SYS; 150+ per minute; not widely supported */
	SGR_BLINK_RAPID     = 6,
/* inverse or reverse; swap foreground and background (reverse video) */
	SGR_IMAGE_NEGATIVE  = 7,
/* Not widely supported. */
	SGR_CONCEAL         = 8,
/* Characters legible, but marked for deletion. Not widely supported. */
	SGR_CROSSED_OUT     = 9,
	SGR_DEFAULT_FONT    = 10,
	SGR_PRIMARY_FONT    = 10,
/* Select the n-th alternate font. */
	SGR_ALTER_FONT_1    = 11,
	SGR_ALTER_FONT_2    = 12,
	SGR_ALTER_FONT_3    = 13,
	SGR_ALTER_FONT_4    = 14,
	SGR_ALTER_FONT_5    = 15,
	SGR_ALTER_FONT_6    = 16,
	SGR_ALTER_FONT_7    = 17,
	SGR_ALTER_FONT_8    = 18,
	SGR_ALTER_FONT_9    = 19,
/* hardly ever supported */
	SGR_FRAKTUR         = 20,
/* Bold off not widely supported; double underline hardly ever supported. */
	SGR_BOLD_OFF        = 21,
/* Neither bold nor faint */
	SGR_NORMAL_COLOR    = 22,
/* Not italic, not Fraktur */
	SGR_ITALIC_OFF      = 23,
/* Not singly or doubly underlined */
	SGR_UNDERLINE_OFF   = 24,
	SGR_BLINK_OFF       = 25,
	SGR_RESERVED_26     = 26,
	SGR_IMAGE_POSITIVE  = 27,
/* conceal off */
	SGR_REVEAL          = 28,
	SGR_CROSSED_OUT_OFF = 29,
/* foreground colours */
	SGR_FG_BLACK        = 30,
	SGR_FG_RED          = 31,
	SGR_FG_GREEN        = 32,
	SGR_FG_YELLOW       = 33,
	SGR_FG_BLUE         = 34,
	SGR_FG_MAGENTA      = 35,
	SGR_FG_CYAN         = 36,
	SGR_FG_WHITE        = 37,
	SGR_FG_RESERVED     = 38,
	SGR_FG_DEFAULT      = 39,
/* background colours */
	SGR_BG_BLACK        = 40,
	SGR_BG_RED          = 41,
	SGR_BG_GREEN        = 42,
	SGR_BG_YELLOW       = 43,
	SGR_BG_BLUE         = 44,
	SGR_BG_MAGENTA      = 45,
	SGR_BG_CYAN         = 46,
	SGR_BG_WHITE        = 47,
	SGR_BG_RESERVED     = 48,
	SGR_BG_DEFAULT      = 49,
	SGR_MAX             = 50,
} SGR;


/* create a string with SGR controls */
extern size_t
create_csi (SGR options[], size_t size, char **out);

/* convert html data string into a string with SGR controls */
extern size_t
convert_html (html_data_t *html, char **out);

#endif /* CLI_H__ */
