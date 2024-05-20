/* MOTION
** Handle various object motion actions
**
**	compile: MWC
**
** Change History:
** 07/04/87  16:14:48	JAS
**	LMoveObj() && MoveObjF() now call MoveTo to set ego's (or another object's)
**	direction.
** 25/03/87  11:13:49	JAS
**	Removed all references to egoDir.
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"

STRPTR	LMoveObj();
STRPTR	MoveObjF();
STRPTR	FollowEgo();
STRPTR	StartWander();
STRPTR	StopMotion();
STRPTR	StartMotion();
STRPTR	NormalMotion();
STRPTR	SetStepSize();
STRPTR	SetStepTime();
STRPTR	SetObjDir();
STRPTR	GetObjDir();
STRPTR	ProgramControl();
STRPTR	PlayerControl();

extern	UBYTE	var[];
extern	BOOL		userCtrl;
extern	ANIOBJ	*aniObj;





STRPTR
LMoveObj(lp)
register	STRPTR	lp;
/* Set up for moving an object to a given coordinate
*/
{
	register	ANIOBJ	*op;
			WORD		i;

	op = aniObj + *lp++;
	op->motion = MOVETO;
	op->parms[0] = *lp++;			/* x coord to move to */
	op->parms[1] = *lp++;			/* y coord to move to */
	op->parms[2] = op->stepsize;
	if ((i = *lp++) != 0)
		op->stepsize = i;
	Reset(op->parms[3] = *lp++);		/* flag to set when done */
	op->control |= UPDATE;
	if (op == ego)
		userCtrl = FALSE;
	MoveTo(op);

	return (lp);
}



STRPTR
MoveObjF(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;
			WORD		i;

	op = aniObj + *lp++;
	op->motion = MOVETO;
	op->parms[0] = var[*lp++];		/* x coord to move to */
	op->parms[1] = var[*lp++];		/* y coord to move to */
	op->parms[2] = op->stepsize;
	if ((i = var[*lp++]) != 0)
		op->stepsize = i;
	Reset(op->parms[3] = *lp++);		/* flag to set when done */
	op->control |= UPDATE;
	if (op == ego)
		userCtrl = FALSE;
	MoveTo(op);

	return (lp);
}





STRPTR
FollowEgo(lp)
register	STRPTR	lp;
/* Set up for an object to follow ego.
*/
{
	register	ANIOBJ	*op;
 			WORD		i;

	op = aniObj + *lp++;
	op->motion = FOLLOW;

	/* distance from ego which is considered to be completion of
	** the motion is the larger of the object's stepsize and
	** the parameter
	*/
	i = *lp++;
	op->parms[0] = (i > op->stepsize)? i : op->stepsize;

	Reset(op->parms[1] = *lp++);		/* flag to set at completion */
	op->parms[2] = -1;				/* 'follow' routine expects this */
	op->control |= UPDATE;

	return (lp);
}




STRPTR
StartWander(lp)
register	STRPTR	lp;
/* Set up for a wandering object
*/
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	if (op == ego)
		userCtrl = FALSE;
	op->motion = WANDER;
	op->control |= UPDATE;

	return (lp);
}



STRPTR
NormalMotion(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->motion = NORMAL;
	return (lp);
}



STRPTR
StopMotion(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->dir = 0;
	op->motion = NORMAL;
	if (op == ego) {
		var[EGODIR] = 0;			/* stop ego */
	  	userCtrl = FALSE;
	  	}
	return (lp);
}
	


STRPTR
StartMotion(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->motion = NORMAL;
	if (op == ego) {
		var[EGODIR] = 0;		/* clear ego's direction */
		userCtrl = TRUE;
		}
	return (lp);
}



STRPTR
SetStepSize(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->stepsize = var[*lp++];
	return (lp);
}



STRPTR
SetStepTime(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->movefreq = op->moveclk = var[*lp++];
	return (lp);
}



STRPTR
SetObjDir(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*obj;

	obj = aniObj + *lp++;
	obj->dir = var[*lp++];
	return (lp);
}



STRPTR
GetObjDir(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*obj;

	obj = aniObj + *lp++;
	var[*lp++] = obj->dir;
	return (lp);
}



STRPTR
ProgramControl(lp)
STRPTR	lp;
{
	userCtrl = FALSE;
	return (lp);
}



STRPTR
PlayerControl(lp)
STRPTR	lp;
{
	userCtrl = TRUE;
	ego->motion = NORMAL;
	return (lp);
}


