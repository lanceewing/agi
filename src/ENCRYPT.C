/* ENCRYPT
** Use a key to encrypt/decrypt a portion of memory
**
**	compile: MWC
**
*/

#include	"types.h"
#include	"game.h"

void		Encrypt();


TEXT		cryptKey[] = "Avis Durgan";



void
Encrypt(mp, end)
register	STRPTR	mp;
		STRPTR	end;
/* Use the key 'cryptKey' to encrypt memory between 'mp' and 'end'
*/
{
	register	STRPTR	cc;		/* pointer to encryption key */

	cc = cryptKey;
	while (mp < end) {
		if (!*cc)
			cc = cryptKey;		/* if at end of key, start over again */
		*mp++ ^= *cc++;
		}
}
