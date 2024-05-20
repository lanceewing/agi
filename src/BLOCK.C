/* BLOCK
** Routines dealing with the block() command
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"



STRPTR	Block();
STRPTR	UnBlock();
STRPTR	IgnoreBlocks();
STRPTR	ObserveBlocks();
BOOL		InBlock();



extern	BOOL		blocking;
extern	COORD	ulx, uly, lrx, lry, horizon;
extern	ANIOBJ	*aniObj;





STRPTR
Block(lp)
register	STRPTR	lp;
{
	blocking = TRUE;
	ulx = *lp++;
	uly = *lp++;
	lrx = *lp++;
	lry = *lp++;

	return (lp);
}
	


STRPTR
UnBlock(lp)
STRPTR	lp;
{
	blocking = FALSE;
	return (lp);
}
	


STRPTR
IgnoreBlocks(lp)
register	STRPTR	lp;
{
	aniObj[*lp++].control |= IGNRBLK;
	return (lp);
}
	


STRPTR
ObserveBlocks(lp)
register	STRPTR	lp;
{
	aniObj[*lp++].control &= ~IGNRBLK;
	return (lp);
}




BOOL
InBlock(ox, oy)
register	int	ox, oy;
/* Is the object 'obj' in a block?
*/
{
	return (ox > ulx && ox < lrx && oy > uly && oy < lry);
}

