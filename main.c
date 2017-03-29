#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <uv.h>
#include <curl/curl.h>
#include "vlog.h"


#define CURL_CHECK(x)	do { assert ((x) == CURLE_OK); } while (0)
#define CURLM_CHECK(x)	do { assert ((x) == CURLM_OK); } while (0)
#define NULL_CHECK(x)	do { assert ((x) != NULL); } while (0)
#define UV_CHECK(x)	do { assert ((x) == 0); } while (0)

typedef struct worker_s {
	CURL *curl;
	const char *url;
} worker_t;


void
init_curl (void);

void
fini_curl (void);

void
print_curl_version (void);

void
print_uv_version (void);

void
init_uv (void);

void
fini_uv (void);

void
add_download (const char *url);

void
curl_work_cb (uv_work_t *req);

void
curl_after_work_cb (uv_work_t *req, int status);


/* ------------------------------------------------------------------ */

uv_loop_t *loop;

/* ------------------------------------------------------------------ */


extern int
main (int argc, char *argv[])
{
	init_curl ();
	print_curl_version ();
	init_uv ();
	print_uv_version ();

	while (argc-- > 1) {
		add_download (argv[argc]);
	}

	uv_run (loop, UV_RUN_DEFAULT);

	fini_uv ();
	fini_curl ();	

	return 0;
}


void
init_curl (void)
{
	CURL_CHECK(curl_global_init (CURL_GLOBAL_ALL));
}


void
fini_curl (void)
{
	curl_global_cleanup ();
}


void
print_curl_version (void)
{
	curl_version_info_data * nfo = curl_version_info (CURLVERSION_NOW);

	vlog (VLOG_INFO, "Powered by curl version %s", nfo->version);
}


void
print_uv_version (void)
{
	vlog (VLOG_INFO, "Powered by libuv version %s", uv_version_string ());
}


void
init_uv (void)
{
	NULL_CHECK(loop = uv_default_loop ());
}


void
fini_uv (void)
{
}


void
add_download (const char *url)
{
	uv_work_t *req;
	worker_t *w;


	NULL_CHECK(req = (uv_work_t *) malloc (sizeof (*req)));
	NULL_CHECK(w = (worker_t *) malloc (sizeof (*w)));

	w->url = url;
	req->data = w;
	UV_CHECK(uv_queue_work (loop, req, curl_work_cb, curl_after_work_cb));
}


void
curl_work_cb (uv_work_t *req)
{
	CURL *handle;
	worker_t *w = (worker_t *) req->data;


	NULL_CHECK(handle = curl_easy_init ());
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_URL, w->url));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_VERBOSE, 1L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_LOW_SPEED_TIME, 3L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_LOW_SPEED_LIMIT, 1L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_CONNECTTIMEOUT, 30L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_FOLLOWLOCATION, 1L));

	w->curl = handle;
	
	CURL_CHECK(curl_easy_perform (handle));
}


void
curl_after_work_cb (uv_work_t *req, int status)
{
	worker_t *w = (worker_t *) req->data;


	vlog (VLOG_DEBUG, "worker %p status %d", req, status);
	curl_easy_cleanup (w->curl);
	req->data = NULL;
	free (w);
	free (req);
}
