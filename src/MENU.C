/* MENU
** Routines for handling menus in AGI
**
**	compile: MWC
**
** Change History:
** 87 Sept pmk
**	Suppress halftone on VGA
** 20/02/87  10:34:18	JAS
**	Added a call to ButtonToChar() in DoMenu() so that the joystick
**	buttons can be used to select from the menu.
**
** 18/02/87  15:00:44	JAS
**	Mucked around to ensure that menus with no items will not either
**	print garbage or go into endless loops.
*/

#include	"types.h"
#include	"game.h"
#include	"event.h"
#include	"screen.h"

STRPTR	SetMenu();
STRPTR	SetMenuItem();
STRPTR	SubmitMenu();
STRPTR	EnableMenu();
void		EnableAllMenus();
STRPTR	DisableMenu();
void		ChangeItem();
STRPTR	MenuInput();
void		DoMenu();

void		ShowMenu();
void		PutAway();
void		Select();
void		Deselect();
void		ComputeMenuWindow();



#define	DISABLE	0
#define	ENABLE	1

#define	CHARWIDTH		4		/* in our coordinates */
#define	VMARGIN		charHeight
#define	HMARGIN		CHARWIDTH

typedef	struct	menuHead {
	struct	MenuItem	*next;
	struct	MenuItem	*prev;
	STRPTR			name;
	COUNT			row, col;
	BOOL				enabled;
	}	MENUHEADER;


typedef	struct	MenuItem {
	MENUHEADER		h;
	COUNT			ctrl;
	}	MENUITEM;


typedef	struct	Menu {
	MENUHEADER		h;
	struct	MenuItem	*items;
	struct	MenuItem	*curItem;
	COUNT			height;
	}	MENU;


BOOL		activateMenu;
WORD		menuCol, itemRow, itemCol;
BOOL		menuSubmitted;
MENU		*menuBase, *curMenu;
MENUITEM	*curItem;
UWORD	menuPos, menuDim;

extern	COUNT	picTop;
extern	COUNT	charHeight; 		/* modified in EQUIPCHK.ASM for HGC */
extern	BOOL		showStatus, halfTone;

extern	STRPTR	GetMsgPtr();
extern	EVENT	*WaitEvent();
extern	WORD		monType;



STRPTR
SetMenu(lp)
register	STRPTR	lp;
{
	register	MENU		*mp;
	register	STRPTR	menuName;

	menuName = GetMsgPtr(*lp++);

	if (menuSubmitted == TRUE)
		return (lp);

	/* Allocate space for the next menu and link it into the menu list
	*/
	mp = (struct Menu *) GetMem(sizeof(struct Menu));
	if (menuBase == NULL) {
		menuBase = mp;
		menuCol = 1;
		}
	else {
		/* If previous menu didn't have any items, disable it.
		*/
		if (curMenu->items == NULL)
			curMenu->h.enabled = FALSE;
		curMenu->h.next = mp;
		mp->h.prev = curMenu;
		}
	mp->h.next = menuBase;
	menuBase->h.prev = mp;
	curMenu = mp;

	/* Set the position of this menu name in the menu strip (leave two
	** chars between menu titles).
	*/
	mp->h.row = 0;
	mp->height = 0;
	mp->h.name = menuName;
	mp->h.col = menuCol;
	mp->h.enabled = TRUE;
	mp->items = NULL;
	menuCol += strlen(menuName) + 1;

	/* Initialize stuff for the menu items to follow
	*/
	curItem = NULL;
	itemRow = 1;

	return (lp);
}





STRPTR
SetMenuItem(lp)
register	STRPTR	lp;
{
	register	MENUITEM	*ip;
	register	STRPTR	itemName;
			UBYTE	itemController;

	itemName = GetMsgPtr(*lp++);
	itemController = *lp++;

	if (menuSubmitted == TRUE)
		return (lp);

	/* Allocate a menu item structure and link it in
	*/
	ip = (struct MenuItem *) Getmem(sizeof(struct MenuItem));
	if (curItem == NULL) {
		curMenu->curItem = curMenu->items = ip;
		ip->h.prev = ip;
		}
	else {
		curItem->h.next = ip;
		ip->h.prev = curItem;
		}
	ip->h.next = curMenu->items;
	curMenu->items->h.prev = ip;
	curItem = ip;

	/* Position the menu item in the menu
	*/
	ip->h.name = itemName;
	if (itemRow == 1) {
		if (curMenu->h.col + strlen(itemName) < 39)
			itemCol = curMenu->h.col;
		else
			itemCol = 39 - strlen(itemName);
		}
	ip->h.row = ++itemRow;
	ip->h.col = itemCol;
	ip->h.enabled = TRUE;
	ip->ctrl = itemController;

	++curMenu->height;

	return (lp);
}





STRPTR
SubmitMenu(lp)
STRPTR	lp;
{
	/* If the last menu didn't have any items, disable it.
	*/
	if (curMenu->items == NULL)
		curMenu->h.enabled = FALSE;

	/* Set heap base above menu structures.
	*/
	SetBase();

	/* Make the first menu the current one.
	*/
	curMenu = menuBase;
	curItem = curMenu->items;
	menuSubmitted = TRUE;
	return (lp);
}




STRPTR
EnableMenu(lp)
register	STRPTR	lp;
{
	ChangeItem(*lp++, ENABLE);
	return (lp);
}




void
EnableAllMenus()
/* Enable all the menus in the menustrip.
*/
{
	register	struct	Menu		*mp;
	register	struct	MenuItem	*ip;

	if ((mp = menuBase) != NULL) {
		do	{
			if (mp->h.enabled) {
				ip = mp->items;
				do	{
					ip->h.enabled = TRUE;
					ip = ip->h.next;
				} while (ip != mp->items);
				}
			mp = mp->h.next;
		} while (mp != menuBase);
		}
}



STRPTR
DisableMenu(lp)
register	STRPTR	lp;
{
	ChangeItem(*lp++, DISABLE);
	return (lp);
}





void
ChangeItem(c, how)
register	COUNT	c;
		COUNT	how;
/* Enable/Disable (depending on 'how'), the menu item whose controller is 'c'.
*/
{
	register	struct	Menu		*mp;
	register	struct	MenuItem	*ip;

	mp = menuBase;
	do	{
		if (mp->h.enabled) {
			ip = mp->items;
			do	{
				if (ip->ctrl == c)
					ip->h.enabled = (how == ENABLE);
				ip = ip->h.next;
			} while (ip != mp->items);
			}
		mp = mp->h.next;
	} while (mp != menuBase);
}




STRPTR
MenuInput(lp)
{
	if (IsSet(ENABLE_MENU))
		activateMenu = TRUE;
	return (lp);
}




void
DoMenu()
{
	register	MENU		*mp;
	register	MENUITEM	*ip;
			EVENT	*ev;
			
	SaveCurPos();
	SaveTextAtr();

	ClearLine(0, MakeBackColor(15));
	mp = menuBase;
	do	{
		Deselect(mp);
		mp = mp->h.next;
	} while (mp != menuBase);

	mp = curMenu;
	ip = curItem;
	ShowMenu(mp);

	forever {
		ev = WaitEvent();
		ButtonToChar(ev);
		DirTranslate(ev);
		switch (ev->type) {
			case CHAR:
				switch (ev->val) {
					case '\r':
						if (!ip->h.enabled)
							continue;
						PostEvent(CONTROL, ip->ctrl);

					case ESC:
						PutAway(mp, ip);

						RestoreTextAtr();
						RestoreCurPos();
						if (showStatus)
							DisplayStatusLine();
						else
							ClearLine(0, 0);
						activateMenu = FALSE;
						return;
					}
				break;

			case DIR:
				switch (ev->val) {
					case 1:				/* up */
						Deselect(ip);
						ip = ip->h.prev;
						Select(ip);
						break;

					case 2:				/* top of cur menu */
						Deselect(ip);
						ip = mp->items;
						Select(ip);
						break;

					case 3:				/* right */
						PutAway(mp, ip);
						for (mp = mp->h.next ; !mp->h.enabled ; mp = mp->h.next)
							;
						ip = mp->curItem;
						ShowMenu(mp);
						break;

					case 4:				/* bottom of cur menu */
						Deselect(ip);
						ip = mp->items->h.prev;
						Select(ip);
						break;

					case 5:				/* down */
						Deselect(ip);
						ip = ip->h.next;
						Select(ip);
						break;

					case 6:				/* rightmost menu */
						PutAway(mp, ip);
						mp = menuBase->h.prev;
						ip = mp->curItem;
						ShowMenu(mp);
						break;

					case 7:				/* left */
						PutAway(mp, ip);
						for (mp = mp->h.prev ; !mp->h.enabled ; mp = mp->h.prev)
							;
						ip = mp->curItem;
						ShowMenu(mp);
						break;

					case 8:				/* leftmost menu */
						PutAway(mp, ip);
						mp = menuBase;
						ip = mp->curItem;
						ShowMenu(mp);
						break;
					}
			}

		curMenu = mp;
		curItem = ip;
		}
}




void
ShowMenu(mp)
register	MENU	*mp;
{
	register	MENUITEM	*ip;

	Select(mp);

	ComputeMenuWindow(mp);
	OpenWndw(menuPos, menuDim, 0x000f);

	if ((ip = mp->items) != NULL) {
		do	{
			if (ip == mp->curItem)
				Select(ip);
			else
				Deselect(ip);
			ip = ip->h.next;
		} while (ip != mp->items);
		}
}




void
PutAway(mp, ip)
register	MENU		*mp;
register	MENUITEM	*ip;
{
	mp->curItem = ip;
	Deselect(mp);
	CloseWndw(menuPos, menuDim);
}




void
Select(ip)
register	MENUITEM	*ip;
{
	SetCur(ip->h.row, ip->h.col);
	_SetTextAtr(MakeForeColor(15), MakeBackColor(0));
	if (!ip->h.enabled && monType != VGA)
		halfTone = TRUE;
	printf(ip->h.name);
	halfTone = FALSE;
}




void
Deselect(ip)
register	MENUITEM	*ip;
{
	COUNT	row, col;

	row = ip->h.row;
	col = ip->h.col;

	_SetTextAtr(MakeForeColor(0), MakeBackColor(15));
	SetCur(row, col);
	if (!ip->h.enabled && monType != VGA)
		halfTone = TRUE;
	printf(ip->h.name);
	halfTone = FALSE;
}





void
ComputeMenuWindow(mp)
register	MENU		*mp;
{
	COUNT	height, width;

	width = strlen(mp->items->h.name);
	height = mp->height;

	/* Compute window size and position and put them into the appropriate
	** bytes of the words.
	*/
	menuDim = ((height * charHeight + 2 * VMARGIN) << 8)
				| (width * CHARWIDTH + 2 * HMARGIN);

	menuPos = (((mp->items->h.col - 1) * CHARWIDTH) << 8)
				| ((height + 2 - picTop) * charHeight + VMARGIN - 1);
}

