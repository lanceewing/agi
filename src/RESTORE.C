/* RESTORE
 * Restore the game to a given state, based on the game script
 *
 *	compile: MWC
 *
 * Change History
 *	87.05.27 15:33	JAS
 *		Moved the SetView of an object's view outside the 'if animated'
 *		test in the object re-draw loop.
 */

#include	"types.h"
#include	"game.h"
#include	"aniobj.h"
#include	"picobj.h"
#include	"script.h"
#include	"logic.h"


void		Restore();


extern	BOOL		picVisible;
extern	ANIOBJ	*aniObj, *aniEnd;
extern	PICOBJ	picObj;
extern	WORD		picNo;

extern	SCRIPT	*NextScript();
extern	STRPTR	GetMem();
extern	LOGNODE	*LoadLog();




void
Restore()
/* Reload the necessary things based on the current (i.e. restored)
 * variables
 */
{
	register	SCRIPT	*scriptPtr;
	register	UWORD	i;
			ANIOBJ	*obj;
			UWORD	objCtrl;

	/* Make certain `PlayIt' is not called during timer interrupt
	 */
	StopSnd();

	/* Reset all lists to their base states
	 */
	ResetLists();

	/* Turn off scripting, so as to prevent the restore from adding
	 * to the script.
	 */
	ScriptOff();

	/* 'Erase' all objects, remembering their control bytes. In a memory
	 * conservation move, the control bytes are stored in the object's X
	 * coordinate field which in turn has been saved in the object's number
	 * field. All these fields are restored to their original values when
	 * the objects are redrawn at the end of this subroutine.
	 */
	for (obj = aniObj ; obj < aniEnd ; ++obj) {
		obj->num = obj->x;			/* save X coordinate */
		obj->x = obj->control;		/* save control bytes */
		if (obj->control & ANIMATED) {
			obj->control &= ~DRAWN;
			obj->control |= UPDATE;
			}
		}

	/* Free all allocated memory
	 */
	RestoreAll();
	FreeAll();

	picVisible = FALSE;

	/* Scan the script, executing each specified action
	 */
	OpenScript();
	while ((scriptPtr = NextScript()) != NULL) {
		i = scriptPtr->who;
		switch (scriptPtr->action) {
			case S_LOADLOG:
				RestoreScanOfs(LoadLog(i));
				break;
			case S_LOADVIEW:
				LoadView(i, TRUE);
				break;
			case S_LOADPIC:
				LoadPic(i);
				break;
			case S_LOADSND:
				LoadSnd(i);
				break;
			case S_DRAWPIC:
				Picture(i);
				break;
			case S_ADDPIC:
				scriptPtr = NextScript();
				picObj.view = scriptPtr->action;
				picObj.loop = scriptPtr->who;
				scriptPtr = NextScript();
				picObj.cel = scriptPtr->action;
				picObj.x = scriptPtr->who;
				scriptPtr = NextScript();
				picObj.y = scriptPtr->action;
				picObj.pri = scriptPtr->who;
				AddToPic();
				break;
			case S_DSCRDPIC:
				DiscardPic(i);
				break;
			case S_DSCRDVIEW:
				DiscardView(i);
				break;
			case S_OVERLAYPIC:
				OverlayPic(i);
				break;
			}
		}

	/* Re-enable scripting
	 */
	ScriptOn();

	/* Draw all objects in the animation tables
	 */
	for (obj = aniObj, i = 0 ; obj < aniEnd ; ++obj, ++i) {
		objCtrl = obj->x;			/* get control bytes */
		obj->x = obj->num;			/* restore X coordinate */
		obj->num = i;				/* restore object's number */
		if (FindView(obj->view))
			SetView(obj, obj->view);
		if (objCtrl & ANIMATED) {
			if (objCtrl & DRAWN) {
				DrawObj(obj->num);
                                /* Force reinitialization of follow path
                                */
                                if (obj->motion == FOLLOW)
                                	obj->parms[2] = -1;
                                }
			if ((objCtrl & (UPDATE | DRAWN)) == DRAWN)
				StopUpdate(obj);

                        /* Reset control bits to pre-restore settings
                        */
                        obj->control = objCtrl;
			}
		}

	/* Show what we've drawn
	 */
	EraseCur();
	CancelLine();
	BackToScrn();
	picVisible = TRUE;

	DisplayStatusLine();
	DisplayInput();
}

