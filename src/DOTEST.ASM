;DOTEST
;Do the various logic tests


include	macro.ah
include	game.ah
include	trace.ah


public	DoTest


cgroup	group	code
dgroup	group	data



ANYWORD		equ	1
REST_OF_LINE	equ	9999

testNode	macro	name, numParms, varBits
public	name
dw	offset cgroup:name
db	numParms
db	varBits
endm



data	segment	byte public 'data'

extrn	var:byte
extrn	controller:byte
extrn	object:word
extrn	aniObj:word
extrn	words:word
extrn	numWords:word
extrn	userStr:byte
extrn	traceStatus:word

public	testTbl

testTbl	label	word
	testNode	NoTest, 0, 0
	testNode	EqualN, 2, 10000000b
	testNode	EqualF, 2, 11000000b
	testNode	LessN, 2, 10000000b
	testNode	LessF, 2, 11000000b
	testNode	GreaterN, 2, 10000000b
	testNode	GreaterF, 2, 11000000b
	testNode	TestFlag, 1, 0
	testNode	VTestFlag, 1, 10000000b
	testNode	Has, 1, 0
	testNode	ObjInRm, 2, 01000000b
	testNode	Posn, 5, 0
	testNode	ControllerSet, 1, 0
	testNode	HaveKey, 0, 0
	testNode	Said, 0, 0
	testNode	CmpStrings, 2, 0
	testNode	ObjInBox, 5, 0
	testNode	CenterPosn, 5, 0
	testNode	RightPosn, 5, 0

MAXTEST		equ	($-testTbl)/2


savedTestPtr	dw	0

data	ends






code	segment	byte public 'code'
assume	cs:cgroup,ds:dgroup,es:dgroup


extrn	Error:near
extrn	GetKey:near
extrn	IsFlagSet:near
extrn	SetIt:near
extrn	TraceTest:near
extrn	CompareStrings:near



DoTest	proc	near
;Test number is passed in the al, logic pointer in the si.
;Return 1 if test is true, 0 if false.


	mov	savedTestPtr,si
	dec	savedTestPtr
	cmp	al,MAXTEST
	.if	below
		xor	bh,bh
		mov	bl,al
		shl	bx,1
		shl	bx,1
		call	testTbl[bx]
		cmp	traceStatus,TRACE_ON
		.if	equal
			xor	ah,ah
			push	ax
			pcall	TraceTest,ax,savedTestPtr
			pop	ax
			.end
		return
		.end

	xor	ah,ah
	pcall	Error,#15,ax	;bad test number




EqualN:													;EqualN(FLAG,NUM)

	lodsb			;get var #
	xor	bh,bh
	mov	bl,al		;get index into var table
	lodsb			;get number to compare with
	cmp	al,var[bx]	;do the compare
	mov	al,0		;set up for false
	.if	equal
		inc	al	;if equal, return true
		.end
	return

		

EqualF:													;EqualF(FLAG,FLAG)

	lodsb
	xor	bh,bh
	mov	bl,al		;get index of first var
	mov	ah,var[bx]	;get first var
	lodsb
	mov	bl,al		;get index of second var
	xor	al,al		;set up for false
	cmp	ah,var[bx]	;do compare
	.if	equal
		inc	al	;if equal, return true
		.end
	return




LessN:													;LessN(FLAG,NUM)

	lodsb
	xor	bh,bh
	mov	bl,al		;get index of first var
	lodsb			;get number
	cmp	var[bx],al	;do compare
	mov	al,0		;set up for false
	.if	below		
		inc	al	;if var < number, return true
		.end
	return




LessF:													;LessF(FLAG,FLAG)

	lodsb
	xor	bh,bh
	mov	bl,al		;get index of first var
	mov	ah,var[bx]	;get first var
	lodsb
	mov	bl,al		;get index of second var
	xor	al,al		;set up for false
	cmp	ah,var[bx]	;do compare
	.if	below
		inc	al	;if var1 < var2, return true
		.end
	return





GreaterN:												;GreaterN(FLAG,NUM)

	lodsb
	xor	bh,bh
	mov	bl,al		;get index of var
	lodsb			;get number
	cmp	var[bx],al
	mov	al,0		;set up for false
	.if	above
		inc	al
		.end
	return




GreaterF:												;GreaterF(FLAG,FLAG)

	lodsb
	xor	bh,bh
	mov	bl,al		;get index of first var
	mov	ah,var[bx]	;get first var
	lodsb
	mov	bl,al		;get index of second var
	xor	al,al		;set up for false
	cmp	ah,var[bx]
	.if	above
		inc	al
		.end
	return




TestFlag:		;isset(FLAG)

	lodsb
	call	IsFlagSet
	return


VTestFlag:

	lodsb
	xor	bh,bh
	mov	bl,al
	mov	al,var[bx]
	call	IsFlagSet
	return



Has:		;Has(OBJECT)

	lodsb				;get object number
	xor	ah,ah
	mov	bx,TYPE objstrc
	mul	bx			;get index into object table
	mov	bx,ax
	xor	al,al			;set up for false
	mov	di,object		;get pointer to object table
	cmp	byte ptr [di+bx].room,CARRYING
	.if	equal
		inc	al
		.end
	return




Posn:		;Posn(OBJECT,NUM,NUM;NUM,NUM)
;Is the object's lower left corner in a box?

	call	ObjPosnSetup
	jmp	CheckPosn


CenterPosn:	;CenterPosn(OBJECT,NUM,NUM;NUM,NUM)
;Is the center of the object's baseline in a box?

	call	ObjPosnSetup
	mov	al,byte ptr [bx].xsize	;get x width
	shr	al,1			;/2
	add	dh,al			;get object's center
	mov	ch,dh
	jmp	CheckPosn


RightPosn:	;RightPosn(OBJECT,NUM,NUM;NUM,NUM)
;Is the object's lower rigth corner in a box?

	call	ObjPosnSetup
	add	dh,byte ptr [bx].xsize
	dec	dh
	mov	ch,dh
	jmp	CheckPosn



ObjInBox:	;ObjInBox(OBJECT,NUM,NUM;NUM,NUM)
;Is the object's entire baseline in a box?

	call	ObjPosnSetup
	add	ch,byte ptr [bx].xsize
	dec	ch



CheckPosn:
	lodsb			;get upper left x coord
	cmp	dh,al
	.if	below		;object is left of the box
		add	si,3
		jmp	badpos
		.end
	
	lodsb			;get upper left y coord
	cmp	dl,al
	.if	below		;object is above the box
		add	si,2
		jmp	badpos
		.end
	
	lodsb			;get lower right x coord
	cmp	ch,al
	.if	above		;object is right of the box
		inc	si
		jmp	badpos
		.end
	
	lodsb			;get lower right y coord
	cmp	dl,al
	ja	badpos		;object is below box
	
	mov	al,1
	return

badpos:
	xor	al,al
	return



ObjPosnSetup:
;Output:
;	bx	-> pointer to object's animation structure
;	dh	-> x coord of lower left corner of object
;	ch	->  " "
;	dl	-> y coord of object

	lodsb			;get the object number
	xor	ah,ah
	mov	bx,TYPE anistrc
	mul	bx
	mov	bx,ax
	add	bx,aniObj
	mov	dh,byte ptr [bx].x
	mov	ch,dh		
	mov	dl,byte ptr [bx].y
	return




ControllerSet:		;controller(NUM)

	lodsb			;get controller number
	xor	bh,bh
	mov	bl,al
	mov	al,controller[bx]
	return





ObjInRm:	;obj.in.room(OBJECT,VAR)

	lodsb			;get object number
	xor	ah,ah
	mov	bx,TYPE objstrc
	mul	bx
	mov	bx,ax		;offset into object table
	mov	di,object	;get pointer to the object table
	mov	ah,byte ptr [di+bx].room
	lodsb			;get var number
	xor	bh,bh
	mov	bl,al
	xor	al,al		;set up for false
	cmp	ah,var[bx]
	.if	equal
		inc	al
		.end
	return



Said:
;Match a list of words.
;	bx = index into unmatched words
;	cx = number of words in logics remaining to be matched
;	dx = number of words in input line remaining to be matched

	;Get number of words in logics
	xor	ax,ax
	lodsb
	push	ax

	;Get number of words in the input line and make sure that we
	;didn't somehow get input with no parsed words (unlikely)
	mov	dx,numWords
	or	dx,dx
	jz	SaidAbort

	;If there has already been a match, no other matches can occur.
	mov	al,HADMATCH
	call	IsFlagSet
	jnz	SaidAbort

	;Check for input -- if there is none, there's no point in checking.
	mov	al,INPUT
	call	IsFlagSet
	jz	SaidAbort

	pop	cx		;get number of logic words in cx
	xor	bx,bx		;index into parsed words
	do
		;If no words remain in the logics, quit
		or	cx,cx
		breakif	zero

		;Get the next word from the logics
		lodsw
		dec	cx		;-- number of words remaining in logics

		;Have we matched enough to satisfy the logics?
		cmp	ax,REST_OF_LINE
		.if	equal
			;Make sure that we skip any words that might
			;accidentally follow a ROL
			shl	cx,1
			add	si,cx

			;Signal that both inputs are exhausted
			xor	cx,cx
			mov	dx,cx
			break
			.end

		;If we have exhausted the input line and not encountered
		;a REST_OF_LINE from the logics, there is no match
		or	dx,dx
		.if	zero
			inc	dx	;signal one (at least) unmatched word
			break
			.end

		;Does the input word match that from
		;the logics?
		cmp	ax,words[bx]
		.if	not_equal
			;Will the logics accept any word
			;in this slot?
			cmp	ax,ANYWORD
			breakif	not_equal
			.end

		;Point to next word in the line
		inc	bx
		inc	bx

		;Decrement the number of input words remaining
		dec	dx
	repeat


MatchExit:
	mov	bx,cx		;save number of words remaining in logics
	or	cx,dx		;are both inputs exhausted? if so, then match
	.if	zero
		;Everything matched -- return TRUE
		mov	al,HADMATCH
		call	SetIt
		mov	al,1
	.else
		;Unmatched words -- skip remaining words and return FALSE
		shl	bx,1
		add	si,bx
		xor	al,al
		.end

	return


SaidAbort:
	pop	cx
	jmp	MatchExit





HaveKey:
;Return TRUE if a key has been pressed.
	mov	al,var[LAST_CHAR]
	test	al,al
	.if	zero
		do
			call	GetKey
			cmp	ax,-1	;loop until key event or buffer empty
		while	equal
		.end
	test	al,al
	.if	nonzero
		mov	var[LAST_CHAR],al
		mov	ax,1		;yes
		.end
	return



NoTest:
	xor	ax,ax
	return


CmpStrings:
	xor	ah,ah
	lodsb				;get first string number
	mov	bx,ax
	lodsb				;get second string number
	pcall	CompareStrings,ax,bx
	return




DoTest	endp



code	ends
	end

