/* PRIORITY
** Priority actions on animated objects
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"


STRPTR	SetPriority();
STRPTR	ReleasePriority();
STRPTR	GetPriority();
STRPTR	FSetPriority();



extern	ANIOBJ	*aniObj;
extern	UBYTE	var[];



STRPTR
SetPriority(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->control |= FIXEDPRI;
	op->pri = *lp++;

	return (lp);
}


STRPTR
ReleasePriority(lp)
register	STRPTR	lp;
{
	aniObj[*lp++].control &= ~FIXEDPRI;
	return (lp);
}
			

STRPTR
GetPriority(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	var[*lp++] = op->pri;

	return (lp);
}


STRPTR
FSetPriority(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->control |= FIXEDPRI;
	op->pri = var[*lp++];

	return (lp);
}

