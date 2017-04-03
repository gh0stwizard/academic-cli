#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "vlog.h"


#define HEADER_SIZE 256

extern size_t
vdebug (const char *file,
		const char *func,
		unsigned int line,
		vlog_level level,
		const char *fmt, ...)
{
	va_list ap;
	char header[HEADER_SIZE];
	char *lvlstr = NULL;
	size_t len, hlen;


	switch (level) {
	case VLOG_FATAL:	lvlstr = VLOG_STR_FATAL	" "; break;
	case VLOG_ALERT:	lvlstr = VLOG_STR_ALERT	" "; break;
	case VLOG_CRITICAL:	lvlstr = VLOG_STR_CRIT	" "; break;
	case VLOG_ERROR:	lvlstr = VLOG_STR_ERR	" "; break;
	case VLOG_WARNING:	lvlstr = VLOG_STR_WARN	" "; break;
	case VLOG_NOTICE:	lvlstr = VLOG_STR_NOTE	" "; break;
	case VLOG_INFO:		lvlstr = VLOG_STR_INFO	" "; break;
	case VLOG_DEBUG:	lvlstr = VLOG_STR_DEBUG	" "; break;
	case VLOG_TRACE:	lvlstr = VLOG_STR_TRACE	" "; break;
	default:			lvlstr = VLOG_STR_NONE	" "; break;
	}

	len = strlen (lvlstr);
	hlen = uvls_date (UVLS_DATE_FMT, header, HEADER_SIZE);
	header[hlen] = ' ';
	memcpy (header + hlen + 1, lvlstr, len);
	hlen += len + 1;

	if (file) {
		sprintf (header + hlen, "%s: ", file);
		hlen = strlen (header);
	}

	if (func) {
		sprintf (header + hlen, "%s:%d ", func, line);
		hlen = strlen (header);
	}

	header[hlen] = '\0';

	len = uvls_logf (header);

	va_start (ap, fmt);
	len += uvls_vlogf (fmt, ap);
	va_end (ap);

	len += uvls_logf ("\n");

	return len;
}
