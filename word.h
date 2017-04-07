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

#ifndef DIC_H__
#define DIC_H__

#include <time.h>
#include <uv.h>
#include "academic.h"
#include "html.h"


typedef struct dic_result_s {
	const char *word;
	char *term;
	html_data_t *data;
	int wid;
	int did;
} word_result_t;

typedef void (*word_f) (word_result_t *data);

typedef struct word_async_s {
	uv_async_t async;
	word_f cb;
} word_async_t;

typedef struct word_work_s {
	uv_work_t	req;
	uv_async_t *async;
	const char *word;
	int wid;
	int did;
} word_work_t;

typedef struct word_init_s {
	unsigned int retries;
	struct timespec sleep_ts;
	long connect_timeout;
} word_init_t;


extern void
word_init (word_init_t *options);

void
w_word_cb (uv_work_t *req);

void
w_word_after_cb (uv_work_t *req, int status);

extern void
free_word_results (word_result_t *r);

#endif /* DIC_H__ */
