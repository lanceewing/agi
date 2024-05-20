/* DRAW
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"
#include	"objlist.h"

STRPTR	LDraw();
void		DrawObj();
STRPTR	LErase();
void		Erase();


extern	ANIOBJ	*aniObj, *aniEnd;
extern	OBJLIST	*updateList, *stoppedList;

extern	OBJLIST	*MakeObjList(), *MakeStoppedList(), *MakeUpdList();




STRPTR
LDraw(lp)
register	STRPTR	lp;
{
	DrawObj(*lp++);
	return (lp);
}



void
DrawObj(n)
register	WORD		n;
/* Draw object n on the screen, relocating it if necessary
*/
{
	register	ANIOBJ	*obj;

	obj = aniObj + n;

	if (obj > aniEnd)
		Error(19, n);
	if (obj->celptr == NULL)
		Error(20, n);

	/* If object is already drawn, return
	*/
	if (obj->control & DRAWN)
		return;

	obj->control |= UPDATE;

	/* Find a position for the object
	*/
	FindPosn(obj);

	/* Set previous values to current for 'obj2scrn'
	*/
	obj->prevcel = obj->celptr;
	obj->prevx = obj->x;
	obj->prevy = obj->y;

	RestoreObjList(&updateList);	/* restore backgrounds of updating objects */
	obj->control |= DRAWN;
	ListInBack(MakeUpdList());	/* remake update list and insert objects */
	Obj2Scrn(obj);				/* put the object on the screen */

	obj->control &= ~NOADVANC;
}



STRPTR
LErase(lp)
register	STRPTR	lp;
{
	Erase(*lp++);
	return (lp);
}




void
Erase(n)
register	WORD		n;
/* Erase object n from the screen
*/
{
	register	ANIOBJ	*obj;
			BOOL		wasStopped;	/* was updating stopped? */

	/* If object is not drawn, just return
	*/
	obj = aniObj + n;
	if (obj > aniEnd)
		Error(12, n);
	if (!(obj->control & DRAWN))
		return;

	RestoreObjList(&updateList);
	if (wasStopped = ((obj->control & UPDATE) == 0))
		RestoreObjList(&stoppedList);
	obj->control &= ~DRAWN;
	if (wasStopped)
		ListInBack(MakeStoppedList());
	ListInBack(MakeUpdList());
	Obj2Scrn(obj);			/* erase object from the screen */
}

