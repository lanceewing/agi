/* LOG
** Write the input line out to a log file for debugging/design
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"


STRPTR	Log();

FILE_HANDLE	logFile = NO_HANDLE;
TEXT			logFileName[] = "logfile";


extern	TEXT			prevLine[];
extern	UBYTE		var[];

extern	FILE_HANDLE	open(), creat();




STRPTR
Log(lp)
STRPTR	lp;
{
	register	FILE_HANDLE	fd;
			TEXT			outLine[1000];
			COUNT		theMsg;

	theMsg = *lp++;

	if (logFile == NO_HANDLE)
		OpenLogFile();

	/* If the log file is open and we are able to duplicate the handle,
	** log the messages.  Otherwise, bag it.
	*/
	if (logFile != NO_HANDLE && (fd = Duplicate(logFile)) != NO_HANDLE) {
		/* Write the room number, input line, and message to the file
		*/
		sprintf(outLine, "\n\nRoom %d\nInput line: %s\n", var[CURROOM], prevLine);
		write(fd, outLine, strlen(outLine));
		Print(outLine, GetMsgPtr(theMsg), 78);
		write(fd, outLine, strlen(outLine));
	
		close(fd);
		}

	return (lp);
}



OpenLogFile()
{
	/* Open (or create) the file and seek to the end
	*/
	if (logFile == NO_HANDLE) {
		if ((logFile = open(logFileName, 2)) == NO_HANDLE)
			logFile = creat(logFileName, 0);
		lseek(logFile, 0L, 2);
		}
}



CloseLogFile()
{
	if (logFile != NO_HANDLE) {
		close(logFile);
		logFile = NO_HANDLE;
		}
}

