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

static struct timespec ts;
static uint64_t hr;

static uv_once_t guard = UV_ONCE_INIT;
static uv_loop_t *myloop;
static uv_mutex_t timelock;

/* ------------------------------------------------------------------ */

static void
write_cb (uv_write_t *req, int status);

static void
uvls_init_once (void);

inline static void
uvls_update_time (void);

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

	CHECK(uv_tty_init (myloop, (uv_tty_t *) uvls_out, UVLS_STDOUT, 0));
	CHECK(uv_tty_init (myloop, (uv_tty_t *) uvls_err, UVLS_STDERR, 0));

	if (uv_guess_handle (UVLS_STDOUT) == UV_TTY) {
		CHECK(uv_tty_set_mode
			((uv_tty_t *) uvls_out, UV_TTY_MODE_NORMAL));
	}

	if (uv_guess_handle (UVLS_STDERR) == UV_TTY) {
		CHECK(uv_tty_set_mode
			((uv_tty_t *) uvls_err, UV_TTY_MODE_NORMAL));
	}

	CHECK(clock_gettime (CLOCK_REALTIME, &ts));
	hr = uv_hrtime ();

	CHECK(uv_mutex_init (&timelock));
}


extern void
uvls_destroy (void)
{
	uv_mutex_destroy (&timelock);
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
		return 0;

	NULL_CHECK(req = (write_req_t *) malloc (sizeof (*req)));
	NULL_CHECK(data = malloc (sizeof (*data) * data_size));

	vsnprintf (data, data_size, fmt, ap);
	data_len = strlen (data);
	/* increase buffer when needed */
	while (data_len == (data_size - 1)) {
		data_size *= UVLS_BUFFER_FACTOR;
		NULL_CHECK(newdata = realloc (data, data_size));
		data = newdata;
		va_copy (apcopy, ap);
		vsnprintf (data, data_size, fmt, apcopy);
		data_len += strlen (data + data_len);
	}

	req->buf = uv_buf_init (data, data_len);
	CHECK(uv_write ((uv_write_t *) req, stream, &(req->buf), 1, write_cb));

	return data_len;
}


extern size_t
uvls_flogf (uv_stream_t *stream, const char *fmt, ...)
{
	char date[UVLS_DATE_SIZE];
	va_list ap;
	size_t result;


	/* write date string first */
	(void) uvls_date (UVLS_DATE_FMT, date, UVLS_DATE_SIZE);
	result = uvls_fprintf (stream, date);
	/* write user data */
	va_start (ap, fmt);
	result += uvls_vprintf (stream, fmt, ap);
	va_end (ap);

	return result;
}


extern size_t
uvls_date (const char *fmt, char date[], size_t size)
{
	struct tm *tmp;
	size_t len;
	/* nanoseconds part */
	char nstr[UVLS_NSEC_SIZE + 1];
	long n;
	int nnum;


	uvls_update_time ();
	/* create date string */
	NULL_CHECK(tmp = localtime (&ts.tv_sec));
	CHECK_0(len = strftime (date, size, fmt, tmp));
	/* nanosecs: prepend zeros */
	memset (nstr, '0', UVLS_NSEC_SIZE - 1);
	for (nnum = 0, n = ts.tv_nsec;
		(n != 0) && (nnum < UVLS_NSEC_SIZE);
		nnum++, n /= 10L);
	snprintf (nstr + UVLS_NSEC_SIZE - nnum, nnum + 1, "%li", ts.tv_nsec);
	/* nanosecs: append to the end of the date string */
	date[len] = '.';
	memcpy (date + len + 1, nstr, UVLS_NSEC_DIGITS);
	len += UVLS_NSEC_DIGITS + 1;
	date[len] = ' ';
	date[len+1] = '\0';

	return len;
}


inline static void
uvls_update_time (void)
{
	uint64_t now, delta;


	uv_mutex_lock (&timelock);
	now = uv_hrtime ();
	delta = now - hr;
	ts.tv_sec += delta / 1000000000;
	/* prevent tv_nsec overflow */
	ts.tv_nsec += delta % 1000000000;
	ts.tv_nsec %= 1000000000;
	hr = now;
	uv_mutex_unlock (&timelock);
}


static void
write_cb (uv_write_t *req, int status)
{
	(void) status;
	write_req_t *wr = (write_req_t *) req;
	free (wr->buf.base);
	free (wr);
}
