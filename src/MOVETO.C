/* MOVETO
** Move an object to a given position
**
**	compile: MWC
**
** Change History:
** 25/03/87  11:16:08	JAS
**	Removed references to egoDir.
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"


void		MoveTo();
void		EndMoveObj();
WORD		MoveDir();
WORD		DirIndex();

extern	UBYTE	var[];
extern	ANIOBJ	*aniObj;



#define	xt		parms[0]		/* target x coordinate */
#define	yt		parms[1]		/* target y coordinate */
#define	oldStep	parms[2]		/* old stepsize for object */
#define	endFlag	parms[3]		/* flag to set when object reaches target */



extern	BOOL		userCtrl;




void
MoveTo(obj)
register	ANIOBJ	*obj;
/* Move the object pointed to by 'obj' toward the given coordinate
*/
{

	/* Get the direction to move
	*/
	obj->dir = MoveDir(obj->x, obj->y, obj->xt, obj->yt, obj->stepsize);

	/* If ego, set var[EGODIR]
	 */
	if (obj == ego)
		var[EGODIR] = obj->dir;

	/* If 0, signal completion
	*/
	if (obj->dir == 0)
		EndMoveObj(obj);
}



void
EndMoveObj(obj)
register	ANIOBJ	*obj;
{
	obj->stepsize = obj->oldStep;	/* restore old step size */
	Set(obj->endFlag);			/* set flag indicating completion */
	obj->motion = NORMAL;		/* set normal motion */
	if (obj == ego) {
		userCtrl = TRUE;
		var[EGODIR] = 0;
		}
}




WORD
MoveDir(oldx, oldy, newx, newy, delta)
register	COORD	delta;
		COORD	oldx, oldy, newx, newy;
/* Return the direction from (oldx, oldy) to (newx, newy).  If the
** object is within 'delta' of the position in both directions, return 0
*/
{
	static	WORD		newdir[3][3] = { {8, 1, 2}, {7, 0, 3}, {6, 5, 4} };

	return (newdir[DirIndex(newy-oldy, delta)][DirIndex(newx-oldx, delta)]);
}




WORD
DirIndex(d, delta)
register	WORD		d;
		WORD		delta;
/* Return 0, 1, or 2 depending on whether the difference between coords, d,
** indicates that the coordinate should decrease, stay the same, or increase.
** The return value is used as one of the indeces into 'newdir' above.
*/
{
	register	WORD		i;

	if (d <= -delta)
		i = 0;
	else if (d >= delta)
		i = 2;
	else
		i = 1;

	return (i);
}
