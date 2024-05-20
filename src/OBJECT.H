/* OBJECT.H
*/


/* object structure */
typedef	struct	objstrc {
	WORD		nameofs;			/* offset to object name */
	UBYTE	room;			/* room which object is in */
	}	OBJECT;



/* special room numbers
*/
#define	LIMBO	0		/* room number of objects which are gone */
#define	CARRYING	255		/* room number of object in ego's possession */




/* Structure of entry in the inventory table.
*/
typedef	struct	invStrc {
	COUNT	num;
	STRPTR	name;
	COUNT	row;
	COUNT	col;
	}	INV;

