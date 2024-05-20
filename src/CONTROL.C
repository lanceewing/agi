/* CONTROL
** Routines to deal with controllers.
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"
#include	"event.h"

void		ClearControl();
void		HandleControl();
STRPTR	SetControl();



		UBYTE	controller[NUMCONTROL];

extern	KEYMAP	keyMap[];




void
ClearControl()
/* Clear all controllers
*/
{
	FillMem(controller, NUMCONTROL, 0);
}




STRPTR
SetControl(lp)
register	STRPTR	lp;
{
	register	WORD		i;
			WORD		key, keyval;


	/* Get key code to assign to the controller
	*/
	key = *lp++;
	key += (*lp++) * 256;
	keyval = *lp++;

	/* Find an empty keymap slot, then add key mapping to it.
	*/
	for (i = 0 ; i < KEYMAPSIZE-1 ; ++i)
		if (keyMap[i].key == 0) {
			keyMap[i].key = key;
			keyMap[i].keyval = keyval;
			break;
			}

	return (lp);
}

