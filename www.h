#ifndef WWW_H__
#define WWW_H__

#include "html.h"
#include "json.h"
#include <time.h>
#include <uv.h>

#define WWW_TERM_LIMIT 20
#define WWW_CONNECT_TIMEOUT 10L
#define WWW_RETRIES 0
#define WWW_RETRY_SLEEP 1000

typedef struct word_result_s
{
    char* term;
    html_data_t* data;
} word_result_t;

typedef struct www_data_s
{
    uv_async_t* async;
    const char* word;
    int did;
    int wid;
    struct
    {
        char* term;
        char* word;
    } url;
    struct
    {
        term_result_t* term;
        word_result_t* word;
    } result;
} www_data_t;

typedef void (*www_user_f)(www_data_t* data);

typedef struct www_options_s
{
    struct
    {
        unsigned int retries;
        struct timespec retry_sleep;
        long connect_timeout;
    } curl;
    struct
    {
        int limit;
        www_user_f cb;
    } term;
    struct
    {
        www_user_f cb;
    } word;
} www_options_t;

extern void
www_init(www_options_t* options);

extern void
www_term(const char* word, int did, const char* dname, const char* wfmt);

extern void
www_word_data(www_data_t* data);

extern void
free_www_data(www_data_t* data);

extern void
free_term_result(term_result_t* r);

extern void
free_word_result(word_result_t* r);

#endif /* WWW_H__ */
