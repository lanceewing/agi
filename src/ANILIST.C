/* ANILIST
** Routines dealing with lists of animated objects.
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"
#include	"aniobj.h"
#include	"objlist.h"

BOOL		IsUpdObj();
BOOL		IsStoppedObj();
OBJLIST	*MakeUpdList();
OBJLIST	*MakeStoppedList();
void		RestoreAll();
void		FreeAllObjLists();
void		AllInBack();
void		InsertAll();
STRPTR	LStopUpdate();
STRPTR	LStartUpdate();
STRPTR	ForceUpdate();
void		StopUpdate();
void		StartUpdate();


OBJLIST	updateList, stoppedList;

extern	ANIOBJ	*aniObj, *aniEnd;

extern	OBJLIST	*MakeObjList();





BOOL
IsUpdObj(obj)
register	ANIOBJ	*obj;
/* Return TRUE if object is an updating object, FALSE otherwise
*/
{
	return ((obj->control & (UPDATE | DRAWN)) == (UPDATE | DRAWN));
}



BOOL
IsStoppedObj(obj)
ANIOBJ	*obj;
/* Return TRUE is 'obj' is a stop-updated object, FALSE otherwise
*/
{
	return ((obj->control & (UPDATE | DRAWN)) == DRAWN);
}



OBJLIST	*
MakeUpdList()
{
	return (MakeObjList(IsUpdObj, &updateList));
}



OBJLIST	*
MakeStoppedList()
{
	return (MakeObjList(IsStoppedObj, &stoppedList));
}




void
RestoreAll()
{
	RestoreObjList(&updateList);
	RestoreObjList(&stoppedList);
}



void
FreeAllObjLists()
{
	FreeObjList(&updateList);
	FreeObjList(&stoppedList);
}



void
AllInBack()
{
	ListInBack(MakeStoppedList());
	ListInBack(MakeUpdList());
}




void
AllToScreen()
{
	ListToScreen(&stoppedList);
	ListToScreen(&updateList);
}




STRPTR
LStopUpdate(lp)
register	STRPTR	lp;
{
	StopUpdate(aniObj + *lp++);
	return (lp);
}




STRPTR
LStartUpdate(lp)
register	STRPTR	lp;
{
	StartUpdate(aniObj + *lp++);
	return (lp);
}




STRPTR
ForceUpdate(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*obj;

	obj = aniObj + *lp++;

	RestoreAll();			/* restore all backgrounds */
	AllInBack();			/* remake lists and re-insert objects */
	AllToScreen();			/* re-display all objects */

	return (lp);
}




void
StopUpdate(obj)
register	ANIOBJ	*obj;
{
	if ((obj->control & UPDATE) != 0) {
		RestoreAll();			/* restore backgrounds of all objects */
		obj->control &= ~UPDATE;	/* mark the object as not updating */
		AllInBack();			/* remake lists and re-insert objects */
		}
}



void
StartUpdate(obj)
register	ANIOBJ	*obj;
{
	if ((obj->control & UPDATE) == 0) {
		RestoreAll();			/* restore backgrounds of all objects */
		obj->control |= UPDATE;	/* mark the object as updating */
		AllInBack();			/* remake lists and re-insert objects */
		}
}

