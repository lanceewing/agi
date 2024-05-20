/* PICTURE.H
*/


/* picture table structure */
typedef	struct picstrc{
	struct	picstrc	*next;
			UBYTE	num;		/* picture number */
			STRPTR	ptr;		/* pointer to the picture code */
	}	PICNODE;
