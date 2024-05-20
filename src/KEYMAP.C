/* KEYMAP
** The mapping of keys to codes used internally by the interpreter.
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"
#include	"event.h"
#include	"keydefs.h"



KEYMAP	directionKey[] = {
		UPARW, 1,
		PGUP, 2,
		RGHTARW, 3,
		PGDN, 4,
		DNARW, 5,
		END, 6,
		LFTARW, 7,
		HOME, 8,
		0, 0
		};

KEYMAP	tandyDir[] = {
		'8', 1,
		'9', 2,
		'6', 3,
		'3', 4,
		'2', 5,
		'1', 6,
		'4', 7,
		'7', 8,
		'5', 0,
		0, 0
		};

