/* SCRIPT
** Routines for managing a transcript of important (memory-management-wise)
** events.  This enables us to accurately restore a game in tight
** memory situations.
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"script.h"


void		ScriptOff();
void		ScriptOn();
void		InitScript();
void		AddScript();
void		OpenScript();
SCRIPT	*NextScript();
STRPTR	SetScriptSize();
STRPTR	PushScript();
STRPTR	PopScript();




SCRIPT	*script;			/* pointer to script buffer */
SCRIPT	*scriptTop;		/* pointer next free buffer entry */
SCRIPT	*scriptPtr;		/* pointer to next entry to be read */

BOOL		doScript;
WORD		maxScript;

extern	WORD		scriptSize, scriptEntries, savedScript;

extern	STRPTR	GetMem();





void
ScriptOff()
{
	doScript = FALSE;
}


void
ScriptOn()
{
	doScript = TRUE;
}



void
InitScript()
/* Initialize the script buffer.
*/
{
	if (scriptSize > 0 && script == NULL) {
		script = (SCRIPT *) GetMem(scriptSize * sizeof(SCRIPT));
		SetBase();
		}
	scriptTop = script;
	scriptEntries = 0;
}



void
AddScript(action, who)
UBYTE	action, who;
/* Add an event to the script buffer
*/
{
	if (IsSet(NO_SCRIPT))
		return;

	if (doScript) {
		if (scriptTop >= script + scriptSize)
			Error(11, maxScript);
		scriptTop->action = action;
		scriptTop->who = who;
		++scriptTop;
		++scriptEntries;
		}

	if (scriptTop - script > maxScript)
		maxScript = scriptTop - script;
}



void
OpenScript()
/* Prepare the script buffer for reading
*/
{
	scriptPtr = script;
	scriptTop = script + scriptEntries;
}



SCRIPT	*
NextScript()
/* Return a pointer to the next event in the script buffer, NULL if
** at end of buffer
*/
{
	return ((scriptPtr >= scriptTop)?  NULL : scriptPtr++);
}



STRPTR
SetScriptSize(lp)
register	STRPTR	lp;
{
	scriptSize = *lp++;
	RestoreAll();
	InitScript();
	AllInBack();
	return (lp);
}




STRPTR
PushScript(lp)
STRPTR	lp;
{
	savedScript = scriptEntries;
	return (lp);
}



STRPTR
PopScript(lp)
STRPTR	lp;
{
	scriptEntries = savedScript;
	scriptTop = script + scriptEntries;
	return(lp);
}

