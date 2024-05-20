/* MOVEOBJS
** Move the objects
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"
#include	"aniobj.h"


void		MoveObjs();


static	WORD		xs[] = {0,0,1,1,1,0,-1,-1,-1};
static	WORD		ys[] = {0,-1,-1,0,1,1,1,0,-1};

extern	UBYTE	var[];
extern	COORD	horizon;
extern	ANIOBJ	*aniObj, *aniEnd;

extern	BOOL		Collide();
extern	BOOL		CanBHere();





void
MoveObjs()
/* At least one animated object is supposed to move.  Update all objects.
*/
{
	register	ANIOBJ	*obj;
	register	COORD	oy;
			COORD	ox, px, py;
			WORD		os,od;
			WORD		border;
	
	
	var[EGOEDGE] = var[OBJHIT] = var[OBJEDGE] = 0;
	
	for (obj = aniObj ; obj < aniEnd ; ++obj) {
		if ((obj->control & (ANIMATED | UPDATE | DRAWN))
			!= (ANIMATED | UPDATE | DRAWN))
				continue;
	
		/* Decrement the move clock for this object.  Don't move
		** the object unless the clock has reached 0.
		*/
		if (obj->moveclk != 0 && --obj->moveclk != 0)
			continue;

		/* Reset the move clock
		*/
		obj->moveclk = obj->movefreq;

		border = 0;			/* clear border collision flag */
	
		ox = px = obj->x;
		oy = py = obj->y;

		/* If object has not been repositioned, move it
		*/
		if ((obj->control & REPOS) == 0) {
			od = obj->dir;
			os = obj->stepsize;
			ox += xs[od] * os;
			oy += ys[od] * os;
			}
	
		/* Check for object border collision
		*/
		if (ox < MINX) {
			ox = MINX;
			border = LEFT;
			}
		else if (ox + obj->xsize > MAXX + 1) {
			ox = MAXX + 1 - obj->xsize;
			border = RIGHT;
			}
	
		if (oy - obj->ysize < MINY - 1) {
			oy = MINY - 1 + obj->ysize;
			border = TOP;
			}
		else if (oy > MAXY) {
			oy = MAXY;
			border = BOTTOM;
			}
		else if ((obj->control & IGNRHRZ) == 0 && oy <= horizon) {
			oy = horizon + 1;
			border = TOP;
			}
	
		/* if object can't be in this position, move back to previous
		** position and clear the border collision flag
		*/
		obj->x = ox;
		obj->y = oy;
		if (Collide(obj) || !CanBHere(obj)) {
			obj->x = px;
			obj->y = py;
			border = 0;
			/* make sure that this position is OK */
			FindPosn(obj);
			}
	
		/* If the object hit the border, set the appropriate flags
		*/
		if (border) {
			if (obj->num == 0)
				var[EGOEDGE] = border;
			else {
				var[OBJHIT] = obj->num;
				var[OBJEDGE] = border;
				}
	
			/* If the object was on a 'moveobj', set the move as finished
			*/
			if (obj->motion == MOVETO)
				EndMoveObj(obj);
			}
	
		/* If object was not to be repositioned, it can be repositioned
		** from now on
		*/
		obj->control &= ~REPOS;
		}
}
