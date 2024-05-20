/* SEGIO.H
** Segment io definitions.
*/


/* Segment pointer
*/

#if		PRODUCTION
#include	"prodio.h"
#else
typedef	STRPTR	DIRPTR;
#endif


#define	ERROR		-1

/* Error codes for segment io
*/
#define	SEGOK	0		/* all okay */
#define	WRONGVOL	1		/* wrong disk volume */
#define	NOTSEG	2		/* not a segment */
#define	DISKERR	3		/* bios level disk error */
#define	NOTSAVE	4		/* not a save game segment */
#define	NOMEM	5		/* not enough memory for segment */

