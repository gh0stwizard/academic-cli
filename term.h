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

#ifndef TERM_H__
#define TERM_H__

#include <time.h>
#include <uv.h>
#include "academic.h"


typedef struct term_entry_s {
	char *id;
	char *value;
	char *info;
} term_entry_t;

typedef struct term_result_s {
	const char *word;
	int did;
	int limit;
	term_entry_t *list;
	int entries;
} term_result_t;

typedef void (*term_f) (term_result_t *terms);

typedef struct term_async_s {
	uv_async_t async;
	term_f cb;
} term_async_t;

typedef struct term_work_s {
	uv_work_t	req;
	uv_async_t *async;
	const char *word;
	int did;
	int limit;
} term_work_t;

typedef struct term_init_s {
	unsigned int retries;
	struct timespec sleep_ts;
	long connect_timeout;
} term_init_t;


extern void
term_init (term_init_t *options);

extern void
w_term_cb (uv_work_t *req);

extern void
w_term_after_cb (uv_work_t *req, int status);

extern void
free_term_results (term_result_t *t);

#endif /* TERM_H__ */
