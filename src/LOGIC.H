/* LOGIC.H
*/


/* logic structure */
typedef	struct logicstrc {
	struct	logicstrc	*next;		/* pointer to next logic */
			UBYTE	num;			/* logic number */
			UBYTE	noMsgs;		/* number of messages for this logic table */
			UBYTE	*ptr;		/* pointer to logics */
			UBYTE	*scan;		/* pointer to scan start */
			UBYTE	*msg;		/* pointer to message directory */
	}	LOGNODE;


#define	MAXSCANS		30
typedef	struct	scanOfsStrc {
	COUNT	num;					/* logic number */
	COUNT	ofs;					/* offset to scan start */
	}	SCANOFS;
/* SAVEAREA
** Header for save areas
*/

typedef	struct	saveStruc {
	struct	saveStruc	*next;		/* pointer to next save area */
	struct	saveStruc	*prev;		/* pointer to previous save area */
	ANIOBJ	*theObject;			/* pointer to object */
	COUNT	x;					/* x position of saved area */
	COUNT	y;					/* y position of saved area */
	COUNT	width;				/* width of saved area */
	COUNT	height;				/* height of saved area */
	STRPTR	bitMap;				/* the save area bit map */
	}	SAVEAREA;

