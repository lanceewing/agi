/* SHOWOBJ
** Show the player an object on the screen
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"
#include	"object.h"
#include	"objlist.h"
#include	"screen.h"


STRPTR	ShowObj();
STRPTR	VShowObj();
void		_ShowObj();


#define	SHOWY	MAXY


extern	OBJECT	object[];
extern	TEXT		ENTERcont[];
extern	VIEWNODE	viewList;
extern	BOOL		returnIfNoMem;
extern	UBYTE	var[];
extern	WORD		monType;

extern	UWORD	MemLeft();
extern	VIEWNODE	*FindView(), *LoadView();
extern	STRPTR	GetMem();
extern	SAVEAREA	*GetSaveArea();




STRPTR
VShowObj(lp)
register	STRPTR	lp;
{
	_ShowObj(var[*lp++]);
	return (lp);
}





STRPTR
ShowObj(lp)
register	STRPTR	lp;
{
	_ShowObj(*lp++);
	return (lp);
}





void
_ShowObj(objView)
COUNT	objView;
/* Display the given object view, print the message, and wait for the
** player to press enter.
*/
{
	register	VIEWNODE	*vEnt;
			SAVEAREA	*sp;
			COUNT	backSize;
			ANIOBJ	obj;
			BOOL		viewHere, objShown;


	ScriptOff();
	objShown = FALSE;

	/* Load the object view
	*/
	viewHere = (FindView(objView) != NULL);
	returnIfNoMem = TRUE;
	if (LoadView(objView, FALSE) == NULL) {
		/* Not enough memory to load the view
		*/
		returnIfNoMem = FALSE;
		WindowPrint("Not now.");
		return;
		}
	returnIfNoMem = FALSE;

	/* Set up the aniObj structure which will be used to display this view
	*/
	obj.loop = obj.cel = 0;
	SetView(&obj, objView);
	obj.prevcel = obj.celptr;
	obj.x = obj.prevx = (MAXX - obj.xsize)/2;
	obj.y = obj.prevy = MAXY;
	obj.pri = 15;
	obj.control |= FIXEDPRI;
	obj.num = -1;
	backSize = obj.xsize;
	if (monType == MONO)
		backSize += (((obj.x & 1) + backSize + 1) / 2) * 2;
	backSize = backSize * obj.ysize + sizeof(SAVEAREA);

	/* If there is enough memory to save the background so that we can
	** draw the object, do so.
	*/
	if (MemLeft() > backSize) {
		objShown = TRUE;
		sp = GetSaveArea(&obj);
		SaveBack(sp);
		InsertObj(&obj);
		Obj2Scrn(&obj);
		}

	vEnt = FindView(objView);
	WindowPrint((char *) vEnt->ptr + W8086(vEnt->ptr->vdesc));

	if (objShown) {
		RestBack(sp);
		Obj2Scrn(&obj);
		FreeSaveArea(sp);
		}

	/* Toss the view
	*/
	if (!viewHere)
		DiscardView(objView);

	ScriptOn();
	return;
}

