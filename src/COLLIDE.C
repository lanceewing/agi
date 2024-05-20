/* COLLIDE
** Check for object collisions
**
**	compile: MWC
**
** CHANGE HISTORY:
**	06/04/87  10:52:38	JAS
**		Fixed Distance() so that it doesn't wrap if the combined x & y
**		distances are too great.
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"

BOOL		Collide();
STRPTR	IgnoreObjs();
STRPTR	ObserveObjs();
STRPTR	Distance();


extern	ANIOBJ	*aniObj, *aniEnd;
extern	UBYTE	var[];




BOOL
Collide(obj)
register	ANIOBJ	*obj;
/* Return TRUE if object 'obj' has collided with another object,
** FALSE if not
*/
{
	register	ANIOBJ	*otherObj;
	
	if (obj->control & IGNROBJ)		/* object won't collide with anyone */
		return(FALSE);
	
	for (otherObj = aniObj ; otherObj < aniEnd ; ++otherObj) {
		/* Collision with another object if:
		**	- other object is animated and drawn
		**	- other object is not ignoring objects
		**	- other object is not this object
		**	- the two objects have overlapping baselines
		*/
		if ((otherObj->control & (ANIMATED | DRAWN)) == (ANIMATED | DRAWN)
	        && !(otherObj->control & IGNROBJ)
	        && (obj->num != otherObj->num)
	        && (obj->x + obj->xsize >= otherObj->x)
	        && (obj->x <= otherObj->x + otherObj->xsize))
			/* At this point, the two objects have overlapping
			** x coordinates. A collision has occurred if they have
			** the same y coordinate or if the object in question has
			** moved across the other object in the last animation cycle
			*/
			if ((obj->y == otherObj->y)
	                  || (obj->y > otherObj->y && obj->prevy < otherObj->prevy)
	                  || (obj->y < otherObj->y && obj->prevy > otherObj->prevy))
					return(TRUE);
		}
	return(FALSE);
}


	
STRPTR
IgnoreObjs(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->control |= IGNROBJ;

	return (lp);
}

	

STRPTR
ObserveObjs(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->control &= ~IGNROBJ;

	return (lp);
}



STRPTR
Distance(lp)
STRPTR	lp;
{
	register	ANIOBJ	*op, *obj;
			COUNT	dist;
			

	op = aniObj + *lp++;
	obj = aniObj + *lp++;
	if ((op->control & DRAWN) == 0 || (obj->control & DRAWN) == 0)
		var[*lp++] = MAXVAR;
	else {
		dist = abs((op->x + op->xsize/2) - (obj->x + obj->xsize/2))
					+ abs(op->y - obj->y);
		var[*lp++] = (dist > 254)? 254 : dist;
		}

	return (lp);
}

