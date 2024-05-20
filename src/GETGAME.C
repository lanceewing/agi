/* GETGAME
** Get a save game number.
**
**	compile: MWC
**
** CHANGE HISTORY:
**	87.04.17 10:40		JAS
**		GetGame() now only writes the cursor if it was written when
**		we were called.
**	87 aug 30 pmk - (optional) simple save/restore
*/


#include	"types.h"
#include	"game.h"
#include	"event.h"
#include	"textwin.h"
#include	"screen.h"

extern	TEXT simpleName[] ;		/* master definition in savegame.c */
#define simpleSave simpleName[0]
static	TEXT simpleSelect[] = 
"   Sorry, this disk is full.\n\
Position pointer and press ENTER\n\
    to overwrite a saved game\n\
or press ESC and try again \n\
    with another disk\n" ;
static	BOOL FirstTime = TRUE;
static	TEXT simpleFirst[] =
"Use the arrow keys to move\n\
     the pointer to your name.\n\
Then press ENTER\n";

#define	SAVENAME_LEN	30
typedef	struct	gameStrc {
	COUNT	num;
	TEXT		descript[SAVENAME_LEN + 1];
	}	GAME;

COUNT	GetGame();
BOOL		GetPath();
COUNT	GetGameNumber();
BOOL		GetDescription();
BOOL		GetWindowStr();
void		WritePointer();
void		ErasePointer();
BOOL		ChangeDisk();


#define	NUM_GAMES		12
#define	SAVENAME_LEN	30
#define	GAME_INDENT	3

#define	POINTER_CHAR	26		/* right arrow character */
#define	ERASE_CHAR	32		/* space character */



extern	TEXT			gameID[];
extern	TEXT			exampleSaveName[];
extern	TEXT_WINDOW	textWin;
extern	BOOL			twoDrive;
extern	UBYTE		saveDrive;
extern	WORD			monType;


TEXT		savePath[SAVENAME_LEN + 1] = "";
TEXT		saveDescript[SAVENAME_LEN + 1];
TEXT		saveFile[64];

TEXT		saveMsg[] =
"         SAVE GAME\n\
\n\
On which disk or in which directory do you wish to save this game?\n\
\n\
%s\n\
\n";

TEXT		restMsg[] =
"        RESTORE GAME\n\
\n\
On which disk or in which directory is the game that you want to restore?\n\
\n\
%s\n\
\n";

TEXT		swapDiskMsg[] =
"Please put your save game\n\
disk in drive %c.\n\
Press ENTER to continue.\n\
Press ESC to not %s a game.";

TEXT		selectSave[] =
"Use the arrow keys to select the slot in which you wish to save the game.\
 Press ENTER to save in the slot, ESC to not save a game.";

TEXT		selectRestore[] =
"Use the arrow keys to select the game which you wish to restore. Press ENTER\
 to restore the game, ESC to not restore a game.";

TEXT		newDescript[] =
"How would you like to describe this saved game?\n\
\n";

TEXT		noGamesMsg[] =
"There are no games to\n\
restore in\n\
\n\
%s\n\
\n\
Press ENTER to continue.";


TEXT		badPathMsg[] =
"There is no directory named\n\
%s.\n\
Press ENTER to try again.\n\
Press ESC to cancel.";


extern	EVENT	*WaitEvent();
extern	TIME_VAL	FileDT();
extern	BOOL		CursorState();





COUNT
GetGame(function)
UBYTE	function;
{
	COUNT	gameNum;
	BOOL		cursorOn;

	cursorOn = CursorState();
	EraseCur();
	SaveTextAtr();
	SaveCurPos();
	StopSnd();

	_SetTextAtr(0, 15);
	if (!GetPath(function)) {
		gameNum = 0;
		goto GetExit;
		}

	/* If the user requests floppy drive 'B:' but there is only one floppy,
	** change the drive letter to 'A:'.
	*/
	if (saveDrive == 'b' && !twoDrive)
		saveDrive = savePath[0] = 'a';

	/* If user wants to save on the current drive and that drive is a
	** floppy, close all our files and prompt for a disk change.
	*/
	if (ChangeDisk(function) == ABORT) {
			gameNum = 0;
			goto	GetExit;
			}

	gameNum = GetGameNumber(function);
	if (gameNum != 0) {
		if (function == 's' && !simpleSave) {
			if (GetWindowStr(newDescript, saveDescript) == ABORT) {
				gameNum = 0;
				goto GetExit;
				}
			}
		MakeSaveFileName(saveFile, gameNum);
		}

GetExit:
	RestoreCurPos();
	RestoreTextAtr();
	if (cursorOn)
		WriteCur();

	return (gameNum);
}	/* GetGame */


BOOL
ChangeDisk(function)
BYTE function ;
{
	TEXT		msg[160];
	if (GetCurDrive() == saveDrive && saveDrive <= 'b') {
		CloseAllVols();
		sprintf(msg, swapDiskMsg, saveDrive,
				(function == 'r')? "restore" : "save");
		return (WindowPrint(msg)) ;
		}
	else
		return (ACCEPT);
}	/* ChangeDisk */


BOOL
GetPath(function)
UBYTE	function;
{
	TEXT		msg[160];

	if (savePath[0] == '\0')
		GetCurDir(savePath);

	if (simpleSave)
		return (ACCEPT);

	forever {
		sprintf(msg, (function == 's')? saveMsg : restMsg, exampleSaveName);
		if (GetWindowStr(msg, savePath) == ABORT)
			return (ABORT);

		if (ValidPath(savePath))
			return (ACCEPT);

		sprintf(msg, badPathMsg, savePath);
		if (WindowPrint(msg) == ABORT)
			return (ABORT);
		}
}




BOOL
GetWindowStr(msg, str)
STRPTR	msg, str;
/* 
*/
{
	UWORD	retVal;
	UWORD	inputRow;

	WindowNoWait(msg, 0, SAVENAME_LEN+1, TRUE);
	inputRow = textWin.bottom;
	SetCur(inputRow, textWin.left);
	ClearRect(inputRow, textWin.left, inputRow, textWin.right - 1, 0);
	SaveTextAtr();
	_SetTextAtr(15, 0);
	retVal = GetLine(str, SAVENAME_LEN + 1);
	RestoreTextAtr();
	CloseWindow();

	return (retVal == '\r');
}





COUNT
GetGameNumber(function)
UBYTE		function;
{
	register	EVENT	*event;
			GAME		game[NUM_GAMES];
			TEXT		msg[100];
			COUNT	gameNum, numGames, mostRecentGame;
			COUNT	height, descriptTop;
			TIME_VAL	mostRecentTime, fileTime;

	mostRecentTime = 0;
	mostRecentGame = 0;

	/* Look for the game files and get their descriptions.
	*/
	if (function == 's') {
		for (gameNum = 0 ; gameNum < NUM_GAMES ; ++gameNum) {
			if (GetDescription(gameNum + 1, &game[gameNum], &fileTime)
				&& fileTime > mostRecentTime)
					{
						mostRecentTime = fileTime;
						mostRecentGame = gameNum;
					}
			}
		numGames = NUM_GAMES;
		}
	else {
		for (gameNum = numGames = 0 ; gameNum < NUM_GAMES ; ++gameNum)
			if (GetDescription(gameNum + 1, &game[numGames], &fileTime)) {
				if (fileTime > mostRecentTime) {
					mostRecentTime = fileTime;
					mostRecentGame = numGames;
					}
				++numGames;
				}
		}

	/* If there are no games to display, tell the user so.
	*/
	if (numGames == 0 && !simpleSave) {
		sprintf(msg, noGamesMsg, savePath);
		WindowPrint(msg);
		return (0);
		}

	if (simpleSave && !FirstTime)	{
		strcpy(saveDescript, simpleName) ;
		for (gameNum = 0 ; gameNum < NUM_GAMES ; ++gameNum)
			if (0 == strcmp(simpleName, game[gameNum].descript))
				return (game[gameNum].num) ;
		if (function == 's')	/* find empty slot for new save */
			for (gameNum = 0 ; gameNum < NUM_GAMES ; ++gameNum)
				if (0 == strcmp("", game[gameNum].descript))
					return (game[gameNum].num);
				/* if none available, fall thru to window */
		if (function == 'r')	/* can't happen, but . . . */
			return (0);
		}
	if (simpleSave && FirstTime)
		saveDrive = GetCurDrive();
	
	/* Compute the height of the window desired and put it up
	*/
	descriptTop = 5;
	height = numGames + descriptTop;
	WindowNoWait(simpleSave? (FirstTime? simpleFirst : simpleSelect) :
				(function == 's')? selectSave : selectRestore,
				height, SAVENAME_LEN + GAME_INDENT + 1, TRUE);
	descriptTop += textWin.top;
	FirstTime = FALSE;

	/* Print the game descriptions.
	*/
	for (gameNum = 0 ; gameNum < numGames ; ++gameNum) {
		SetCur(descriptTop + gameNum, textWin.left);
		printf(" - %s", game[gameNum].descript);
		}

	/* Put up the pointer and then let the user start scrolling
	** around with it.
	*/
	gameNum = mostRecentGame;
	WritePointer(descriptTop + gameNum);
	forever {
		event = WaitEvent();
		ButtonToChar(event);
		switch (event->type) {
			case CHAR:
				switch (event->val) {
					case '\r':
						if (!simpleSave)
							strcpy(saveDescript, 
								game[gameNum].descript);
						else	if (function == 'r')
							strcpy(simpleName,
								game[gameNum].descript);
						CloseWindow();
						return (game[gameNum].num);
					case 0x1b:
						CloseWindow();
						return (0);
					}
				break;

			case DIR:
				switch (event->val) {
					case 1:			/* move up */
						ErasePointer(descriptTop + gameNum);
						gameNum = (gameNum == 0)?
									numGames - 1 : gameNum - 1;
						WritePointer(descriptTop + gameNum);
						break;

					case 5:			/* move down */
						ErasePointer(descriptTop + gameNum);
						gameNum = (gameNum == numGames - 1)?
									0 : gameNum + 1;
						WritePointer(descriptTop + gameNum);
						break;
					}
			}
		}
}




BOOL
GetDescription(num, theGame, fileTime)
COUNT	num;
GAME		*theGame;
TIME_VAL	*fileTime;
{
	TEXT			fileName[64];
	TEXT			idCheck[ID_LEN];
	FILE_HANDLE	fd;

	theGame->num = num;
	MakeSaveFileName(fileName, num);
	if ((fd = open(fileName, 0)) == NO_HANDLE) {
		theGame->descript[0] = '\0';
		return (FALSE);
		}
	else {
		/* Get the file's date and time
		*/
		*fileTime = FileDT(fd);

		/* Get the description and the game ID
		*/
		read(fd, theGame->descript, SAVENAME_LEN + 1);
		lseek(fd, 2L, 1);			/* point past the length of savevars */
		read(fd, idCheck, ID_LEN);
		close(fd);

		/* If the saved gameID doesn't match the current, don't use
		** this game.
		*/
		if (strcmp(idCheck, gameID) != 0) {
			theGame->descript[0] = '\0';
			return (FALSE);
			}

		return (TRUE);
		}
}




void
WritePointer(posn)
COUNT	posn;
{
	SetCur(posn, textWin.left);
	putchar(POINTER_CHAR);
}



void
ErasePointer(posn)
COUNT	posn;
{
	/* Overwrite the pointer character with a space character
	*/
	SetCur(posn, textWin.left);
	putchar(ERASE_CHAR);
}

