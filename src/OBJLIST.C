/* OBJLIST
** Routines to deal with lists of objects and their background save areas.
** Background save areas for objects are kept in doubly linked lists.  Objects
** are put in background from back (low priority) to front (high priority).
** As this is done, the object's background save area is linked into the
** head of the list.  This allows us to restore the backgrounds in the
** proper order by simply restoring from the head to the tail of the list.
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"
#include	"objlist.h"

void		RestoreObjList();
void		FreeObjList();
void		ListToScreen();
OBJLIST	*MakeObjList();
void		AddObjToList();
void		ListInBack();


extern	ANIOBJ	*aniObj, *aniEnd;

extern	SAVEAREA	*GetSaveArea();
extern	void		FreeSaveArea();




void
RestoreObjList(listPtr)
register	OBJLIST	*listPtr;
/* Restore backgrounds for all objects in a listand free the save areas.
*/
{
	register	SAVEAREA	*sp;

	for (sp = listPtr->head ; sp != NULL ; sp = sp->next)
		RestBack(sp);			/* restore background for this object */

	FreeObjList(listPtr);
}



void
FreeObjList(listPtr)
OBJLIST	*listPtr;
{
	register	SAVEAREA	*sp, *ptr;

	for (sp = listPtr->head ; sp != NULL ; sp = ptr) {
		ptr = sp->next;
		FreeSaveArea(sp);
		}

	/* Mark list as empty.
	*/
	listPtr->head = listPtr->tail = NULL;
}



OBJLIST	*
MakeObjList(SelectObj, listPtr)
BOOL		(* SelectObj)();
OBJLIST	*listPtr;
/* Put all objects selected by the function 'SelectObj' into background in
** y-coordinate order, and link their saved backgrounds into the list
** pointed to by 'listPtr'.
*/
{
#define	BIGCOORD	0xff

	register	ANIOBJ	*obj;
	register	WORD		i;
			COUNT	minI, numToAdd, numAdded;
			ANIOBJ	*aniTable[NUMANIMATED];
			COORD	minY, yCoord[NUMANIMATED];


	/* Create a table of pointers to all objects meeting the criterion.
	** Determine the effective y coordinates of all objects in aniTable
	** -- effective coord is:
	**	y coord if not fixed priority
	**	y coord corresponding to priority band if fixed priority)
	**
	*/
	for (obj = aniObj, numToAdd = 0 ; obj < aniEnd ; ++obj) {
		if ((* SelectObj)(obj)) {
			aniTable[numToAdd] = obj;
			yCoord[numToAdd] = (obj->control & FIXEDPRI)?
							Pri2Coord(obj->pri) : obj->y;
			++numToAdd;
			}
		}

	/* Add the objects to the list in order from lowest effective y coordinate
	** to the highest.  The sort is a simple insertion sort.
	*/
	for (numAdded = 0 ; numAdded < numToAdd ; ++numAdded) {
		/* Find the object with the smallest remaining effective y coordinate
		*/
		minY = BIGCOORD;
		for (i = 0 ; i < numToAdd ; ++i) {
			if (yCoord[i] < minY) {		/* if smaller than current min, */
				minI = i;				/*   save index of obj */
				minY = yCoord[i];		/*   and set new min */
				}
			}

		/* Remove this object from further consideration
		*/
		yCoord[minI] = BIGCOORD;

		/* Add it to the list
		*/
		AddObjToList(aniTable[minI], listPtr);
		}

	return (listPtr);
}





void
AddObjToList(obj, listPtr)
		ANIOBJ	*obj;
register	OBJLIST	*listPtr;
{
	register	SAVEAREA	*sp;

	/* Get a save area for the object and link it in at the head of the list.
	*/
	sp = GetSaveArea(obj);
	if ((sp->next = listPtr->head) != NULL)
		sp->next->prev = sp;
	listPtr->head = sp;

	/* If this is the first object added to the list (i.e. there is
	** no tail), point the tail of the list at this object.
	*/
	if (listPtr->tail == NULL)
		listPtr->tail = sp;
}





void
ListInBack(listPtr)
OBJLIST	*listPtr;
/* Insert an object list into the background starting with the lowest
** effective y coordinate.
*/
{
	register	SAVEAREA	*sp;

	for (sp = listPtr->tail ; sp != NULL ; sp = sp->prev) {
		SaveBack(sp);
		InsertObj(sp->theObject);
		}
}




void
ListToScreen(listPtr)
OBJLIST	*listPtr;
/* Insert a list into background, then copy all objects in it to the screen.
*/
{
	register	SAVEAREA	*sp;
	register	ANIOBJ	*obj;

	/* Copy the objects in the list to the screen.
	*/
	for (sp = listPtr->head ; sp != NULL ; sp = sp->next) {
		obj = sp->theObject;
		Obj2Scrn(obj);

		/* If the object was supposed to have moved this cycle, see if
		 * it actually did and set flags accordingly.
		 */
		if (obj->moveclk == obj->movefreq) {
			if (obj->x == obj->prevx && obj->y == obj->prevy)
				obj->control |= STOPPED;
			else	{
				obj->prevx = obj->x;
				obj->prevy = obj->y;
				obj->control &= ~STOPPED;
				}
			}
		}
}

