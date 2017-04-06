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
print_dids (unsigned int start, unsigned int end);

static void
print_dtypes (void);

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
	size_t entries = t->entries;
	term_entry_t *e = t->list;
	term_entry_t *end = e + entries;
	const char *word = t->word;
	size_t len = strlen (word);
	int matched = 0;


	vlog (VLOG_TRACE, "%s [did: %d]: found %d entries",
		word, t->did, entries);

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
#ifndef _DEBUG
		if (entries > 0)
			uvls_printf ("%s: no exact match, %d records available:\n",
				word, entries);
		else
			uvls_printf ("%s: no records\n", word);
		uvls_puts (
			"------------------------------------"
			"------------------------------------");
		for (e = t->list; e != end; e++)
			uvls_printf ("%s:\n%s\n\n", e->value, e->info);
		uvls_printf ("\n");
#else
		vlog (VLOG_NOTE, "%s [did %d]: no exact match, %d available",
			word, t->did, entries);
#endif
	}

	free_term_results (t);
}


static void
word_cb (word_result_t *d)
{
	char *out = NULL;
	html_data_t *data = d->data;
	size_t len;


#ifndef _DEBUG


	if (data != NULL) {
		uvls_printf ("%s [%d: %s]\n",
			d->word, d->did, academic_dname_en[d->did]);
		uvls_puts (
			"------------------------------------"
			"------------------------------------");
		/* remove line breaks at the begining */
		len = convert_html (data, &out);
		char *t = out;
		size_t i;
		for (i = 0; *t == '\n' || *t == '\r'; t++, i++);
		len -= i;
		if (i > 0)
			memmove (out, t, len);
		/* remove line breaks at the end */
		t = out + len - 1;
		for (i = len; i > 0 && (*t == '\n' || *t == '\r'); i--, t--)
			*t = '\0';
		len = i;
		uvls_printf ("%.*s\n\n", len, out);
	}
	else {
		uvls_logf ("%s: error: no data\n", d->word);
	}
#else
	if (data != NULL) {
		vlog (VLOG_TRACE, "%s [wid: %d did: %d]: data length %zu bytes",
			d->word, d->wid, d->did, data->length);
		vlog (VLOG_TRACE, "%s [wid: %d did: %d]: %.*s",
			d->word, d->wid, d->did, data->length, data->text);
		len = convert_html (d->data, &out);
		vlog (VLOG_TRACE, "%.*s", len, out);
	}
	else {
		vlog (VLOG_ERROR, "%s [wid: %d did: %d]: no data",
			d->word, d->wid, d->did);
	}
#endif

	if (out != NULL)
		free (out);
	free_word_results (d);
}


static void
parse_args (int argc, char *argv[])
{
	int c;
	int *did;
	int didnum = 0;
	int term_limit = ACADEMIC_TERM_LIMIT;
	queue_init_t queue_opts;


	queue_opts.curl.retries = QUEUE_CURL_RETRIES;
	queue_opts.curl.retry_sleep.tv_sec = QUEUE_CURL_RETRY_SLEEP / 1000;
	queue_opts.curl.retry_sleep.tv_nsec =
		(QUEUE_CURL_RETRY_SLEEP % 1000) * 1000000;

	NULL_CHECK(did = malloc (sizeof (*did)));

	while (1) {
		int index = 0;
		static struct option opts[] = {
			{ "dictionary",     required_argument,  0, 'd' },
			{ "help",           no_argument,        0, 'h' },
			{ "list",           required_argument,  0, 'l' },
			{ "list-all",       no_argument,        0, 'L' },
			{ "list-types",     no_argument,        0, 'T' },
			{ "retries",        required_argument,  0, 'r' },
			{ "retry-timeout",  required_argument,  0, 't' },
			{ "version",        no_argument,        0, 'v' },
			{ 0, 0, 0, 0 }
		};

		c = getopt_long (argc, argv, "d:l:hr:t:v?LT", opts, &index);

		if (c == -1)
			break;

		switch (c) {
		case 0: /* long options */
			break;

		case 'd': {
			int id = atoi (optarg);
			if (id >= 0 && id < ACADEMIC_DID_MAX) {
				did[didnum++] = id;
				NULL_CHECK(did = realloc (did, sizeof (*did) * (didnum + 1)));
			}
			else {
				uvls_printf ("dictionary: invalid value '%s', ignoring\n",
					optarg);
			}
		}	break;

		case 'h':
		case '?':
			print_usage ();
			return;

		case 'l':
			if (strncmp ("all", optarg, 4) == 0)
				print_dids (0, ACADEMIC_DID_MAX);
			else if (strncmp ("en-ru", optarg, 6) == 0)
				print_dids (
					ACADEMIC_DID_UNIVERSAL_EN_RU,
					ACADEMIC_DID_SYNONYMUM_RU_EN);
			else if (strncmp ("ru-en", optarg, 6) == 0)
				print_dids (
					ACADEMIC_DID_SYNONYMUM_RU_EN,
					ACADEMIC_DID_ENC_BIOLOGY);
			else if (strncmp ("enc:ru", optarg, 7) == 0)
				print_dids (
					ACADEMIC_DID_ENC_BIOLOGY,
					ACADEMIC_DID_MAX);
			else
				uvls_logf ("list: invalid value '%s'\n", optarg);
			return;

		case 'r': {
			int retries = atoi (optarg);
			if (retries < 0) {
				uvls_logf ("retries: invalid value '%s'\n", optarg);
				return;
			}
			queue_opts.curl.retries = retries;
		} 	break;

		case 't': {
			int sleep = atoi (optarg);
			if (sleep < 0) {
				uvls_logf ("retry-sleep: invalid value '%s'\n", optarg);
				return;
			}
			queue_opts.curl.retry_sleep.tv_sec = sleep / 1000;
			queue_opts.curl.retry_sleep.tv_nsec = (sleep % 1000) * 1000000;
		}	break;

		case 'v':
			print_version ();
			return;

		case 'L':
			print_dids (0, ACADEMIC_DID_MAX);
			return;

		case 'T':
			print_dtypes ();
			return;

		default:
			print_usage ();
			return;
		}
	}

	if (didnum == 0) {
		uvls_logf ("error: no dictionaries, see --dictionary option\n");
		return;
	}

	/* set up options */
	queue_init (&queue_opts);

	while (optind < argc)
		queue_term (argv[optind++], did, didnum, term_limit, term_cb);
}


static void
print_usage (void)
{
	uvls_puts ("Usage:\n"
		"academic-cli: -d ID1 [-d ID2...] [options] word1 word2 ... wordN\n"
		"Options:");
#define p(o, d) uvls_printf ("  %-28s %s\n", (o), (d))
	p ("--dictionary ID, -d ID", "Use this dictionary ID.");
	p ("--help, -h, -?", "Display this information.");
	p ("--list TYPE, -l TYPE", "Display dictionary IDs by type. See --list-types.");
	p ("--list-all, -L", "Display all dictionary IDs.");
	p ("--list-types, -T", "Display dictionary types.");
	p ("--retries NUM, -r NUM",
		"How many times to retry establish a connection.");
	p ("--retry-timeout MS, -t MS",
		"Delay between connection retries in milliseconds.");
	p ("--version, -v", "Display version information.");
#undef p
}


static void
print_dids (unsigned int start, unsigned int end)
{
	uvls_puts (" ID  | DICTIONARY");
	uvls_puts (
		"------------------------------------"
		"------------------------------------");
	for (unsigned int i = start; i < end; i++)
		uvls_printf ("%4d   %s\n", i, academic_dname_en[i]);
}


static void
print_dtypes (void)
{
	uvls_puts ("List of available dictionary types:");
#define p(o, d) uvls_printf ("  %-14s %s\n", (o), (d))
	p ("all", "All dictionaries.");
	p ("en-ru", "English-Russian dictionaries.");
	p ("ru-en", "Russian-English dictionaries.");
	p ("enc:ru", "Russian encyclopedies.");
#undef p
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
