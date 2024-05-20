/* STATUS
** Display player status
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"object.h"
#include	"event.h"


STRPTR	Status();
void		InvSelect();
void		StatusScreen();
INV		*MoveSelect();
INV		*NewSelect();
void		DisplayStatusLine();
STRPTR	StatusLineOn();
STRPTR	StatusLineOff();
void		InitStatusLine();


#define	SCORECOL		1
#define	SOUNDCOL		30

#define	STARTROW		0
#define	FIRSTCOL		1
#define	SECNDCOL		20



extern	BOOL		showStatus;
extern	COUNT	statusRow;
extern	OBJECT	*object, *objEnd;
extern	UBYTE	var[];
extern	WORD		numObjs;
extern	TEXT		statusReturn[];

extern	EVENT	*WaitEvent();





STRPTR
Status(lp)
STRPTR	lp;
{
	/* Go to text screen
	*/
	EraseCur();
	SaveTextAtr();
	_SetTextAtr(0, 15);			/* inverse */
	LTextScrn();

	/* Display inventory and allow user to select an object, if appropriate.
	*/
	InvSelect();

	/* Go back to the graphics screen
	*/
	RestoreTextAtr();
	GraphicsScreen();

	/* Start checking number of lines displayed again
	*/
	return (lp);
}




void
InvSelect()
{
	register	INV		*ip;
	register	EVENT	*event;
			OBJECT	*obj;
			INV		inv[50];
			INV		*selected, *invEnd;
			COUNT	i, row, howMany;

	row = STARTROW + 2;

	/* Construct the table of objects being carried, deciding where on
	** the screen they are to be printed as we go.
	*/
	howMany = 0;
	selected = ip = inv;
	for (obj = object, i = 0 ; obj < objEnd ; ++obj, ++i) {
		if (obj->room == CARRYING) {
			if (i == var[SELECTED_OBJ])
				selected = ip;
			ip->num = i;
			ip->name = ((STRPTR) object) + obj->nameofs;
			ip->row = row;
			if ((howMany & 1) == 0)
				ip->col = FIRSTCOL;
			else {
				++row;
				ip->col = 39 - strlen(ip->name);
				}
			++howMany;
			++ip;
			}
		}

	/* If no objects in inventory, say so
	*/
	if (howMany == 0) {
		ip->num = 0;
		ip->name = "nothing";
		ip->row = row;
		ip->col = 16;
		++ip;
		}

	/* Point 'invEnd' to last entry in table.
	*/
	invEnd = ip - 1;
	ip = selected;

	/* Display the status screen.
	*/
	StatusScreen(inv, invEnd, ip);

	/* Handle events.
	*/
	if (!IsSet(ENABLE_SELECT)) {
		WaitEvent();
		return;
		}

	forever {
		event = WaitEvent();
		ButtonToChar(event);
		switch (event->type) {
			case CHAR:
				switch (event->val) {
					case '\r':
						var[SELECTED_OBJ] = ip->num;
						return;
	
					case ESC:
						var[SELECTED_OBJ] = 0xff;
						return;
					}
				break;

			case DIR:
				ip = MoveSelect(inv, invEnd, ip, event->val);
				break;
			}
		}
}




void
StatusScreen(start, end, selected)
		INV	*start, *end;
register	INV	*selected;
{
	register	INV	*ip;

	SetCur(STARTROW, 11);
	printf("You are carrying:");

	for (ip = start ; ip <= end ; ++ip) {
		SetCur(ip->row, ip->col);
		if (ip == selected && IsSet(ENABLE_SELECT))
			_SetTextAtr(15, 0);
		else
			_SetTextAtr(0, 15);
		printf(ip->name);
		}

	_SetTextAtr(0, 15);

	if (IsSet(ENABLE_SELECT)) {
		SetCur(24, 2);
		printf("Press ENTER to select, ESC to cancel");
		}
	else {
		SetCur(24, 4);
		printf("Press a key to return to the game");
		}
}




INV	*
MoveSelect(start, end, ip, dir)
INV		*start, *end, *ip;
COUNT	dir;
{
	register	INV		*np;

	switch (dir) {
		case 1:				/* up */
			np = ip - 2;
			break;
		case 3:				/* right */
			np = ip + 1;
			break;
		case 5:				/* down */
			np = ip + 2;
			break;
		case 7:				/* left */
			np = ip - 1;
			break;
		}
	if (np < start || np > end)
		np = ip;

	return (NewSelect(ip, np));
}




INV	*
NewSelect(ip, np)
register	INV	*ip, *np;
/* Highlight the new selection (pointed to by np)
*/
{

	if (ip != np) {
		_SetTextAtr(15, 0);
		SetCur(np->row, np->col);
		printf(np->name);

		_SetTextAtr(0, 15);
		SetCur(ip->row, ip->col);
		printf(ip->name);
		}

	return (np);
}




void
DisplayStatusLine()
{
	SaveCurPos();
	SaveTextAtr();

	if (showStatus) {
		ClearLine(statusRow, 0xff);
		_SetTextAtr(0, 15);			/* inverse */
		SetCur(statusRow, SCORECOL);
		printf("Score:%d of %d  ", var[SCORE], var[MAXSCORE]);
		SetCur(statusRow, SOUNDCOL);
		printf("Sound:%s", (IsSet(SOUNDON)? "on " : "off"));
		}

	RestoreTextAtr();
	RestoreCurPos();
}



STRPTR
StatusLineOn(lp)
STRPTR	lp;
{
	showStatus = TRUE;
	DisplayStatusLine();
	return (lp);
}


STRPTR
StatusLineOff(lp)
STRPTR	lp;
{
	showStatus = FALSE;
	ClearLine(statusRow, 0);
	return (lp);
}

