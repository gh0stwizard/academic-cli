#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "timeinfo.h"


extern int
get_current_time_string (char *out[], size_t out_len)
{
	struct timespec tp;
	struct tm *tmp;
	size_t len;
	char nsec[TIMEINFO_NSEC_SIZE + 1];
	long x;


	if (clock_gettime (CLOCK_REALTIME, &tp) == -1) {
		fprintf (stderr, "%s: clock_gettime: %s\n", __func__, strerror (errno));
		return 0;
	}

	tmp = localtime (&tp.tv_sec);

	if (tmp == NULL) {
		fprintf (stderr, "%s: localtime: %s\n", __func__, strerror (errno));
		return 0;
	}

	/* fill in a date to 'out' first */
	if (strftime (*out, out_len, TIMEINFO_FMT, tmp) == 0) {
		fprintf (stderr, "%s: strftime: %s\n", __func__, strerror (errno));
		return 0;
	}

	/* prepend zeros */
	memset (nsec, '0', TIMEINFO_NSEC_SIZE - 1);
	for (len = 0, x = tp.tv_nsec;
		(len < TIMEINFO_NSEC_SIZE) && (x != 0);
		len++, x /= 10L);
	snprintf (nsec + TIMEINFO_NSEC_SIZE - len, len + 1, "%li", tp.tv_nsec);
	
	/* cut last digits and append the result */
	len = strlen (*out);

	/* MAX_DIGITS + '.' + '\0' */
	if (out_len >= (len + TIMEINFO_MAX_DIGITS + 2)) {
#if 0
		/* XXX: snprintf may return more bytes than it wrote to str */
		len += snprintf (*out + len, TIMEINFO_MAX_DIGITS + 2, ".%s", nsec);
		return len; /*- TIMEINFO_NSEC_SIZE - TIMEINFO_MAX_DIGITS;*/
#endif
		snprintf (*out + len, TIMEINFO_MAX_DIGITS + 2, ".%s", nsec);
		return len + TIMEINFO_MAX_DIGITS + 1;
	}

	/* out buffer is too small */
	fprintf (stderr, "%s: out buffer too small\n", __func__);
	return 0;
}
