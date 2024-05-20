/* ANIOBJ.H
*/

#include	"view.h"


/* animation structure */
typedef	struct	anistrc {
	UBYTE	movefreq;		/* number of animation cycles between motion */
	UBYTE	moveclk;		/* number of cycles between moves of object */
	UBYTE	num;			/* object number */
	COORD	x;			/* current x coordinate */
	COORD	y;			/* current y coordinate */
	UBYTE	view;		/* current view number */
	VIEW		*viewptr;		/* pointer to current view */
	UBYTE	loop;		/* current loop in view */
	UBYTE	loopcnt;		/* number of loops in view */
	LOOP		*loopptr;		/* pointer to current loop */
	UBYTE	cel;			/* current cell in loop */
	UBYTE	celcnt;		/* number of cells in current loop */
	CEL		*celptr;		/* pointer to current cell */
	CEL		*prevcel;		/* pointer to previous cell */
	STRPTR	save;		/* pointer to background save area */
	COORD	prevx;		/* previous x coordinate */
	COORD	prevy;		/* previous y coordinate */
	COORD	xsize;		/* x dimension of current cell*/
	COORD	ysize;		/* y dimension of current cell*/
	UBYTE	stepsize;		/* distance object can move */
	UBYTE	cyclfreq;		/* time interval between cells of object */
	UBYTE	cycleclk;		/* counter for determining when object cycles */
	UBYTE	dir;			/* object direction */
	UBYTE	motion;		/* object motion type */
	UBYTE	cycle;		/* cell cycling type */
	UBYTE	pri;			/* priority of object */
	UWORD	control;		/* object control flag (bit mapped) */
	UBYTE	parms[4];		/* space for various motion parameters */
	}	ANIOBJ;


/* object control bits */
#define	DRAWN	0x0001	/* 1 -> object is drawn on screen */
#define	IGNRBLK	0x0002	/* 1 -> object ignores blocks */
#define	FIXEDPRI	0x0004	/* 1 -> object has fixed priority */
#define	IGNRHRZ	0x0008	/* 1 -> object ignores the horizon */
#define	UPDATE	0x0010	/* 1 -> update the object */
#define	CYCLE	0x0020	/* 1 -> cycle the object */
#define	ANIMATED	0x0040	/* 1 -> object can move */
#define	BLOCKED	0x0080	/* 1 -> object is blocked */
#define	PRICTRL1	0x0100	/* 1 -> object must be on 'water' priority */
#define	IGNROBJ	0x0200	/* 1 -> object won't collide with objects */
#define	REPOS	0x0400	/* 1 -> object being reposn'd in this cycle */
#define	PRICTRL2	0x0800	/* 1 -> object must not be entirely on water */
#define	NOADVANC	0x1000	/* 1 -> don't advance object's cel in this loop */
#define	FIXEDLOOP	0x2000	/* 1 -> object's loop is fixed */
#define	STOPPED	0x4000	/* 1 -> object did not move during last animation cycle */


/* object cycle definitions */
#define	NORMAL	0		/* normal repetative cycling of object */
#define	ENDLOOP	1		/* animate to end of loop and stop */
#define	RVRSLOOP	2		/* reverse of ENDLOOP */
#define	REVERSE	3		/* cycle continually in reverse */


/* object direction type definitions */
#define	WANDER	1		/* random movement */
#define	FOLLOW	2		/* follow an object */
#define	MOVETO	3		/* move to a given coordinate */

