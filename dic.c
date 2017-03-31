#include <stdlib.h>
#include <curl/curl.h>
#include <uv.h>
#include "dic.h"
#include "vlog.h"
#include "check.h"
#include "curl_helpers.h"
#include "html.h"


/* ------------------------------------------------------------------ */


void
dic_cb (uv_work_t *req)
{
	CURL *handle;
	char url[MAX_URL_SIZE];
	curl_mem_t storage;
	dic_t *w;
	dic_result_t *result;


	w = (dic_t *) req->data;

	snprintf (url, MAX_URL_SIZE, academic_durl_fmt[w->did], w->word_id);

	vlog (VLOG_DEBUG, "%s: %d: %s", __func__, w->word_id, url);

	NULL_CHECK(storage.data = malloc (sizeof (char)));

	NULL_CHECK(handle = curl_easy_init ());

	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_URL, url));

#ifdef _DEBUG_CURL
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_VERBOSE, 1L));
#else
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_VERBOSE, 0L));
#endif

	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_LOW_SPEED_TIME, 3L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_LOW_SPEED_LIMIT, 1L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_CONNECTTIMEOUT, 30L));
	CURL_CHECK(curl_easy_setopt (handle, CURLOPT_FOLLOWLOCATION, 1L));
	CURL_CHECK(curl_easy_setopt
		(handle, CURLOPT_WRITEFUNCTION, curl_mem_write_cb));
	CURL_CHECK(curl_easy_setopt
		(handle, CURLOPT_WRITEDATA, (void *) &storage));
	
	CURL_CHECK(curl_easy_perform (handle));

	vlog (VLOG_TRACE, "%s: data size %zu", __func__, storage.size);
	parse_html (storage.data, storage.size);

	NULL_CHECK(result = malloc (sizeof (*result)));
	/* copy query data back that the main thread recogninise it */
	result->word_id = w->word_id;
	result->did = w->did;
	/* write result & send it to main thread */
	uv_rwlock_wrlock (w->lock);
	w->async->data = result;
	uv_rwlock_wrunlock (w->lock);
	/* notify main thread that we have finished */
	uv_async_send (w->async);

	curl_easy_cleanup (handle);
	free (storage.data);
}


void
dic_after_cb (uv_work_t *req, int status)
{
	dic_t *w = (dic_t *) req->data;


	if (status != 0)
		vlog (VLOG_ERROR, "%s: %p: status %d", __func__, req, status);

	if (w->lock != NULL) {
		uv_rwlock_destroy (w->lock);
		free (w->lock);
	}

	req->data = NULL;
	free (w);
	free (req);
}
