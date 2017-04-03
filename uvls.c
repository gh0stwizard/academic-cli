#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "uvls.h"

#ifndef _DEBUG
#define CHECK
#define CHECK_0
#define NULL_CHECK
#else
#include <assert.h>
#define CHECK(x) do { assert ((x) == 0); } while (0)
#define CHECK_0(x) do { assert ((x) != 0); } while (0)
#define NULL_CHECK(x) do { assert ((x) != NULL); } while (0)
#endif


/* ------------------------------------------------------------------ */

uv_stream_t *uvls_out = NULL;
uv_stream_t *uvls_err = NULL;

typedef struct {
	uv_write_t req;
	uv_buf_t buf;
} write_req_t;

static uv_tty_t tty_out;
static uv_tty_t tty_err;

static struct timespec ts_prev;
static uint64_t hr_prev;

static uv_once_t guard = UV_ONCE_INIT;
static uv_loop_t *myloop;
static uv_mutex_t mylock;

/* ------------------------------------------------------------------ */

static void
write_cb (uv_write_t *req, int status);

inline static void
free_write_req (uv_write_t *req);

static void
uvls_init_once (void);

/* ------------------------------------------------------------------ */


extern void
uvls_init (uv_loop_t *loop)
{
	myloop = loop;
	uv_once (&guard, uvls_init_once);
}


static void
uvls_init_once (void)
{
	if (uvls_out == NULL)
		uvls_out = (uv_stream_t *) &tty_out;

	if (uvls_err == NULL)
		uvls_err = (uv_stream_t *) &tty_err;

	CHECK(uv_tty_init (myloop, (uv_tty_t *) uvls_out, 1, 0));
	CHECK(uv_tty_init (myloop, (uv_tty_t *) uvls_err, 2, 0));

	if (uv_guess_handle (1) == UV_TTY) {
		CHECK(uv_tty_set_mode
			((uv_tty_t *) uvls_out, UV_TTY_MODE_NORMAL));
	}

	if (uv_guess_handle (2) == UV_TTY) {
		CHECK(uv_tty_set_mode
			((uv_tty_t *) uvls_err, UV_TTY_MODE_NORMAL));
	}

	CHECK(clock_gettime (CLOCK_REALTIME, &ts_prev));
	hr_prev = uv_hrtime ();

	CHECK(uv_mutex_init (&mylock));
}


extern void
uvls_destroy (void)
{
	uv_mutex_destroy (&mylock);
	uv_tty_reset_mode ();
}


extern size_t
uvls_fprintf (uv_stream_t *stream, const char *fmt, ...)
{
	size_t r;
	va_list ap;


	va_start (ap, fmt);
	r = uvls_vprintf (stream, fmt, ap);
	va_end (ap);

	return r;
}


extern size_t
uvls_vprintf (uv_stream_t *stream, const char *fmt, va_list ap)
{
	va_list apcopy;
	write_req_t *req;
	char *data, *newdata;
	size_t data_size = UVLS_BUFFER_SIZE;
	size_t data_len = 0;


	if (fmt == NULL || strlen (fmt) == 0)
		return UV_EINVAL;

	data = malloc (sizeof (*data) * data_size);
	NULL_CHECK(data);

	vsnprintf (data, data_size, fmt, ap);

	data_len = strlen (data);
	while (data_len == (data_size - 1)) {
		data_size *= UVLS_BUFFER_FACTOR;
		newdata = realloc (data, data_size);
		NULL_CHECK(newdata);
		data = newdata;
		va_copy (apcopy, ap);
		vsnprintf (data, data_size, fmt, apcopy);
		data_len += strlen (data + data_len);
	}

	req = (write_req_t *) malloc (sizeof (*req));
	NULL_CHECK(req);
	req->buf = uv_buf_init (data, data_len);
	uv_write ((uv_write_t *) req, stream, &req->buf, 1, write_cb);

	return data_len;
}


extern size_t
uvls_flogf (uv_stream_t *stream, const char *fmt, ...)
{
	struct timespec ts;
	struct tm *tmp;
	uint64_t hr_now, hr_delta;
	char date[UVLS_DATE_SIZE];
	size_t date_len;
	va_list ap;
	size_t result;
	/* nanoseconds part */
	char nstr[UVLS_NSEC_SIZE + 1];
	long x;
	int len;


	hr_now = uv_hrtime ();
	hr_delta = hr_now - hr_prev;
	ts = ts_prev;
	ts.tv_sec += hr_delta / 1000000000;
	/* prevent tv_nsec overflow */
	ts.tv_nsec += hr_delta % 1000000000;
	ts.tv_nsec %= 1000000000;
	/* c.s. */
	uv_mutex_lock (&mylock);
	ts_prev = ts;
	hr_prev = hr_now;
	uv_mutex_unlock (&mylock);
	/* create date string */
	NULL_CHECK(tmp = localtime (&ts.tv_sec));
	date_len = strftime (date, UVLS_DATE_SIZE, UVLS_DATE_FMT, tmp);
	CHECK_0(date_len);
	/* nanosecs: prepend zeros */
	memset (nstr, '0', UVLS_NSEC_SIZE - 1);
	for (len = 0, x = ts.tv_nsec;
		(x != 0) && (len < UVLS_NSEC_SIZE);
		len++, x /= 10L);
	snprintf (nstr + UVLS_NSEC_SIZE - len, len + 1, "%li", ts.tv_nsec);
	/* nanosecs: append to date string */
	date[date_len] = '.';
	memcpy (date + date_len + 1, nstr, UVLS_NSEC_DIGITS);
	date_len += UVLS_NSEC_DIGITS + 1;
	date[date_len] = ' ';
	date[date_len+1] = '\0';
	/* write date string first */
	result = uvls_fprintf (stream, date);
	/* write user data */
	va_start (ap, fmt);
	result += uvls_vprintf (stream, fmt, ap);
	va_end (ap);

	return result;
}


static void
write_cb (uv_write_t *req, int status)
{
	(void) status;
	free_write_req (req);
}


inline static void
free_write_req (uv_write_t *req)
{
	write_req_t *wr = (write_req_t *) req;
	free (wr->buf.base);
	free (wr);
}
