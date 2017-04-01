#include "uv_tty.h"
#include "loop.h"
#include "vlog.h"
#include "check.h"


/* ------------------------------------------------------------------ */

uv_tty_t tty_out, tty_err;
bool colorized = false;

/* ------------------------------------------------------------------ */


extern void
init_tty (void)
{
	/**
	 * initialisation for stdout:
	 *  0 - stdin, 1 - stdout, 2 - stderr
	 *  readable: stdin -- yes, stdout -- no
	 */
	UV_CHECK(uv_tty_init (loop, &tty_out, 1, 0));
	UV_CHECK(uv_tty_set_mode (&tty_out, UV_TTY_MODE_NORMAL));

	UV_CHECK(uv_tty_init (loop, &tty_out, 2, 0));
	UV_CHECK(uv_tty_set_mode (&tty_err, UV_TTY_MODE_NORMAL));

	if (uv_guess_handle (1) == UV_TTY)
		colorized = true;
}

extern void
fini_tty (void)
{
	uv_tty_reset_mode ();
}
