/* RESTGAME
** Restore game
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"
#include	"object.h"
#include	"script.h"
#include	"logic.h"


STRPTR	RestoreGame();
BOOL		ReadSave();


#define	SAVENAME_LEN	30

extern	BOOL		ChangeDisk();

extern	TEXT		simpleName[] ;		/* master definition in savegame.c */
#define	simpleSave simpleName[0]

extern	TEXT		saveDescript[];
extern	TEXT		saveFile[];
extern	TEXT		contCancelMsg[];
extern	UBYTE	escapeChar;
extern	BOOL		gamePaused;

extern	ANIOBJ	*aniObj;
extern	OBJECT	*object;
extern	COUNT	aniLen, objLen;
extern	SCANOFS	scanOfs[];

extern	SCRIPT	*script;
extern	COUNT	scriptSize, scriptEntries;

extern	UBYTE	saveStart, saveEnd;





STRPTR
RestoreGame(lp)
STRPTR	lp;
{
	COUNT		gameNum;
	TEXT			msg[200];
	FILE_HANDLE	fd;
	STRPTR		retVal;
	UBYTE		savedEscape;

	/* Don't count time spent in restore game against the user
	*/
	gamePaused = TRUE;
	retVal = lp;
	savedEscape = escapeChar;
	escapeChar = '@';

	if ((gameNum = GetGame('r')) == 0)
		goto	RestoreExit;

	if  (!simpleSave)	{
		sprintf(
			msg,
			"About to restore the game\ndescribed as:\n\n%s\n\nfrom file:\n%s\n\n%s",
			saveDescript, saveFile, contCancelMsg
			);
		WindowNoWait(msg, 0, 35, FALSE);
		if (WaitAcAb() == ABORT)
			goto RestoreExit;
		}
#ifdef P_NO
	else	{	/* simpleSave */
		if (ChangeDisk('r') == ABORT) {
			goto	RestoreExit;
			}
		}
#endif /* P_NO */
	if ((fd = open(saveFile, 0)) == NO_HANDLE) {
		sprintf(msg, "Can't open file:\n%s", saveFile);
		WindowPrint(msg);
		goto RestoreExit;
		}

	/* Seek past the description
	*/
	lseek(fd, (LONG) (SAVENAME_LEN + 1), 0);

	/* Read in the pieces
	*/
	if (!ReadSave(fd, &saveStart)
		|| !ReadSave(fd, aniObj)
		|| !ReadSave(fd, object)
		|| !ReadSave(fd, script)
		|| !ReadSave(fd, scanOfs))
			{
			close(fd);
			WindowPrint("Error in restoring game.\nPress ENTER to quit.");
			Quit();
			}

	close(fd);

	Restore();
	ClearControl();
	Set(RESTORE);
	CloseAllVols();
	retVal = NULL;
	EnableAllMenus();

RestoreExit:
	CloseWindow();
	escapeChar = savedEscape;
	gamePaused = FALSE;
	return (retVal);
}	/* RestoreGame */



BOOL
ReadSave(fd, addr)
FILE_HANDLE	fd;
STRPTR		addr;
{
	COUNT	theLength;
	UBYTE	c;

	/* Read in the length of the piece
	*/
	if (read(fd, &c, 1) != 1)
		return (FALSE);
	theLength = c;
	if (read(fd, &c, 1) != 1)
		return (FALSE);
	theLength += c * 256;

	/* Read in the piece itself
	*/
	return (read(fd, addr, theLength) == theLength);
}

