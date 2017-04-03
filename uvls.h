#ifndef UVLS_H__
#define UVLS_H__

#include <stdarg.h>
#include <uv.h>

/* format of date string */
#define UVLS_DATE_FMT		"%z %F %T"
/* array size where date string resides */
#define UVLS_DATE_SIZE		64
/* amount of digits of nanoseconds to print */
#define UVLS_NSEC_DIGITS	6
/* how many digits in tv_nsec */
#define UVLS_NSEC_SIZE		9
#define UVLS_BUFFER_SIZE	1024
#define UVLS_BUFFER_FACTOR	2

/* user-defined streams */
extern uv_stream_t *uvls_out;
extern uv_stream_t *uvls_err;

extern void
uvls_init (uv_loop_t *loop);

extern void
uvls_destroy (void);

#define uvls_puts(str) uvls_fprintf (uvls_out, "%s\n", (str))
#define uvls_printf(fmt, ...) uvls_fprintf (uvls_out, (fmt), __VA_ARGS__)
#define uvls_log(str) uvls_flogf (uvls_err, "%s\n", (str))
#define uvls_logf(fmt, ...) uvls_flogf (uvls_err, (fmt), __VA_ARGS__)

extern size_t
uvls_fprintf (uv_stream_t *stream, const char *fmt, ...);

extern size_t
uvls_vprintf (uv_stream_t *stream, const char *fmt, va_list ap);

extern size_t
uvls_flogf (uv_stream_t *stream, const char *fmt, ...);

#endif /* UVLS_H__ */
