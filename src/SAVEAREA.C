/* SAVEAREA
** Routines for allocating/deallocating save areas.
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"
#include	"aniobj.h"
#include	"screen.h"
#include	"objlist.h"


SAVEAREA	*GetSaveArea();
void		FreeSaveArea();

extern	WORD		monType;

extern	STRPTR	GetMem();




SAVEAREA	*
GetSaveArea(obj)
register	ANIOBJ	*obj;
/* Allocate space to save the background for the object 'obj' and return
** a pointer to this space.
*/
{
	register	SAVEAREA	*sp;
	register	COUNT	width, height;

	/* Allocate a save area header and fill it in.
	*/
	sp = (SAVEAREA *) GetMem(sizeof(SAVEAREA));
	sp->next = sp->prev = NULL;
	sp->theObject = obj;
	sp->x = obj->x;
	sp->y = obj->y - obj->ysize + 1;
	width = sp->width = obj->xsize;
	if (monType == MONO)
		width += (((sp->x & 1) + width + 1) / 2) * 2;
	height = sp->height = obj->ysize;

	/* Allocate space for the actual save.
	*/
	sp->bitMap = GetMem(width * height);

	/* Put the pointer to this in the object's structure.
	*/
	obj->save = (STRPTR) sp;

	/* Return the pointer
	*/
	return (sp);
}





void
FreeSaveArea(sp)
register	SAVEAREA	*sp;
/* Free a save area.
*/
{
	SetMemPtr(sp);
}

