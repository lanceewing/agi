/* LOGIC
** Routines to handle loading and executing logics.
*/

/* This file was assumed to exist. A real example on an original Sierra game disk
** has not been found. If this file is currently void of code, then it exists 
** merely as a placeholder to indicate that Siera's AGI interpreter had such a 
** file. If this file contains code, then it is because an attempt has been made
** to reconstruct what it might have looked like using a disassembly tool.
*/

/* The following is the section of the AGI.MAP file that relates to this module:

    Module LOGIC from file LOGIC
        Segment CODE.CODE, Addr = 10C0, Size = 2EE
            CALLLOGIC                                             0:1232
            CALLLOGICF                                            0:1258
            FINDLOG                                               0:10E7
            FLOADLOG                                              0:1131
            INITLOG                                               0:10C0
            LLOADLOG                                              0:1115
            LOADLOG                                               0:1172
            RESETLOG                                              0:10CF
            RESETSCANSTART                                        0:1322
            RESTORESCANOFS                                        0:137D
            SETSCANSTART                                          0:130D
            SETUPSCANOFS                                          0:133C
            _CALLLOGIC                                            0:1286
            _LLOADLOG                                             0:1155
        Segment CONST.CONST, Addr = BAB0, Size = 0
        Segment DATA.DATA, Addr = A50F, Size = 86
            CURLOG                                              9BA: 979
            LOGLIST                                             9BA: 96F
            PREVLOG                                             9BA: 97B
            SCANOFS                                             9BA: 97D

*/

#include	"types.h"
#include	"game.h"
#include	"logic.h"
#include	"script.h"
#include    "trace.h"


void        InitLog();
void        ResetLog();
LOGNODE *FindLog();
STRPTR  LLoadLog();
STRPTR  FLoadLog();
void        _LLoadLog();
LOGNODE *LoadLog();
STRPTR  CallLogic();
STRPTR  CallLogicF();
STRPTR  _CallLogic();
STRPTR  SetScanStart();
STRPTR  ResetScanStart();
UWORD   SetupScanOfs();
void        RestoreScanOfs();


LOGNODE logList;
LOGNODE *curLog;
LOGNODE *prevLog;
SCANOFS scanOfs[MAXSCANS];


extern	UBYTE	var[];
extern  COUNT	traceStatus;
extern  BOOL	traceDivider;


extern	STRPTR	MustGet();
extern  UBYTE   *ScanLog();



void
InitLog()
{
    logList.next = NULL;
}



void
/* Clears all but LOGIC.0
*/
ResetLog()
{
    if (logList.next != NULL) {
        (logList.next)->next = NULL;
    }
}



LOGNODE *
FindLog(l)
WORD    l;
/* Return a pointer to logic number 'l', or NULL if logic isn't loaded
*/
{
	register	LOGNODE	*ln;
    register    LOGNODE *pl;

	for (ln = logList.next, pl = &logList ;
		ln != NULL && ln->num != l ;
		pl = ln, ln = ln->next)
			;

	prevLog = pl;
	return (ln);
}



STRPTR
LLoadLog(lp)
register    STRPTR	lp;
{
    _LLoadLog(*lp++);
    return (lp);
}



STRPTR
FLoadLog(lp)
register    STRPTR	lp;
{
    _LLoadLog(var[*lp++]);
    return (lp);
}



void
_LLoadLog(l)
register    UWORD	l;
{
    LoadLog(l);
    AddScript(S_LOADLOG, l);
}



LOGNODE *
LoadLog(l)
UWORD	l;
/* If logic l is not already present, load it. Otherwise return already loaded logic.
*/
{
    register LOGNODE	*ln;
    register MEMPTR     lp;
    MEMPTR     msg;
    LOGNODE *orig_cur;

    if ((ln = FindLog(l)) == NULL) {

        /* Restore all backgrounds and toss all save areas
        */
        RestoreAll();

        /* Allocate and link a new logic node into the list
        */
        prevLog->next = ln = (LOGNODE *) GetMem(sizeof(LOGNODE));
        ln->next = NULL;

        /* Load the logic file
        */
        ln->num = l;
        lp = MustGet(LogSeg(l), NULL);
        ln->ptr = lp + 2;
        ln->scan = ln->ptr;

        msg = ln->ptr + (*lp + 0x100 * *(lp+1));
        ln->noMsgs = *msg++;
        ln->msg = msg;

        /* GetMsgPtr uses the curLog var, so we need to temporarily point it at 
        ** the newly loaded logic.
        */
        orig_cur = curLog;
        curLog = ln;

        /* Decrypts the messages. Message ptr 0 is to end of messages.
        */
        if (ln->noMsgs != 0) {
            Encrypt(ln->msg + ((ln->noMsgs + 1) << 1), GetMsgPtr(0));
        }

        /* Point curLog back at the original value that we stored.
        */
        curLog = orig_cur;

        /* Reinsert all objects in background
        */
        AllInBack();
    }

	return (ln);
}



STRPTR
CallLogic(lp)
register    STRPTR	lp;
{
    if (_CallLogic(*lp++) == 0)
        return 0;
    else
        return (lp);
}



STRPTR
CallLogicF(lp)
register    STRPTR	lp;
{
    if (_CallLogic(var[*lp++]) == 0)
        return 0;
    else
        return (lp);
}



STRPTR
_CallLogic(l)
UWORD	l;
{
    LOGNODE *orig_cur;
    LOGNODE *orig_prev;
    UBYTE *retval;
    LOGNODE *logic_new;
    register BOOL found;

    orig_cur = curLog;
    found = TRUE;

    /* Check if the LOGIC is already loaded.
    */
    if ((curLog = FindLog(l)) == NULL) {
        /** If not found, then load it. */
        orig_prev = prevLog;
        logic_new = curLog = LoadLog(l);
        found = FALSE;
    }

    if (traceStatus == TRACE_NEXT_ROOM)
     	traceStatus = TRACE_ON;

    /* If it is LOGIC.0, then add a divider line in trace output.
    */
    if (l == 0)
     	traceDivider = TRUE;

    /* Execute the LOGIC.
    */
    retval = ScanLog(curLog);

    /* If the LOGIC wasn't originally loaded, then unload the LOGIC. 
    */
    if (!found)
    {
        orig_prev->next = 0;

        /* Restore all backgrounds and toss all save areas
	    */
	    RestoreAll();

        SetMemPtr(logic_new);

        /* Reinsert all objects in background
	    */
	    AllInBack();
    }

    curLog = orig_cur;
    return retval;
}



STRPTR
SetScanStart(lp)
register    STRPTR	lp;
{
    return (curLog->scan = lp);
}



STRPTR
ResetScanStart(lp)
STRPTR	lp;
{
    curLog->scan = curLog->ptr;

    return (lp);
}



UWORD
SetupScanOfs()
{
    register LOGNODE	*ln;
    register SCANOFS    *so;

    for (ln = &logList, so = scanOfs ;
		ln != NULL ;
		ln = ln->next, so++) {

        so->num = ln->num;
        so->ofs = ln->scan - ln->ptr;
    }

    so->num = 0xFFFF;

    return (((so - scanOfs) + 1) * 4);
}



void
RestoreScanOfs(ln)
register    LOGNODE *ln;
{
    register    SCANOFS *so;

    /* Find the scan offset that matches the LOGIC
    */
    for (so = scanOfs ;
		so->num != 0xFFFF ;
		so++) {
        /* If found, apply scan offset to LOGIC
        */
        if (so->num == ln->num) {
            ln->scan = ln->ptr + so->ofs;
            break;
        }
    }
}
