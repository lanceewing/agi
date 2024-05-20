/* SCRIPT.H
** Header file for definitions required to keep a transcript of events
** leading to the current state in this room
*/

typedef	struct {
	UBYTE	action;
	UBYTE	who;
	}	SCRIPT;

#define	S_LOADLOG		0
#define	S_LOADVIEW	1
#define	S_LOADPIC		2
#define	S_LOADSND		3
#define	S_DRAWPIC		4
#define	S_ADDPIC		5
#define	S_DSCRDPIC	6
#define	S_DSCRDVIEW	7
#define	S_OVERLAYPIC	8

/* SEGERR
** Segment error definitions
*/

#define	ERROR		-1

/* error codes for segment io */
#define	SEGOK	0		/* all okay */
#define	WRONGVOL	1		/* wrong disk volume */
#define	NOTSEG	2		/* not a segment */
#define	DISKERR	3		/* bios level disk error */
#define	NOTSAVE	4		/* not a save game segment */
#define	NOMEM	5		/* not enough memory for segment */
