#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include <curl/curl.h>
#include "vlog.h"
#include "queue.h"
#include "check.h"
#include "loop.h"
#include "html.h"


#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))


static void
init_curl (void);

static void
fini_curl (void);

static void
print_curl_version (void);

static void
print_uv_version (void);

static void
init_uv (void);

static void
fini_uv (void);

static void
print_myhtml_version (void);


/* ------------------------------------------------------------------ */

uv_loop_t *loop;

/* ------------------------------------------------------------------ */


extern int
main (int argc, char *argv[])
{
	init_curl ();
	print_curl_version ();
	init_myhtml ();
	print_myhtml_version ();
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
	fini_myhtml ();
	fini_curl ();	

	return 0;
}


static void
init_curl (void)
{
	CURL_CHECK(curl_global_init (CURL_GLOBAL_ALL));
}


static void
fini_curl (void)
{
	curl_global_cleanup ();
}


static void
print_curl_version (void)
{
	curl_version_info_data * nfo = curl_version_info (CURLVERSION_NOW);

	vsay (VLOG_INFO, "Powered by curl version %s", nfo->version);
}


static void
print_uv_version (void)
{
	vsay (VLOG_INFO, "Powered by libuv version %s", uv_version_string ());
}


static void
init_uv (void)
{
	NULL_CHECK(loop = uv_default_loop ());
}


static void
fini_uv (void)
{
	vsay (VLOG_INFO, "Shutdown. Bye-bye!");
}


static void
print_myhtml_version (void)
{
	myhtml_version_t v = myhtml_version ();

	vsay (VLOG_INFO, "Powered by myhtml version %d.%d.%d",
		v.major, v.minor, v.patch);
}
