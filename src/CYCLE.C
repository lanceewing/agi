/* CYCLE
** Set up routines for the various cycling types of objects
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"


STRPTR	NormalCycle();
STRPTR	EndOfLoop();
STRPTR	ReverseCycle();
STRPTR	ReverseLoop();
STRPTR	SetCycleFreq();
STRPTR	StopCycling();
STRPTR	StartCycling();


extern	ANIOBJ	*aniObj;
extern	UBYTE	var[];



STRPTR
NormalCycle(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->cycle = NORMAL;
	op->control |= CYCLE;
	return (lp);
}





STRPTR
EndOfLoop(lp)
register	STRPTR	lp;
/* Set up for cycling an object from the first cel of the current loop to
** the last, setting a flag when done.
*/
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->cycle = ENDLOOP;
	op->control |= UPDATE | CYCLE | NOADVANC;
	Reset(op->parms[0] = *lp++);

	return (lp);
}



STRPTR
ReverseCycle(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->cycle = REVERSE;
	op->control |= CYCLE;
	return (lp);
}



STRPTR
ReverseLoop(lp)
register	STRPTR	lp;
/* Set up for cycling an object from the last cel of the current loop to the
** first, setting a flag on completion.
*/
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->cycle = RVRSLOOP;
	op->control |= UPDATE | CYCLE | NOADVANC;
	Reset(op->parms[0] = *lp++);

	return (lp);
}



STRPTR
SetCycleFreq(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;

	op = aniObj + *lp++;
	op->cycleclk = op->cyclfreq = var[*lp++];
	return (lp);
}



STRPTR
StopCycling(lp)
register	STRPTR	lp;
{
	aniObj[*lp++].control &= ~CYCLE;
	return (lp);
}



STRPTR
StartCycling(lp)
register	STRPTR	lp;
{
	aniObj[*lp++].control |= CYCLE;
	return (lp);
}


