/* DEBUG
** Debug logic actions
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"aniobj.h"
#include	"screen.h"


STRPTR	GetNum();
STRPTR	ObjStatus();
STRPTR	ShowPriScreen();

BOOL		showPri = FALSE;

extern	TEXT		versionMsg[];
extern	UBYTE	var[];
extern	ANIOBJ	*aniObj;
extern	WORD		inputRow, monType;
extern	BOOL		dialogueOpen;


extern	STRPTR	Print();





STRPTR
GetNum(lp)
register	STRPTR	lp;
{
	TEXT		numStr[4];
	TEXT		msg[400];

	numStr[0] = '\0';
	if (monType != MONO || dialogueOpen) {
		EraseCur();
		SetCur(inputRow, 0);
		printf(Print(msg, GetMsgPtr(*lp++), 40)); /* print the prompt */
		WriteCur();
		GetLine(numStr, 4);					  /* get the response */
		DisplayInput();
		}
	else
		{
		OpenDialogue(Print(msg, GetMsgPtr(*lp++), 36), 4); /* print the prompt */
		GetLine(numStr, 4);							 /* get the response */
		CloseDialogue();
		}
	var[*lp++] = (UBYTE) AToI(numStr);

	return (lp);
}




STRPTR
ObjStatus(lp)
register	STRPTR	lp;
{
	register	ANIOBJ	*op;
			WORD		n;
			TEXT		msg[100];

	op = aniObj + (n = var[*lp++]);
	sprintf(msg,
		"Object %d:\nx: %d  xsize: %d\ny: %d  ysize: %d\npri: %d\nstepsize: %d",
		n, op->x, op->xsize, op->y, op->ysize, op->pri, op->stepsize);
	WindowPrint(msg);
	return (lp);
}




STRPTR
ShowPriScreen(lptr)
STRPTR	lptr;
{
	showPri = TRUE;
	BackToScrn();
	WaitAcAb();
	BackToScrn();
	showPri = FALSE;
	return (lptr);
}



STRPTR
Version(lp)
STRPTR	lp;
{
	WindowPrint(versionMsg);
	return (lp);
}

