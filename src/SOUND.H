/* SOUND.H
*/


#define	NUM_CHANNELS	4


/* sound table structure
*/
typedef	struct sndstrc{
	struct	sndstrc	*next;
			COUNT	num;
			MEMPTR	ptr;
			MEMPTR	channel[NUM_CHANNELS];
	}	SNDNODE;


