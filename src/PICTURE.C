/* PICTURE
** Draw a picture
**
**	compile: MWC
**
** Change History:
**	17/02/87  15:17:05	JAS
**		Added code to ShowPic() to close a window.
*/


#include	"types.h"
#include	"game.h"
#include	"picture.h"
#include	"script.h"


void		InitPic();
void		ResetPic();
PICNODE	*FindPic();
STRPTR	LLoadPic();
PICNODE	*LoadPic();
STRPTR	LPicture();
void		Picture();
STRPTR	ShowPic();
STRPTR	LOverlayPic();
void		OverlayPic();
STRPTR	LDiscardPic();
void		DiscardPic();
STRPTR	SetUpperLeft();


		PICNODE	picList;
		PICNODE	*prevPic;

		BOOL		picVisible;


extern	WORD		picNo;
extern	STRPTR	picPtr;
extern	UBYTE	var[];


extern	STRPTR	MustGet();



void
InitPic()
{
	picList.next = NULL;
}


void
ResetPic()
{
	InitPic();
}




PICNODE	*
FindPic(p)
WORD		p;
{
	register	PICNODE	*pn, *pp;

	for (pn = picList.next, pp = &picList ;
		pn != NULL && pn->num != p ;
		pp = pn, pn = pn->next)
			;

	prevPic = pp;
	return (pn);
}




STRPTR
LLoadPic(lp)
STRPTR	lp;
{
	LoadPic(var[*lp++]);
	return (lp);
}




PICNODE	*
LoadPic(p)
register	WORD		p;
{
	register	PICNODE	*pp;

	if ((pp = FindPic(p)) != NULL)
		return (pp);

	/* Restore all backgrounds and toss all save areas
	*/
	RestoreAll();

	AddScript(S_LOADPIC, p);
	if (prevPic == NULL)
		pp = &picList;
	else {
		prevPic->next = pp = (PICNODE *) GetMem(sizeof(PICNODE));
		pp->next = NULL;
		}

	pp->num = p;
	if ((pp->ptr = MustGet(PicSeg(p), NULL)) == NULL)
		return (NULL);

	/* Reinsert all objects in background
	*/
	AllInBack();
	return (pp);
}



STRPTR
LPicture(lp)
STRPTR	lp;
{
	Picture(var[*lp++]);
	return (lp);
}



void
Picture(p)
register	WORD		p;
{
	register	PICNODE	*pp;

	picNo = p;

	if ((pp = FindPic(p)) == NULL)
		Error(18, p);

	AddScript(S_DRAWPIC, p);
	picPtr = pp->ptr;
	RestoreAll();
	DrawPic();
	AllInBack();

	picVisible = FALSE;
}



STRPTR
LOverlayPic(lp)
register	STRPTR	lp;
{
	OverlayPic(var[*lp++]);
	return (lp);
}



void
OverlayPic(p)
register	WORD		p;
{
	register	PICNODE	*pp;

	if ((pp = FindPic(p)) == NULL)
		Error(18, p);
	AddScript(S_OVERLAYPIC, p);

	picPtr = pp->ptr;

	RestoreAll();
	DrwPicNC();
	AllInBack();
	AllToScreen();

	picVisible = FALSE;
}




STRPTR
ShowPic(lp)
STRPTR	lp;
{
	Reset(LEAVE_WIN);
	CloseWindow(NULL);

	BackToScrn();

	picVisible = TRUE;
	return (lp);
}




STRPTR
LDiscardPic(lp)
register	STRPTR	lp;
{
	DiscardPic(var[*lp++]);
	return (lp);
}



void
DiscardPic(p)
register	WORD		p;
/* Discard the memory allocated for picture 'p'.
*/
{
	register	PICNODE	*pp;
	
	if ((pp = FindPic(p)) == NULL)
		Error(21, p);

	AddScript(S_DSCRDPIC, p);
	prevPic->next = NULL;

	/* Restore all backgrounds, toss all save areas, and toss the picture
	*/
	RestoreAll();
	SetMemPtr(pp);

	/* Reinsert all objects in background
	*/
	AllInBack();
	MemLeft();
}



STRPTR
SetUpperLeft(lp)
STRPTR	lp;
/* Eat the parameters.  This only is used on the Apple.
*/
{
	return (lp + 2);
}

