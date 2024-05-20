/* TIMERINT
** The timer interrupt handler.
**
**	compile: MWC
**
** Change History:
**	87.05.11 9:59	JAS
**		Changed updating of elapsed time to handle the possibility of
**		ticks overrunning TICKS_PER_SECOND.  This might be happening
**		in the PCjr.
*/

#include	"types.h"
#include	"game.h"

void		TimerInt();
void		WaitToAnimate();


UWORD	aniTimer;

extern	UBYTE	var[];
extern	BOOL		gamePaused;
extern	LONG		sysTimer;

#define	TICKS_PER_SECOND	20




void
TimerInt()
/* Timer interrupt handler
*/
{
	static	WORD		ticks;

	++sysTimer;

	/* Poll devices (keyboard and joystick buttons)
	*/
	PollDevices();

	/* If we're paused, don't do any timer-related stuff
	*/
	if (gamePaused)
		return;

	++aniTimer;

	/* Update clock time every second.  Every minute, do a complete update
	** of system time based on the correct interrupt frequency of 18.2/sec.
	** This is done by calling SetTime with the number of seconds since
	** startup.
	*/
	++ticks;
	while (ticks >= TICKS_PER_SECOND) {
		ticks -= TICKS_PER_SECOND;

		if (++var[SECONDS] >= 60) {
			var[SECONDS] = 0;
			++var[MINUTES];
			}

		if (var[MINUTES] >= 60) {
			var[MINUTES] = 0;
			++var[HOURS];
			}

		if (var[HOURS] >= 24) {
			var[HOURS] = 0;
			++var[DAYS];
			}
		}
}



void
WaitToAnimate()
{
	while (aniTimer < var[ANIMATION_INT])
		;
	aniTimer = 0;
}

