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

#ifndef UVLS_H__
#define UVLS_H__

#include <stdarg.h>
#include <stdio.h>
#include <uv.h>

/* format of date string */
#define UVLS_DATE_FMT "%z %F %T"
/* array size where date string resides */
#define UVLS_DATE_SIZE 64
/* amount of digits of nanoseconds to print */
#define UVLS_NSEC_DIGITS 6
/* how many digits in tv_nsec */
#define UVLS_NSEC_SIZE 9
/* the data will be copied into buffer before printed out */
#define UVLS_BUFFER_SIZE 1024
#define UVLS_BUFFER_FACTOR 2
/* constants */
#define UVLS_STDOUT 1
#define UVLS_STDERR 2

/* user-defined streams */
extern uv_stream_t* uvls_out;
extern uv_stream_t* uvls_err;

extern void
uvls_init(uv_loop_t* loop);

extern void
uvls_destroy(void);

#if 0
#define uvls_puts(str) uvls_fprintf(uvls_out, "%s\n", (str))
#define uvls_printf(...) uvls_fprintf(uvls_out, __VA_ARGS__)
#define uvls_log(str) uvls_fprintf(uvls_err, "%s\n", (str))
#define uvls_logf(...) uvls_fprintf(uvls_err, __VA_ARGS__)
#define uvls_vlogf(fmt, ap) uvls_vprintf(uvls_err, (fmt), (ap))
#endif

#define uvls_puts(str) puts(str)
#define uvls_printf(...) fprintf(stdout, __VA_ARGS__)
#define uvls_log(str) fprintf(stderr, "%s\n", (str))
#define uvls_logf(...) fprintf(stderr, __VA_ARGS__)
#define uvls_vlogf(fmt, ap) vfprintf(stderr, (fmt), (ap))

extern size_t
uvls_fprintf(uv_stream_t* stream, const char* fmt, ...);

extern size_t
uvls_vprintf(uv_stream_t* stream, const char* fmt, va_list ap);

/* prepends a date string */
extern size_t
uvls_flogf(uv_stream_t* stream, const char* fmt, ...);

/* creates a date string */
extern size_t
uvls_date(const char* fmt, char date[], size_t size);

#endif /* UVLS_H__ */
