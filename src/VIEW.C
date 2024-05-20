/* VIEW
** Routines to handle view setting, loading, etc.
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"
#include	"aniobj.h"
#include	"script.h"


void			InitView();
void			ResetView();
VIEWNODE		*FindView();
STRPTR		LLoadView();
STRPTR		LoadViewF();
VIEWNODE		*LoadView();
BOOL			TryLoadView();
STRPTR		LSetView();
STRPTR		SetViewF();
void			SetView();
STRPTR		LSetLoop();
STRPTR		SetLoopF();
void			SetLoop();
void			_SetLoop();
STRPTR		LSetCel();
STRPTR		SetCelF();
void			SetCel();
void			_SetCel();
STRPTR		LastCel();
STRPTR		CurrentCel();
STRPTR		CurrentLoop();
STRPTR		CurrentView();
STRPTR		NumberLoops();
STRPTR		LDiscardView();
STRPTR		VDiscardView();
void			DiscardView();




VIEWNODE		viewList;
VIEWNODE		*prevView;

extern	ANIOBJ	*aniObj, *aniEnd;
extern	COORD	horizon;
extern	UBYTE	var[];

extern	STRPTR	MustGet();





void
InitView()
{
	viewList.next = NULL;
}


void
ResetView()
{
	InitView();
}



VIEWNODE	*
FindView(v)
register	WORD		v;
/* Return a pointer to view number 'v', or NULL if view isn't loaded
*/
{
	register	VIEWNODE	*vn, *pv;

	for (vn = viewList.next, pv = &viewList ;
		vn != NULL && vn->num != v ;
		pv = vn, vn = vn->next)
			;

	prevView = pv;
	return (vn);
}




STRPTR
LLoadView(lp)
register	STRPTR	lp;
{
	LoadView(*lp++, FALSE);
	return (lp);
}


STRPTR
LoadViewF(lp)
register	STRPTR	lp;
{
	LoadView(var[*lp++], FALSE);
	return (lp);
}



VIEWNODE	*
LoadView(v, forceLoad)
register	WORD		v;
		BOOL		forceLoad;
/* If view v is not already present, or if forceLoad is TRUE, load it.
** Otherwise return.
*/
{
	register	VIEWNODE	*vp;

	/* If the view is present and we are not forcing a load, just return
	*/
	if ((vp = FindView(v)) != NULL && !forceLoad)
		return (vp);

	/* Restore all backgrounds and toss all save areas
	*/
	RestoreAll();

	if (vp == NULL) {
		AddScript(S_LOADVIEW, v);
		prevView->next = vp = (VIEWNODE *) GetMem(sizeof(VIEWNODE));
		vp->next = NULL;
		vp->num = v;
		vp->ptr = NULL;
		}

	if ((vp->ptr = (VIEW *) MustGet(ViewSeg(vp->num), vp->ptr)) == NULL)
		return (NULL);
	XlatObjC(vp->ptr);

	/* Reinsert all objects in background
	*/
	AllInBack();
	return (vp);
}





STRPTR
LSetView(lp)
register	STRPTR	lp;
{
	register	WORD		n;

	n = *lp++;
	SetView(aniObj+n, *lp++);
	return (lp);
}


STRPTR
SetViewF(lp)
register	STRPTR	lp;
{
	register	WORD		n;

	n = *lp++;
	SetView(aniObj+n, var[*lp++]);
	return (lp);
}



void
SetView(obj, v)
register	ANIOBJ	*obj;
register	WORD		v;
/* Set the view for object to view v.
*/
{
	register	VIEW		*vp;
			VIEWNODE	*vn;

	if ((vn = FindView(v)) == NULL)
		Error(3, v);

	vp = obj->viewptr = vn->ptr;
	obj->view = v;

	/* Get characteristics of this view
	*/
	obj->loopcnt = vp->numloops;

	/* If the current loop is greater than the number of loops for the view,
	** set the loop number to 0.  Otherwise, leave it alone.
	*/
	SetLoop(obj, (obj->loop >= obj->loopcnt)? 0 : obj->loop);

}




STRPTR
LSetLoop(lp)
register	STRPTR	lp;
{
	register	WORD		n;

	n = *lp++;
	SetLoop(aniObj+n, *lp++);
	return (lp);
}


STRPTR
SetLoopF(lp)
register	STRPTR	lp;
{
	register	WORD		n;

	n = *lp++;
	SetLoop(aniObj+n, var[*lp++]);
	return (lp);
}



void
SetLoop(obj, l)
register	ANIOBJ	*obj;
register	WORD		l;
/* Set the loop in the current view for object n to l.
*/
{
	WORD		n;

	n = obj - aniObj;
	if (obj->viewptr == NULL)
		Error(6, n);
	if (l > obj->loopcnt)
		Error(5, n);

	_SetLoop(obj, l);
	SetCel(obj, obj->cel);
}


void
_SetLoop(obj, l)
register	ANIOBJ	*obj;
register	WORD		l;
{
	obj->loop = l;				/* set the loop number */

	/* Get the pointer to the loop
	*/
	obj->loopptr = (LOOP *)
			((STRPTR) obj->viewptr + W8086(obj->viewptr->loopofs[l]));

	obj->celcnt = obj->loopptr->numcels;	/* set the cel count */

	/* If the current cel # is greater than the cel count for this loop, set
	** it to 0, otherwise leave it alone
	*/
	if (obj->cel >= obj->celcnt)
		obj->cel = 0;
}




STRPTR
LSetCel(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*obj;

	obj = aniObj + *lp++;
	SetCel(obj, *lp++);
	obj->control &= ~NOADVANC;
	return (lp);
}


STRPTR
SetCelF(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*obj;

	obj = aniObj + *lp++;
	SetCel(obj, var[*lp++]);
	obj->control &= ~NOADVANC;
	return (lp);
}



void
SetCel(obj, c)
register	ANIOBJ	*obj;
register	WORD		c;
{
	WORD		n;

	n = obj - aniObj;
	if (obj->viewptr == NULL)
		Error(10, n);
	if (c >= obj->celcnt)
		Error(8, n);

	_SetCel(obj, c);

	/* Make sure that the new cel size doesn't cause a border collision
	*/
	if (obj->x + obj->xsize > MAXX + 1) {
		obj->control |= REPOS;		/* don't let the object move */
		obj->x = MAXX + 1 - obj->xsize;
		}

	if (obj->y - obj->ysize < MINY - 1) {
		obj->control |= REPOS;
		obj->y = MINY - 1 + obj->ysize;
		if (obj->y <= horizon && (obj->control & IGNRHRZ) == 0)
			obj->y = horizon + 1;
		}
}



void
_SetCel(obj, c)
register	ANIOBJ	*obj;
		WORD		c;
{
	register	CEL		*cp;

	obj->cel = c;				/* set the cel number */

	/* Get the pointer to the cel
	*/
	cp = obj->celptr = (CEL *)
			((STRPTR) obj->loopptr + W8086(obj->loopptr->celofs[c]));

	/* Get the x and y dimensions of the cel
	*/
	obj->xsize = cp->xdim;
	obj->ysize = cp->ydim;
}



STRPTR
LastCel(lp)
register	STRPTR	lp;
{
	register	WORD		n;

	n = (aniObj + *lp++)->loopptr->numcels - 1;
	var[*lp++] = n;
	return (lp);
}



STRPTR
CurrentCel(lp)
register	STRPTR	lp;
{
	register	WORD		n;

	n = (aniObj + *lp++)->cel;
	var[*lp++] = n;
	return (lp);
}



STRPTR
CurrentLoop(lp)
register	STRPTR	lp;
{
	register	WORD		n;

	n = (aniObj + *lp++)->loop;
	var[*lp++] = n;
	return (lp);
}



STRPTR
CurrentView(lp)
register	STRPTR	lp;
{
	register	WORD		n;

	n = (aniObj + *lp++)->view;
	var[*lp++] = n;
	return (lp);
}


STRPTR
NumberLoops(lp)
register	STRPTR	lp;
{
	register	WORD		n;

	n = (aniObj + *lp++)->loopcnt;
	var[*lp++] = n;
	return (lp);
}



STRPTR
LDiscardView(lp)
register	STRPTR	lp;
{
	DiscardView(*lp++);
	return (lp);
}



STRPTR
VDiscardView(lp)
register	STRPTR	lp;
{
	DiscardView(var[*lp++]);
	return (lp);
}




void
DiscardView(v)
WORD		v;
{
	register	VIEWNODE	*vp;
	
	if ((vp = FindView(v)) == NULL)
		Error(1, v);

	AddScript(S_DSCRDVIEW, v);
	prevView->next = NULL;

	/* Restore all backgrounds, toss all save areas, then toss the view.
	*/
	RestoreAll();
	SetMemPtr(vp);
	
	/* Reinsert all objects in background
	*/
	AllInBack();
	MemLeft();
}

