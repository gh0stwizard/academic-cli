#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include <curl/curl.h>
#include "vlog.h"
#include "queue.h"
#include "check.h"
#include "loop.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

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

	int did[] = {
//		ACADEMIC_DID_SYNONYMUM_RU_EN,
		ACADEMIC_DID_SYNONYMUM_EN_RU,
//		ACADEMIC_DID_UNIVERSAL_EN_RU,
	};

	while (argc-- > 1) {
		queue_word (argv[argc], did, ARRAY_SIZE(did));
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
