/* SAVENAME
** Routines dealing with the local DOS names used for making save-game files.
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"dos.h"


void		MakeSaveFileName();
BOOL		ValidPath();



/* File attributes
*/
#define	READONLY		0x01
#define	HIDDEN		0x02
#define	SYSTEM		0x04
#define	LABEL		0x08
#define	SUBDIR		0x10




UBYTE	saveDrive;
TEXT		exampleSaveName[] =
"(For example, \"B:\" or \"C:\\savegame\")";
static	TEXT			slashes[] = "\\/";


extern	TEXT		savePath[];
extern	TEXT		gameID[];

extern	STRPTR	index();
extern	STRPTR	SToLower();




void
MakeSaveFileName(fileName, gameNum)
register	STRPTR	fileName;
		COUNT	gameNum;
{
	register	STRPTR	str;

	if (index(slashes, savePath[strLen(savePath) - 1]) != NULL)
		str = "";
	else
		if (index(savePath, '/') == NULL)
			str = "\\";
		else
			str = "/";

	sprintf(fileName, "%s%s%ssg.%d", savePath, str, gameID, gameNum);
	SToLower(fileName);
}




BOOL
ValidPath(path)
register	STRPTR	path;
{
	register	COUNT		length;
			TEXT			file[43];
			STRPTR		ptr;

	while (*path == ' ')
		++path;

	/* No string -> current directory
	*/
	if ((length = strlen(path)) == 0) {
		GetCurDir(path);
		length = strlen(path);
		}

	/* Remove the slash from the end of the pathname if there is one
	** (but not if the pathname is the root directory).
	*/
	ptr = path + length - 1;
	if (index(slashes, *ptr) != NULL && length != 1) {
		--length;
		*ptr = '\0';
		}

	/* Save the drive letter if there is one
	*/
	saveDrive = (path[1] == ':')? lower(path[0]) : GetCurDrive();

	/* Check for validity of the drive && path.
	*/
	if (length == 1 && index(slashes, path[0]) != NULL)	/* root directory */
		return (TRUE);
	else if (path[1] == ':' && length == 2)				/* drive designator */
		return (ValidDrive(lower(path[0])));
	else
		return (FindFirst(path, SUBDIR, file) != -1);	/* subdirectory */
}

