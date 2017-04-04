#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include <curl/curl.h>
#include "vlog.h"
#include "queue.h"
#include "check.h"
#include "loop.h"
#include "html.h"
#include "cli.h"


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

static void
term_cb (term_result_t *terms);

static void
word_cb (word_result_t *d);


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
		ACADEMIC_DID_SYNONYMUM_EN_RU,
//		ACADEMIC_DID_UNIVERSAL_EN_RU,
//		ACADEMIC_DID_ENG_RUS,
	};

	while (argc-- > 1) {
		queue_term (argv[argc], did, ARRAY_SIZE(did), ACADEMIC_TERM_LIMIT, term_cb);
	}

	free (args);
	uv_timer_stop (handle);
}


static void
term_cb (term_result_t *t)
{
	term_entry_t *e = t->list;
	term_entry_t *end = e + t->entries;
	const char *word = t->word;
	size_t len = strlen (word);
	int matched = 0;


	vlog (VLOG_TRACE, "%s [did: %d]: found %d entries",
		word, t->did, t->entries);

	/* find word id */
	for (; e != end && ! matched; e++) {
		vlog (VLOG_TRACE, "%s: id: %s value: '%s'", word, e->id, e->value);
		if (strncmp (word, e->value, len + 1) == 0)
			matched = atoi (e->id);
	}

	if (matched > 0) {
		/* get term (word) definition */
		vlog (VLOG_TRACE, "%s [did: %d]: exact match by id %d",
			word, t->did, matched);
		queue_word_id (word, matched, t->did, word_cb);
	}
	else {
#ifndef __DEBUG
		uvls_printf ("%s: no exact match, %d records available:\n",
			word, t->entries);
		uvls_puts (
			"------------------------------------"
			"------------------------------------");
		for (e = t->list; e != end; e++)
			uvls_printf ("%s: %s\n", e->value, e->info);

		uvls_printf ("\n");
#else
		vlog (VLOG_NOTE, "%s [did %d]: no exact match, %d available",
			word, t->did, t->entries);
#endif
	}

	free_term_results (t);
}


static void
word_cb (word_result_t *d)
{
	char *cli_out = NULL;


#ifndef _DEBUG
	uvls_puts (d->word);
	uvls_puts (
		"------------------------------------"
		"------------------------------------");

	if (d->term && d->data) {
		(void) convert_html (d->data, &cli_out);
		uvls_printf ("%s\n\n", cli_out);
	}
	else
		uvls_printf ("ERROR: no data\n\n");
#else
	vlog (VLOG_DEBUG, "%s [wid: %d did: %d]: length of data %zu bytes",
		d->word, d->wid, d->did, d->data->length);
	vlog (VLOG_DEBUG, "%s [wid: %d did: %d]: %s",
		d->word, d->wid, d->did, d->data->text);
	(void) convert_html (d->data, &cli_out);
	uvls_logf ("%s\n", cli_out);
#endif

	if (cli_out != NULL)
		free (cli_out);
	free_word_results (d);
}
