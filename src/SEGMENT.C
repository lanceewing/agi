/* SEGMENT
** Segment load/save routines
**
**	compile: MWC
**
** Change History:
**	87.04.30 9:30	JAS
**		Fixed LoadFile() so that an error opening the file can be
**		retried.
*/

#include	"types.h"
#include	"game.h"
#include	"prodflag.h"
#include	"segio.h"


STRPTR	MustGet();
STRPTR	GetSeg();
void		NewVol();
void		NewVolMsg();
BOOL		WrongVol();
STRPTR	LoadFile();
void		OpenAllVols();
void		CloseAllVols();


WORD		segErr = 0;
BOOL		returnIfNoMem;
UWORD	segLen;
WORD		currentDisk;

extern	WORD		dosError;
extern	TEXT		volMsg[], wrongMsg[], escQuitMsg[], tryAgain[];
extern	UBYTE	escapeChar;

extern	STRPTR		GetMem(), GetMemPtr();
extern	FILE_HANDLE	open();
extern	ULONG		lseek();
extern	UWORD		MemLeft();



#if	PRODUCTION
/*************************    direct disk io    *************************/

/* These are the file handles for the various volumes.  We try opening all
** volumes present on the current disk.  If we go to read a segment and the
** appropriate volume is not open, we will try opening it.  If we fail,
** we will ask for a disk change.
*/
FILE_HANDLE	volHandle[MAXVOL] = {
							NO_HANDLE,
							NO_HANDLE,
							NO_HANDLE,
							NO_HANDLE,
							NO_HANDLE,
							NO_HANDLE,
							NO_HANDLE,
							NO_HANDLE,
							NO_HANDLE,
							NO_HANDLE
							};




STRPTR
MustGet(dp, addr)
DIRPTR	dp;			/* pointer to directory entry for segment */
STRPTR	addr;		/* memory address at which to load segment */
/* This routine is 'GetSeg', but we MUST get the segment -- there is no
** point in returning if there is an error.
*/
{
	register	STRPTR	ptr;

	while ((ptr = GetSeg(dp, addr)) == NULL) {
		if (segErr == NOMEM)
			return (ptr);
		}

	return (ptr);
}



STRPTR
GetSeg(dp, addr)
DIRPTR	dp;			/* pointer to directory entry for segment */
STRPTR	addr;		/* memory address at which to load segment */
/* Read the segment pointed to by 'dp' in at address 'addr'.
** If 'addr' is NULL, allocate memory for the segment.  Return the address
** at which it was loaded as the function value.  If an error occurs,
** return an address of NULL and put the error code in 'segErr'
*/
{
	ULONG		offset;
	FILE_HANDLE	fd;
	WORD			vol;
	STRPTR		savePtr;
	UBYTE		header[SEG_HEADER_LEN];

	/* Save current memory base for restoration in case of an error
	*/
	savePtr = GetMemPtr();

	/* If volume 0 is not opened, nothing else is open, so open everything.
	*/
	if (volHandle[0] == NO_HANDLE)
		OpenAllVols();

	/* Get the volume from the directory and make sure that it is open
	** (prompt for a disk change if necessary).
	*/
	vol = (dp->vho) >> 4;
	if (vol != 0)
		currentDisk = vol;
	if (currentDisk == 0)
		currentDisk = 1;

	if ((fd = volHandle[vol]) == NO_HANDLE) {
		CloseAllVols();
		segErr = WRONGVOL;
		NewVol(vol);
		OpenAllVols();
		goto GetAbort;
		}

	/* Seek to the appropriate position in the volume.
	*/
	offset = (ULONG) ((dp->vho & 0x0f) * 0x10000L) + ((dp->mo) * 0x100) + dp->lo;
	lseek(fd, offset, 0);

	/* Check the segment header and get the segment length
	*/
	if (read(fd, header, SEG_HEADER_LEN) != SEG_HEADER_LEN) {
		if (DiskError() == ABORT)
			Quit();
		goto GetAbort;
		}

	if (header[0] != SEGID1 || header[1] != SEGID2 || header[2] != vol) {
		CloseAllVols();
		segErr = WRONGVOL;
		if (WrongVol(vol) == FALSE)
			Quit();
		OpenAllVols();
		goto GetAbort;
		}
	segLen = header[3] + 256 * header[4];

	/* If the address for loading is NULL, allocate memory for the segment
	*/
	if (addr == NULL) {
		if (returnIfNoMem && MemLeft() < segLen) {
			segErr = NOMEM;
			goto GetAbort;
			}
		addr = GetMem(segLen);
		}

	/* Read in the segment and return its address.
	*/
	if (read(fd, addr, segLen) == segLen)
		return (addr);


	if (DiskError() == ABORT)
		Quit();

GetAbort:
	/* Restore the previous memory base in case we allocated memory to the
	** segment and it wasn't read
	*/
	SetMemPtr(savePtr);
	return (NULL);
}



void
NewVol(v)
WORD		v;
{
	TEXT		msg[100];

	NewVolMsg(msg, v);
	WindowPrint(msg);
}



void
NewVolMsg(mp, v)
register	STRPTR	mp;
register	WORD		v;
{
	sprintf(mp, volMsg, (v == 0)? currentDisk : v);
}



BOOL
WrongVol(v)
WORD		v;
/* The wrong volume is in the drive.  Prompt for the correct volume.
** Let the user abort the operation if he chooses.
*/
{
	TEXT		nvMsg[100];
	TEXT		msg[200];

	ErrBeep();
	NewVolMsg(nvMsg, v);
	sprintf(msg, "%s%s\n%s", wrongMsg, nvMsg, escQuitMsg);
	return (WindowPrint(msg));
}



void
OpenAllVols()
/* Open all volumes present on the current disk.
*/
{
	register	WORD		i;
			TEXT		volName[10];

	for (i = 0 ; i < MAXVOL ; ++i) {
		sprintf(volName, "vol.%d", i);
		do {
			volHandle[i] = open(volName, 0);
			if (dosError != 0 && DiskError() == ABORT)
				Quit();
		} while (dosError != 0);
		}
}




void
CloseAllVols()
/* Close all open volumes.
*/
{
	register	WORD		i;

	for (i = 0 ; i < MAXVOL ; ++i)
		if (volHandle[i] != NO_HANDLE) {
			close(volHandle[i]);
			volHandle[i] = NO_HANDLE;
			}

	CloseLogFile();
}



#else
/*************************    file io    *************************/



STRPTR
MustGet(fn, ptr)
DIRPTR	fn;
STRPTR	*ptr;
{
	return (GetSeg(fn, ptr));
}


STRPTR
GetSeg(fn, ptr)
DIRPTR	fn;
STRPTR	ptr;
/* Load the segment pointed to by 'fn' at the address 'ptr'.  If 'ptr'
** is NULL, allocate the space for the segment.  Return the address at which
** the segment was loaded and put the length of the segment in 'segLen'.
*/
{
	return (LoadFile(fn, ptr));
}



/* The following functions are stubs for linking with direct I/O type
** functions.
*/

void		NewVol() {}
BOOL		WrongVol() {}
void		CloseAllVols() {}


#endif




STRPTR
LoadFile(fn, addr)
STRPTR	fn, addr;
/* Load the file 'fn' at the address 'addr'.  If 'addr'is NULL, allocate
** the space for the segment.  Return the address at which the segment
** was loaded and put the length of the segment in 'segLen'.
*/
{
	int		fd, length;
	TEXT		msg[100];

	while ((fd = open(fn, 0)) == NO_HANDLE) {
		sprintf(msg, "Can't find %s.%s%s", fn, tryAgain, escQuitMsg);
		escapeChar = '@';
		if (!WindowPrint(msg))
			Quit();
		}

	/* Determine the length of the segment and allocate space, if necessary
	*/
	length = (WORD) lseek(fd, 0L, 2);
	lseek(fd, 0L, 0);
	segLen = length;
	if (addr == NULL)
		addr = GetMem(length);

	if (read(fd, addr, length) != length && DiskError() == ABORT)
		Quit();

	close(fd);

	return (addr);
}


