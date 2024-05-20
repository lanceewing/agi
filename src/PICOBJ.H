/* PICOBJ.H
*/


/* structure for objects added to picture */
typedef	struct postrc {
			UBYTE	view;		/* view number */
			UBYTE	loop;		/* loop number */
			UBYTE	cel;			/* cel number */
			UBYTE	x;			/* position at which object is drawn */
			UBYTE	y;
			UBYTE	pri;			/* view priority */
	} PICOBJ;


