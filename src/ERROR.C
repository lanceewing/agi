/* ERROR
** Error routine for testing.
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"setjmp.h"
#include	"game.h"

void		Error();
BOOL		DiskError();

#define	WRITE_PROTECT	0x100
#define	NOT_READY		2



TEXT		tryAgain[] = 
"\nPress ENTER to try again.";


STRPTR	diskErr[] = {
			"",						/* write protect */
			"Unknown unit",
			"",						/* drive not ready */
			"Unknown command",
			"Data error",
			"Bad request structure",
			"Seek error",
			"Unknown media type",
			"Sector not found",
			"",						/* printer out of paper */
			"Write fault",
			"Read fault",
			"General failure"
			};


extern	jmp_buf	rescanJmpBuf;
extern	TEXT		escQuitMsg[];
extern	UBYTE	var[];
extern	UWORD	dosError;



void
Error(errNo, n)
WORD		errNo, n;
{
	StopSnd();	/* turn off sound */
	FreeAll();	/* free memory allocated to various objects, logics, etc. */
	ResetEv();	/* flush the event queue */
	ResetLists();	/* initialize the lists */

	ErrBeep();
	ErrBeep();

	var[ERROR_NUM] = errNo;
	var[ERROR_PARAM] = n;

	/* Blast back to rescan room 0 logics
	*/
	longjmp(rescanJmpBuf, 1);
}




BOOL
DiskError()
{
	TEXT		errStr[100];

	EraseCur();
	ErrBeep();
	ErrBeep();

	switch (dosError) {
		case 0:
			strcpy(errStr, "Disk error.\n");
			break;

		case WRITE_PROTECT:
			strcpy(errStr, "The disk is write protected.\n");
			break;

		case NOT_READY:
			strcpy(errStr, "The disk drive is not ready.\n");
			break;

		default:
			sprintf(errStr, "Disk error:\n%s\n", diskErr[dosError]);
			break;
		}

	strcat(errStr, tryAgain);
	strcat(errStr, escQuitMsg);

	return (WindowPrint(errStr));
}

