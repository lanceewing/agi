/* ADVANCEL
** Determine the next cel to display for objects which cycle.
**
**	compile: MWC
**
*/



#include	"types.h"
#include	"game.h"
#include	"aniobj.h"


void		AdvanceCel();
STRPTR	FixLoop();
STRPTR	ReleaseLoop();


extern	ANIOBJ	*aniObj;




void
AdvanceCel(obj)
register	ANIOBJ	*obj;
/* Determine which cel of an object to display next
*/
{
	register		UBYTE		theCel;
				UBYTE		lastCel;


	if (obj->control & NOADVANC) {
		obj->control &= ~NOADVANC;
		return;
		}

	/* Advance to the next cel in the loop
	*/
	theCel = obj->cel;
	lastCel = obj->celcnt - 1;
	switch (obj->cycle) {
		case NORMAL:
			/* Move to the next sequential cel
			*/
			if (++theCel > lastCel)
				theCel = 0;
			break;

		case ENDLOOP:
			/* Advance to the end of the loop, set flag in parms[0]
			** when done
			*/
			if (theCel >= lastCel || ++theCel == lastCel) {
				Set(obj->parms[0]);			/* set flag */
				obj->control &= ~CYCLE;		/* done cycling object */
				obj->dir = 0;
				obj->cycle = NORMAL;
				}
			break;

		case RVRSLOOP:
			/* Move backwards, celwise, until beginning of loop, then
			** set flag
			*/
			if (theCel == 0 || --theCel == 0) {
				Set(obj->parms[0]);			/* set flag */
				obj->control &= ~CYCLE;
				obj->dir = 0;
				obj->cycle = NORMAL;
				}
			break;

		case REVERSE:
			/* Cycle continually, but from end of loop to beginning
			*/
			if (theCel > 0)
				--theCel;
			else
				theCel = lastCel;
			break;
		}

	/* Get pointer to the new cel and set cel dimensions
	*/
	SetCel(obj, theCel);
}



STRPTR
FixLoop(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*obj;

	obj = aniObj + *lp++;
	obj->control |= FIXEDLOOP;
	return (lp);
}




STRPTR
ReleaseLoop(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*obj;

	obj = aniObj + *lp++;
	obj->control &= ~FIXEDLOOP;
	return (lp);
}

