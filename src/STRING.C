/* STRING
 * Handle user-entered strings
 *
 *	compile: MWC
 *
 * Change History:
 *	87.07.14 21:31	JAS
 *		WordToString() fixed -- must never have worked!!
 *	87.06.01 8:24	JAS
 *		Fixed CompareStrings() so that it properly handles a first
 *		string which is null.
 */

#include	"types.h"
#include	"game.h"
#include	"screen.h"


STRPTR	GetString();
STRPTR	SetString();
STRPTR	WordToString();
WORD		GetLine();
STRPTR	SetGameID();
BOOL		CompareStrings();
void		PrepareString();



#define	GLSIZE	40

TEXT		punctuation[] = " \t.,;:'!-";


extern	UBYTE	userStr[][STRLENGTH];
extern	STRPTR	wordPtr[];
extern	TEXT		gameID[];
extern	BOOL		dialogueOpen;
extern	WORD		monType;

extern	STRPTR	GetMsgPtr();
extern	BOOL		CursorState();
extern	STRPTR	Print();
extern	STRPTR	index();




STRPTR
GetString(lp)
register	STRPTR	lp;
{
	register	STRPTR	str;
			WORD		newRow, newCol, length;
			WORD		msgNum;
			BOOL		cursorOn;
			TEXT		printBuf[400];

	cursorOn = CursorState();
	SaveCurPos();
	EraseCur();

	/* Get all the info
	 */
	str = userStr[*lp++];
	msgNum = *lp++;
	newRow = *lp++;
	newCol = *lp++;

	/* Get the input line from the user (max length in next logic byte)
	 */
	if ((length = *lp++ + 1) > STRLENGTH)
		length = STRLENGTH;
	*str = 0;

	/* If the new cursor position is on the screen, put the prompt
	 * there.  Otherwise, leave the prompt where it is
	 */
	if (newRow < TEXTLINES) {
		SetCur(newRow, newCol);
		}
	if (monType != MONO || dialogueOpen) {
		printf(Print(printBuf, GetMsgPtr(msgNum), 40));
		GetLine(str, length);
		}
	else
		{
		OpenDialogue(Print(printBuf, GetMsgPtr(msgNum), 36), length);
		GetLine(str, length);
		CloseDialogue();
		}

	/* Put everything that we mucked with back
	 */
	RestoreCurPos();
	if (cursorOn)
		WriteCur();

	return (lp);
}




STRPTR
SetString(lp)
register	STRPTR	lp;
{
	register	STRPTR	str;

	str = userStr[*lp++];
	strncpy(str, GetMsgPtr(*lp++), STRLENGTH);
	return (lp);
}




STRPTR
WordToString(lp)
register	STRPTR	lp;
{
	register	STRPTR	str;

	str = &userStr[*lp++][0];
	strncpy(str, wordPtr[*lp++], STRLENGTH);
	return (lp);
}




WORD
GetLine(str, len)
STRPTR	str;
WORD		len;
/* Return a line of maximum length 'len' to the caller in the string 'str'.
 */
{
	register	STRPTR	lp;
			STRPTR	le;
	register	WORD		c;
			TEXT		line[GLSIZE];

	if (len > GLSIZE)
		len = GLSIZE;
	le = line + len;

	strncpy(line, str, len);
	printf(line);
	lp = line + strlen(line);
	WriteCur();
	forever {
		c = WaitChar();
		EraseCur();
		switch (c) {
			case '\b':
				if (lp > line) {
					--lp;
					putchar(c);
					}
				break;

			case 'C'-'@':
			case 'X'-'@':
				while (lp > line) {
					--lp;
					putchar('\b');
					}
				break;

			case '\r':
				*lp = '\0';
				strcpy(str, line);
			case ESC:
				return (c);

			default:
				if (lp < le - 1) {
					*lp++ = c;
					putchar(c);
					}
			}
		WriteCur();
		}
}




STRPTR
SetGameID(lp)
register	STRPTR	lp;
{
	strncpy(gameID, GetMsgPtr(*lp++), ID_LEN);
	VerifyGameID();
	return (lp);
}




BOOL
CompareStrings(n1, n2)
COUNT	n1, n2;
/* Compare the two strings whose numbers are n1 and n2.  Ignore case,
 * whitespace, and punctuation.
 */
{
	register	STRPTR	s1, s2;
			TEXT		str1[STRLENGTH+1], str2[STRLENGTH+1];

	PrepareString(n1, str1);
	PrepareString(n2, str2);
	for (s1 = str1, s2 = str2 ; *s1 != '\0' ; ++s1, ++s2)
		if (*s1 != *s2)
			return (FALSE);

	return (*s1 == *s2);
}




void
PrepareString(n, s)
		COUNT	n;
register	STRPTR	s;
/* Copy string number n into the space pointed to by s.  Convert to lowercase,
 * and remove punctuation and white space.
 */
{
	register	STRPTR	t;

	for (t = userStr[n] ; *t != '\0' ; ++t) {
		if (index(punctuation, *t) == NULL)
			*s++ = lower(*t);
		}
	*s = '\0';
}

