#ifndef DB_H__
#define DB_H__

#include <sqlite3.h>

extern int
db_open(const char* dbfile, const char* backup);

extern void
db_close(void);

extern int
print_dictionaries(const char* lang, const char* type);

extern int
print_types(const char* lang);

extern int
get_did_info(int did, char** outName, char** outFmt);

#endif /* DB_H__ */
