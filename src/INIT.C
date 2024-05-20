/* INIT
** Initialize stuff for KQ (& load overlays)
**
**	compile: MWC
**
** Change History:
** 87.04.30 9:43	JAS
**	ResetAll() now properly sets up the NUM_VOICES and HAS_NOISE vars
**	properly (used to set up for JR & TD on the PC).
** 07/04/87  10:07:27	JAS
**	ResetAll() now decrypts 'object', which is encrypted by 'mkobj'.
** 25/03/87  11:17:23	JAS
**	Removed references to egoDir.
** 18/02/87  16:10:19	JAS
** 	Changed 'wordFileName' and 'objectFileName' from TEXT to STRPTR,
** 	so that they can be reassigned by the test version of GetDirs.
*/

#include	"types.h"
#include	"game.h"
#include	"aniobj.h"
#include	"object.h"
#include	"segio.h"


void		Init();
void		ResetAll();
void		ResetLists();


ANIOBJ	*aniObj;
ANIOBJ	*aniEnd;
WORD		aniLen;
OBJECT	*object;
OBJECT	*objEnd;
WORD		objLen;


STRPTR	wordFileName = "words.tok";
STRPTR	objectFileName = "object";



extern	ANIOBJ	*poAniEntry;
extern	UBYTE	var[];
extern	WORD		picNo;
extern	BOOL		blocking, userCtrl;
extern	STRPTR	wordTbl;
extern	WORD		machType, monType;
extern	WORD		aniInt;
extern	UWORD	segLen;

extern	STRPTR	LoadFile();
extern	STRPTR	GetMem();





void
Init()
{
	/* Do the machine specific initializations
	*/
	InitMachine();

	/* Get the segment directories
	*/
	GetDirs();

	/* Set the text attribute to default, and display the input line.
	*/
	_SetTextAtr(15, 0);			/* white on black */
	DisplayInput();
	
	/* Initialize the input event routines
	*/
	InitEv();

	/* Load the word tables
	*/
	wordTbl = LoadFile(wordFileName, NULL);

	/* Initialize the various lists
	*/
	InitLog();
	InitView();
	InitSnd();
	InitPic();

	/* Load object tables and initialize all variables
	*/
	ResetAll();

	/* Load the room 0 logics
	*/
	LoadLog(0);

	SetBase();		/* set this as the base of free memory */

	/* Turn sounds on
	*/
	Set(SOUNDON);
}




void
ResetAll()
/* Load the object tables and reset all variables.  This routine should
** put the game in a state for a restart.  Note that the word tables and
** room 0 logics remain resident.
*/
{
	register	ANIOBJ	*obj;
	register	WORD		i;
			WORD		numAni;
			WORD		objEndOfs;

	/* Load the object table
	*/
	if (object != NULL)		/* Point to beginning of object table */
		--object;			/* if already loaded. */
	object = (OBJECT *)	LoadFile(objectFileName, object);
	Encrypt(object, (STRPTR) object + segLen);
	objLen = segLen - sizeof(OBJECT);
	numAni = object->room;
	objEndOfs = W8086(object->nameofs);
	++object;
	objEnd = (OBJECT *) ((STRPTR) object + objEndOfs);

	/* Get the number of animated objects from the room number of the
	** first entry of the object table, point past that entry to the
	** objects, and allocate and initialize aniObj.  Add one object at
	** the end of the table for use by picobj
	*/
	++numAni;
	if (aniObj == NULL)
		aniObj = (ANIOBJ *) GetMem(aniLen = numAni * sizeof(ANIOBJ));
	FillMem(aniObj, aniLen, 0);
	aniEnd = aniObj + numAni;
	poAniEntry = aniEnd - 1;
	for (i = 0, obj = aniObj ; i < numAni ; ++i, ++obj)
		obj->num = i;
	
	/* Reset variables, flags, controllers, and various lists
	*/
	FillMem(var, NUMVARS, 0);
	ClearFlags();
	ClearControl();
	ResetLists();

	RestoreAll();

	/* Reset various variables
	*/
	var[MACHINE_TYPE] = machType;
	var[MONITOR_TYPE] = monType;
	var[INPUTLEN] = MAXINPUT + 1;
	Set(INITLOGS);				/* set 'initialize logics' flag */
	userCtrl = TRUE;
	picNo = 0;
	blocking = 0;

	/* Set vars and flags relating to the sound capabilities of the machine.
	*/
	if (machType == PC)
		var[NUM_VOICES] = 1;
	else {
		var[NUM_VOICES] = 3;
		Set(HAS_NOISE);
		}
}



void
ResetLists()
{
	ResetLog();
	ResetView();
	ResetSnd();
	ResetPic();
}

