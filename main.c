#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <uv.h>
#include <curl/curl.h>
#include "vlog.h"

#define CURL_CHECK(x)	do { assert ((x) == CURLE_OK); } while (0)
#define CURLM_CHECK(x)	do { assert ((x) == CURLM_OK); } while (0)

int
curlm_socket_cb (CURL *easy, 
		curl_socket_t s,
		int status,
		void *userp,
		void *socketp);

int
curlm_timer_cb (CURLM *multi, long timeout_ms, void *userp);

void
init_curl (void);

void
fini_curl (void);

void
print_curl_version (void);


/* ------------------------------------------------------------------ */


CURLM *curlm;


/* ------------------------------------------------------------------ */


extern int
main (int argc, char *argv[])
{
	init_curl ();
	print_curl_version ();
	fini_curl ();

	return 0;
}


int
curlm_socket_cb (CURL *easy, 
		curl_socket_t s,
		int status,
		void *userp,
		void *socketp)
{
	return 0;
}


int
curlm_timer_cb (CURLM *multi, long timeout_ms, void *userp)
{
	return 0;
}



void
init_curl (void)
{
	CURL_CHECK(curl_global_init (CURL_GLOBAL_ALL));

	curlm = curl_multi_init ();

	assert (curlm != NULL);

	CURLM_CHECK(curl_multi_setopt 
		(curlm, CURLMOPT_SOCKETFUNCTION, curlm_socket_cb));

	CURLM_CHECK(curl_multi_setopt
		(curlm, CURLMOPT_TIMERFUNCTION, curlm_timer_cb));
}


void
fini_curl (void)
{
	CURLM_CHECK(curl_multi_cleanup (curlm));
	curl_global_cleanup ();
}


void
print_curl_version (void)
{
	curl_version_info_data * nfo = curl_version_info (CURLVERSION_NOW);

	vlog (VLOG_INFO, "Powered by curl version %s", nfo->version);
}
