/* ANIMATE
** Handle one cycle of animation in an adventure game
**
**	compile: MWC
**
** Change History:
**	25/03/87  11:11:37	JAS
**		Removed all references to egoDir.
**	13/03/87	12:38:00	DCI
**		Fixed ChkBlock() to also set egoDir to zero when Ego is blocked.
**	26/02/87  09:50:39	JAS
**		Fixed GetObjDirs() to only get directions of animated objects
**		(oops).
**	17/02/87  15:38:03	JAS
**		No longer call ObjDir() in Animate().  A call is made to
**		GetObjDirs() before starting the logic scan in main().
**		This allows ego's direction to be known to the logics
**		even when ego is on a move.obj().
**
**		ObjDir() is now only called when obj->moveclk == 1, i.e.
**		when the object is ready to move.  This fixes (I hope) the
**		problem with objects with a movefreq > 1 on a wander() thinking
**		that they're blocked and picking a new direction on those
**		cycles in which they're not moving.
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"
#include	"script.h"
#include	"objlist.h"


STRPTR	LAnimateObj();
void		AnimateObj();
STRPTR	UnAnimateAll();
void		Animate();
void		GetObjDirs();
void		ObjDir();
void		ChkBlock();




extern	ANIOBJ	*aniObj, *aniEnd;
extern	BOOL		blocking;
extern	OBJLIST	*updateList;
extern	UBYTE	var[];


extern	STRPTR	GetMem();
extern	OBJLIST	*MakeUpdList();





STRPTR
LAnimateObj(lp)
register	STRPTR	lp;
{
	AnimateObj(*lp++);
	return (lp);
}




void
AnimateObj(n)
register	WORD		n;
{
	register	ANIOBJ	*op;

	if ((op = aniObj + n) >= aniEnd)
		Error(13, n);
	if (op->control & ANIMATED)
		return;

	op->control = ANIMATED | UPDATE | CYCLE;
	op->motion = NORMAL;
	op->cycle = NORMAL;
	op->dir = 0;
}




STRPTR
UnAnimateAll(lp)
STRPTR	lp;
{
	register	ANIOBJ	*obj;

	/* Restore all backgrounds and free background memory
	*/
	RestoreAll();

	/* Unanimate and erase all objects
	*/
	for (obj = aniObj ; obj < aniEnd ; ++obj)
		obj->control &= ~(ANIMATED | DRAWN);

	return (lp);
}


	

void
Animate()
{
/* object views -- same, right, left, front, back */
#define		S	4
#define		R	0
#define		L	1
#define		F	2
#define		B	3

	register	ANIOBJ	*obj;		/* currently animated object */
	register	WORD		updFlag;		/* does anything need updating? */
			UBYTE	newLoop;
	static	UBYTE	twoLoop[] = {S, S, R, R, R, S, L, L, L};
	static	UBYTE	fourLoop[] = {S, B, R, R, R, F, L, L, L};

	updFlag = 0;					/* initialize update flag */

	/* Loop through the animated objects, pointing to the next cel for all
	** objects which are to be updated
	*/
	for (obj = aniObj ; obj < aniEnd ; ++obj) {
		if ((obj->control & (ANIMATED | UPDATE | DRAWN)) == (ANIMATED | UPDATE | DRAWN)) {
			/* Increment the number of objects to update
			*/
			++updFlag;

			/* Get the appropriate loop based on the current direction
			*/
			newLoop = S;
			if ((obj->control & FIXEDLOOP) == 0) {
				if (obj->loopcnt == 2 || obj->loopcnt == 3)
					newLoop = twoLoop[obj->dir];
				else if (obj->loopcnt == 4)
					newLoop = fourLoop[obj->dir];
				}

			/* If the object is to move in this cycle and the loop
			** has changed, point to the new loop.
			*/
			if (obj->moveclk == 1 && newLoop != S && obj->loop != newLoop)
				SetLoop(obj, newLoop);

			/* If it is time to cycle the object, advance its cel
			*/
			if ((obj->control & CYCLE) &&
				obj->cycleclk && --obj->cycleclk == 0) {
					AdvanceCel(obj);
					obj->cycleclk = obj->cyclfreq;
				}
			}
		}

	/* If at least one object is to be updated, update all objects
	*/
	if (updFlag) {
		/* Restore backgrounds of updating objects
		*/
		RestoreObjList(&updateList);

		/* Move all the objects
		*/
		MoveObjs();

		/* Re-make the update list and display all objects in it.
		*/
		ListInBack(MakeUpdList());
		ListToScreen(&updateList);

		/* Clear the 'must be on water or land' bits for ego
		*/
		ego->control &= ~(PRICTRL1 | PRICTRL2);
		}
}




void
GetObjDirs()
{
	register	ANIOBJ	*obj;

	for (obj = aniObj ; obj < aniEnd ; ++obj)
		if ((obj->control & (ANIMATED | UPDATE | DRAWN)) ==
				(ANIMATED | UPDATE | DRAWN)
			&& obj->moveclk == 1)
				ObjDir(obj);
}




void
ObjDir(obj)
register	ANIOBJ	*obj;
/* Set the direction for the current object based on its motion type
*/
{

	switch (obj->motion) {
		case WANDER:
			Wander(obj);
			break;
		case FOLLOW:
			Follow(obj);
			break;
		case MOVETO:
			MoveTo(obj);
			break;
		}

	/* If no blocks are in effect, clear the 'blocked' flag.  Otherwise,
	** if object must observe blocks, check for blocking
	*/
	if (!blocking)
		obj->control &= ~BLOCKED;
	else if ((obj->control & IGNRBLK) == 0 && obj->dir != 0)
		ChkBlock(obj);
}





void
ChkBlock(obj)
register	ANIOBJ	*obj;	/* currently animated object */
/* A block is in effect and the object must observe blocks -- check to see
** if the object can move in its current direction
*/
{
	BOOL		objInBlock;
	int		ox, oy;

	/* Get obj coord into temp vars and determine if the object is
	** currently within the block.
	*/
	ox = obj->x;
	oy = obj->y;
	objInBlock = InBlock(ox, oy);

	/* Get object coordinate after moving
	*/
	switch (obj->dir) {
		case 1:
			oy -= obj->stepsize;
			break;
		case 2:
			ox += obj->stepsize;
			oy -= obj->stepsize;
			break;
		case 3:
			ox += obj->stepsize;
			break;
		case 4:
			ox += obj->stepsize;
			oy += obj->stepsize;
			break;
		case 5:
			oy += obj->stepsize;
			break;
		case 6:
			ox -= obj->stepsize;
			oy += obj->stepsize;
			break;
		case 7:
			ox -= obj->stepsize;
			break;
		case 8:
			ox -= obj->stepsize;
			oy -= obj->stepsize;
			break;
		}

	/* If moving the object will not change its 'in block' status, let
	** it move.
	*/
	if (objInBlock == InBlock(ox, oy))
		obj->control &= ~BLOCKED;
	else {
		obj->control |= BLOCKED;
		obj->dir = 0;
		/* When Ego is the blocked object also set ego's direction
		 * to zero.
		 */
		if (obj == ego)
			var[EGODIR] = 0;
		}
}

