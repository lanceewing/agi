/* INITMACH
** Machine specific initializations.
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"
#include	"screen.h"


void		InitMachine();
void		InitGraphics();
void		ResetMachine();



/* Overlay numbers of graphics routines
*/
#define	PCGRAF		1		/* PC graphics */
#define	JRGRAF		2		/* PCjr graphics */
#define	XTGRAF		3		/* EGA graphics */
#define	HGRAF		4		/* Hercules graphics */
#define	VGGRAF		5		/* VGA/MCGA graphics (PS/2) */

/* Overlay numbers of object routines
*/
#define	IBMOBJS		6		/* IBM objects */
#define	HGCOBJS		7		/* Hercules objects */



WORD		saveMode;

extern	WORD		monType, machType;



void
InitMachine()
{
	register	COUNT	i;

	/* Clear the device event buffer and any events waiting in the devices
	*/
	ResetEv();

	/* Initialize the graphics system.
	*/
	InitGraphics();

	/* Set up the interrupt handlers that we will use.
	*/
	SetInterrupts();

	/* Close all the file handles which DOS has given us in order
	** to get enough file handles to do our thing.
	*/
	for (i = 0 ; i < 5 ; ++i)
		close(i);
}



void
InitGraphics()
/* Load the appropriate overlays and initialize the graphics
*/
{
	register	WORD		objOverlay, grafOverlay;

	/* Get and save current screen mode
	*/
	saveMode = GetMode();

	if (monType == MONO) {
		objOverlay = HGCOBJS;
		grafOverlay = HGRAF;
		}
	else {
		objOverlay = IBMOBJS;
		if (monType == EGA)
				grafOverlay = XTGRAF;
		else if (machType == PC)
				grafOverlay = PCGRAF;
		else
				grafOverlay = JRGRAF;
		}
	Ovload(grafOverlay);
	Ovload(objOverlay);
	SetDisp();
}	




void
ResetMachine()
{
	/* Close the log file
	*/
	CloseLogFile();

	/* Reset any interrupt vectors which we're using
	*/
	ResetInterrupts();

	/* Restore original screen mode
	*/
	SetMode(saveMode);
}

