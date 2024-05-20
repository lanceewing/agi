/* SEGPTR
** Routines to return pointers to various segments
**
**	compile: MWC
**
** Change History:
** 18/02/87  16:16:09	JAS
**	GetDirs() in the test version now also gets a directory for 'object'
**	and 'words.tok'.
*/

#include	"types.h"
#include	"game.h"
#include	"prodflag.h"
#include	"segio.h"


static	TEXT			fn[40];

extern	STRPTR		GetMem();
extern	STRPTR		LoadFile();

extern	FILE_HANDLE	open();



#if	PRODUCTION
/***************   Production i/o (using volumes)   ***********************/

void		NotFound();


TEXT		logDirName[] = "logdir";
TEXT		viewDirName[] = "viewdir";
TEXT		picDirName[] = "picdir";
TEXT		sndDirName[] = "snddir";


/* Pointers to loaded directories
*/
static	DIRPTR	logDir;
static	DIRPTR	viewDir;
static	DIRPTR	picDir;
static	DIRPTR	sndDir;



void
GetDirs()
/* Load in the appropriate segment directories
*/
{
	logDir = (DIRPTR) LoadFile(logDirName, NULL);
	picDir = (DIRPTR) LoadFile(picDirName, NULL);
	viewDir = (DIRPTR) LoadFile(viewDirName, NULL);
	sndDir = (DIRPTR) LoadFile(sndDirName, NULL);
}




DIRPTR
DirEntry(dp)
register	DIRPTR	dp;
/* Check for presence of the given entry in a directory.  If not present,
** return NULL, otherwise return a pointer to the directory entry.
*/
{
	/* If volume is 15, entry is not present.
	*/
	if ((dp->vho & 0xf0) == 0xf0)
		return (NULL);

	return (dp);
}




DIRPTR
LogSeg(n)
WORD		n;
{
	register	DIRPTR	dp;

	if ((dp = DirEntry(logDir + n)) == NULL)
		NotFound("logic", n);
	return (dp);
}



DIRPTR
ViewSeg(n)
WORD		n;
{
	register	DIRPTR	dp;

	if ((dp = DirEntry(viewDir + n)) == NULL)
		NotFound("view", n);
	return (dp);
}



DIRPTR
PicSeg(n)
WORD		n;
{
	register	DIRPTR	dp;

	if ((dp = DirEntry(picDir + n)) == NULL)
		NotFound("picture", n);
	return (dp);
}



DIRPTR
SndSeg(n)
WORD		n;
{
	register	DIRPTR	dp;

	if ((dp = DirEntry(sndDir + n)) == NULL)
		NotFound("sound", n);
	return (dp);
}




void
NotFound(str, n)
STRPTR	str;
WORD		n;
{
	TEXT		msg[100];

	sprintf(msg, "%s %d not found", str, n);
	WindowPrint(msg);
	Quit();
}



#else
/*************************    file io    *************************/


static	TEXT		logDir[30];
static	TEXT		picDir[30];
static	TEXT		viewDir[30];
static	TEXT		sndDir[30];
static	TEXT		wordPath[50];
static	TEXT		objPath[50];

extern	STRPTR	wordFileName;
extern	STRPTR	objectFileName;


GetDirs()
{
	STRPTR		dp;
	FILE_HANDLE	fd;
	COUNT		len;

	SetMark();
	dp = GetMem(1000);
	if ((fd = open("where", 0)) == NO_HANDLE) {
		WindowPrint("No 'where' file.\nPress ESC to quit.");
		Quit();
		}
	len = read(fd, dp, 1000);
	*(dp+len) = 0;
	close(fd);

	GetStr(logDir, &dp);
	GetStr(picDir, &dp);
	GetStr(viewDir, &dp);
	GetStr(sndDir, &dp);

	GetStr(wordPath, &dp);
	strcpy(objPath, wordPath);
	strcat(wordPath, "words.tok");
	strcat(objPath, "object");

	wordFileName = wordPath;
	objectFileName = objPath;

	FreeMark();
}



GetStr(dirp, dp)
STRPTR	dirp;
APTR		dp;
{
	while (**dp && **dp != '\r')
		*dirp++ = *(*dp)++;
	*dp += 2;
}






DIRPTR
LogSeg(n)
WORD		n;
{
	sprintf(fn, "%srm.%d", logDir, n);
	return (fn);
}



DIRPTR
ViewSeg(n)
WORD		n;
{
	sprintf(fn, "%sview.%d", viewDir, n);
	return (fn);
}



DIRPTR
PicSeg(n)
WORD		n;
{
	sprintf(fn, "%spic.%d", picDir, n);
	return (fn);
}



DIRPTR
SndSeg(n)
int	n;
{
	sprintf(fn, "%ssnd.%d", sndDir, n);
	return (fn);
}


#endif

