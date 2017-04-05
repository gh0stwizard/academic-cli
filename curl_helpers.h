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

#ifndef CURL_HELPERS_H__
#define CURL_HELPERS_H__

#include <curl/curl.h>


#define MAX_URL_SIZE 1024


typedef struct curl_mem_s {
	char *data;
	size_t size;
} curl_mem_t;


extern size_t
curl_mem_write_cb (char *data, size_t size, size_t nmemb, void *userp);


#endif /* CURL_HELPERS_H__ */
