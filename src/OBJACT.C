/* OBJACT
** Object related actions (not for animated objects)
**
**	compile: MWC
**
*/


#include	"types.h"
#include	"game.h"
#include	"object.h"


STRPTR	Get();
STRPTR	GetF();
STRPTR	Drop();
STRPTR	Put();
STRPTR	PutF();
STRPTR	GetRoomF();
OBJECT	*ObjPtr();
OBJECT	*ObjVarPtr();

extern	OBJECT	*object, *objEnd;
extern	UBYTE	var[];




STRPTR
Get(lp)
STRPTR	lp;
{
	ObjPtr(&lp)->room = CARRYING;
	return (lp);
}



STRPTR
GetF(lp)
STRPTR	lp;
{
	ObjVarPtr(&lp)->room = CARRYING;
	return (lp);
}



STRPTR
Drop(lp)
STRPTR	lp;
{
	ObjPtr(&lp)->room = LIMBO;
	return (lp);
}



STRPTR
Put(lp)
STRPTR	lp;
{
	register	OBJECT	*op;

	op = ObjPtr(&lp);
	op->room = var[*lp++];
	return (lp);
}


STRPTR
PutF(lp)
STRPTR	lp;
{
	register	OBJECT	*op;

	op = ObjVarPtr(&lp);
	op->room = var[*lp++];
	return (lp);
}



STRPTR
GetRoomF(lp)
STRPTR	lp;
{
	register	OBJECT	*op;

	op = ObjVarPtr(&lp);
	var[*lp++] = op->room;
	return (lp);
}



OBJECT	*
ObjPtr(pp)
STRPTR	*pp;
{
	register	OBJECT	*op;
	register	STRPTR	lp;

	lp = *pp;
	if ((op = object + *lp++) >= objEnd)
		Error(23, op - objEnd);
	*pp = lp;
	return (op);
}



OBJECT	*
ObjVarPtr(pp)
STRPTR	*pp;
{
	register	OBJECT	*op;
	register	STRPTR	lp;

	lp = *pp;
	if ((op = object + var[*lp++]) >= objEnd)
		Error(23, op - objEnd);
	*pp = lp;
	return (op);
}

