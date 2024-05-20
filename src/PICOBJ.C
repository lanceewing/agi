/* PICOBJ
** Add an object to the picture, without saving the background.  The object
** is thus a permanent part of the picture
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"
#include	"picobj.h"
#include	"script.h"


STRPTR	LAddToPic();
STRPTR	AddToPicF();
void		AddToPic();



		PICOBJ	picObj;
		ANIOBJ	poAniEntry;

extern	UBYTE	var[];




STRPTR
LAddToPic(lp)
register	STRPTR	lp;
{
	picObj.view = *lp++;
	picObj.loop = *lp++;
	picObj.cel = *lp++;
	picObj.x = *lp++;
	picObj.y = *lp++;
	picObj.pri = *lp++;			/* object priority */
	picObj.pri |= *lp++ << 4;	/* box priority */
	AddToPic();

	return (lp);
}



STRPTR
AddToPicF(lp)
register	STRPTR	lp;
{
	picObj.view = var[*lp++];
	picObj.loop = var[*lp++];
	picObj.cel = var[*lp++];
	picObj.x = var[*lp++];
	picObj.y = var[*lp++];
	picObj.pri = var[*lp++];			/* object priority */
	picObj.pri |= var[*lp++] << 4;	/* box priority */
	AddToPic();

	return (lp);
}






void
AddToPic()
/* Add a view to the picture.
*/
{

	AddScript(S_ADDPIC, 0);
	AddScript(picObj.view, picObj.loop);
	AddScript(picObj.cel, picObj.x);
	AddScript(picObj.y, picObj.pri);

	SetView(&poAniEntry, picObj.view);
	SetLoop(&poAniEntry, picObj.loop);
	SetCel(&poAniEntry, picObj.cel);
	poAniEntry.prevcel = poAniEntry.celptr;
	
	/* Get the pointer to the object and save information about
	** it in 'addobj'
	*/
	poAniEntry.x = poAniEntry.prevx = picObj.x;
	poAniEntry.y = poAniEntry.prevy = picObj.y;
	poAniEntry.control = IGNRHRZ | FIXEDPRI | IGNROBJ;
	
	poAniEntry.pri = 15;	/* set to 15 to ignore priority lines */
	FindPosn(&poAniEntry);	/* get a position for object */
	if ((picObj.pri & 0x0f) == 0)
		poAniEntry.control = IGNRHRZ;
	poAniEntry.pri = picObj.pri;
	RestoreAll();			/* restore all backgrounds */
	ObjInPic(&poAniEntry);	/* put the object in the picture */
	AllInBack();
	Obj2Scrn(&poAniEntry);
}

