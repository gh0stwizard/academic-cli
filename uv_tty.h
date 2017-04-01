#ifndef UV_TTY_H__
#define UV_TTY_H__

#include <uv.h>
#include <stdbool.h>


extern uv_tty_t tty_out, tty_err;
extern bool colorized;


extern void
init_tty (void);

extern void
fini_tty (void);

#endif /* TTY_H__ */
