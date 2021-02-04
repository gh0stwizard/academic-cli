#include "db.h"
#include "check.h"
#include "vlog.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef isspace
#define isspace(x)                                                             \
    ((x) == '\f' || (x) == '\n' || (x) == '\r' || (x) == '\t' || (x) == '\v')
#endif

#define MAX_SQL_SIZE 1024
#define MAX_URL_SIZE 2048

/* ------------------------------------------------------------------ */

static int
count_cb(void* userp, int argc, char** argv, char** cols);

static int
db_is_empty(sqlite3* pDB);

static int
db_import(sqlite3* pDB, FILE* stream);

static int
db_init(sqlite3* pDB, const char* file);

static int
print_dictionaries_cb(void* userp, int argc, char** argv, char** cols);

static int
print_types_cb(void* userp, int argc, char** argv, char** azColName);

/* ------------------------------------------------------------------ */

static sqlite3* db;

/* ------------------------------------------------------------------ */

extern void
db_close(void)
{
    if (db != NULL)
        sqlite3_close(db);
}

extern int
db_open(const char* dbfile, const char* backup)
{
    int rc;

    rc = sqlite3_open(dbfile, &db);

    if (rc != SQLITE_OK) {
        vlog(VLOG_ERROR, "sqlite3_open: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return rc;
    }

    if (db_is_empty(db) == 0) {
        vlog(VLOG_DEBUG, "database is empty...");
        rc = db_init(db, backup);
    }

    return rc;
}

static int
db_is_empty(sqlite3* pDB)
{
    int rc;
    char* msg = NULL;
    int entries = 0;
    char sql[] = "SELECT * FROM sqlite_master WHERE type = 'table'";

    rc = sqlite3_exec(pDB, sql, count_cb, (void*)&entries, &msg);

    if (rc != SQLITE_OK) {
        vlog(VLOG_ERROR, msg);
        sqlite3_free(msg);
    }

    return entries;
}

static int
count_cb(void* userp, int argc, char** argv, char** azColName)
{
    int* entries = (int*)userp;
    (void)argc;
    (void)argv;
    (void)azColName;

    (*entries)++;

    return SQLITE_OK;
}

static int
db_init(sqlite3* pDB, const char* file)
{
    int rc = SQLITE_OK;
    FILE* stream;

    if (file == NULL)
        return rc;

    stream = fopen(file, "r");

    if (stream != NULL) {
        rc = db_import(pDB, stream);
        fclose(stream);
    } else {
        vlog(VLOG_ERROR, "%s: %s", file, strerror(errno));
        rc = SQLITE_CANTOPEN;
    }

    return rc;
}

/**
 * import data from a file into the database
 */
static int
db_import(sqlite3* pDB, FILE* stream)
{
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    char* msg = NULL;
    ssize_t size = 1024;
    char* sql;
    int pos = 0;
    char* e;
    int rc = SQLITE_OK;

    sql = malloc(sizeof(char) * size);

    do {
        read = getline(&line, &len, stream);

        if (read == -1)
            break;

        /* skip comments */
        for (e = line; isspace(*e); e++)
            ;
        if (*e == '-' && *(e + 1) == '-')
            continue;

        for (e = line + read - 1; e != line && isspace(*e); e--)
            ;

        if (isspace(*e))
            continue;
        else if (*e != ';') {
            /* join until ';' */
            if (read > size) {
                sql = realloc(sql, read);
                size = read;
            }

            memcpy(sql + pos, line, read);
            pos += read;
            size += read;
        } else if (pos > 0) {
            /* joined valid SQL string */
            if (read > size) {
                sql = realloc(sql, read);
                size = read;
            }

            memcpy(sql + pos, line, read);
            sql[pos + read] = '\0';
            rc = sqlite3_exec(pDB, sql, NULL, NULL, &msg);
            pos = 0;
        } else
            rc = sqlite3_exec(pDB, line, NULL, NULL, &msg);

        if (rc != SQLITE_OK) {
            vlog(VLOG_ERROR, msg);
            sqlite3_free(msg);
            break;
        }
    } while (read != -1);

    if (ferror(stream) != 0 && feof(stream) != 0) {
        vlog(VLOG_ERROR, strerror(errno));
        rc = SQLITE_IOERR_READ;
    }

    free(sql);
    free(line);
    return rc;
}

#define FMT "%4s %8s %s\n"
extern int
print_dictionaries(const char* lang, const char* type)
{
    int rc;
    char* msg = NULL;
    char sql[MAX_SQL_SIZE];

#define Q                                                                      \
    "SELECT id, type, name FROM v_dictionary_names "                           \
    "WHERE lang = '%s' AND type LIKE '%s' "                                    \
    "ORDER by id;"

    uvls_printf(FMT, "ID", "TYPE", "DICTIONARY");
    snprintf(sql, MAX_SQL_SIZE, Q, lang, type);
    rc = sqlite3_exec(db, sql, print_dictionaries_cb, NULL, &msg);

    if (rc != SQLITE_OK) {
        vlog(VLOG_ERROR, msg);
        sqlite3_free(msg);
    }

#undef Q

    return rc;
}

static int
print_dictionaries_cb(void* userp, int argc, char** argv, char** cols)
{
    (void)userp;
    (void)argc;
    (void)cols;

    uvls_printf(FMT, argv[0], argv[1], argv[2]);

    return SQLITE_OK;
}
#undef FMT

#define FMT "%8s %s\n"
extern int
print_types(const char* lang)
{
    int rc;
    char* msg = NULL;
    char sql[MAX_SQL_SIZE];

#define Q                                                                      \
    "SELECT type, [desc] FROM v_dictionary_types "                             \
    "WHERE lang = '%s';"

    uvls_printf(FMT, "TYPE", "DESCRIPTION");
    snprintf(sql, MAX_SQL_SIZE, Q, lang);
    rc = sqlite3_exec(db, sql, print_types_cb, NULL, &msg);

    if (rc != SQLITE_OK) {
        vlog(VLOG_ERROR, msg);
        sqlite3_free(msg);
    }

#undef Q

    return rc;
}

static int
print_types_cb(void* userp, int argc, char** argv, char** cols)
{
    (void)userp;
    (void)argc;
    (void)cols;

    uvls_printf(FMT, argv[0], argv[1]);

    return SQLITE_OK;
}
#undef FMT

extern int
get_did_info(int did, char** outName, char** outFmt)
{
    int rc;
    char sql[] = "SELECT name, fmt FROM dictionaries WHERE id = ?";
    char* name = NULL;
    char* fmt = NULL;
    sqlite3_stmt* ppStmt = NULL;
    const unsigned char *f, *n;
    int len;

    rc = sqlite3_prepare_v2(db, sql, -1, &ppStmt, NULL);
    rc = sqlite3_bind_int(ppStmt, 1, did);
    rc = sqlite3_step(ppStmt);

    if (rc == SQLITE_ROW) {
        rc = SQLITE_OK;

        n = sqlite3_column_text(ppStmt, 0);
        len = sqlite3_column_bytes(ppStmt, 0);
        name = malloc(sizeof(char) * (len + 1));
        memcpy(name, n, len);
        name[len] = '\0';

        f = sqlite3_column_text(ppStmt, 1);
        len = sqlite3_column_bytes(ppStmt, 1);
        fmt = malloc(sizeof(char) * (len + 1));
        memcpy(fmt, f, len);
        fmt[len] = '\0';
    } else if (rc == SQLITE_DONE) {
        vlog(VLOG_TRACE, "did = %d: no records", did);
    } else {
        vlog(VLOG_ERROR, sqlite3_errmsg(db));
    }

    if (outName != NULL)
        *outName = name;

    if (outFmt != NULL)
        *outFmt = fmt;

    (void)sqlite3_finalize(ppStmt);
    return rc;
}

extern int
get_dictionary_name(int did, char** outName)
{
    int rc;
    char sql[] = "SELECT name FROM dictionary_names WHERE did = ?";
    char* name = NULL;
    sqlite3_stmt* ppStmt = NULL;
    const unsigned char *n;
    int len;

    rc = sqlite3_prepare_v2(db, sql, -1, &ppStmt, NULL);
    rc = sqlite3_bind_int(ppStmt, 1, did);
    rc = sqlite3_step(ppStmt);

    if (rc == SQLITE_ROW) {
        rc = SQLITE_OK;
        n = sqlite3_column_text(ppStmt, 0);
        len = sqlite3_column_bytes(ppStmt, 0);
        name = malloc(sizeof(char) * (len + 1));
        memcpy(name, n, len);
        name[len] = '\0';
    } else if (rc == SQLITE_DONE) {
        vlog(VLOG_TRACE, "did = %d: no records", did);
    } else {
        vlog(VLOG_ERROR, sqlite3_errmsg(db));
    }

    if (outName != NULL)
        *outName = name;

    (void)sqlite3_finalize(ppStmt);

    return rc;
}
