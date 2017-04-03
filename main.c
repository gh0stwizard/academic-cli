#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include <curl/curl.h>
#include "vlog.h"
#include "queue.h"
#include "check.h"
#include "loop.h"
#include "html.h"
#include "output.h"


#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))


typedef struct timer_arg_s {
	int argc;
	char **argv;
} timer_arg_t;


/* ------------------------------------------------------------------ */


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

static void
sig_int_cb (uv_signal_t *handle, int signum);

static void
sig_term_cb (uv_signal_t *handle, int signum);

static void
timer_arg_cb (uv_timer_t *handle);


/* ------------------------------------------------------------------ */

uv_loop_t *loop;
static uv_signal_t sig_int, sig_term;
static uv_timer_t timer_arg;

/* ------------------------------------------------------------------ */


extern int
main (int argc, char *argv[])
{
	timer_arg_t *myargs;


	init_curl ();
	init_myhtml ();
	init_uv ();

	print_curl_version ();
	print_myhtml_version ();
	print_uv_version ();

	NULL_CHECK(myargs = malloc (sizeof (*myargs)));
	myargs->argc = argc;
	myargs->argv = argv;
	timer_arg.data = (void *) myargs;

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
#ifdef _DEBUG
	curl_version_info_data * nfo = curl_version_info (CURLVERSION_NOW);

	vsay (VLOG_INFO, "Powered by curl version %s", nfo->version);
#endif
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

	

	uv_signal_init (loop, &sig_int);
	uv_signal_start (&sig_int, sig_int_cb, SIGINT);

	uv_signal_init (loop, &sig_term);
	uv_signal_start (&sig_term, sig_term_cb, SIGTERM);

	uv_unref ((uv_handle_t*) &sig_int);
	uv_unref ((uv_handle_t*) &sig_term);

	uv_timer_init (loop, &timer_arg);
	uv_timer_start (&timer_arg, timer_arg_cb, 100, 0);

	uvls_init (loop);
}


static void
fini_uv (void)
{
	uvls_destroy ();
	vsay (VLOG_INFO, "Shutdown. Bye-bye!");
}


static void
print_myhtml_version (void)
{
#ifdef _DEBUG
	myhtml_version_t v = myhtml_version ();

	vsay (VLOG_INFO, "Powered by myhtml version %d.%d.%d",
		v.major, v.minor, v.patch);
#endif
}


static void
sig_int_cb (uv_signal_t *handle, int signum)
{
	vlog (VLOG_TRACE, "SIGINT");
	uv_signal_stop (handle);
	uv_stop (loop);
}


static void
sig_term_cb (uv_signal_t *handle, int signum)
{
	vlog (VLOG_TRACE, "SIGTERM");
	uv_signal_stop (handle);
	uv_stop (loop);
}


static void
timer_arg_cb (uv_timer_t *handle)
{
	timer_arg_t *args = (timer_arg_t *) handle->data;
	int argc = args->argc;
	char **argv = args->argv;


	int did[] = {
//		ACADEMIC_DID_SYNONYMUM_RU_EN,
//		ACADEMIC_DID_SYNONYMUM_EN_RU,
//		ACADEMIC_DID_UNIVERSAL_EN_RU,
		ACADEMIC_DID_ENG_RUS,
	};

	while (argc-- > 1) {
		queue_word (argv[argc], did, ARRAY_SIZE(did));
	}

	free (args);
	uv_timer_stop (handle);
}
