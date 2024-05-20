/* PARSE
 * Parse a line of input, putting the token values in the array 'words'
 *
 *	compile: MWC
 *
 */

#include	"types.h"
#include	"game.h"


void		Parse();
STRPTR	ParseString();
void		MakeLine();
WORD		Match();
STRPTR	NextToken();


/* Character seperating multiple word tokens in the token table
 */
#define	SEPCHAR		' '
#define	LASTCHAR		0x80
#define	TERMCHAR		0


#define	IsLower(c)	(c >= 'a' && c <= 'z')
#define	TokChar(c)	(lower(c) ^ 0x7f)
#define	CharVal(c)	((c) & 0x7f)
#define	IsLast(c)		((c & LASTCHAR) != 0)



WORD		words[NUMWORDS];		/* recognized words */
STRPTR	wordPtr[NUMWORDS];		/* pointers to recognized words */
WORD		numWords;				/* number of words recognized */
STRPTR	wordTbl;				/* pointer to token table */
TEXT		parseLine[MAXINPUT+1];	/* modified line for parsing */
STRPTR	pl;					/* pointer to current token in parseLine */

TEXT		sepStr[] = " ,.?!();:[]{}";
TEXT		delStr[] = "'`-\"";

extern	TEXT		inLine[];		/* input line from user */
extern	UBYTE	var[];
extern	UBYTE	userStr[][STRLENGTH];

extern	STRPTR	index();






void
Parse(il)
register	STRPTR	il;		/* input line to parse */
{
	register	WORD		i;		/* current index into 'words' */
			WORD		wordVal;	/* word value returned from match */
			STRPTR	wp;		/* pointer to matched word */

	/* Zero the word pointers and the token values
	 */
	FillMem(wordPtr, sizeof(wordPtr), 0);
	FillMem(words, sizeof(words), 0);

	/* Make 'parseLine' from 'il'
	 */
	MakeLine(il);

	i = 0;
	wp = pl = parseLine;	/* start at beginning of the line */

	while (*pl != '\0' && i < NUMWORDS) {
		switch (wordVal = Match()) {
			case -1:				/* unrecognized word */
				wordPtr[i] = pl;
				numWords = var[UNKNOWN_WORD] = i+1;
				Set(INPUT);
				return;
			case 0:				/* an ignored word */
				break;
			default:
				words[i] = wordVal;	/* get the token value */
				wordPtr[i++] = wp;	/* save pointer to the matched word */
			}
		wp = pl;					/* point to next word */
		}

	if (i > 0) {
		numWords = i;		/* put number of words in input flag */
		Set(INPUT);
		}
}



STRPTR
ParseString(lp)
register	STRPTR	lp;
{
	register	WORD		n;

	Reset(INPUT);
	Reset(HADMATCH);

	if ((n = *lp++) < NUMSTRINGS)
		Parse(userStr[n]);
	return (lp);
}





void
MakeLine(il)
register	STRPTR	il;
/* Copy the input line into 'parseLine', collapsing consecutive spaces
 * to a single SEPCHAR.
 */
{
	register	UBYTE	c;
			STRPTR	pl;

	pl = parseLine;

	/* Copy to parseLine
	 */
	while ((c = *il) != '\0') {
		/* Remove seperators and deleted characters
		 */
		while (c != '\0' &&
				(index(sepStr, c) != NULL || index(delStr, c) != NULL))
					c = *++il;

		/* Get out of here if there's nothing left on the line
		 */
		if (c == '\0')
			break;

		/* Copy a word, removing deleted characters
		 */
		while (c != '\0' && index(sepStr, c) == NULL) {
			if (index(delStr, c) == NULL)
				*pl++ = c;
			c = *++il;
			}

		if (c == '\0')
			break;

		*pl++ = SEPCHAR;			/* add seperating character */
		}

	/* Remove a terminating space
	 */
	if (pl > parseLine && *(pl-1) == SEPCHAR)
		--pl;

	*pl = '\0';
}




WORD
Match()
/* 'pl' points at a position in 'parseLine'.  Return the token value of the
 * longest match found in the token table, or -1 if no match found.  If a
 * match is found, point pl past the token.  Otherwise, leave pl where it is
 * and terminate the token in 'parseLine' with an '\0'
 */
{
	register	STRPTR	tp, lp;	/* token pointer, line pointer */
			STRPTR	mp;	 	/* match pointer */
			WORD		wordVal, i, c, index, offset;

	wordVal = -1;
	mp = NULL;

	c = lower(*pl);		/* get first character of word */
	if (!IsLower(c)) {
		BadWord();
		return (wordVal);
		}

	/* Check for the one letter words 'a' and 'I'.
	 */
	if (*(pl+1) == ' ' || *(pl+1) == TERMCHAR) {
		if (c == 'a' || c == 'i') {
			wordVal = 0;
			mp = pl + 1;
			if (*(pl+1) == ' ')
				++mp;
			}
		}

	/* Get the pointer to the start of the words beginning with
	 * the character
	 */
	i = 2 * (c - 'a');
	offset = 256 * *(wordTbl + i) + *(wordTbl + i + 1);
	if (offset == 0) {
		BadWord();
		return (wordVal);
		}
	tp = wordTbl + offset;

	/* Now scan through the word table, looking for a match
	 */
	lp = pl;
	for (index = 0 ; *tp >= index ; tp = NextToken(tp)) {
		/* At the end of this letter's tokens?
		 */
		if (tp == NULL)
			break;

		/* If index values are the same, scan characters
		 */
		if (*tp++ == index) {
			for ( ; CharVal(*tp) == TokChar(*lp) ; ++tp) {
				++lp;
				++index;
				if (IsLast(*tp)) {
					/* We're at the end of the token.  If we're at a
					 * seperator in the input line, we have a match.
					 */
					if (*lp == TERMCHAR || *lp == SEPCHAR) {
						wordVal = 256 * *(tp+1) + *(tp+2);
						mp = lp;	/* point mp past the match */
						if (*lp != TERMCHAR)
							++mp;
						}
					break;
					}
				}
			}
		if (*lp == TERMCHAR)
			break;
		}

	if (mp == NULL)
		BadWord();
	else {
		pl = mp;
		if (*pl != '\0')	/* terminate the word in the line */
			*(pl-1) = '\0';
		}

	return (wordVal);
}



BadWord()
/* Terminate the unknown word.
 */
{
	register	STRPTR	lp;

	lp = pl;
	while (*lp != SEPCHAR && *lp != '\0')
		++lp;
	*lp = '\0';
}





STRPTR
NextToken(tp)
register	STRPTR	tp;
{
	while ((*tp & LASTCHAR) == 0)
		++tp;

	return ((*tp == TERMCHAR)? NULL : (tp + 3));
}

