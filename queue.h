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

#ifndef QUEUE_H__
#define QUEUE_H__

#include <time.h>
#include "academic.h"
#include "term.h"
#include "word.h"


#define QUEUE_CURL_RETRIES		0
#define QUEUE_CURL_RETRY_SLEEP	1000


typedef struct queue_init_s {
	struct {
		unsigned int retries;
		struct timespec retry_sleep;
	} curl;
} queue_init_t;


extern void
queue_init (queue_init_t *options);

extern void
queue_term (const char *word, int did[], int didnum, int limit, term_f cb);

extern void
queue_word_id (const char *word, int wid, int did, word_f cb);

#endif /* QUEUE_H__ */
