#include <stdio.h>
#include <stdarg.h>
#include "vlog.h"
#include "timeinfo.h"


extern int
vdebug (const char *prepend, vlog_level level, const char *fmt, ...)
{
	va_list args;
	char date[TIMEINFO_DATE_SIZE];
	char *datep = date;
	char *lvlstr = NULL;


	switch (level) {
	case VLOG_FATAL:	lvlstr = VLOG_STR_FATAL; break;
	case VLOG_ALERT:	lvlstr = VLOG_STR_ALERT; break;
	case VLOG_CRITICAL:	lvlstr = VLOG_STR_CRIT; break;
	case VLOG_ERROR:	lvlstr = VLOG_STR_ERR; break;
	case VLOG_WARNING:	lvlstr = VLOG_STR_WARN; break;
	case VLOG_NOTICE:	lvlstr = VLOG_STR_NOTE; break;
	case VLOG_INFO:		lvlstr = VLOG_STR_INFO; break;
	case VLOG_DEBUG:	lvlstr = VLOG_STR_DEBUG; break;
	case VLOG_TRACE:	lvlstr = VLOG_STR_TRACE; break;
	default:		lvlstr = VLOG_STR_NONE; break;
	}

	va_start (args, fmt);

	if (get_current_time_string (&datep, sizeof (date))) {
		if (prepend)
			fprintf (stderr, "%s %s %s: ", date, lvlstr, prepend);
		else
			fprintf (stderr, "%s %s ", date, lvlstr);
	}
	else
		fprintf (stderr, lvlstr);

	vfprintf (stderr, fmt, args);
	fprintf (stderr, "\n");
	va_end (args);

	/* TODO: return value must be useful */
	return 0;
}
