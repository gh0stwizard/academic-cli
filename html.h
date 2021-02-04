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

#ifndef HTML_H__
#define HTML_H__

#include <myhtml/api.h>

typedef struct html_tag_s
{
    int tag_id;
    char* href;
} html_tag_t;

typedef struct html_el_s
{
    html_tag_t* tags;
    size_t tag_count;
    size_t start;
    size_t end;
} html_el_t;

typedef struct html_data_s
{
    char* text;
    size_t size;
    size_t length;
    html_el_t* els;
    size_t el_count;
} html_data_t;

extern myhtml_t* myhtml;
extern myhtml_tree_t* myhtml_tree;

extern void
init_myhtml(void);

extern void
fini_myhtml(void);

extern char*
parse_html(const char* data, size_t size, html_data_t** out);

extern void
free_html_data(html_data_t* d);

#endif /* HTML_H__ */
