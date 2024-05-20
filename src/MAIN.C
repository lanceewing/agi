/* MAIN
** Main module for AGI
**
**	compile: MWC
**
** Change History:
**	17/02/87  15:43:11	JAS
**		Added a call to GetObjDirs() before the logic scan in main().
**		This determines the object directions before the logics are
**		entered, giving them a chance to muck with them before animation.
*/


#include	"types.h"
#include	"setjmp.h"
#include	"game.h"
#include	"aniobj.h"
#include	"logic.h"



void		main();
STRPTR	Pause();
STRPTR	QuitGame();
void		Quit();


WORD		previousScore;
BOOL		gamePaused;
jmp_buf	rescanJmpBuf;

TEXT		quitMsg[] =
"Press ENTER to quit.\n\
Press ESC to keep playing.";

extern	TEXT		pauseMsg[];
extern	UBYTE	var[];
extern	ANIOBJ	*aniObj;
extern	LOGNODE	*curLog;

extern	SCR_MODE	screenMode;

extern	BOOL		userCtrl;

extern	STRPTR	_CallLogic();





void
main()
{
	BOOL		soundStatus;

	/* Initialize game, load necessary segments
	*/
	Init();

	/* Main loop
	*/
	forever {
		/* Delay until it's time to animate again, then reset the
		** animation clock;
		*/
		WaitToAnimate();

		/* Get user input
		*/
		ClearControl();				/* clear the controllers */
		Reset(INPUT);
		Reset(HADMATCH);
		PollJoy();					/* poll joystick, if present */
		UserIn();						/* handle user input */

		/* If ego is under program control, override user input
		 * as to his direction.
		 */
		if (!userCtrl)
			var[EGODIR] = ego->dir;
		else
			ego->dir = var[EGODIR];

		/* Compute the direction in which objects will move.
		*/
		GetObjDirs();

		/* Scan the room 0 logics while they return NULL (signifying
		** that a rescan is needed)
		*/
		previousScore = var[SCORE];
		soundStatus = IsSet(SOUNDON);
		setjmp(rescanJmpBuf);
		while (_CallLogic(0) == NULL) {
			var[OBJHIT] = var[OBJEDGE] = var[UNKNOWN_WORD] = 0;
			Reset(INPUT);
			previousScore = var[SCORE];
			}

		/* Set ego's direction from the variable.
		*/
		ego->dir = var[EGODIR];

		/* Update the status line if the score or sound status have changed.
		*/
		if (var[SCORE] != previousScore || soundStatus != IsSet(SOUNDON))
			DisplayStatusLine();

		var[OBJHIT] = var[OBJEDGE] = 0;

		/* Clear the restart, restore, & init logics flags
		*/
		Reset(INITLOGS);
		Reset(RESTART);
		Reset(RESTORE);

		/* Do the animation
		*/
		if (screenMode == GRAPHMODE)
			Animate();
		}
}




STRPTR
Pause(lp)
STRPTR	lp;
{
	gamePaused = TRUE;
	ResetEv();
	StopSnd();
	WindowPrint(pauseMsg);
	gamePaused = FALSE;
	return (lp);
}




STRPTR
QuitGame(lp)
register	STRPTR	lp;
/* End kq
*/
{
	/* Stop any sounds which are playing
	*/
	StopSnd();

	if (*lp++ == 1 || WindowPrint(quitMsg) == ACCEPT)
		Quit();

	return (lp);
}



void
Quit()
{
	ResetMachine();
	exit(0);
}

