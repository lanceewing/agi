/* TRACE
** Functions for tracing the execution of the logic scanner.
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"logic.h"
#include	"trace.h"
#include	"event.h"


STRPTR	TraceOn();
STRPTR	TraceInfo();
void		_TraceOn();
void		TraceOff();
void		TraceAction();
void		TraceTest();
void		TraceParms();
void		TraceIt();
WORD		GetParm();

#define	WINDOWWIDTH	36
#define	VMARGIN		5
#define	HMARGIN		5
#define	CHARHEIGHT	8
#define	CHARWIDTH		4		/* in our coordinates */

#define	TEST_MSG_OFS	220

#define	SAIDCODE		14


BOOL		saidTest;
COUNT	traceStatus, traceLogs;
COUNT	trWindowPos = 1;
COUNT	trWindowSize = 15;
WORD		roomToTrace = -1;
COUNT	twTopRow, twLeftCol, twBottomRow, twRightCol;
COUNT	twPos, twDim;
BOOL		traceDivider;


typedef	struct {
	STRPTR	(* actRoutine) ();
	UBYTE	actParms;
	BYTEBITS	actVars;
	}	TBL_ENTRY;

extern	LOGNODE	*curLog;
extern	TBL_ENTRY	actionTbl[], testTbl[];
extern	COUNT	picTop;
extern	UBYTE	var[];

extern	STRPTR	GetMsgPtr();
extern	LOGNODE	*FindLog();
extern	EVENT	*NextEvent();





STRPTR
TraceOn(lp)
register	STRPTR	lp;
{
	if (traceStatus != TRACE_OFF)
		return (lp + 1);

	_TraceOn();

	return (lp);
}




void
_TraceOn()
{
	if (traceStatus != TRACE_OFF || !IsSet(TRACE_ENABLE))
		return;

	traceStatus = TRACE_ON;

/*	if (traceLogs != 0)
		LoadLog(traceLogs);
*/

	twTopRow = picTop + 1 + trWindowPos;
	twBottomRow = twTopRow + trWindowSize - 1;
	twLeftCol = 2;
	twRightCol = twLeftCol + WINDOWWIDTH + - 1;

	twPos = ((twLeftCol * CHARWIDTH - HMARGIN) << 8)
			| (twBottomRow * CHARHEIGHT + VMARGIN);
	twDim = ((trWindowSize * CHARHEIGHT + 2 * VMARGIN) << 8)
			| (WINDOWWIDTH * CHARWIDTH + 2 * HMARGIN);

	OpenWndw(twPos, twDim, 0x40f);
}




STRPTR
TraceInfo(lp)
register	STRPTR	lp;
{
	traceLogs = *lp++;
	trWindowPos = *lp++;
	if ((trWindowSize = *lp++) < 2)
		trWindowSize = 2;

	return (lp);
}




void
TraceOff()
{
	if (traceStatus != TRACE_OFF) {
		traceStatus = TRACE_OFF;
		roomToTrace = -1;
		CloseWndw(twPos, twDim);
		}
}



void
TraceAction(actNum, lp)
COUNT	actNum;
STRPTR	lp;
{
	saidTest = FALSE;
	TraceIt(actNum, actionTbl, lp, 0, -1);
}




void
TraceTest(truthValue, lp)
WORD		truthValue;
STRPTR	lp;
{
	COUNT	testNum;

	testNum = *lp++;
	saidTest = (testNum == SAIDCODE);
	TraceIt(testNum, testTbl, lp, TEST_MSG_OFS, truthValue);
}





void
TraceIt(num, tblPtr, lp, msgOffset, truthValue)
COUNT		num;
TBL_ENTRY		*tblPtr;
STRPTR		lp;
COUNT		msgOffset;
WORD			truthValue;
{
	LOGNODE	*savedLog;
	EVENT	*event;

	/* Point to the table entry
	*/
	tblPtr += num;

	/* Make room for the next action in the window
	*/
	SaveCurPos();
	SaveTextAtr();
	_SetTextAtr(0, 15);
	ScrollTrace();
	if (traceDivider) {
		/* Print a divider in the trace output
		*/
		traceDivider = FALSE;
		printf("==========================");
		ScrollTrace();
		}


	/* Print the room number and the action/test name (or number, depending)
	*/
	savedLog = curLog;
	if (traceLogs == 0 || (curLog = FindLog(traceLogs)) == NULL)
		printf("%d: %d", curLog->num, num);
	else
		printf("%d: %s", savedLog->num,
				(num == 0)? "return" : GetMsgPtr(num + msgOffset));
	curLog = savedLog;

	/* Print the parameters of the action
	*/
	TraceParms(tblPtr, lp);

	/* If a truth value has been passed, print it
	*/
	if (truthValue != -1) {
		SetCur(twBottomRow, twRightCol - 2);
		printf(" :%c", (truthValue == 0)? 'F' : 'T');
		}

	/* Wait for the user before proceeding
	*/
	while (traceStatus != TRACE_OFF &&
		((event = NextEvent()) == NULL || event->type != CHAR))
			;
	if (event != NULL && event->val == '+')
		traceStatus = TRACE_NEXT_ROOM;

	RestoreCurPos();
	RestoreTextAtr();
}



void
TraceParms(tblPtr, lp)
TBL_ENTRY		*tblPtr;
STRPTR		lp;
{
	WORDBITS		varBits, varMask;
	COUNT		numParms, i;
	COUNT		parm;

	/* Save the current cursor position for later use
	*/
	SaveCurPos();

	numParms = (saidTest)? *lp++ : tblPtr->actParms;
	varBits = tblPtr->actVars;

	/* Print the parameter list
	*/
	putchar('(');
	for (i = 0 ; i < numParms ; ) {
		parm = GetParm(lp, i);
		printf("%d", parm);
		if (++i < numParms)
			putchar(',');
		}
	putchar(')');

	/* If there are any variable's values to be printed, do so
	*/
	if (varBits != 0)
		ScrollTrace();

	RestoreCurPos();

	if (varBits != 0) {
		varMask = 0x80;
		putchar('(');
		for (i = 0 ; i < numParms ; varMask >>= 1) {
			parm = GetParm(lp, i);
			printf("%d", (varBits & varMask) == 0?  parm : var[parm]);
			if (++i < numParms)
				putchar(',');
			}
		putchar(')');
		}
}




WORD
GetParm(lp, i)
register	STRPTR	lp;
register	COUNT	i;
/* This is used to get parameters of a function, handling the special
** case of the 'said' test, which has word-length parameters
*/
{
	if (saidTest)
		return ((WORD) (*(lp + 2*i) + 0x100 * *(lp + 2*i + 1)));
	else
		return ((WORD) (*(lp + i)));
}




ScrollTrace()
{
	Scroll(twTopRow, twLeftCol, twBottomRow, twRightCol, 0xff);
}

