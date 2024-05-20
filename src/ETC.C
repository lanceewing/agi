/* ETC
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"


COORD	Pri2Coord();
UWORD	Abs();
WORD		AToI();
STRPTR	IToA();
STRPTR	IToH();
STRPTR	strcpy();
STRPTR	strncpy();
STRPTR	strcat();
int		strcmp();
WORD		strlen();
STRPTR	Reverse();
UBYTE	lower();
STRPTR	GetRandom();
STRPTR	NoAction();
STRPTR	index();
STRPTR	SToLower();


#define	PRIBASE	48
#define	PRIWIDTH	12
#define	BACKPRI	4
#define	STRLEN	10

#define	IsUpper(c)	(c >= 'A' && c <= 'Z')


static	TEXT		str[STRLEN+1];

extern	BOOL		twoDrive;
extern	UBYTE	var[];

extern	UWORD	Random();




UWORD
Abs(x)
register	WORD	x;
/* Return the absolute value of x
*/
{
	return ((x < 0)? -x : x);
}




COORD
Pri2Coord(pri)
WORD		pri;
{
	return (PRIBASE + PRIWIDTH * (pri - BACKPRI - 1));
}




WORD
strlen (s)
/* Return the length of the string pointed to by 's'.
*/
register	STRPTR	s;
{
	register	STRPTR	t;

	t = s;
	while (*t++)
		;
	return (t - s - 1);
}



STRPTR
strcpy(d, s)
register	STRPTR	s, d;
{
	STRPTR	ptr = d;

	while (*d++ = *s++)
		;

	return (ptr);
}



STRPTR
strncpy(d, s, n)
register	STRPTR	s, d;
		WORD		n;
{
	STRPTR	ptr = d;

	while (--n && (*d++ = *s++))
		;
	*d++ = '\0';

	return (ptr);
}



STRPTR
strcat(s, t)
register	STRPTR	s, t;
{
	STRPTR	ptr = s;

	while (*s)
		++s;
	while (*s++ = *t++)
		;

	return (ptr);
}



int
strcmp(s, t)
register	STRPTR	s, t;
{
	for ( ; *s != '\0' && *s == *t ; ++s, ++t)
		;
	return (*s - *t);
}




WORD
AToI(str)
register	STRPTR	str;
{
	register	WORD		n;

	/* Scan off spaces
	*/
	for ( ; *str == ' ' ; str++)
		;

	/* Convert the number
	*/
	n = 0;
	while (*str >= '0' && *str <= '9')
		n = 10*n + *str++ - '0';

	return (n);
}




STRPTR
IToA(n)
register	UWORD	n;
{
	register	STRPTR	s;

	s = str;
	do
		*s++ = n % 10 + '0';
	while ((n /= 10) > 0);
	*s = '\0';

	return (Reverse(str));
}



STRPTR
LeadingZeros(s, fieldWidth)
		STRPTR	s;
register	UWORD	fieldWidth;
{
	register	UWORD	length;
			TEXT		tmpStr[STRLEN+1];

	length = strlen(strcpy(tmpStr, s));

	FillMem(str, STRLEN, '0');

	if (fieldWidth < length)
		fieldWidth = length;

	strcpy(str + fieldWidth - length, tmpStr);
	
	return (str);
}






STRPTR
IToH(n)
WORD		n;
{
	register	STRPTR	s;
	register	UBYTE	c;

	s = str;
	do	{
		c = n % 16;
		*s++ = c + ((c > 9)?  'a'-10 : '0');
	} while ((n /= 16) > 0);
	*s = '\0';

	return (Reverse(str));
}




STRPTR
Reverse(str)
STRPTR	str;
/* Reverse the string 'str' in place.
*/
{
			UBYTE	temp;
	register	STRPTR	s1, s2;

	s1 = str;
	s2 = str + strlen(str) - 1;
	while (s1 < s2) {
		temp = *s1;
		*s1++ = *s2;
		*s2-- = temp;
		}

	return (str);
}



UBYTE
lower(c)
register	UBYTE	c;
{
	return ((IsUpper(c))? c + ('a' - 'A') : c);
}



STRPTR
GetRandom(lp)
register	STRPTR	lp;
{
	UWORD	maxVal, minVal;

	minVal = *lp++;
	maxVal = *lp++;
	var[*lp++] = (Random() % (maxVal - minVal + 1)) + minVal;

	return (lp);
}



STRPTR
NoAction(lp)
STRPTR	lp;
/* A 'no action' action for filling the action table
*/
{
	return (lp);
}



STRPTR
index(s, c)
register	STRPTR	s;
register	UBYTE	c;
{
	while (*s != '\0' && *s != c)
		++s;

	return ((*s == c)? s : NULL);
}



STRPTR
SToLower(s)
register	STRPTR	s;
{
	register	STRPTR	t;

	for (t = s ; *t != '\0' ; ++t)
		*t = lower(*t);

	return (s);
}

