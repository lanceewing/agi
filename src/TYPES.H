/* TYPES.H
** Type definitions
*/


typedef	unsigned char		UBYTE;
typedef	char				BYTE;
typedef	UBYTE			BYTEBITS;
typedef	unsigned short		UWORD;
typedef	short			WORD;
typedef	UWORD			WORDBITS;
typedef	short			INT;
typedef	unsigned long		ULONG;
typedef	long				LONG;
typedef	ULONG			LONGBITS;

typedef	UBYTE			TEXT;		/* UBYTE used as text */
typedef	UBYTE			*STRPTR;		/* a pointer to text */
typedef	STRPTR			MEMPTR;
typedef	STRPTR			ADDRESS;
typedef	STRPTR			*APTR;		/* a pointer to a memory pointer */
typedef	UWORD			BOOL;
typedef	UWORD			COUNT;
typedef	COUNT			ENUM;

typedef	WORD				FILE_HANDLE;
#define	NO_HANDLE			-1

typedef	LONG				TIME_VAL;

#define	forever		for(;;)
#define	NULL			((char *) 0)
#define	TRUE			1
#define	FALSE		0

