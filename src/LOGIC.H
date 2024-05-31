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
