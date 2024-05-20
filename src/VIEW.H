/* VIEW.H
** View structures
*/

#define	VLOOPNUM	0x30			/* virtual loop number mask */
#define	MIRRORED	0x80			/* mirrored loop bit flag */


/* cel structure
*/
typedef	struct	celstrc {
	UBYTE	xdim;			/* x dimension of cel */
	UBYTE	ydim;			/* y dimension of cel */
	UBYTE	skipclr;			/* skip color for cel */
	UBYTE	celcode;			/* code for the cel */
	}	CEL;

/* loop structure
*/
typedef	struct	loopstrc {
	UBYTE	numcels;			/* number of cels in the loop */
	WORD		celofs[10];		/* offsets to the cels */
	}	LOOP;

/* view structure
*/
typedef	struct	viewstrc {
	UBYTE	stepsz;			/* step size of cels in view */
	UBYTE	cycletm;			/* cycling time of cels in view */
	UBYTE	numloops;			/* number of loops in the view */
	WORD		vdesc;			/* offset to view description */
	WORD		loopofs[4];		/* offsets to the loops */
	}	VIEW;


/* view entry structure
*/
typedef	struct viewentry{
	struct	viewentry	*next;
			UBYTE	num;
			VIEW		*ptr;
	}	VIEWNODE;


