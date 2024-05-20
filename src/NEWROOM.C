/* NEWROOM
** Change rooms in the game
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"


STRPTR	LNewRoom();
STRPTR	LNewRoomF();
STRPTR	NewRoom();


extern	ANIOBJ	*aniObj, *aniEnd;
extern	UBYTE	var[];
extern	BOOL 	userCtrl, blocking;
extern	COORD	horizon;
extern	COUNT	traceLogs;


extern	VIEWNODE	*FindView();




STRPTR
LNewRoom(lp)
STRPTR	lp;
{
	return (NewRoom(*lp));
}



STRPTR
LNewRoomF(lp)
STRPTR	lp;
{
	return (NewRoom(var[*lp]));
}




STRPTR
NewRoom(n)
WORD		n;
/* Reset the various room-related variables and load the logics for
** room number n
*/
{
	register	ANIOBJ	*obj;

	StopSnd();	/* turn off sound */
	FreeAll();	/* free memory allocated to various objects, logics, etc. */
	ResetEv();	/* flush the event queue */
	InitScript();	/* initialize the script buffer */
	ScriptOn();	/* turn scripting on */
	
	/* Reset stuff for all objects
	*/
	for (obj = aniObj ; obj < aniEnd ; ++obj) {
		obj->control &= ~(ANIMATED | DRAWN);
		obj->control |= UPDATE;
		obj->celptr = NULL;
		obj->viewptr = NULL;
		obj->save = NULL;
		obj->stepsize = obj->cyclfreq = obj->cycleclk
			= obj->moveclk = obj->movefreq = 1;
		}

	/* Reset all lists to their base states
	*/
	ResetLists();

	userCtrl = TRUE;
	blocking = FALSE;
	horizon = HORIZON;			/* reset the horizon to default */
	
	var[PREVROOM] = var[CURROOM];
	var[CURROOM] = n;
	var[OBJHIT] = var[OBJEDGE] = 0;
	var[CURRENT_EGO] = ego->view;
	
	_LLoadLog(n);				/* load the logics for the new room */
	if (traceLogs != 0)
		LoadLog(traceLogs);
	
	/* If ego collided with a border, set his position in the new room to
	** the appropriate edge of the screen
	*/
	switch (var[EGOEDGE]) {
		case TOP:
			ego->y = MAXY;
			break;
		case RIGHT:
			ego->x = MINX;
			break;
		case BOTTOM:
			ego->y = HORIZON + 1;
			break;
		case LEFT:
			ego->x = MAXX + 1 - ego->xsize;
			break;
		}
	var[EGOEDGE] = 0;

	Set(INITLOGS);			/* signal to initialize logics */

	ClearControl();

	DisplayStatusLine();
	DisplayInput();
	return (NULL);			/* signal rescan of logics */
}

