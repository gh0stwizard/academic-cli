#ifndef VLOG_H__
#define VLOG_H__

#include "uvls.h"

typedef enum {
	VLOG_NONE		= 0,
	VLOG_FATAL		= 1,
	VLOG_ALERT		= 2,
	VLOG_CRITICAL	= 3,
	VLOG_CRIT		= 3,
	VLOG_ERROR		= 4,
	VLOG_ERR		= 4,
	VLOG_WARNING	= 5,
	VLOG_WARN		= 5,
	VLOG_NOTICE		= 6,
	VLOG_NOTE		= 6,
	VLOG_INFO		= 7,
	VLOG_DEBUG		= 8,
	VLOG_TRACE		= 9
} vlog_level;

#ifndef VLOG_STR_FATAL
#define VLOG_STR_FATAL	"fatal"
#endif
#ifndef VLOG_STR_ALERT
#define VLOG_STR_ALERT	"alert"
#endif
#ifndef VLOG_STR_CRIT
#define VLOG_STR_CRIT	"crit "
#endif
#ifndef VLOG_STR_ERR
#define VLOG_STR_ERR	"error"
#endif
#ifndef VLOG_STR_WARN
#define VLOG_STR_WARN	"warn "
#endif
#ifndef VLOG_STR_NOTE
#define VLOG_STR_NOTE	"note "
#endif
#ifndef VLOG_STR_INFO
#define VLOG_STR_INFO	"info "
#endif
#ifndef VLOG_STR_DEBUG
#define VLOG_STR_DEBUG	"debug"
#endif
#ifndef VLOG_STR_TRACE
#define VLOG_STR_TRACE	"trace"
#endif
#ifndef VLOG_STR_NONE
#define VLOG_STR_NONE	""
#endif

#ifdef _DEBUG
#define vlog(level, ...) vdebug(__FILE__,__func__,__LINE__,(level),__VA_ARGS__)
#define vsay(level, ...) vdebug(NULL, NULL, 0, (level), __VA_ARGS__)
#else /* -U_DEBUG */
#define vlog(level, ...) 
#define vsay(level, ...) 
#endif /* _DEBUG */

extern size_t
vdebug (const char *file,
		const char *func,
		unsigned int line,
		vlog_level level,
		const char *fmt, ...);

#endif /* DLOGGER_H__ */
