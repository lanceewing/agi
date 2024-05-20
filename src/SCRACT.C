/* SCRACT
** Screen related actions
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"screen.h"

#define	INVERSE	0x8f			/* inverse video ? orig 0x8f */

STRPTR	LTextScrn();
STRPTR	LGrafScrn();
STRPTR	LClearLines();
STRPTR	LClearRect();
STRPTR	SetTextAtr();
void		_SetTextAtr();
UWORD	MakeTextAtr();
UWORD	MakeForeColor();
UWORD	MakeBackColor();
void		GraphicsScreen();
STRPTR	ConfigureScreen();
STRPTR	ToggleMonitor();
void		SaveTextAtr();
void		RestoreTextAtr();



SCR_MODE	screenMode = GRAPHMODE;

#define	WHITE_ON_BLACK	0x07
#define	BLACK_ON_WHITE	0x70
#define	MAXSAVED		5
UWORD	savedAtr[MAXSAVED][3];
COUNT	savedIndex;


extern	COUNT	picTop, picBottom;
extern	WORD		machType, monType;
extern	UWORD	textAtr, foreColor, backColor;
extern	UWORD	picReloc, statusRow, inputRow;
extern	UBYTE	var[];




STRPTR
LTextScrn(lp)
register	STRPTR	lp;
{
	EraseCur();
	screenMode = TEXTMODE;
	_SetTextAtr(foreColor, backColor);
	TextScrn();
	ClrLines(0, 24, textAtr);
	return (lp);
}
	


STRPTR
LGrafScrn(lp)
STRPTR	lp;
{
	EraseCur();
	GraphicsScreen();
	return (lp);
}

	

STRPTR
LClearLines(lp)
register	STRPTR	lp;
{
	register	WORD		top;
			WORD		bottom;

	top = *lp++;
	bottom = *lp++;
	ClrLines(top, bottom, MakeBackColor(*lp++));
	return (lp);
}




STRPTR
LClearRect(lp)
register	STRPTR	lp;
{
	WORD		top, bottom, left, right;

	top = *lp++;
	left = *lp++;
	bottom = *lp++;
	right = *lp++;
	ClearRect(top, left, bottom, right, MakeBackColor(*lp++));

	return (lp);
}




STRPTR
SetTextAtr(lp)
register	STRPTR	lp;
{
	register	UWORD	fore;

	fore = *lp++;
	_SetTextAtr(fore, *lp++);
	return (lp);
}



void
_SetTextAtr(fore, back)
register	UWORD	fore, back;
{
	textAtr = MakeTextAtr(fore, back);
	foreColor = MakeForeColor(fore);
	backColor = MakeBackColor(back);
}



UWORD
MakeTextAtr(fore, back)
register	UWORD	fore, back;
/* Return the requested text attribute in it's internal representation
*/
{
	if (monType == MONO) {
		if (back < fore)
			return(WHITE_ON_BLACK);
		else
			return(BLACK_ON_WHITE);
		}
	else	if (screenMode == TEXTMODE)
		/* For text mode, put background in high nibble, fore in low
		*/
		return ((back << 4) | fore);
	else if (back != 0)
		/* In graphics if back is not black, approximate with inverse
		** text (black on white).
		*/
		return (INVERSE);
	else if (monType == EGA || machType != PC)
		/* EGA and PCjr can do any color foreground in graphics (I think...)
		*/
		return (fore);
	else {
		/* If colored text in graphics mode, approximate with 'nearest'
		** color
		*/
		switch (fore) {
			case 0:				/* black on black */
				return (0);
				break;

			case 1:				/* map all these to cyan */
			case 2:
			case 3:
			case 9:
			case 10:
			case 11:
				return (1);
				break;

			case 4:				/* map all these to magenta */
			case 5:
			case 6:
			case 12:
			case 13:
			case 14:
				return (2);
				break;

			default:				/* everything else goes to white */
				return (3);
			}
		}
}



UWORD
MakeForeColor(fore)
UWORD	fore;
/* Return the internal representation for the requested foreground color.
*/
{
	return (fore);
}



UWORD
MakeBackColor(back)
register	UWORD	back;
/* Return the internal representation for the requested background color.
*/
{
	if (screenMode == GRAPHMODE && back != 0)
		/* In graphics if back is not black, approximate with inverse
		** text (black on white).
		*/
		return (0xff);	/* mask off inverse */
	else 
		return (0);
}





void
GraphicsScreen()
{
	screenMode = GRAPHMODE;
	_SetTextAtr(foreColor, backColor);
	GrafScrn();
	DisplayStatusLine();
	DisplayInput();
}



STRPTR
ConfigureScreen(lp)
register	STRPTR	lp;
{

	picTop = *lp++;
	picBottom = picTop + 21;
	if (monType == MONO)
		if (picTop > 1)
			picReloc = HGC_RELOC_FACTOR;
		else
			picReloc = picTop * HGC_RELOC_FACTOR;
	else
		picReloc = picTop * IBM_RELOC_FACTOR;
	inputRow = *lp++;
	statusRow = *lp++;
	return (lp);
}



STRPTR
ToggleMonitor(lp)
STRPTR	lp;
{
	if (machType == PC && var[CURROOM] != 0 && monType != EGA
		&& monType != MONO) {
			SetupScanOfs();
			monType ^= 1;
			SaveCurPos();
			SetDisp();
			RestoreCurPos();
			Restore();			/* restore room, reloading ego */
			}

	return (lp);
}



void
SaveTextAtr()
{
	register	UWORD	*savePtr;

	if (savedIndex < MAXSAVED) {
		savePtr = &savedAtr[savedIndex][0];
		*savePtr = foreColor;
		*++savePtr = backColor;
		*++savePtr = textAtr;
		++savedIndex;
		}
}


void
RestoreTextAtr()
{
	register	UWORD	*savePtr;

	if (savedIndex > 0) {
		savePtr = &savedAtr[--savedIndex][0];
		foreColor = *savePtr;
		backColor = *++savePtr;
		textAtr = *++savePtr;
		}
}

