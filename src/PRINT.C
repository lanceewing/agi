/* PRINT
** Message printing routines
**
**	compile: MWC
**
** Change History:
** 20/02/87  10:04:51	JAS
**	If LEAVE_WIN is set, it gets reset after the window is put up.
**
** 17/02/87  15:27:31	JAS
**	Added a check for a new flag 'LEAVE_WIN' in WindowPrint().  If
**	flag is set, leave the window up and return.  Otherwise either
**	wait for a delay or a user response and take the window down.
**	This allows the programmer to leave a window up when changing
**	rooms.  The ShowPic() in the next room writes over the window.
**
**	Made CloseWindow() a routine callable from the logics to put
**	away a window left by 'LEAVE_WIN', if necessary.
*/

#include	"types.h"
#include	"game.h"
#include	"logic.h"
#include	"textwin.h"
#include	"object.h"


STRPTR	LPrintMsg();
STRPTR	FPrintMsg();
STRPTR	PrintMsgAt();
STRPTR	FPrintMsgAt();
STRPTR	PrintAt();
BOOL		WindowPrint();
void		WindowNoWait();
STRPTR	CloseWindow();
STRPTR	Print();
STRPTR	_Print();
STRPTR	GetMsgPtr();
STRPTR	NumFromStr();
STRPTR	Display();
STRPTR	DisplayF();
void		DisplaySetup();
void		DisplayDone();
void		UpdateLineStats();


COUNT		linePos, lineWidth;
int			winWidth = -1, winULRow = -1, winULCol = -1;
STRPTR		lastSpace;
BOOL			windowUp;
BOOL			dialogueOpen = FALSE;
COUNT		numLines, maxLength;
COUNT		windowPos, windowDim;
COUNT		winBot = 20, charHeight = 8;	/* modified in EQUIPCHK.ASM for HGC */
TEXT_WINDOW	textWin;
UBYTE		escapeChar = '\\';			/* the escape character */



#define	WINTOP		1
#define	WINWIDTH		30
#define	VMARGIN		5
#define	HMARGIN		5
#define	CHARWIDTH		4		/* in our coordinates */


extern	LOGNODE	*curLog;
extern	STRPTR	wordPtr[];
extern	UBYTE	userStr[][STRLENGTH];
extern	UBYTE	var[];
extern	COUNT	numWords, winCol;
extern	COUNT	picTop;
extern	LONG		sysTimer;
extern	OBJECT	*object;

extern	LOGNODE	*FindLog();
extern	STRPTR	IToA(), LeadingZeros();
extern	STRPTR	strcpy();





STRPTR
LPrintMsg(lp)
register	STRPTR	lp;
{
	WindowPrint(GetMsgPtr(*lp++));
	return (lp);
}



STRPTR
FPrintMsg(lp)
register	STRPTR	lp;
{
	WindowPrint(GetMsgPtr(var[*lp++]));
	return (lp);
}



STRPTR
PrintMsgAt(lp)
register	STRPTR	lp;
{
	register	COUNT	theMsg;

	theMsg = *lp++;
	return (PrintAt(theMsg, lp));
}



STRPTR
FPrintMsgAt(lp)
register	STRPTR	lp;
{
	register	COUNT	theMsg;

	theMsg = var[*lp++];
	return (PrintAt(theMsg, lp));
}



STRPTR
PrintAt(theMsg, lp)
		COUNT	theMsg;
register	STRPTR	lp;
{
	winULRow = *lp++;
	winULCol = *lp++;
	if ((winWidth = *lp++) == 0)
		winWidth = WINWIDTH;
	WindowPrint(GetMsgPtr(theMsg));
	winWidth = winULRow = winULCol = -1;
	return (lp);
}





BOOL
WindowPrint(str)
STRPTR	str;
/* Print the string 'str' in a window on the screen and wait for ACCEPT
** or ABORT before disposing of it.  Return TRUE for ACCEPT, FALSE for
** ABORT.
*/
{
	register	BOOL		retVal;
			LONG		timeOut;

	/* Display the window
	*/
	WindowNoWait(str, 0, 0, FALSE);

	/* If we're to leave the window up, just return
	*/
	if (IsSet(LEAVE_WIN)) {
		Reset(LEAVE_WIN);
		return (TRUE);
		}

	/* Get the response.
	*/
	if (var[PRINT_TIMEOUT] == 0)
		retVal = (WaitAcAb() == ACCEPT);
	else {
		/* The timeout value is given in half seconds.
		*/
		timeOut = sysTimer + var[PRINT_TIMEOUT] * 10;
		while (sysTimer < timeOut && ChkAcAb() == -1)
			;
		retVal = TRUE;
		var[PRINT_TIMEOUT] = 0;
		}

	/* Close the window
	*/
	CloseWindow(NULL);

	return (retVal);
}




void
WindowNoWait(str, height, width, fixedSize)
STRPTR	str;
COUNT	height, width;
BOOL		fixedSize;
{
	TEXT		printBuf[600];
	TEXT		msg[100];
	UBYTE	c;

	if (windowUp)
		CloseWindow(NULL);

	SaveTextAtr();
	SaveCurPos();
	_SetTextAtr(0, 15);			/* black on white (inverse) */

	if (winWidth == -1 && width == 0)
		width = WINWIDTH;
	else if (winWidth != -1)
		width = winWidth;

PrintStr:
	/* First make a formatting pass through the message, getting maximum
	** line length and number of lines
	*/
	Print(printBuf, str, width);
	if (fixedSize) {
		maxLength = width;
		if (height != 0)
			numLines = height;
		}
	if (numLines > (winBot - WINTOP)) {
		c = str[20];
		str[20] = '\0';
		sprintf(msg, "Message too verbose:\n\n\"%s...\"\n\nPress ESC to continue.", str);
		str[20] = c;
		str = msg;
		goto PrintStr;
		}

	/* Compute window size and position and put them into the appropriate
	** bytes of the words.
	*/
	windowDim = ((numLines * charHeight + 2 * VMARGIN) << 8)
				| (maxLength * CHARWIDTH + 2 * HMARGIN);

	/* Compute the row/column coordinates of the text portion of the window
	*/
	if (winULRow == -1)
		textWin.top = WINTOP + (winBot - WINTOP - numLines)/2;
	else
		textWin.top = winULRow;
	textWin.top += picTop;
	textWin.bottom = textWin.top + numLines - 1;
	if (winULCol == -1)
		textWin.left = (TEXTCOLS - maxLength)/2;
	else
		textWin.left = winULCol;
	winCol = textWin.left;
	textWin.right = textWin.left + maxLength;

	SetCur(textWin.top, textWin.left);
	windowPos = ((textWin.left * CHARWIDTH - HMARGIN) << 8)
				| ((textWin.bottom + 1 - picTop) * charHeight + VMARGIN - 1);

	/* Open the window and print the string (white with a red border)
	*/
	OpenWndw(windowPos, windowDim, 0x40f);
	windowUp = TRUE;
	printf(printBuf);

	winCol = 0;
	RestoreCurPos();
	RestoreTextAtr();
	dialogueOpen = TRUE;
}




STRPTR
CloseWindow(lp)
STRPTR	lp;
{
	if (windowUp)
		CloseWndw(windowPos, windowDim);
	windowUp = dialogueOpen = FALSE;
	return (lp);
}




STRPTR
Print(buffer, str, width)
register	STRPTR	str, buffer;
		COUNT	width;
{
	linePos = 0;
	lineWidth = width;
	lastSpace = NULL;
	numLines = maxLength = 0;

	/* (Recursively) print the string, expanding references to other
	** strings.
	*/
	if (str != NULL) {
		str = _Print(str, buffer);
		*str = '\0';
		UpdateLineStats();
		}

	return (buffer);
}



STRPTR
_Print(sp, lp)
register	STRPTR	sp, lp;
/* Recursive printer.  Adds characters from the string pointed to by sp
** at the position pointed to by lp.  Expands a reference to another string
** by calling itself with pointers to the beginning of the new string and
** the current position.
*/
{
	STRPTR	spStart, mPtr, str;
	COUNT	n, i;
	LOGNODE	*savedLog;

	spStart = sp;

	/* Copy characters to the line until the end of the message or the end
	** of the line.  Special codes are:
	**		%wn		use the nth word parsed
	** 		%sn		use the nth user-defined string
	**		%mn		use message number n
	**		%gn		use global (room 0) message number n
	**		%vn		print numeric value of var
	**		%on		print the name of the object whose number
	**				is in var number n
	*/
	while (*sp != '\0') {
		if (numLines > (winBot - WINTOP))
			return (lp);

		/* Prepare a line for printing
		*/
		while (linePos < lineWidth) {
			if (*sp == escapeChar) {
				++sp;
				*lp = *sp;
				++lp;
				++sp;
				++linePos;
				}
			else	switch (*sp) {

				case '\0':		/* end of string */
					return (lp);

				case '%':			/* a substring of some sort */
					++sp;
					switch (*sp++) {
						case 'w':
							sp = NumFromStr(sp, &n);
							--n;
							if (n >= 0 && n < numWords)
								lp = _Print(wordPtr[n], lp);
							break;
						case 's':
							sp = NumFromStr(sp, &n);
							lp = _Print(userStr[n], lp);
							break;
						case 'm':
							sp = NumFromStr(sp, &n);
							if ((mPtr = GetMsgPtr(n)) != NULL)
								lp = _Print(mPtr, lp);
							break;
						case 'g':
							sp = NumFromStr(sp, &n);
							savedLog = curLog;
							curLog = FindLog(0);
							if ((mPtr = GetMsgPtr(n)) != NULL)
								lp = _Print(mPtr, lp);
							curLog = savedLog;
							break;
						case 'v':
							i = 0;
							sp = NumFromStr(sp, &n);
							str = IToA(var[n]);
							if (*sp == '|') {
								sp = NumFromStr(++sp, &i);
								str = LeadingZeros(str, i);
								}
							lp = _Print(str, lp);
							break;
						case 'o':
							sp = NumFromStr(sp, &n);
							str = ((STRPTR) object) +
									(object + var[n])->nameofs;
							lp = _Print(str, lp);
							break;
						}
					break;

				case '\n':
					*lp = *sp;
					++lp;
					++sp;
					UpdateLineStats();
					break;

				case ' ':
					lastSpace = lp;	/* fall through to default */

				default:				/* just add a character */
					*lp = *sp;
					++lp;
					++sp;
					++linePos;
					break;
				}
			}

		/* If we hit the edge of the window and there was a space
		** in the line, word wrap the line.
		*/
		if (lastSpace == NULL) {
			*lp++ = '\n';
			UpdateLineStats();
			}
		else {
			*lp = '\0';				/* terminate the buffer here */
			linePos -= lp - lastSpace;	/* adjust the line length */
			UpdateLineStats();			/* update the stats */
			*(lp = lastSpace) = '\n';	/* terminate at last space */

			/* Get rid of any whitespace at the beginning of the new
			** line.
			*/
			while (*++lp == ' ')		/* scan off any leading spaces */
				;
			lp = strcpy(lastSpace+1, lp);
			lastSpace = NULL;

			/* Find the end of the buffer
			*/
			while (*lp != 0) {
				++lp;
				++linePos;
				}
			}
		}

	return (lp);
}




STRPTR
GetMsgPtr(m)
COUNT	 m;
/* Return a pointer to message # m in the logics for logic l
*/
{
	register	STRPTR	mp;
	register	STRPTR	op;
			COUNT	offset;

	if (m > curLog->noMsgs)
		return (NULL);		/* message number too large */

	mp = curLog->msg;		/* pointer to message directory */
	op = mp + 2*m;			/* pointer to message offset within directory */

	offset = (UWORD) *op + (UWORD) (256 * *(op + 1));
	if (offset == 0)
		Error(14, m);

	return (mp + offset);
}



STRPTR
Display(lp)
register	STRPTR	lp;
{
	register	COUNT	i;
			TEXT		printBuf[1000];

	SaveCurPos();
	i = *lp++;
	SetCur(i, *lp++);							/* set new position */
	printf(Print(printBuf, GetMsgPtr(*lp++), TEXTCOLS));	/* print the message */
	RestoreCurPos();

	return (lp);
}



STRPTR
DisplayF(lp)
register	STRPTR	lp;
{
	register	COUNT	i;
			TEXT		printBuf[1000];

	SaveCurPos();
	i = var[*lp++];
	SetCur(i,var[*lp++]);							/* set new position */
	printf(Print(printBuf, GetMsgPtr(var[*lp++]), TEXTCOLS));	/* print the message */
	RestoreCurPos();

	return (lp);
}




STRPTR
NumFromStr(sp, np)
register	STRPTR	sp;
		INT		*np;
/* Sp points into a string where there are (presumably) digits.  Scan the
** digits, returning their value in the variable pointed to by np and the
** pointer to the next non-digit as the function value
*/
{
	register	WORD		n;

	n = 0;
	while (*sp >= '0' && *sp <= '9')
		n = 10*n + *sp++ - '0';

	*np = n;
	return (sp);
}




void
UpdateLineStats()
/* We've just finished formatting another line -- update the stats
*/
{
	++numLines;
	if (linePos > maxLength)
		maxLength = linePos;
	linePos = 0;
}

