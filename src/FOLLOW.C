/* FOLLOW
** The object 'obj' is supposed to follow ego.
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"
#include	"aniobj.h"


/* Distance from ego which is considered to be completion of the motion
*/
#define	endDist	parms[0]

/* Flag to set on completion of the motion
*/
#define	endFlag	parms[1]

/* Distance to move in current direction (for random search)
*/
#define	randDist	parms[2]


extern	UWORD	Abs();
extern	UWORD	Random();
extern	WORD		MoveDir();
extern	WORD		RandmDir();

extern	ANIOBJ	*aniObj;





void
Follow(obj)
register	ANIOBJ	*obj;
{
	WORD		dir;
	COORD	ecx,ocx;
	COORD	maxDist;
	
	/* Get coordinates of center of object's & ego's bases
	*/
	ecx = ego->x + (COORD) ego->xsize/2;
	ocx = obj->x + (COORD) obj->xsize/2;
	
	/* Get direction from object's center to ego's center
	*/
	dir = MoveDir(ocx,obj->y,ecx,ego->y,obj->endDist);
	
	/* If the direction is zero, the object and ego have collided -- signal
	** completion.
	*/
	if (dir == 0) {
		obj->dir = 0;
		obj->motion = NORMAL;
		Set(obj->endFlag);
		return;
		}
	
	/* If the object has not moved since last time, assume it is blocked and
	** move in a random direction for a random distance no greater than the
	** distance between the object and ego
	*/
	/* NOTE: randDist = -1 indicates that this is initialization, and thus
	** we don't care about the previous position
	*/
	if (obj->randDist == -1)
		obj->randDist = 0;
	else if (obj->control & STOPPED) {
		/* Make sure that the object goes in some direction
		*/
		while ((obj->dir = RandmDir()) == 0)
			;
	
		/* Average the x and y distances to the object for movement limit
		*/
		maxDist = (Abs(ocx - ecx) + Abs(obj->y - ego->y))/2 + 1;
	
		/* Make sure that the distance is at least the object stepsize
		*/
		if (maxDist <= obj->stepsize)
			obj->randDist = obj->stepsize;
		else
			while ((obj->randDist = Random() % maxDist) < obj->stepsize)
				;
	
		return;
		}
	
	/* If 'randDist' is non-zero, keep moving the object in the current
	** direction.
	*/
	if (obj->randDist != 0) {
		if ((obj->randDist -= obj->stepsize) < 0)
			obj->randDist = 0;		/* done with the random movement */
		return;
		}
	
	/* Otherwise, just move the object towards ego.  Whew...
	*/
	obj->dir = dir;
}

