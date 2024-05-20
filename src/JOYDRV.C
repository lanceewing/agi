/* JOYDRV
** Joystick driver routines
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"
#include	"event.h"


STRPTR	InitJoy();
void		PollJoy();
void		PollButtons();
void		FlushButtons();
WORD		GetDir();



#define	JOYDELAY			9


typedef	struct	butstrc {
	BOOL		pressed;
	WORD		state;
	WORD		click;
	WORD		dblclk;
	LONG		timer;
	}	BUTTON;

BOOL		onStatus;
LONG		joyTimer;
WORD		jx, jy, jPrevDir;
BUTTON	button[2]	= { 	0, 0, B0_CLICK, B0_DBLCLICK, 0L,
					0, 0, B1_CLICK, B1_DBLCLICK, 0L };

TEXT		pcJoyMsg[] =
"Please center your joystick.\n\
\n\
Press ENTER when it is\n\
centered.\n\
\n\
Press ESC if you do not wish\n\
to use the joystick.";


/*
TEXT		tdJoyMsg[] =
"If you have a joystick,\n\
please center it and then\n\
\n\
Press ENTER when it is\n\
centered.\n\
Press ESC if you do not wish\n\
to use the joystick.";
*/

extern	WORD		machType, keyDir;
extern	LONG		sysTimer;
extern	UBYTE	var[];

static	BOOL		joyHere;
static	WORD		jxMax, jyMax, jxMin, jyMin;




STRPTR
InitJoy(lp)
STRPTR	lp;
{
	register	WORD		dx, dy;

	jPrevDir = joyHere = -1;

/*
	if (machType == TD)
		WindowNoWait(tdJoyMsg, 0, 0, FALSE);
	else {
*/
		ReadJoy();
		if (jx == 0 || jy == 0)
			joyHere = FALSE;
		else
			WindowNoWait(pcJoyMsg, 0, 0, FALSE);

	while (joyHere == -1) {
		switch (WaitChar()) {
			case '\r':
				joyHere = TRUE;
				break;
			case ESC:
				joyHere = FALSE;
				break;
			}
		}

	CloseWindow();

	if (joyHere) {
		ReadJoy();

	 	dx = jx / 2;
		jxMax = jx + dx;
		jxMin = jx - dx;

		dy = jy / 2;
		jyMax = jy + dy;
		jyMin = jy - dy;

		do						/* wait for button release */
			ReadButtons();
		while (button[0].pressed || button[1].pressed);
		}

	ResetEv();
	return(lp);
}



void
PollJoy()
/* Read the joystick and post the event, if any.
*/
{
	register	WORD		joyDir;

	if	(
		joyHere &&						/* joystick here */
		(!onStatus || joyTimer <= sysTimer) &&	/* time to read the joystick */
		ReadJoy() != -1 					/* no error on joystick read */
		)
			{
			joyDir = GetDir();
			if (joyDir != jPrevDir) {
		   		PostEvent(DIR, joyDir);
				jPrevDir = joyDir;
				}

			joyTimer = sysTimer + JOYDELAY;
			}
}




void
PollButtons()
/* Check the joystick buttons and add an event to the event queue
** if necessary.
*/
{
	register	BUTTON	*bp;

	if (!joyHere)
		return;

	ReadButtons();
	for (bp = button ; bp <= &button[1] ; ++bp) {
		if (bp->state == 2 && bp->timer <= sysTimer) {
			bp->state = 0;
			PostEvent(CHAR, bp->click);
			}
		switch (bp->state) {
			case	0:
				if (bp->pressed)
					++bp->state;
				break;
			case	1:
				if (!bp->pressed) {
					if (IsSet(DBL_CLK)) {
						bp->timer = sysTimer + var[DBL_CLK_DELAY];
						++bp->state;
						}
					else {
						PostEvent(CHAR, bp->click);
						bp->state = 0;
						}
					}
				break;
			case	2:
				if (bp->pressed)
					++bp->state;
				break;
			case	3:
				if (!bp->pressed) {
					PostEvent(CHAR, bp->dblclk);
					bp->state = 0;
					}
				break;
			}
		}
}



void
FlushButtons()
{
	button[0].pressed = button[1].pressed = 0;
	button[0].state = button[1].state = 0;
}



WORD
GetDir()
/* Translate the joystick coordinates into a direction
*/
{
	register	WORD		dir;

	if (jy < jyMin) {
		if (jx < jxMin)
			dir = 8;
		else if (jx > jxMax)
			dir = 2;
		else
			dir = 1;
		}
	else if (jy > jyMax) {
		if (jx < jxMin)
			dir = 6;
	 	else if (jx > jxMax)
			dir = 4;
		else
			dir = 5;
		}
	else {
		if (jx < jxMin)
			dir = 7;
		else if (jx > jxMax)
			dir = 3;
		else
			dir = 0;
		}

	return (dir);
}

