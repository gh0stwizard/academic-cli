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

typedef struct curl_context_s {
	uv_poll_t poll_handle;
	curl_socket_t sockfd;
} curl_context_t;


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

void
print_uv_version (void);

void
init_uv (void);

void
fini_uv (void);

void
on_timeout_cb (uv_timer_t *handle);

curl_context_t *
create_curl_context (curl_socket_t sockfd);

void
add_download (const char *url);

void
poll_cb (uv_poll_t *handle, int status, int events);

void
check_multi_info (void);

void
destroy_curl_context (curl_context_t *ctx);

void
close_cb (uv_handle_t *handle);


/* ------------------------------------------------------------------ */


CURLM *curlm;
uv_loop_t *loop;
uv_timer_t timeout_timer;




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


int
curlm_socket_cb (CURL *easy, 
		curl_socket_t s,
		int status,
		void *userp,
		void *socketp)
{
	curl_context_t *ctx;
	int poll_events = 0;


	switch (status) {
	case CURL_POLL_NONE:
		/* register, not interested in readiness (yet) */
		vlog (VLOG_DEBUG, "%d: CURL_POLL_NONE", s);
		break;

	case CURL_POLL_IN:
		/* interested in read readiness */
		vlog (VLOG_DEBUG, "%d: CURL_POLL_IN", s);
		poll_events |= UV_READABLE;
		break;

	case CURL_POLL_OUT:
		/* interested in write readiness */
		vlog (VLOG_DEBUG, "%d: CURL_POLL_OUT", s);
		poll_events |= UV_WRITABLE;
		break;

	case CURL_POLL_INOUT:
		/* interested in both read and write readiness */
		vlog (VLOG_DEBUG, "%d: CURL_POLL_INOUT", s);
		poll_events |= UV_READABLE | UV_WRITABLE;
		break;

	case CURL_POLL_REMOVE:
		/* unregister */
		vlog (VLOG_DEBUG, "%d: CURL_POLL_REMOVE", s);
		ctx = (curl_context_t *) socketp;

		if (ctx) {
			UV_CHECK(uv_poll_stop (&ctx->poll_handle));
			destroy_curl_context (ctx);
			CURLM_CHECK(curl_multi_assign (curlm, s, NULL));
		}

		break;
	}


	if (poll_events) {
		ctx = socketp
			? (curl_context_t *) socketp
			: create_curl_context (s);

		CURLM_CHECK(curl_multi_assign (curlm, s, (void *) ctx));
		UV_CHECK(uv_poll_start (&ctx->poll_handle, poll_events, poll_cb));
	}

	return 0;
}


int
curlm_timer_cb (CURLM *multi, long timeout_ms, void *userp)
{
	vlog (VLOG_DEBUG, "%s: timeout_ms = %li", __func__, timeout_ms);

	if (timeout_ms < 0) {
		UV_CHECK(uv_timer_stop (&timeout_timer));
	}
	else {
		if (timeout_ms == 0)
			timeout_ms = 1;
		UV_CHECK(uv_timer_start
			(&timeout_timer, on_timeout_cb, timeout_ms, 0));
	}

	return 0;
}


void
init_curl (void)
{
	CURL_CHECK(curl_global_init (CURL_GLOBAL_ALL));

	NULL_CHECK(curlm = curl_multi_init ());

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


void
print_uv_version (void)
{
	vlog (VLOG_INFO, "Powered by libuv version %s", uv_version_string ());
}

#if 0
void
dumb_cb (uv_timer_t *handle)
{
}

uv_timer_t dumb_timer;
#endif

void
init_uv (void)
{
	NULL_CHECK(loop = uv_default_loop ());
	UV_CHECK(uv_timer_init (loop, &timeout_timer));
#if 0
	UV_CHECK(uv_timer_init (loop, &dumb_timer));
	UV_CHECK(uv_timer_start (&dumb_timer, dumb_cb, 1000, 1000));
#endif
}


void
fini_uv (void)
{
}


void
on_timeout_cb (uv_timer_t *handle)
{
	int running;
	CURLM_CHECK(curl_multi_socket_action
		(curlm, CURL_SOCKET_TIMEOUT, 0, &running));
	vlog (VLOG_DEBUG, "%s: running %d", __func__, running);
}


curl_context_t *
create_curl_context (curl_socket_t sockfd)
{
	curl_context_t *ctx;


	NULL_CHECK(ctx = (curl_context_t *) malloc (sizeof (*ctx)));
	ctx->sockfd = sockfd;

	UV_CHECK(uv_poll_init_socket (loop, &ctx->poll_handle, sockfd));
	ctx->poll_handle.data = ctx;

	return ctx;
}


void
add_download (const char *url)
{
	CURL *handle;


	NULL_CHECK(handle = curl_easy_init ());
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_URL, url));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_VERBOSE, 1L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_LOW_SPEED_TIME, 3L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_LOW_SPEED_LIMIT, 1L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_CONNECTTIMEOUT, 30L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_FOLLOWLOCATION, 1L));
	CURLM_CHECK(curl_multi_add_handle (curlm, handle));
}


void
poll_cb (uv_poll_t *handle, int status, int events)
{
	int curl_flags = 0;
	int running;
	curl_context_t *ctx;


	NULL_CHECK(ctx = (curl_context_t *) handle->data);

	vlog (VLOG_DEBUG, "%s: %d: status = %d events = %d",
		__func__, ctx->sockfd, status, events);

	if (events & UV_READABLE)
		curl_flags |= CURL_CSELECT_IN;

	if (events & UV_WRITABLE)
		curl_flags |= CURL_CSELECT_OUT;

	CURLM_CHECK(curl_multi_socket_action
		(curlm, ctx->sockfd, curl_flags, &running));

	check_multi_info ();
}


void
check_multi_info (void)
{
	CURLMsg *msg;
	int pending;


	while ((msg = curl_multi_info_read (curlm, &pending))) {
		switch (msg->msg) {
		case CURLMSG_DONE: {
			char *url;
			CURL *handle = msg->easy_handle;


			CURL_CHECK(curl_easy_getinfo
				(handle, CURLINFO_EFFECTIVE_URL, &url));
			vlog (VLOG_DEBUG, "DONE: %s", url);
			CURLM_CHECK(curl_multi_remove_handle (curlm, handle));
			curl_easy_cleanup (handle);
		} break;

		default:
			vlog (VLOG_DEBUG, "CURLMSG default");
			break;
		}
	}
}


void
destroy_curl_context (curl_context_t *ctx)
{
	uv_close((uv_handle_t *) &ctx->poll_handle, close_cb);
}


void
close_cb (uv_handle_t *handle)
{
	curl_context_t *ctx = (curl_context_t *) handle->data;
	handle->data = NULL;
	free (ctx);
}
