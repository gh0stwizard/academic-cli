#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include <curl/curl.h>
#include "vlog.h"
#include "check.h"
#include "loop.h"
#include "www.h"
#include "json.h"

#define URL_MAX_SIZE    2048

static const char term_fmt[] =
    "http://academic.ru/seek4term.php?json=true&limit=%d&did=%s&q=%s";

typedef struct www_work_s {
    uv_work_t worker;
    www_data_t *data;
} www_work_t;

typedef struct curl_mem_s {
	char *data;
	size_t size;
} curl_mem_t;


static size_t
curl_mem_write_cb (char *data, size_t size, size_t nmemb, void *userp);

static void
async_term_cb (uv_async_t *req);

static void
async_word_cb (uv_async_t *req);

static void
async_close_cb (uv_handle_t *handle);

static void
term_cb (uv_work_t *work);

static void
word_cb (uv_work_t *work);

static void
after_cb (uv_work_t *req, int status);

static www_user_f user_term_cb;
static www_user_f user_word_cb;
static int user_term_limit;
static long user_connect_timeout;
static unsigned int user_retries;
static struct timespec user_retry_ts;


extern void
www_init (www_options_t *options)
{
    user_term_cb = options->term.cb;
    user_word_cb = options->word.cb;
    user_term_limit = options->term.limit;
    user_connect_timeout = options->curl.connect_timeout;
    user_retries = options->curl.retries;
    user_retry_ts = options->curl.retry_sleep;
}


extern void
www_term (const char *word, int did, const char *dname, const char *wfmt)
{
    www_work_t *w;
    www_data_t *d;
    uv_async_t *a;
    char url[URL_MAX_SIZE];


    NULL_CHECK(a = malloc (sizeof (*a)));
    UV_CHECK(uv_async_init (loop, a, async_term_cb));
    NULL_CHECK(w = malloc (sizeof (*w)));
    NULL_CHECK(d = malloc (sizeof (*d)));
    d->result.term = NULL;
    d->result.word = NULL;
    d->word = word;
    d->did = did;
    d->wid = 0;
    snprintf (url, URL_MAX_SIZE, term_fmt, user_term_limit, dname, word);
    d->url.term = strdup (url);
    d->url.word = strdup (wfmt);
    d->async = a;
    w->data = d;
	UV_CHECK(uv_queue_work (loop, (uv_work_t *) w, term_cb, after_cb));
}


extern void
www_word_data (www_data_t *data)
{
    www_work_t *w;
    uv_async_t *a;
    char url[URL_MAX_SIZE];


    NULL_CHECK(a = malloc (sizeof (*a)));
    UV_CHECK(uv_async_init (loop, a, async_word_cb));
    NULL_CHECK(w = malloc (sizeof (*w)));
    snprintf (url, URL_MAX_SIZE, data->url.word, data->wid);
    free (data->url.word);
    data->url.word = strdup (url);
    data->async = a;
    w->data = data;
	UV_CHECK(uv_queue_work (loop, (uv_work_t *) w, word_cb, after_cb));
}


static void
async_term_cb (uv_async_t *req)
{
    www_data_t *d = (www_data_t *) req->data;

    d->async = NULL;

	if (user_term_cb != NULL)
		user_term_cb (d);
    else
        free_www_data (d);

    uv_close((uv_handle_t *) req, async_close_cb);
}


static void
async_word_cb (uv_async_t *req)
{
    www_data_t *d = (www_data_t *) req->data;

    d->async = NULL;

	if (user_word_cb != NULL)
		user_word_cb (d);
    else
        free_www_data (d);

    uv_close((uv_handle_t *) req, async_close_cb);
}


static void
async_close_cb (uv_handle_t *handle)
{
    free(handle);
}


extern void
free_www_data (www_data_t *data)
{
    if (data != NULL) {
        free (data->url.term);
        free (data->url.word);
        free_term_result (data->result.term);
        free (data);
    }    
}


static void
term_cb (uv_work_t *work)
{
    www_work_t *w = (www_work_t *) work;
    www_data_t *d = w->data;
    uv_async_t *a = d->async;
	term_result_t *r;
	CURLcode code;
	CURL *curl;
	curl_mem_t storage;
	unsigned int retry = 0;
    const char *word = d->word;
    int did = d->did;
    char *url = d->url.term;


	NULL_CHECK(storage.data = malloc (sizeof (char)));
	storage.size = 0;
	NULL_CHECK(r = malloc (sizeof (*r)));
    r->list = NULL;
	r->entries = 0;

	NULL_CHECK(curl = curl_easy_init ());
	CURL_CHECK(curl_easy_setopt (curl, CURLOPT_NOSIGNAL, 1L));
	CURL_CHECK(curl_easy_setopt (curl, CURLOPT_URL, url));
	CURL_CHECK(curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1L));
	CURL_CHECK(curl_easy_setopt
		(curl, CURLOPT_CONNECTTIMEOUT, user_connect_timeout));
	CURL_CHECK(curl_easy_setopt
		(curl, CURLOPT_WRITEFUNCTION, curl_mem_write_cb));
	CURL_CHECK(curl_easy_setopt
		(curl, CURLOPT_WRITEDATA, (void *) &storage));
#ifdef _DEBUG_CURL
	CURL_CHECK(curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L));
#else
	CURL_CHECK(curl_easy_setopt (curl, CURLOPT_VERBOSE, 0L));
#endif

	/* perform request */
	vlog (VLOG_TRACE, "%s: %s", word, url);

	do {
		code = curl_easy_perform (curl);

		if (code == CURLE_OK)
			break;

		if (code == CURLE_COULDNT_RESOLVE_HOST ||
			code == CURLE_OPERATION_TIMEDOUT)
		{
			if (retry++ < user_retries) {
				vlog (VLOG_WARN, "%s: retry #%d", word, retry);
				storage.size = 0;
				(void) nanosleep (&user_retry_ts, NULL);
			}
			else
				break;
		}
	} while (code != CURLE_OK);


    if (code == CURLE_OK) {
        vlog (VLOG_TRACE, "%s [%d]: parsing json, size %zu",
            word, did, storage.size);
	    r->entries = parse_json (storage.data, storage.size, &r->list);
    }
    else {
#ifndef _DEBUG
		uvls_logf ("%s [%d]: curl error #%d: %s\n",
			word, did,
			code, curl_easy_strerror (code));
#else
		vlog (VLOG_ERROR, "%s [%d]: curl error #%d: %s",
			word, did, code, curl_easy_strerror (code));
#endif
	}

	curl_easy_cleanup (curl);
	free (storage.data);

	vlog (VLOG_TRACE, "%s [%d]: sending results...", word, did);
    d->result.term = r;
	a->data = d;
	UV_CHECK(uv_async_send (a));
}

extern void
word_cb (uv_work_t *work)
{
    www_work_t *w = (www_work_t *) work;
    www_data_t *d = w->data;
    uv_async_t *a = d->async;
	word_result_t *r;
	CURLcode code;
	CURL *curl;
	curl_mem_t storage;
	unsigned int retry = 0;
    const char *word = d->word;
    int did = d->did;
    int wid = d->wid;
    char *url = d->url.word;


	NULL_CHECK(storage.data = malloc (sizeof (char)));
	storage.size = 0;
	NULL_CHECK(r = malloc (sizeof (*r)));
    r->term = NULL;
    r->data = NULL;

	NULL_CHECK(curl = curl_easy_init ());
	CURL_CHECK(curl_easy_setopt (curl, CURLOPT_NOSIGNAL, 1L));
	CURL_CHECK(curl_easy_setopt (curl, CURLOPT_URL, url));
	CURL_CHECK(curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1L));
	CURL_CHECK(curl_easy_setopt
		(curl, CURLOPT_CONNECTTIMEOUT, user_connect_timeout));
	CURL_CHECK(curl_easy_setopt
		(curl, CURLOPT_WRITEFUNCTION, curl_mem_write_cb));
	CURL_CHECK(curl_easy_setopt
		(curl, CURLOPT_WRITEDATA, (void *) &storage));
#ifdef _DEBUG_CURL
	CURL_CHECK(curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L));
#else
	CURL_CHECK(curl_easy_setopt (curl, CURLOPT_VERBOSE, 0L));
#endif

	/* perform request */
	vlog (VLOG_DEBUG, "%s [%d:%d]: %s", word, did, wid, url);

	do {
		code = curl_easy_perform (curl);

		if (code == CURLE_OK)
			break;

		if (code == CURLE_COULDNT_RESOLVE_HOST ||
			code == CURLE_OPERATION_TIMEDOUT)
		{
			if (retry++ < user_retries) {
				vlog (VLOG_WARN, "%s [%d:%d]: retry #%d",
                    word, did, wid, retry);
				storage.size = 0;
				(void) nanosleep (&user_retry_ts, NULL);
			}
			else
				break;
		}
	} while (code != CURLE_OK);

    if (code == CURLE_OK) {
    	vlog (VLOG_TRACE, "%s [%d:%d]: parsing html, size %zu",
	    	word, did, wid, storage.size);
    	r->term = parse_html (storage.data, storage.size, &r->data);
    }
    else {
#ifndef _DEBUG
		uvls_logf ("%s [%d:%d]: curl error #%d: %s\n",
			word, did, wid, code, curl_easy_strerror (code));
#else
		vlog (VLOG_ERROR, "%s [%d:%d]: curl error #%d: %s",
			word, did, wid, code, curl_easy_strerror (code));
#endif
	}

	curl_easy_cleanup (curl);
	free (storage.data);

	vlog (VLOG_TRACE, "%s [%d:%d]: sending results...", word, did, wid);
    d->result.word = r;
	a->data = d;
	UV_CHECK(uv_async_send (a));
}


static void
after_cb (uv_work_t *req, int status)
{
	www_work_t *w = (www_work_t *) req;
	free (w);
}


extern void
free_term_result (term_result_t *r)
{
	term_entry_t *s, *e;


    if (r != NULL) {
        if (r->list != NULL) {
        	for (s = r->list, e = r->list + r->entries; s != e; s++) {
	        	free (s->id);
	    	    free (s->value);
    		    free (s->info);
	        }

            free (r->list);
        }
	    
    	free (r);
    }
}


extern void
free_word_result (word_result_t *r)
{
    if (r != NULL) {
        if (r->term != NULL) {
            free (r->term);
        }

        free_html_data (r->data);
        free (r);
    }
}


static size_t
curl_mem_write_cb (char *data, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	curl_mem_t *mem = (curl_mem_t *) userp;


	mem->data = realloc (mem->data, mem->size + realsize);

	if (mem->data != NULL) {
		memcpy (&(mem->data[mem->size]), data, realsize);
		mem->size += realsize;
	}
    else {
        realsize = 0;
	    vlog (VLOG_ERROR, "realloc: out of memory");
    }

	return realsize;
}
