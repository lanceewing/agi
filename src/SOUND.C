/* SOUND
** Play a sound.
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"
#include	"sound.h"
#include	"script.h"


void		InitSnd();
void		ResetSnd();
SNDNODE	*FindSound();
STRPTR	LLoadSnd();
SNDNODE	*LoadSnd();
STRPTR	Sound();
STRPTR	LStopSnd();
void		StopSnd();


SNDNODE	sndList;
SNDNODE	*prevSnd;
BOOL		playSnd = 0;
WORD		sndEndFlag;

extern	UWORD	segLen;

extern	STRPTR	MustGet();




void
InitSnd()
{
	sndList.next = NULL;
}



void
ResetSnd()
{
	InitSnd();
}



SNDNODE	*
FindSnd(s)
register	COUNT	s;
{
	register	SNDNODE	*sn, *ps;

	for (sn = sndList.next, ps = &sndList ;
		sn != NULL && sn->num != s ;
		ps = sn, sn = sn->next)
			;

	prevSnd = ps;
	return (sn);
}




STRPTR
LLoadSnd(lp)
register	MEMPTR	lp;
{
	LoadSnd(*lp++);
	return (lp);
}



SNDNODE	*
LoadSnd(s)
COUNT	s;
{
	register	SNDNODE	*sn;
	register	MEMPTR	sp;
			COUNT	i;

	if ((sn = FindSnd(s)) != NULL)
		return (sn);

	/* Restore all backgrounds and toss all save areas
	*/
	RestoreAll();

	/* Allocate and link a new sound node into the list
	*/
	if (prevSnd == NULL)
		sn = &sndList;
	else {
		prevSnd->next = sn = (SNDNODE *) GetMem(sizeof(SNDNODE));
		sn->next = NULL;
		}

	/* Load the sound file
	*/
	AddScript(S_LOADSND, s);
	sn->num = s;
	sn->ptr = MustGet(SndSeg(s), NULL);

	/* Get pointers to the channels
	*/
	for (i = 0, sp = sn->ptr ; i < NUM_CHANNELS ; ++i, sp += 2)
		sn->channel[i] = sn->ptr + *sp + 0x100 * *(sp+1);

	/* Reinsert all objects in background
	*/
	AllInBack();

	return (sn);
}



STRPTR
Sound(lp)
register	STRPTR	lp;
/* Play sound 'n' and set flag 'ef' when the sound is finished
*/
{
	register	COUNT	s;
			SNDNODE	*sn;

	StopSnd();

	s = *lp++;
	sndEndFlag = *lp++;		/* get the flag number to signal completion */
	Reset(sndEndFlag);		/* clear the completion flag */

	if ((sn = FindSnd(s)) == NULL)
		Error(9, s);

	StartSound(sn);

	return (lp);
}



STRPTR
LStopSnd(lp)
STRPTR	lp;
{
	StopSnd();
	return (lp);
}



void
StopSnd()
{
	if (playSnd) {
		playSnd = FALSE;
		Set(sndEndFlag);
		SoundOff();
		}
}

