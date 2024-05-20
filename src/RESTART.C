/* RESTART
** Restart the game.
**
**	compile: MWC
**
** Change History:
**	87.04.30 10:24	JAS
**		Added NO_PRMPT_RSTRT to allow programmer to do a restart with
**		no user interaction.
*/


#include	"types.h"
#include	"game.h"


extern	TEXT		rstrtMsg[];
extern	ULONG	sysTimer;
extern	COUNT	traceLogs;

extern	BOOL		WindowPrint();



STRPTR
Restart(lp)
STRPTR	lp;
{
	BOOL		doRestart, soundOn;

	StopSnd();
	EraseCur();

	if (IsSet(NO_PRMPT_RSTRT))
		doRestart = TRUE;
	else
		doRestart = WindowPrint(rstrtMsg);

	if (doRestart) {
		CancelLine();
		soundOn = IsSet(SOUNDON);
		FreeAll();
		ResetAll();
		CloseAllVols();
		Set(RESTART);
		if (soundOn)
			Set(SOUNDON);
		sysTimer = 0;
		if (traceLogs != 0)
			LoadLog(traceLogs);
		EnableAllMenus();
		}

	WriteCur();

	return ((doRestart)? NULL : lp);
}

