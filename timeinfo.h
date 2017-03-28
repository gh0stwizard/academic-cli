#ifndef TIMEINFO_H__
#define TIMEINFO_H__

#include <sys/types.h>

#ifndef TIMEINFO_FMT
#define TIMEINFO_FMT "%z %F %T"
#endif

/* output buffer size */
#ifndef TIMEINFO_DATE_SIZE
#define TIMEINFO_DATE_SIZE 64
#endif

/* how many digits from nanoseconds part will be append to result string */
#ifndef TIMEINFO_MAX_DIGITS
#define TIMEINFO_MAX_DIGITS 6
#endif

/* max. digits returned by clock_gettime() in the tv_nsec field */
#ifndef TIMEINFO_NSEC_SIZE
#define TIMEINFO_NSEC_SIZE 9
#endif

/**
 * get_current_time_string: fill in 'out' buffer as a string
 * with the specified date format TIMEINFO_FMT, including nanoseconds
 * part up to TIMEINFO_MAX_DIGITS digits.
 * Returns total length of the string (without '\0') or 0 on error.
 */
extern int
get_current_time_string (char *out[], size_t out_len);


#endif /* TIMEINFO_H__ */
