/* FINDPOSN
** Find a position for an object on the screen
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"


void		FindPosn();
BOOL		GoodPos();


extern	BOOL		Collide();
extern	BOOL		CanBHere();


extern	COORD	horizon;
extern	ANIOBJ	*aniObj;



void
FindPosn(obj)
register	ANIOBJ	*obj;
/* Find a position for object 'obj' where it does not collide with any
** unappropriate objects or priority regions.  If the object can't be in
** its current position, first try moving it left, then right, then start
** scanning in a spiral pattern for a position at which it can be placed.
*/
{
	register	COORD	legLen;
			WORD		legDir,legCnt;		/* variables for spiral scan */
/*
			COORD	savex;
			COORD	delta;
*/
	
	if (obj->y <= horizon && (obj->control & IGNRHRZ) == 0)
		obj->y = horizon + 1;

	/* If current position is OK, return
	*/
	if (GoodPos(obj) && !Collide(obj) && CanBHere(obj))
		return;
	
	/* Try moving horizontally by up to object's baseline
	*/
/*	savex = obj->x;
	for (delta = 1 ; delta <= obj->xsize ; ++delta) {
		if ((obj->x = savex - delta) < MINX)
			obj->x = MINX;
		if (GoodPos(obj) && !Collide(obj) && CanBHere(obj))
			return;

		if ((obj->x = savex + obj->xsize) > MAXX + 1)
			obj->x = MAXX + 1 - obj->xsize;
		if (GoodPos(obj) && !Collide(obj) && CanBHere(obj))
			return;
		}
*/	
	/* Start scan
	*/
	legDir = 0;
	legLen = legCnt = 1;
	while (!GoodPos(obj) || Collide(obj) || !CanBHere(obj)) {
		switch (legDir) {
			case	0:			/* move left */
				--obj->x;
				if (--legCnt == 0) {
					legDir = 1;
					legCnt = legLen;
					}
				break;
			case	1:			/* move down */
				++obj->y;
				if (--legCnt == 0) {
					legDir = 2;
					legCnt = ++legLen;
					}
				break;
			case	2:			/* move right */
				++obj->x;
				if (--legCnt == 0) {
					legDir = 3;
					legCnt = legLen;
					}
				break;
			case	3:			/* move up */
				--obj->y;
				if (--legCnt == 0) {
					legDir = 0;
					legCnt = ++legLen;
					}
				break;
			}
		}
}



BOOL
GoodPos(obj)
register	ANIOBJ	*obj;
/* Return TRUE if the object's position puts it on the screen, FALSE otherwise
*/
{
	return (obj->x >= MINX && (obj->x + obj->xsize) <= MAXX + 1
	     && (obj->y - obj->ysize) >= MINY - 1 && obj->y <= MAXY
	     && ((obj->control & IGNRHRZ) || obj->y > horizon));
}

