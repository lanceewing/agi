/* GAME.H
** Header file for King's Quest
**
** Change History:
** 87.04.30 9:58	JAS
**	Added NO_PRMPT_RSTRT flag to not prompt user on a restart (allows
**	programmer to restart game from logics).
** 18/02/87  15:03:27	JAS
**	Increased NUMCONTROL to 50.
*/


#define	W8086(addr)	(addr)


/* Another name for the first animated object entry is ego...
*/
#define	ego			aniObj


/* Machine types
*/
#define	PC			0		/* PC, compatibles */
#define	JR			1		/* PCjr */
#define	TD			2		/* Tandy 1000 */
#define	APPLEII		3		/* Apple IIe, IIc */
#define	ST			4		/* Atari ST */
#define	AMIGA		5		/* Amiga */
#define	MAC			6		/* Fat Mac */
#define	CORTLAND	7		/* Apple IIgs */
#define	PS2			8		/* PS2 */




/* System variables
*/
#define	CURROOM		0		/* current.room */
#define	PREVROOM		1		/* previous.room */
#define	EGOEDGE		2		/* edge.ego.hit */
#define	SCORE		3		/* score */
#define	OBJHIT		4		/* obj.hit.edge */
#define	OBJEDGE		5		/* edge.obj.hit */
#define	EGODIR		6		/* ego's direction */
#define	MAXSCORE		7		/* maximum possible score */
#define	MEMLEFT		8		/* remaining heap space in pages */
#define	UNKNOWN_WORD	9		/* number of unknown word */
#define	ANIMATION_INT	10		/* animation interval */
#define	SECONDS		11
#define	MINUTES		12		/* time since game start */
#define	HOURS		13
#define	DAYS			14
#define	DBL_CLK_DELAY	15
#define	CURRENT_EGO	16
#define	ERROR_NUM		17
#define	ERROR_PARAM	18
#define	LAST_CHAR		19
#define	MACHINE_TYPE	20
#define	PRINT_TIMEOUT	21
#define	NUM_VOICES	22
#define	ATTENUATION	23
#define	INPUTLEN		24
#define	SELECTED_OBJ	25		/* selected object number */
#define	MONITOR_TYPE	26




/* System flags
*/
#define	ONWATER		0		/* on.water */
#define	SEE_EGO		1		/* can we see ego? */
#define	INPUT		2		/* have.input */
#define	HITSPEC		3		/* hit.special */
#define	HADMATCH		4		/* had a word match */
#define	INITLOGS		5		/* signal to init logics */
#define	RESTART		6		/* is a restart in progress? */
#define	NO_SCRIPT		7		/* don't add to the script buffer */
#define	DBL_CLK		8		/* enable double click on joystick */
#define	SOUNDON		9		/* state of sound playing */
#define	TRACE_ENABLE	10		/* to enable tracing */
#define	HAS_NOISE		11		/* does machine have noise channel */
#define	RESTORE		12		/* restore game in progress */
#define	ENABLE_SELECT	13		/* allow selection of objects from inventory screen */
#define	ENABLE_MENU	14
#define	LEAVE_WIN		15		/* leave windows on the screen */
#define	NO_PRMPT_RSTRT	16		/* don't prompt on restart */




#define	NUMVARS		256		/* number of vars */
#define	NUMFLAGS		256		/* number of flags */
#define	NUMCONTROL	50		/* number of controllers */
#define	NUMWORDS		10		/* maximum # of words recognized in input */
#define	NUMANIMATED	256		/* maximum # of animated objects */

#define	MAXVAR		255		/* maximum value for a var */

#define	TEXTCOLS		40		/* number of columns of text */
#define	TEXTLINES		25		/* number of lines of text */
#define	MAXINPUT		40		/* maximum length of user input */
#define	DIALOGUE_WIDTH	35		/* maximum width of dialog box */

#define	NUMSTRINGS	12		/* number of user-definable strings */
#define	STRLENGTH		40		/* maximum length of user strings */
#define	PROMPTSTR		0		/* string number of prompt */
#define	ID_LEN		7		/* length of gameID string */


/* Default status and input row numbers
*/
#define	STATUSROW		21
#define	INPUTROW		23



/* Screen edges
*/
typedef	WORD			COORD;

#define	TOP			1
#define	RIGHT		2
#define	BOTTOM		3
#define	LEFT			4
#define	MINX			0
#define	MINY			0
#define	MAXX			(COORD) 159
#define	MAXY			(COORD) 167
#define	HORIZON		36

typedef	enum		{GRAPHMODE, TEXTMODE} SCR_MODE;



#define	ESC			0x001b

#define	TRUE			1
#define	FALSE		0
#define	ACCEPT		TRUE
#define	ABORT		FALSE

#define	BIGINT		0x7fff	/* largest possible integer */

