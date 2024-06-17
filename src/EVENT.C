/* EVENTS
** The event interface to the input device drivers.
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"event.h"

void			InitEv();
void			ResetEv();
void			PollDevices();
BOOL			PostEvent();
EVENT		*NextEvent();
EVENT		*WaitEvent();
EVENT		*Translate();
WORD			GetKey();
WORD			WaitChar();
WORD			ChkAcAb();
WORD			WaitAcAb();
void			ButtonToChar();




#define	BUFSIZE	20


static	EVENT	eventBuf[BUFSIZE];
		EVENT	*ebHead, *ebTail;

extern	KEYMAP	keyMap[];
extern	LONG		sysTimer;




void
InitEv()
/* Initialize the device drivers
*/
{
	InitJoy();
	ResetEv();
}




void
ResetEv()
/* Clear the device event buffer and any events waiting in the devices
*/
{
	FlushKbd();
	FlushButtons();
	ebHead = ebTail = eventBuf;
}



void
PollDevices()
/* Read the various devices, posting any events in the event buffer.
*/
{
	PollButtons();
	PollKbd();
}




BOOL
PostEvent(type, value)
WORD		type, value;
/* Add an event to the event buffer.  Return 1 if there was room, 0 otherwise.
*/
{
	register	EVENT	*svdHead;
			BOOL		retval;

	/* Add the event to the buffer at the current head
	*/
	retval = 0;
	ebHead->type = type;
	ebHead->val = value;

	/* Advance the head.  If the new head is equal to the tail, then
	** the buffer is full, so retract the head back to its original
	** position.
	*/
	svdHead = ebHead;
	if (++ebHead >= &eventBuf[BUFSIZE])
		ebHead = eventBuf;
	if (ebHead != ebTail)
		return (TRUE);
	else {
		ebHead = svdHead;
		return (FALSE);
		}
}




EVENT	*
NextEvent()
/* Return a pointer to the next event in the event buffer.  If there are
** none in the buffer, return NULL.
*/
{
	register	EVENT	*event;

	event = (ebHead == ebTail)? NULL : ebTail++;

	if (ebTail >= &eventBuf[BUFSIZE])
		ebTail = eventBuf;

	return (event);
}



EVENT	*
WaitEvent()
{
	register	EVENT	*event;
			LONG		oldSysTimer;

	oldSysTimer = sysTimer;
	while ((event = NextEvent()) == NULL)
		if (oldSysTimer != sysTimer) {
			PollJoy();
			oldSysTimer = sysTimer;
			}

	return (event);
}





EVENT	*
Translate(ePtr)
register	EVENT	*ePtr;
/* Translate a character to a controller or a character to a direction key
** if necessary.
*/
{
	register	KEYMAP	*keyPtr;

	if (ePtr->type == CHAR) {
		/* Do a controller translation
		*/
		for (keyPtr = keyMap ; keyPtr->key != 0 ; ++keyPtr)
			if (ePtr->val == keyPtr->key) {
				ePtr->type = CONTROL;
				ePtr->val = keyPtr->keyval;
				break;
				}

		/* Translate the char to a direction key if appropriate
		*/
		DirTranslate(ePtr);
		}

	return (ePtr);
}





WORD
GetKey()
/* Get a character.  Return 0 if none available.
*/
{
	register	WORD		v, t;
			EVENT	*event;

	if ((event = NextEvent()) == NULL)
		return (0);

	ButtonToChar(event);
	t = event->type;
	v = event->val;

	if (t == CHAR)
		return (v);
	else
		return (-1);
}




WORD
WaitChar()
/* Wait for a character, then return it.  Treat the ESC key (which is the
** pause controller) as a character.
*/
{
	register	WORD		c;

	while ((c = GetKey()) == 0 || c == -1)
		;
	return (c);
}




WORD
ChkAcAb()
/* Check if either ACCEPT or ABORT has been selected.  Return the value if
** so, zero otherwise.
*/
{
	register	WORD		c;

	if ((c = GetKey()) == '\r')
		return (ACCEPT);
	else if (c == ESC)
		return (ABORT);
	else
		return (-1);
}



WORD
WaitAcAb()
/* Wait for and return either 'ACCEPT' or 'ABORT'
*/
{
	register	WORD		c;

	/* Clear devices
	*/
	ResetEv();

	/* Wait for the appropriate key
	*/
	while ((c = ChkAcAb()) == -1)
		;

	return (c);
}



void
ButtonToChar(ePtr)
register	EVENT	*ePtr;
{
	register	WORD		c;

	if (ePtr->type == CHAR) {
		if ((c = ePtr->val) == B0_CLICK || c == B0_DBLCLICK)
			ePtr->val = '\r';
		else if (c == B1_CLICK || c == B1_DBLCLICK)
			ePtr->val = ESC;
		}
}

