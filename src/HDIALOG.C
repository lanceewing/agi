/* HDIALOG
** HGC dialogue box routines.
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"event.h"
#include	"textwin.h"
#include	"screen.h"


void		OpenDialogue();
WORD		GetDialogue();
void		CloseDialogue();


extern	TEXT_WINDOW	textWin;
extern	UBYTE		var[];


TEXT		enterCmndStr[] = "ENTER COMMAND";


void
OpenDialogue(str, width)
STRPTR	str;
COUNT	width;
{
	register	UWORD	inputRow;
	register	COUNT	tSIndex;
			TEXT		titleStr[DIALOGUE_WIDTH + 4];
			TEXT		spacingStr[3];

	if (width < strlen(str)) {
		width = strlen(str);
		strcpy(spacingStr, "\n");
		}
	else {
		width++;
		strcpy(spacingStr, "\n\n");
		}
	tSIndex = (width - strlen(str)) / 2;
	titleStr[0] = ' ';
	if (tSIndex > 1)
		strncpy(&titleStr[1], titleStr, tSIndex);
	strcpy(&titleStr[tSIndex], str);
	strcat(titleStr, spacingStr);
	WindowNoWait(titleStr, 0, width, TRUE);
	inputRow = textWin.bottom;
	SetCur(inputRow, textWin.left);
	ClearRect(inputRow, textWin.left, inputRow, textWin.right - 1, 0);
	SaveTextAtr();
	_SetTextAtr(15, 0);
	WriteCur();
}


WORD
GetDialogue(c)
register	WORD		c;
{
	forever {
		switch(c) {
			case	'\0':
				break;

			case 'E'-'@':
				EchoLine(NULL);
				break;

			case	ESC:
				c = '\r';

			case 'C'-'@':
			case 'X'-'@':
				CancelLine(NULL);
				break;

			default:
				EditLine(c);
				break;
			}
		if (c == '\r')
			break;
		if ((c = WaitChar()) == '\r')
			break;
		}

	return (c);
}


void
CloseDialogue()
{
	EraseCur();
	RestoreTextAtr();
	CloseWindow();
}
