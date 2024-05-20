/* WANDER
** Make an object move in a random manner.
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"
#include	"aniobj.h"

void		Wander();
UWORD	RandmDir();
UWORD	Random();

extern	UBYTE	var[];
extern	ANIOBJ	*aniObj;

#define	dist		parms[0]		/* use parm 0 for distance in current dir */
#define	MAXDIST	50			/* maximum movement distance */
#define	MINDIST	6			/* minimum movement distance */



void
Wander(obj)
register	ANIOBJ	*obj;
/* Make the object pointed to by 'obj' wander randomly
*/
{
	if (obj->dist-- == 0 || obj->control & STOPPED) {
		obj->dir = RandmDir();
		if (obj == ego)
			var[EGODIR] = obj->dir;
		while (obj->dist < MINDIST)
			obj->dist = Random() % (MAXDIST + 1);
		}
}





UWORD
RandmDir()
/* Return a random direction (0 - 8)
*/
{
	return(Random() % 9);
}
