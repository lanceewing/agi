/* MSGSTR
** Messages issued by the interpreter
**
**	compile: MWC
**
*/

#include	"types.h"



TEXT		escQuitMsg[] =
"\nPress ESC to quit.";


TEXT		versionMsg[] =
"\
Adventure Game Interpreter\n\
      Version x.yyy";




/* -------- restart.c */
TEXT		rstrtMsg[] =
"Press ENTER to restart\n\
the game.\n\
\n\
Press ESC to continue\n\
this game.";





/* -------- equipchk.asm (for 'initmem') */

TEXT		noMemMsg[] =
"Sorry, your computer does\n\
not have enough memory to\n\
play this game.\n\
256K of RAM is required.";

TEXT		noFontMsg[] =
"Can't find HGC_Font file.";



/* -------- segment.c */
TEXT		volMsg[] =
"Please insert disk %d\n\
and press ENTER.";


TEXT		saveVol[] =
"Please insert your save game\n\
disk and press ENTER.";


TEXT		wrongMsg[] =
"That is the wrong disk.\n\n";



TEXT		pauseMsg[] =
"\
      Game paused.\n\
Press Enter to continue.";



TEXT		contCancelMsg[] =
"Press ENTER to continue.\n\
Press ESC to cancel.";
