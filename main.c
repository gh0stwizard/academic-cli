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
#include "check.h"
#include "loop.h"
#include "www.h"
#include "cli.h"
#include "db.h"


#define VERSION "0.2"
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

typedef struct timer_arg_s {
	int argc;
	char **argv;
} timer_arg_t;

static int quite = 0;

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
term_cb (www_data_t *data);

static void
word_cb (www_data_t *data);

static void
parse_args (int argc, char *argv[]);

static void
print_usage (void);

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

    db_close ();
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
term_cb (www_data_t *data)
{
    term_result_t *t = data->result.term;
	size_t entries = t->entries;
	const char *word = data->word;
	size_t len = strlen (word);
	int matched = 0;
    int did = data->did;


	vlog (VLOG_TRACE, "%s [%d]: found %d entries", word, did, entries);

	/* find word id */
    if (t->list != NULL) {
	    term_entry_t *s, *e;

    	for (s = t->list, e = t->list + entries; s != e && ! matched; s++) {
	    	vlog (VLOG_TRACE, "%s [%d]: id = %s value = '%s'",
                word, did, s->id, s->value);
    		if (strncmp (word, s->value, len + 1) == 0)
	    		matched = atoi (s->id);
    	}
    }

	if (matched > 0) {
		/* get term (word) definition */
		vlog (VLOG_TRACE, "%s [%d]: exact match by id %d", word, did, matched);
        data->wid = matched;
        free_term_result (t);
        data->result.term = NULL;
		www_word_data (data);
        return;
	}
#ifndef _DEBUG
	else if (! quite) {
		if (t->list != NULL) {
            term_entry_t *s, *e;

			uvls_logf ("%s [%d]:\nno exact match, %d records available:\n",
				word, did, entries);
			uvls_logf (
				"------------------------------------"
				"------------------------------------\n");
			for (s = t->list, e = t->list + entries; s != e; s++)
				uvls_logf ("%s:\n%s\n\n", s->value, s->info);
		}
		else {
			uvls_logf ("%s [%d]: no records\n", word, did);
		}
#else
    else {
		vlog (VLOG_NOTE, "%s [%d]: no exact match, %d available",
            word, did, entries);
#endif
	}
}


static void
word_cb (www_data_t *d)
{
    word_result_t *r = d->result.word;
	char *out = NULL;
	size_t len;
	html_data_t *data = r->data;
    const char *word = d->word;
    int did = d->did;
    int wid = d->wid;


#ifndef _DEBUG
	if (data != NULL) {
		uvls_printf ("%s [%d:%d]\n", word, did, wid);
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
		uvls_logf ("%s [%d:%d]: no data\n", word, did, wid);
	}
#else
	if (data != NULL) {
		vlog (VLOG_TRACE, "%s [%d:%d]: data length %zu bytes",
			word, did, wid, data->length);
		vlog (VLOG_TRACE, "%s [%d:%d]:\n%.*s",
			word, did, wid, data->length, data->text);
		len = convert_html (data, &out);
		uvls_printf ("%.*s\n", len, out);
	}
	else {
		vlog (VLOG_ERROR, "%s [%d:%d]: no data", word, did, wid);
	}
#endif

	if (out != NULL) {
		free (out);
    }

	free_www_data (d);
}


static void
parse_args (int argc, char *argv[])
{
	int r;
	int *did;
	int didnum = 0;
	www_options_t wo;
	char *lang = "en";

    
    wo.term.limit = WWW_TERM_LIMIT;
    wo.term.cb = term_cb;
	wo.word.cb = word_cb;
	wo.curl.connect_timeout = WWW_CONNECT_TIMEOUT;
	wo.curl.retries = WWW_RETRIES;
	wo.curl.retry_sleep.tv_sec = WWW_RETRY_SLEEP / 1000;
	wo.curl.retry_sleep.tv_nsec = (WWW_RETRY_SLEEP % 1000) * 1000000;

	NULL_CHECK(did = malloc (sizeof (*did)));

    if (SQLITE_OK != db_open("academic.db", "academic.sql")) {
        return;
    }

#define add_did(n) do { \
did[didnum++] = (n); \
NULL_CHECK(did = realloc (did, sizeof (*did) * (didnum + 1))); \
} while (0)

	while (1) {
		int index = 0;
		static struct option opts[] = {
			{ "dictionary",     required_argument,  0, 'd' },
			{ "help",           no_argument,        0, 'h' },
			{ "list",           required_argument,  0, 'l' },
			{ "language",       required_argument,  0, 'L' },
			{ "list-types",     no_argument,        0, 'T' },
			{ "retries",        required_argument,  0, 'r' },
			{ "retry-timeout",  required_argument,  0, 't' },
			{ "quite",          no_argument,        0, 'q' },
			{ "version",        no_argument,        0, 'v' },
			{ 0, 0, 0, 0 }
		};

		r = getopt_long (argc, argv, "d:l:hr:t:qv?C:D:L:T", opts, &index);

		if (r == -1)
			break;

		switch (r) {
		case 0: /* long options */
			break;

		case 'd': {
			int id = atoi (optarg);
			if (id >= 1)
				add_did(id);
			else
				uvls_printf ("dictionary: invalid value '%s', ignoring\n",
					optarg);
		}	break;

		case 'h':
		case '?':
			print_usage ();
			return;

		case 'l':
			if (strncmp ("all", optarg, 4) == 0)
				print_dictionaries (lang, "%");
			else
				print_dictionaries (lang, optarg);
			return;

		case 'r': {
			int retries = atoi (optarg);
			if (retries < 0) {
				uvls_logf ("retries: invalid value '%s'\n", optarg);
				return;
			}
			wo.curl.retries = retries;
		} 	break;

		case 't': {
			int sleep = atoi (optarg);
			if (sleep < 0) {
				uvls_logf ("retry-sleep: invalid value '%s'\n", optarg);
				return;
			}
			wo.curl.retry_sleep.tv_sec = sleep / 1000;
			wo.curl.retry_sleep.tv_nsec = (sleep % 1000) * 1000000;
		}	break;

		case 'q':
			quite = 1;
			break;

		case 'v':
			print_version ();
			return;

		case 'C': {
			long connect_timeout = atol (optarg);
			if (connect_timeout < 0) {
				uvls_logf ("connect timeout: invalid value '%s'\n", optarg);
				return;
			}
			wo.curl.connect_timeout = connect_timeout;
		}	break;

		case 'D': {
			char *t = optarg;
			char c = '\0';
			int s = 0, e = 0;
			sscanf (t, "%i%c%i", &s, &c, &e);
			if (c != '-')
				e = s;
			if (s >= 1 && e >= 1) {
				for (; s <= e; s++)
					add_did(s);
			}
			/* handle comma separated list of ranges */
			for (; *t != '\0'; t++) {
				if (*t == ',' && *(t+1) != '\0') {
					s = e = 0;
					c = '\0';
					sscanf (++t, "%i%c%i", &s, &c, &e);
					if (c != '-')
						e = s;
					if (s >= 1 && e >= 1) {
						for (; s <= e; s++)
							add_did(s);
					}
				} /* if */
			} /* for */
		}	break;

		case 'L':
			lang = optarg;
			break;

		case 'T':
			print_types (lang);
			return;

		default:
			print_usage ();
			return;
		}
	}

#undef add_did

	if (didnum == 0) {
		uvls_logf ("error: no dictionaries, see --dictionary option\n");
		return;
	}

	/* set up options */
	www_init (&wo);

	while (optind < argc) {
        for (int i = 0; i < didnum; i++) {
            char *dn, *wfmt;
            int id = did[i];

            if (SQLITE_OK == get_did_info (id, &dn, &wfmt)) {
		        www_term (argv[optind], id, dn, wfmt);
                free (dn);
                free (wfmt);
            }
            else
                vlog(VLOG_ERROR, "did = %d: database error", id);
        }

        optind++;
    }

    free (did);
}


static void
print_usage (void)
{
	uvls_puts ("Usage:\n"
		"academic-cli: -d ID1 [-d ID2...] [options] word1 word2 ... wordN\n"
		"Options:");
#define p(o, d) uvls_printf ("  %-28s %s\n", (o), (d))
	p ("-C CONNECT-TIMEOUT", "A connection timeout in seconds.");
	p ("--dictionary ID, -d ID", "Use this dictionary ID.");
	p ("-D RANGE", "Use range of dictionary IDs, e.g. \"1-5,7,12\".");
	p ("--help, -h, -?", "Display this information.");
	p ("--list TYPE, -l TYPE", "Display dictionary IDs by type. See --list-types.");
	p ("--language LANG, -L LANG", "Choose output language. Default \"en\".");
	p ("--list-types, -T", "Display dictionary types.");
	p ("--retries NUM, -r NUM",
		"How many times to retry establish a connection.");
	p ("--retry-timeout MS, -t MS",
		"Delay between connection retries in milliseconds.");
    p ("--quite, -q", "Be quite: print out valueable information only.");
	p ("--version, -v", "Display version information.");
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
