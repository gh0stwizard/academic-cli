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

#include <stdlib.h>
#include <string.h>
#include "curl_helpers.h"
#include "vlog.h"


extern size_t
curl_mem_write_cb (char *data, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	curl_mem_t *mem = (curl_mem_t *) userp;


	mem->data = realloc (mem->data, mem->size + realsize + 1);

	if (mem->data != NULL) {
		memcpy (&(mem->data[mem->size]), data, realsize);
		mem->size += realsize;
		mem->data[mem->size] = '\0';

		return realsize;
	}

	vlog (VLOG_ERROR, "%s: realloc: out of memory", __func__);

	return 0;
}
