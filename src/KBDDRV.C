/* KBDDRV
** Keyboard driver for adventure games
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"
#include	"event.h"


void			FlushKbd();
void			PollKbd();
WORD			IsDir();
void			DirTranslate();


extern	COUNT	machType;
extern	KEYMAP	directionKey[], tandyDir[];



void
FlushKbd()
/* Clear the keyboard buffer
*/
{
	while (GetChar())
		;
}





void
PollKbd()
/* Post keyboard events.
*/
{
	register	WORD		c, value;

	while ((c = GetChar()) != 0) {
		if ((value = IsDir(c)) != -1)
			PostEvent(DIR, value);
		else
			PostEvent(CHAR, c);
		}
}




WORD
IsDir(c)
register	WORD		c;
/* If c is a direction key, return the corresponding direction, else -1.
** This also handles toggling of movement when under keyboard control.
*/
{
	register	KEYMAP	*kp;
			WORD		value;

	value = -1;

	/* See if the key is a direction key
	*/
	for (kp = directionKey ; kp->key != 0 ; ++kp)
		if (c == kp->key) {
			value = kp->keyval;
			break;
			}

	return (value);
}




void
DirTranslate(ePtr)
register	EVENT	*ePtr;
{
	register	KEYMAP	*keyPtr;

	if (machType == TD) {
		for (keyPtr = tandyDir ; keyPtr->key != 0 ; ++keyPtr)
			if (ePtr->val == keyPtr->key) {
				ePtr->type = DIR;
				ePtr->val = keyPtr->keyval;
				break;
				}
		}
}

