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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <uv.h>
#include <curl/curl.h>
#include "vlog.h"
#include "queue.h"
#include "check.h"
#include "loop.h"
#include "html.h"
#include "cli.h"


/* FIXME */
#define VERSION "0.1"
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

static void
parse_args (int argc, char *argv[]);

static void
print_usage (void);

static void
print_dids (void);

static void
print_version (void);

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


	/* used only when debug is on */
	print_curl_version ();
	print_myhtml_version ();
	print_uv_version ();

	/* process args with a delay */
	NULL_CHECK(myargs = malloc (sizeof (*myargs)));
	myargs->argc = argc;
	myargs->argv = argv;
	timer_arg.data = (void *) myargs;

	/* run loop */
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

	parse_args (args->argc, args->argv);
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
	uvls_printf ("%s [%d: %s]\n",
		d->word, d->did, academic_dname_en[d->did]);
	uvls_puts (
		"------------------------------------"
		"------------------------------------");

	if (d->term && d->data) {
		size_t len = convert_html (d->data, &cli_out);
		uvls_printf ("%.*s\n\n", len, cli_out);
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


static void
parse_args (int argc, char *argv[])
{
	int c;
	int *did;
	int didnum = 0;
	int term_limit = ACADEMIC_TERM_LIMIT;


	NULL_CHECK(did = malloc (sizeof (*did)));

	while (1) {
		int index = 0;
		static struct option opts[] = {
			{ "dictionary",	required_argument,	0, 'd' },
			{ "help",		no_argument,		0, 'h' },
			{ "list",		no_argument,		0, 'l' },
			{ "version",	no_argument,		0, 'v' },
			{ 0, 0, 0, 0 }
		};

		c = getopt_long (argc, argv, "d:lhv?", opts, &index);

		if (c == -1)
			break;

		switch (c) {
		case 0: /* long options */
			break;

		case 'd':
			did[didnum++] = atoi (optarg);
			NULL_CHECK(did = realloc (did, sizeof (*did) * (didnum + 1)));
			break;

		case 'h':
		case '?':
			print_usage ();
			return;

		case 'l':
			print_dids ();
			return;

		case 'v':
			print_version ();
			return;

		default:
			print_usage ();
			return;
		}
	}

	while (optind < argc)
		queue_term (argv[optind++], did, didnum, term_limit, term_cb);
}


static void
print_usage (void)
{
	uvls_puts ("Usage:\n"
				"academic-cli: -d ID1 [-d ID2...] [options] word1 word2 ... wordN\n"
				"Options:");
#define p(o, d) uvls_printf ("  %-24s %s\n", (o), (d))
	p ("--dictionary ID, -d ID", "Use this dictionary ID.");
	p ("--help, -h, -?", "Display this information.");
	p ("--list, -l", "Display dictionary IDs.");
	p ("--version, -v", "Display version information.");
#undef p
}


static void
print_dids (void)
{
	uvls_puts (" ID  | DICTIONARY");
	uvls_puts (
		"------------------------------------"
		"------------------------------------");
	for (int i = 0; i < ACADEMIC_DID_MAX; i++)
		uvls_printf ("%4d   %s\n", i, academic_dname_en[i]);
}


static void
print_version (void)
{
	curl_version_info_data *c = curl_version_info (CURLVERSION_NOW);
	myhtml_version_t m = myhtml_version ();


	uvls_printf ("academic-cli version %s\n", VERSION);
	uvls_printf ("Powered by: curl %s; libuv %s; myhtml %d.%d.%d\n",
		c->version,
		uv_version_string (),
		m.major, m.minor, m.patch);
}
