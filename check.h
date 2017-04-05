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

#ifndef CHECK_H__
#define CHECK_H__

#include <assert.h>

#define CURL_CHECK(x)	do { assert ((x) == CURLE_OK); } while (0)
#define CURLM_CHECK(x)	do { assert ((x) == CURLM_OK); } while (0)
#define NULL_CHECK(x)	do { assert ((x) != NULL); } while (0)
#define UV_CHECK(x)	do { assert ((x) == 0); } while (0)
#define MYHTML_CHECK(x)	do { assert ((x) == MyHTML_STATUS_OK); } while (0)

#endif /* CHECK_H__ */
