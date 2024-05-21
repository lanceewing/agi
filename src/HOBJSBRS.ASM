;HOBJSBRS
;Hercules object animation subroutines

cgroup	group code
dgroup	group data
assume	cs:cgroup,ds:dgroup,es:dgroup


public	HSSB, HRSB, HIO

extrn	h_ram:word, s_ram:word, patrnMasks:word
extrn	SetIt:near, ResetIt:near, getShdwOfst:near, MirrorCell:near

include	macro.ah
include	game.ah
include	cmequ8s.ah
include	pcequ8s.ah

data	segment public byte 'data'

egoHidn		db	?
lineCnt		db	?
niblMask	db	?
pMasks		dw	?
skpColor	db	?

data	ends


code	segment byte public 'code'

;|-----------------------------------------------------------------------------|
;|      This subroutine moves a block of Hercules shadow ram background into a |
;|  save area that can be restored later by the `HRSB' subroutine.	       |
;|-----------------------------------------------------------------------------|

HSSB:
	enter	sbSaveArea

	mov	bp,sbSaveArea		;point BX to save area
	mov	al,byte ptr[bp+saY]	;get save area's upper left corner Y coord
	mov	ah,byte ptr[bp+saX] 	;get save area's upper left corner X coord
	call	getShdwOfst		;get offset in PC shadow ram of point X,Y
	mov	si,di			;SI = PC offset to start of point X,Y
	sub	di,bx			;DI = PC offset to start of point 0,Y
	add	di,SHADOWSIZE*16	;DI = HGC offset to start of point 0,Y
	shr	bx,1			;BX = HGC X coordinate offset
	add	di,bx			;DI = HGC offset to start of point X,Y
	mov	bx,di			;save HGC offset to start of point X,Y
	mov	al,byte ptr[bp+saWidth]	;AL = PC width (in bytes)
	and	ah,1			;AH = X & 1
	add	ah,al			;AH = (X & 1) + width
	inc	ah			;AH = (X & 1) + width + 1
	shr	ah,1			;AH = HGC width (in bytes)
	xor	cx,cx
	mov	cl,byte ptr[bp+saHeight] ;CX = # of lines to save
	mov	di,[bp+saBitMap]	;DI = pointer to background save area
	mov	bp,bx			;BP = HGC offset to start of point X,Y
	mov	bx,shdw_lnth
	mov	dx,bx
	sub	bl,al			;compute PC next line offset factor (nlof)
	sub	dl,ah			;compute HGC next line offset factor
	
	push	ds		;save data addressing
	mov	ds,s_ram	;establish PC shadow ram addressing

SBLoop:
	push	cx		;save # of lines remaining to be saved
	mov	cl,al		;set CX to # of bytes to save (PC object width)
	rep	movsb		;save PC shadow line background
	add	si,bx		;move to next PC shadow line to be saved
	xchg	si,bp		;SI = HGC primary shadow line starting offset
	push	si		;save HGC primary shadow line starting offset
	add	si,shdw_lnth/2	;SI = HGC secondary shadow line starting offset
	mov	cl,ah		;set CX to # of bytes to save (HGC object width)
	rep	movsb		;save HGC secondary shadow line background
	pop	si		;restore HGC primary shadow line starting offset
	mov	cl,ah		;set CX to # of bytes to save (HGC object width)
	rep	movsb		;save HGC primary shadow line background
	add	si,dx		;move to next HGC shadow line to be saved
	xchg	si,bp		;SI = PC shadow line starting offset
	pop	cx		;restore # of lines remaining to be saved
	loop	SBLoop		;loop until all lines have been saved

	pop	ds		;restore data addressing

	exit			;return control to calling routine


;|-----------------------------------------------------------------------------|
;|      This subroutine restores a block of Hercules shadow ram background     |
;|  from a save area that was filled by the `HSSB' subroutine.  	       |
;|-----------------------------------------------------------------------------|

HRSB:
	enter	rbSaveArea

	mov	bp,rbSaveArea		;point BX to save area structure
	mov	al,byte ptr[bp+saY]	;get save area's upper left corner Y coord
	mov	ah,byte ptr[bp+saX] 	;get save area's upper left corner X coord
	call	getShdwOfst		;get offset in PC shadow ram of point X,Y
	mov	si,di			;SI = PC offset to start of point X,Y
	sub	si,bx			;SI = PC offset to start of point 0,Y
	add	si,SHADOWSIZE*16	;SI = HGC offset to start of point 0,Y
	shr	bx,1			;BX = HGC X coordinate offset
	add	si,bx			;SI = HGC offset to start of point X,Y
	mov	bx,si			;save HGC offset to start of point X,Y
	mov	al,byte ptr[bp+saWidth]	;AL = PC width (in bytes)
	and	ah,1			;AH = X & 1
	add	ah,al			;AH = (X & 1) + width
	inc	ah			;AH = (X & 1) + width + 1
	shr	ah,1			;AH = HGC width (in bytes)
	xor	cx,cx
	mov	cl,byte ptr[bp+saHeight] ;CX = # of lines to restore
	mov	si,[bp+saBitMap] 	;SI = pointer to background save area
	mov	bp,bx			;BP = HGC offset to start of point X,Y
	mov	bx,shdw_lnth
	mov	dx,bx
	sub	bl,al			;compute PC next line offset factor (nlof)
	sub	dl,ah			;compute HGC next line offset factor
	
	push	es		;save data addressing
	mov	es,s_ram	;establish PC shadow ram addressing

RBLoop:
	push	cx		;save # of lines remaining to be restored
	mov	cl,al		;set CX to # of bytes to restore (PC object width)
	rep	movsb		;restore PC shadow line background
	add	di,bx		;move to next PC shadow line to be restored
	xchg	di,bp		;DI = HGC primary shadow line starting offset
	push	di		;save HGC primary shadow line starting offset
	add	di,shdw_lnth/2	;DI = HGC secondary shadow line starting offset
	mov	cl,ah		;set CX to # of bytes to restore (HGC object width)
	rep	movsb		;restore HGC secondary shadow line background
	pop	di		;restore HGC primary shadow line starting offset
	mov	cl,ah		;set CX to # of bytes to restore (HGC object width)
	rep	movsb		;restore HGC primary shadow line background
	add	di,dx		;move to next HGC shadow line to be restored
	xchg	di,bp		;DI = PC shadow line starting offset
	pop	cx		;restore # of lines remaining to be restored
	loop	RBLoop		;loop until all lines have been restored

	pop	es		;restore data addressing

	exit			;return control to calling routine


;|-----------------------------------------------------------------------------|
;|      This subroutine expands an object cel from its compressed form into    |
;|  shadow ram.  As the cel is expanded, each shadow ram background pixel that |
;|  has a priority greater than or equal to the object's priority is replaced  |
;|  by the object's pixel.                                                     |
;|-----------------------------------------------------------------------------|

HIO:
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
	mov	lineCnt,dl	;put object's height into lineCnt
	mov	ah,byte ptr[bp+x] ;get cell's base X coord
	mov	al,byte ptr[bp+y] ;get cell's base Y coord
	sub	al,dl		;compute cel's top Y coord
	inc	al		;top Y coord = base Y coord - height + 1
	call	getShdwOfst	;set DI to shadow ram offset of: base X, top Y
	sub	di,bx		;set DI to shadow ram offset of: 0, top Y
	mov	bh,[bp+pri]	;get object's priority
	shl	bh,1		;move priority code to high nibble
	shl	bh,1
	shl	bh,1
	shl	bh,1
	mov	bp,di
	add	bp,SHADOWSIZE*16 ;set BP to HGC shadow ram offset of: 0, top Y
				 ;*** NOTE: offset is from start of PC shadow ram ***
	mov	al,ah
	xor	ah,ah		;AX = base X coordinate
	add	di,ax		;set DI to PC shadow ram offset of: base X, top Y
	mov	bl,0Fh		;assume even X coordinate (left nibble)
	shr	ax,1		;compute HGC shadow ram X coord. offset factor
	.if	carry		;X coordinate is odd
		not	bl 	;set nibble mask for right nibble
		.end

	add	bp,ax		;set BP to HGC shadow ram offset of: base X, top Y
	mov	niblMask,bl	;save nibble mask starting value
	mov	egoHidn,1  	;turn `Ego hidden' flag on
	shl	dh,1		;shift skip color to high nibble
	shl	dh,1
	shl	dh,1
	shl	dh,1
	mov	skpColor,dh	;put cell's skip color into skpColor
	push	es		;save data addressing
	mov	es,s_ram	;establish shadow ram addressing
	xor	cx,cx
	jmp	IONewLine

IOSkip:
	cbw			;extend `# of pixels to skip' to a full word
	add	di,ax		;skip over PC shadow ram
	shr	al,1		;AX = # of HGC shadow ram bytes to skip
	.if	carry	   	;# of pixels skipped was odd
		xor	bl,0FFh	;update left/right nibble mask
		.if	plus
			inc	bp ;right to left -- move to nibble in next byte
			.end
		.end

	add	bp,ax	   	;skip over HGC shadow ram

IOGetCC:
	lodsb			;get cell code -- AL = |color|pixel count|
	or	al,al		;is this the end of line code?
	.if	zero
		jmp	IOEOL	;yes
		.end

	mov	ah,al		;duplicate cell code
	and	ax,0F00Fh	;AH = |color|0|, AL = |0|pixel count|
	cmp	ah,skpColor	;is this a skip?
	je	IOSkip		;yes

	mov	cl,al		;load pixel counter
	shr	ah,1
	mov	al,ah		;AL = mask table index: (color / 16) * 8
	shr	ah,1
	shr	ah,1
	shr	ah,1		;AH = |0|color|
	mov	dx,bx		;save BX
	xor	bx,bx
	mov	bl,lineCnt	;lineCnt is used to cycle pattern mask
	and	bl,3
	shl	bl,1
	add	bl,al		;BX = pattern mask pair index
	mov	bx,ss:patrnMasks[bx] ;BH = secondary pattern, BL = primary pattern
	mov	pMasks,bx	;save even X coordinate pattern masks
	xchg	bx,dx		;restore BX, put pattern masks into DX
	or	bl,bl		;is run starting on an even X coordinate?
	jns	IOLoop		;yes

	rol	dl,1		;rotate patterns for odd X coordinate
	rol	dl,1
	rol	dl,1
	rol	dl,1
	rol	dh,1
	rol	dh,1
	rol	dh,1
	rol	dh,1
	mov	pMasks,dx	;save odd X coordinate pattern masks

IOLoop:
	mov	al,es:[di]	;get PC shadow ram (AL = |priority|color|)
	and	al,0F0h		;isolate priority nibble
	cmp	al,20h		;is this a special priority?
	jbe	IOSpclPri	;yes

	cmp	al,bh		;is shadow priority > object's priority?
	ja	IONiblCk	;yes

	mov	al,bh		;replace shadow priority with object priority

IODraw:
	or	al,ah		;replace PC shadow color with object color
	mov	es:[di],al		;put byte back into PC shadow ram
	mov	al,es:[bp]	;get byte from primary line of HGC shadow ram
	and	al,bl		;prepare appropriate nibble for receiving pattern
	not	bl		;invert nibble mask
	and	dl,bl		;prepare appropriate primary pattern nibble
	or	al,dl		;put pattern into byte from HGC shadow ram
	mov	es:[bp],al	;put byte back into primary line of HGC shadow ram
	and	dh,bl		;prepare appropriate secondary pattern nibble
	not	bl		;invert nibble mask
	mov	al,es:[bp+HGCSLNTH] ;get byte from secondary line of HGC shadow ram
	and	al,bl		;prepare appropriate nibble for receiving pattern
	or	al,dh		;put pattern into byte from HGC shadow ram
	mov	es:[bp+HGCSLNTH],al ;put byte back into secondary line of HGC shadow ram
	mov	egoHidn,0	;turn `Ego hidden' flag off

IONiblCk:
	inc	di		;move to the next PC shadow ram byte
	xor	bl,0FFh		;was the right-hand nibble updated?
	js	IOEORCk		;no

	inc	bp		;move to the next HGC shadow ram byte

IOEORCk:
	mov	dx,pMasks	;restore pattern masks
	loop	IOLoop		;if there is another byte to check, then do so

	jmp	IOGetCC

IOEOL:
	pop	bp		;restore address of HGC line start
	pop	di		;restore address of PC line start
	dec	lineCnt		;are there more lines to draw?
	jz	IOEnd		;no

	add	di,shdw_lnth	;move down 1 line in PC shadow ram
	add	bp,shdw_lnth	;move down 1 line in HGC shadow ram

IONewLine:
	push	di		;save address of PC shadow line start
	push	bp		;save address of HGC shadow line start
	mov	bl,niblMask	;set starting nibble mask value
	jmp	IOGetCC		;process next line of object

IOSpclPri:
	push	di		;save address of PC shadow ram pixel
	xor	ch,ch		;default: shadow priority <= object pri.

IOSpclCk:
	cmp	di,sbotm_ln	;is current PC pixel in the bottom line of shadow ram?
	jae	IOPriCk		;yes - default: shadow priority <= object priority

	add	di,shdw_lnth	;move down one pixel
	mov	ch,es:[di]	;get PC shadow ram pixel
	and	ch,0F0h		;isolate shadow priority
	cmp	ch,20h		;is this one also a special priority?
	jbe	IOSpclCk	;yes

IOPriCk:
	pop	di		;restore address of PC shadow ram pixel
	cmp	ch,bh		;is shadow priority > object priority?
	mov	ch,0		;clear CH no matter what
	jna	IODraw		;no -- draw pixel

	jmp	IONiblCk

IOEnd:
	pop	es		;restore data addressing
	pop	ax		;restore object #
	or	al,al		;is this object #0 (Ego)?
	jnz	IOXit		;no

	mov	al,SEE_EGO	;update `Ego is visible' flag
	test	egoHidn,0FFh	;is Ego	visible?
	.if	zero
		call	ResetIt	;no
	.else
		call	SetIt	;yes
	.end

IOXit:
	exit			;return control to calling routine


code	ends

	end
