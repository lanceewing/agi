/* MEMMGR
** Memory management routines
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"script.h"

STRPTR	GetMem();
void		SetMemPtr();
STRPTR	GetMemPtr();
void		SetMark();
void		FreeMark();
void		SetBase();
void		FreeAll();
UWORD	MemLeft();
STRPTR	ShowMem();




STRPTR	memPtr;		/* current free memory position */
STRPTR	heapBase;		/* pointer to base of the heap */
STRPTR	memBase;		/* base of free memory */
STRPTR	maxMem;		/* pointer to top of addressable memory */
STRPTR	mark;		/* for marking memory positions */
STRPTR	maxPtr;		/* maximum value of memPtr */


extern	UBYTE	var[];
extern	WORD		*stackBase;
extern	WORD		maxScript;





STRPTR	
GetMem(size)
register	UWORD	size;
/* Return a pointer to a block of free memory of size 'size'
*/
{
	register	STRPTR	base;
			TEXT		msg[100];
	
	if ((UWORD)(maxMem - memPtr) < size) {
		sprintf(msg, "No memory.\nWant %d, have %d", size, (UWORD) (maxMem - memPtr));
		WindowPrint(msg);
		Quit();
		}
	
	base = memPtr;
	memPtr += size;
	MemLeft();
	
	if (memPtr > maxPtr)
		maxPtr = memPtr;
	
	return (base);
}



STRPTR
GetMemPtr()
{
	return (memPtr);
}



void
SetMemPtr(mp)
STRPTR	mp;
{
	memPtr = mp;
}



void
SetMark()
/* Set a mark at the current memory position.
*/
{
	mark = memPtr;
}



void
FreeMark()
/* Free memory to the current mark.
*/
{
	if (mark != NULL) {
		memPtr = mark;
		mark = NULL;
		}
}



void
SetBase()
/* Set the current 'memPtr' as the base of free memory.
*/
{
	memBase = memPtr;
}



void
FreeAll()
/* Free all allocated memory above 'memBase'
*/
{
	FreeAllObjLists();		/* free all saved backgrounds */
	mark = NULL;
	memPtr = memBase;
	MemLeft();
}



UWORD
MemLeft()
{
	register	UWORD	space;

	space = maxMem - memPtr;
	var[MEMLEFT] = space / 256;
	return (space);
}



STRPTR
ShowMem(lp)
STRPTR	lp;
/* Display the current heap size and the amount of stack which has not
** been used
*/
{
	TEXT		msg[100];
	
	sprintf(msg,
		"heapsize: %u\nnow: %u  max: %u\nrm.0, etc.: %u\nmax script: %d",
	     maxMem - heapBase,
		memPtr - heapBase,
		maxPtr - heapBase,
		memBase - heapBase,
		maxScript);
	WindowPrint(msg);	

	return (lp);
}

