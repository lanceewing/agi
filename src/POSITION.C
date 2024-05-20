/* POSITION
** Routines to handle actions relating to object positions.
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"
#include	"aniobj.h"


STRPTR	Position();
STRPTR	FlagPosn();
STRPTR	GetPosn();
STRPTR	Reposition();
STRPTR	RepositionTo();
STRPTR	VRepositionTo();
STRPTR	OnWater();
STRPTR	OnLand();
STRPTR	OnAnything();
STRPTR	SetHorizon();
STRPTR	IgnoreHorizon();
STRPTR	ObserveHorizon();



extern	UBYTE	var[];
extern	ANIOBJ	*aniObj;
extern	COORD	horizon;





STRPTR
Position(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->x = op->prevx = *lp++;
	op->y = op->prevy = *lp++;
	
	return (lp);
}


STRPTR
FlagPosn(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->x = op->prevx = var[*lp++];
	op->y = op->prevy = var[*lp++];

	return (lp);
}
	

STRPTR
GetPosn(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	var[*lp++] = op->x;
	var[*lp++] = op->y;
	
	return (lp);
}


STRPTR
Reposition(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;
			WORD		delta;

	op = aniObj + *lp++;

	/* Set flag to signal that these positions should not be changed in the
	** next animation cycle.
	*/
	op->control |= REPOS;
	if ((delta = (BYTE) var[*lp++]) < 0 && op->x < -delta)
		op->x = 0;
	else
		op->x += delta;
	if ((delta = (BYTE) var[*lp++]) < 0 && op->y < -delta)
		op->y = 0;
	else
		op->y += delta;

	FindPosn(op);					/* make sure that this position is OK */

	return (lp);
}



STRPTR
RepositionTo(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->x = *lp++;
	op->y = *lp++;
	op->control |= REPOS;

	FindPosn(op);					/* make sure that this position is OK */

	return (lp);
}



STRPTR
VRepositionTo(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->x = var[*lp++];
	op->y = var[*lp++];
	op->control |= REPOS;

	FindPosn(op);					/* make sure that this position is OK */

	return (lp);
}





STRPTR
OnWater(lp)
register	STRPTR	lp;
{
	aniObj[*lp++].control |= PRICTRL1;
	return (lp);
}



STRPTR
OnLand(lp)
register	STRPTR	lp;
{
	aniObj[*lp++].control |= PRICTRL2;
	return (lp);
}
	


STRPTR
OnAnything(lp)
register	STRPTR	lp;
{
	aniObj[*lp++].control &= ~(PRICTRL2 | PRICTRL1);
	return (lp);
}



STRPTR
SetHorizon(lp)
register	STRPTR	lp;
{
	horizon = *lp++;
	return (lp);
}
	


STRPTR
IgnoreHorizon(lp)
register	STRPTR	lp;
{
	aniObj[*lp++].control |= IGNRHRZ;
	return (lp);
}



STRPTR
ObserveHorizon(lp)
register	STRPTR	lp;
{
	aniObj[*lp++].control &= ~IGNRHRZ;
	return (lp);
}

