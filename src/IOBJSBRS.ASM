;IOBJSBRS
;IBM object animation subroutines

cgroup	group code
dgroup	group data
assume	cs:cgroup,ds:dgroup,es:dgroup


public	ISSB, IRSB, IIO

extrn	s_ram:word
extrn	SetIt:near, ResetIt:near, getShdwOfst:near, MirrorCell:near

include	macro.ah
include	game.ah
include	cmequ8s.ah

data	segment byte public 'data'
data	ends	;dummy segment

code	segment byte public 'code'


;|-----------------------------------------------------------------------------|
;|      This subroutine moves a block of shadow ram background into a save     |
;|  area that can be restored later by the `IRSB' subroutine.                  |
;|-----------------------------------------------------------------------------|

ISSB:
	enter	sbSaveArea

	mov	bp,sbSaveArea		;point BX to save area
	mov	al,byte ptr[bp+saY]	;get save area's upper left corner Y coord
	mov	ah,byte ptr[bp+saX] 	;get save area's upper left corner X coord
	call	getShdwOfst		;get offset in shadow ram of point X,Y
	mov	si,di			;SI = shadow ram offset of lower left corner
	mov	ah,byte ptr[bp+saHeight]
	mov	al,byte ptr[bp+saWidth]
	mov	di,[bp+saBitMap]	;DI = pointer to background save area

	push	ds		;save data addressing
	mov	ds,s_ram	;establish shadow ram addressing
	xor	dx,dx
	mov	cx,dx
	mov	dx,shdw_lnth
	sub	dl,al		;compute next line offset factor (nlof)

SBLoop:
	mov	cl,al		;set CX to # of bytes to save (object width)
	rep	movsb		;save background
	add	si,dx		;move to next line to be saved
	dec	ah		;are there more lines to be saved?
	jnz	SBLoop		;yes

	pop	ds		;restore data addressing

	exit			;return control to calling routine


;|-----------------------------------------------------------------------------|
;|      This subroutine restores a block of shadow ram background from a save  |
;|  area that was filled by the `ISSB' subroutine.			       |
;|-----------------------------------------------------------------------------|

IRSB:
	enter	rbSaveArea

	mov	bp,rbSaveArea		;point BX to save area structure
	mov	al,byte ptr[bp+saY]	;get save area's lower left corner Y coord
	mov	ah,byte ptr[bp+saX] 	;get save area's lower left corner X coord
	call	getShdwOfst		;get offset in shadow ram of point X,Y
	mov	si,[bp+saBitMap]	;SI = background save area offset
	mov	ah,byte ptr[bp+saHeight]
	mov	al,byte ptr[bp+saWidth]

	push	es		;save data addressing
	mov	es,s_ram	;establish shadow ram addressing
	xor	dx,dx
	mov	cx,dx
	mov	dx,shdw_lnth
	sub	dl,al		;compute next line offset factor (nlof)

RBLoop:
	mov	cl,al		;set CX to # of bytes to restore (object length)
	rep	movsb		;restore background
	add	di,dx		;move to next line to be restored
	dec	ah		;are there more lines to be restored?
	jnz	RBLoop		;yes

	pop	es		;restore data addressing

	exit			;return control to calling routine


;|-----------------------------------------------------------------------------|
;|      This subroutine expands an object cel from its compressed form into    |
;|  shadow ram.  As the cel is expanded, each shadow ram background pixel that |
;|  has a priority greater than or equal to the object's priority is replaced  |
;|  by the object's pixel.                                                     |
;|-----------------------------------------------------------------------------|

IIO:
	enter	anistrpt	;identify calling routine's parameter

	mov	bp,anistrpt	;point BP to animation data structure
	mov	al,[bp+num]	;get object #
	push	ax		;save object #
	mov	si,[bp+celptr]	;point SI to current cell
	test	byte ptr[si+skipclr],MIRRORED ;is this a mirrored cell?
	jz	IOCell		;no

	push	bp		;pass animation pointer on to MIRROR routine
	call	MirrorCell	;mirror cell
	pop	bp		;point BP to animation structure
	mov	si,[bp+celptr]	;point SI to current cell

IOCell:
	inc	si		;increment passed object length entry
	lodsw			;AH = skip color, AL = object height
	mov	dx,ax		;DH = skip color, DL = object height
	mov	ah,byte ptr[bp+x] ;get cel's base X coord
	mov	al,byte ptr[bp+y] ;get cel's base Y coord
	sub	al,dl		;compute cel's top Y coord
	inc	al		;top Y coord = base Y coord - height + 1
	call	getShdwOfst	;set DI to shadow ram offset of base X, top Y
	shl	dh,1		;move skip color to high nibble
	shl	dh,1
	shl	dh,1
	shl	dh,1
	push	es		;save data addressing
	mov	es,s_ram	;establish shadow ram addressing
	mov	bl,1		;turn `Ego is visible' flag off
	mov	bh,[bp+pri]	;get object's priority
	shl	bh,1		;move priority code to high nibble
	shl	bh,1
	shl	bh,1
	shl	bh,1
	mov	bp,di		;save address of line start
	xor	cx,cx
	jmp	IOGetCC

IOSkip:

	cbw			;extend `# of pixels to skip' to a full word
	add	di,ax		;skip over background

IOGetCC:
	lodsb			;get cell code (ALhi = color, ALlo = # pixels)
	or	al,al		;is this the end of line code?
	jz	IOEOL		;yes

	mov	ah,al		;duplicate cell code
	and	ax,0F00Fh	;isolate color code (AH hi) & pixel count (AL lo)
	cmp	ah,dh		;is this a skip?
	je	IOSkip		;yes

	mov	cl,al		;load pixel counter
	shr	ah,1		;move cell color to low nibble
	shr	ah,1
	shr	ah,1
	shr	ah,1

IOLoop:
	mov	al,es:[di]	;get shadow ram (high = priority, low = color)
	and	al,0F0h		;isolate priority nibble
	cmp	al,20h		;is this a special priority?
	jbe	IOSpclPri	;yes

	cmp	al,bh		;is shadow priority > object priority?
	ja	IONoDraw		;yes

	mov	al,bh		;replace shadow priority with object priority

IODraw:
	or	al,ah		;replace shadow color with object color
	stosb			;draw object pixel in shadow ram
	xor	bl,bl		;turn `Ego is visible' flag on
	loop	IOLoop		;if there is another byte to check, then do so
	jmp	IOGetCC

IOEOL:
	dec	dl		;are there more lines to draw?
	jz	IOEnd		;no

	add	bp,shdw_lnth	;move down 1 line
	mov	di,bp		;set starting DI value
	jmp	IOGetCC

IOSpclPri:
	push	di		;save object pixel offset
	xor	ch,ch		;default: shadow priority <= object pri.

IOSpclCK:
	cmp	di,sbotm_ln	;is current pixel in the bottom line of shadow memory?
	jae	IOPriCk		;yes - default: shadow priority <= object pri.

	add	di,shdw_lnth	;move down one pixel
	mov	ch,es:[di]	;get pixel
	and	ch,0F0h		;isolate shadow priority
	cmp	ch,20h		;is this one also a special priority
	jbe	IOSpclCK	;yes

IOPriCk:
	pop	di		;restore object pixel offset
	cmp	ch,bh		;is shadow priority > object priority?
	mov	ch,0		;clear CH no matter what
	jna	IODraw		;no - draw pixel

IONoDraw:
	inc	di		;this pixel is hidden - don't draw it
	loop	IOLoop
	jmp	IOGetCC

IOEnd:
	pop	es		;restore data addressing
	pop	ax		;restore object #
	or	al,al		;is this object #0 (Ego)?
	jnz	IOXit		;no

	mov	al,SEE_EGO	;update `Ego is visible' flag
	test	bl,bl		;is Ego	visible?
	.if	zero
		call	ResetIt	;no
	.else
		call	SetIt	;yes
	.end

IOXit:
	exit			;return control to calling routine


code	ends

	end

