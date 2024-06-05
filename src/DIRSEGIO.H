/* DIRSEGIO
** The segment definitions for production i/o
*/

#define	MAXVOL		5


typedef	struct {
	UBYTE	vho;		/* volume & high offset		vvvvhhhh */
	UBYTE	mo;		/* middle offset			mmmmmmmm */
	UBYTE	lo;		/* low offset				llllllll */
	}	DIRENTRY;
typedef	DIRENTRY	*DIRPTR;

/* segment io identifying bytes */
#define	SEGID1	0x12
#define	SEGID2	0x34

/* save game volume number */
#define	SAVEVOL	3


#define	SEG_HEADER_LEN		5

