/* OBJLIST.H
*/

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



typedef	struct	{
	SAVEAREA	*head;				/* pointer to head of list */
	SAVEAREA	*tail;				/* pointer to tail of list */
	}	OBJLIST;
