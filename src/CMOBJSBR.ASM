;CMOBJSBR
;Common object animation subroutines

cgroup	group code
dgroup	group data
assume	cs:cgroup,ds:dgroup,es:dgroup


public	initpri, canbhere, obj2scrn, objinpic, xlatobjc, MirrorCell
public	pri_tbl

extrn	s_ram:word, machtype:word, monType:word, picVisible:word
extrn	SetIt:near, ResetIt:near, InsertObj:near, getShdwOfst:near
extrn	blk2Scrn:near, pcxo:near

include	macro.ah
include	game.ah
include	cmequ8s.ah

USE_NEARWATER	equ	0

data	segment public byte 'data'

pri_tbl		db	PICROWS dup(?) ;Y coord priority  x-ref table

data	ends


code	segment byte public 'code'

initpri:

;|-----------------------------------------------------------------------------|
;|	This subroutine generates a 168 byte table which contains the priority |
;| values for each of the 168 lines on the screen.  This allows a Y coordinate |
;| to be translated into its corresponding priority.                           |
;|-----------------------------------------------------------------------------|

	lea	di,dgroup:pri_tbl	;point DI to Y coord priority x-ref table
	mov	cx,48		;set #lines in band 0 to 48
	mov	al,4		;set priority in band to 4

set_pri:
	rep	stosb		;put priority value into table
	cmp	al,14		;is the table full?
	jae	ip_xit		;yes

	inc	al		;increment to next priority value
	mov	cl,12		;set #lines in bands 1-10 to 12
	jmp	set_pri

ip_xit:
	ret


canbhere:

;|-----------------------------------------------------------------------------|
;|	This subroutine performs various checks to determine if an object can  |
;| drawn at a proposed location on the screen.  If the object can be drawn a   |
;| `1' is returned in the AX register, otherwise a `0' is returned.  The       |
;| priority of the object is also loaded into the animation structure.  The    |
;| priority is determined by the priority band containing the object's base    |
;| line.                                                                       |
;|-----------------------------------------------------------------------------|

	enter	anistrpt	;identify calling routine's parameter
	mov	bp,anistrpt	;point BP to animation data structure
	mov	ah,byte ptr[bp+y] ;get object's current base Y coord
	mov	dx,[bp+control]	;get object control bit flags
	test	dx,FIXEDPRI	;is this object's priority fixed?
	jnz	cbh_offset	;yes

	mov	al,ah		;prepare for translation of Y coord to priority
	lea	bx,dgroup:pri_tbl ;point BX to Y coord priority x-ref table
	xlat			;translate Y coord to priority band
	mov	[bp+pri],al	;set object's priority

cbh_offset:
	mov	al,byte ptr[bp+x] ;get object's current base X coord
	xchg	al,ah		;put X,Y coords into proper registers
	call	getShdwOfst
	mov	si,di	  	;SI = offset into shadow ram
	mov	di,[bp+celptr]	;point DI to current cel
	mov	ah,[bp+num]	;get the object number
	xor	cx,cx
	mov	bx,cx		;clear special priorities 2 & 3 flags
	mov	cl,[di+xdim]	;get length of base line
	mov	di,1		;set `can be here' flag to `can'
	cmp	[bp+pri],15	;is object's priority = 15
	je	cbh_egock	;yes

	push	ds		;save data addressing
	mov	ds,s_ram	;establish shadow ram addressing
	mov	bx,di		;set `entirely on water' flag (BL) to `yes'

cbh_gtpri:
	lodsb			;get color/priority byte from shadow ram
	and	al,0F0h		;isolate priority nibble
	jz	cantbhre	;can't be on priority zero

	cmp	al,30h		;is this water?
	je	cbh_xtck	;yes

	xor	bl,bl		;set `entirely on water' flag to `no'
	cmp	al,10h		;is this priority 1?
	je	cbh_pri1	;yes

	cmp	al,20h		;is this priority 2?
	je	cbh_pri2	;yes

cbh_xtck:
	loop	cbh_gtpri	;if there is another byte to check, then do so
	cmp	bl,1		;is object entirely on water?
	je	cbh_tlck	;yes

	test	dx,PRICTRL1	;is the `must be entirely on water' bit set?
	jz	canbehre	;no

	jmp	cantbhre

cbh_tlck:
	test	dx,PRICTRL2	;is the `must be touching land' bit set?
	jz	canbehre	;no

cantbhre:
	xor	di,di		;set `can/cannot be here' flag to `cannot'

canbehre:
	pop	ds		;restore data addressing

cbh_egock:
	test	ah,ah		;is this Ego?
	jnz	cbh_xit		;no - don't update system flags

	mov	dx,bx		;SetIt/ResetIt clobber the BX
	mov	al,HITSPEC
	test	dh,dh		;did Ego hit special priority 2?
	.if	zero
		call	ResetIt	;no
	.else
		call	SetIt	;yes
	.end
	mov	al,ONWATER
	test	dl,dl		;is Ego entirely on water?
	.if	zero
		call	ResetIt	;no
	.else
		call	SetIt	;yes
	.end

cbh_xit:
	mov	ax,di		;return `can/cannot be here' value

	exit

cbh_pri1:
	test	dx,IGNRBLK	;is the ignore block bit set?
	jz	cantbhre	;no

	jmp	cbh_xtck

cbh_pri2:
	mov	bh,1		;set `hit special priority 2' flag to `yes'
	jmp	cbh_xtck


if	USE_NEARWATER
_nearwater:

;|-----------------------------------------------------------------------------|
;|	This subroutine searches for water, priority 3, along a path of pixels |
;| starting at a base set of X,Y coordinates, in the direction Ego is facing.  |
;| The distance in pixels to the water is returned in the AX register.  If the |
;| starting point is on water a value of zero is returned.  If water is not    |
;| found within the specified number of pixels, or if the path is blocked by   |
;| priority 0 or 1, then a value of -1 is returned.                            |
;| dh contains some relevent bit flags:
;|	80h	0 -> increment coord,	1 -> decrement coord
;|	40h	0 -> change y coord,	1 -> change x coord
;|-----------------------------------------------------------------------------|

	enter	anistrpt,facing,x_base,y_base,limit	;identify parameters
	mov	ax,y_base	;get Y coord. of starting search point
	mov	bx,x_base	;get X coord. of starting search point
	mov	cx,limit	;get number of pixels to search
	mov	si,facing	;get direction in which Ego is facing
	mov	bp,anistrpt	;point BP to animation data structure
	mov	di,ax		;move Y coord. to work register
	mov	dl,y_max	;move Y coord. maximum value to work register
	xor	dh,dh		;flags default to increment, y coord, PC
	cmp	si,back		;which direction is Ego facing?
	je	nw_back		;back
	cmp	si,front
	je	nw_front	;front
	or	dh,40h		;set X coord
	mov	di,bx		;move X coord. to work register
	mov	dl,x_max	;move X coord. maximum value to work register
	jb	nw_right	;right

nw_left:
nw_back:
	or	dh,80h		;set decrement coordinate flag
	std			;prepare for decrementing search
	sub	di,cx		;does search path extend off picture?
	jnb	nw_search	;no
	add	cx,di		;limit search path only to picture
	jmp	nw_search

nw_right:
nw_front:
	add	di,cx		;compute end of search path
	sub	di,dx		;does search path extend off picture?
	jbe	nw_search	;no
	sub	cx,di		;limit search path only to picture

nw_search:
	mov	ah,bl		;AH = X coordinate
	call	getShdwOfst
	mov	si,di		;SI = offset into shadow ram
	push 	ds		;save data addressing
	mov	ds,s_ram  	;establish shadow ram addressing
	mov	bh,[bp+pri]	;get Ego's priority
	mov	bl,byte ptr[bp+control] ;get low half of Ego's control flag
	xor	di,di		;initialize distance to water value

nw_get_pri:
	lodsb			;get color/priority byte from shadow ram
	and	al,0F0h		;isolate priority nibble
	cmp	al,30h		;is this pixel a special priority?
	ja	nw_nxt_byte	;no
	je	nw_xit		;here's the water
	cmp	bh,15		;is Ego's priority = 15
	je	nw_nxt_byte	;yes
	cmp	al,10h		;is this a possible block?
	ja	nw_nxt_byte	;no
	jb	nw_no_wtr	;priority 0 - definitely blocked
	test	bl,IGNRBLK	;is the ignore blocks bit set?
	jz	nw_no_wtr	;no

nw_nxt_byte:
	dec	cx		;has the entire path been searched?
	js	nw_no_wtr	;yes
	inc	di		;update distance to water value
	test	dh,40h		;is the search along the X axis?
	jnz	nw_get_pri	;yes - SI has already been adjusted
	test	dh,80h		;is the Y coord. going from high to low?
	jnz	nw_hi2lo	;yes
	dec	si		;adjust SI because of LODSB instruction
	add	si,shdw_lnth	;move down one line
	jmp	nw_get_pri

nw_hi2lo:
	inc	si		;adjust SI because of LODSB instruction
	sub	si,shdw_lnth	;move up one line
	jmp	nw_get_pri

nw_no_wtr:
	mov	di,0ffffh	;no water or blocked - dist. to water = 0ffffh

nw_xit:
	mov	ax,di		;return distance to water value in AX
	pop	ds		;restore data addressing
	cld			;reset direction flag
	exit

endif

obj2scrn:

;|-----------------------------------------------------------------------------|
;|      This subroutine moves an object cel from shadow ram to video ram.  The |
;|  data for the current cel, and the background necessary to erase the        |
;|  previous cel are both moved as a unit from shadow ram to video ram.        |
;|-----------------------------------------------------------------------------|

	enter	anistrpt	;identify calling routine's parameter

	cmp	picVisible,0	;is picture on screen?
	jz	o2s_exit	;nope -- don't put object there

	mov	bp,anistrpt	;point BP to animation data structure
	mov	si,[bp+celptr]	;point SI to current cel
	mov	di,[bp+prevcel]	;point DI to previous cel
	mov	[bp+prevcel],si	;set previous cel to current cel
	mov	al,byte ptr[bp+y] ;get current cel's base Y coord
	mov	cl,byte ptr[si+ydim] ;get current cel's Y height
	mov	bh,byte ptr[bp+prevy] ;get previous cel's base Y coord
	mov	ch,byte ptr[di+ydim] ;get previous cel's Y height
	cmp	al,bh		;does AL contain the bottommost base Y coord?
	jae	o2s_gttmp	;yes

	xchg	al,bh		;AL now contains the bottommost base Y coord
	xchg	ch,cl		;swap lengths accordingly

o2s_gttmp:
	inc	bh		;prevent negative result
	sub	bh,ch		;sub. topmost cel's Y height from base Y coord
	neg	cl
	add	cl,al		;sb. bottommost cel's Y height from base Y coord
	inc	cl		;prevent negative result
	cmp	bh,cl		;is the topmost point in BH?
	jbe	o2s_gthgt	;yes

	mov	bh,cl		;BH now contains topmost point

o2s_gthgt:
	neg	bh		;BH now contains -topmost point
	add	bh,al		;compute block height (bm bs Y crd - tm pnt + 1)
	inc	bh
	mov	ah,byte ptr[bp+x] ;get current cel's base X coord
	mov	cl,byte ptr[si+xdim] ;get current cel's X length
	mov	bl,byte ptr[bp+prevx] ;get previous cel's base X coord
	mov	ch,byte ptr[di+xdim] ;get previous cel's X length
	cmp	ah,bl		;does AH contain the smallest base X coord?
	jbe	o2s_gtrmp	;yes

	xchg	ah,bl		;AH now contains the smallest base X coord
	xchg	ch,cl		;swap lengths accordingly

o2s_gtrmp:
	add	cl,ah		;add leftmost cel's base X coord & X length
	add	bl,ch		;add rightmost cel's base X coord & X length
	cmp	bl,cl		;is the rightmost point + 1 in BL?
	jae	o2s_gtlnth	;yes

	mov	bl,cl		;BL now contains the rightmost point + 1

o2s_gtlnth:
	sub	bl,ah		;compute block length (rm pnt + 1 - sm bs X crd)
	call	blk2Scrn

o2s_exit:
	exit


objinpic:

;|-----------------------------------------------------------------------------|
;|      This subroutine makes an object a permanent part of the picture.  The  |
;| current cel is expanded into shadow ram with both its color and priority.   |
;| After the cel is put into shadow ram, a box of priority zero is drawn       |
;| around the object.  The sides of the box extend to the top of the object or |
;| to the top of the object's base line priority band, whichever is smaller.   |
;|-----------------------------------------------------------------------------|

	enter	anistrpt	;identify calling routine's parameter
	mov	bp,anistrpt	;point BP to animation data structure
	mov	ah,byte ptr[bp+y] ;get object's current base Y coord
	mov	al,ah		;prepare for translation of Y coord to priority
	lea	bx,dgroup:pri_tbl ;point BX to Y coord priority x-ref table
	xlat			;translate Y coord to priority band
	push	ax		;AH = Y coord; AL = priority of base line
	mov	ah,byte ptr[bp+pri] ;get object's priority param
	and	ah,0Fh		;is object's priority specified?
	.if	zero
		or	byte ptr[bp+pri],al ;no -- use its base line priority
	.end
	push	bp		;pass animation pointer on to INSERT routine
	call	InsertObj	;insert object into shadow ram
	pop	bp		;point BP to animation structure
	pop	ax
	mov	cl,ah		;CL = Y coord
	mov	dl,al		;DL = priority of base line
	cmp	byte ptr[bp+pri],3Fh ;is a special priority box required?
	ja	ObjInPicExit

	lea	bx,dgroup:pri_tbl ;point BX to Y coord priority x-ref table
	xor	dh,dh

oip_ptop:
	inc	dh
	or	cl,cl		;is this the top of shadow ram?
	jz	oip_gtofst	;yes -  DH = Y offset to start of the next band

	dec	cl		;move up one line
	mov	al,cl		;prepare for translation of Y coord to priority
	xlat			;translate Y coord to priority band
	cmp	al,dl		;is this the same priority band?
	je	oip_ptop	;yes

oip_gtofst:
	mov	al,byte ptr[bp+x] ;get object's current base X coord
	xchg	al,ah		;put X,Y coords into proper registers
	call	getShdwOfst	;compute offset of base X,Y in shadow ram
	mov	si,di		;save base line offset
	mov	dl,byte ptr[bp+pri]; get priority of box (in high nibble)
	mov	bp,[bp+celptr]	;point BP to current cel
	xor	bx,bx
	mov	bl,byte ptr[bp+xdim] ;get cel's X dimension
	mov	ah,byte ptr[bp+ydim] ;get cel's Y dimension
	cmp	ah,dh		;is the object completely within this band?
	jbe	oip_savx	;yes

	mov	ah,dh		;limit Y dimension to stay within this band

oip_savx:
	mov	al,bl		;save cel's X dimension
	dec	bx		;set BX to cel's X dimension - 1
	and	dl,0F0h		;isolate box priority
	mov	dh,0Fh		;build priority mask
	push	es		;save data addressing
	mov	es,s_ram	;establish shadow ram addressing
	xor	cx,cx
	mov	cl,al		;set loop counter to X dimension

oip_botm:
	and	byte ptr es:[di],dh ;build bottom of special priority box
	or	byte ptr es:[di],dl
	inc	di		;move one pixel to the right
	loop	oip_botm	;loop until bottom is complete
	mov	di,si		;reset to bottom left corner
	mov	cl,ah
	dec	cl	  	;set loop counter to Y dimension - 1
	jz	oip_xit

oip_sides:
	sub	di,shdw_lnth	;move up one line
	and	byte ptr es:[di],dh ;build left side of special priority box
	or	byte ptr es:[di],dl
	and	byte ptr es:[bx+di],dh	;build right side of special priority box
	or	byte ptr es:[bx+di],dl
	loop	oip_sides	;loop until sides are complete
	mov	cl,al		;set loop counter to X dimension - 2
	sub	cl,2		;the end points have already been drawn

oip_tlop:
	inc	di		;move one pixel to the right
	and	byte ptr es:[di],dh ;build top of special priority box
	or	byte ptr es:[di],dl
	loop	oip_tlop	;loop until top is complete

oip_xit:
	pop	es		;restore data addressing

ObjInPicExit:
	exit


MirrorCell:

;|-----------------------------------------------------------------------------|
;|      This subroutine takes a cell and modifies its data so that when the    |
;| cell is expanded it creates a mirror image of itself.  The stack is used    |
;| as a temporary storage area for the mirrored cell data.  SI already points  |
;| to the cell data.						       	       |
;|-----------------------------------------------------------------------------|

	enter	anistrpt	;identify calling routine's parameter
	mov	bp,anistrpt	;point BP to animation data structure
	mov	al,byte ptr[si+skipclr] ;get skip color byte
	mov	bl,al
	and	al,VLOOPNUM	;get the virtual loop number
	mov	cl,4
	shr	al,cl
	mov	ah,byte ptr[bp+loop] ;get the real loop number
	cmp	al,ah		;are the loop numbers the same?
	jz	MCXit		;yes -- no mirroring required for this cell

	mov	al,VLOOPNUM	;set virtual loop number to real loop number
	not	al
	and	al,bl
	shl	ah,cl
	or	al,ah	
	mov	byte ptr[si+skipclr],al
	sub	sp,2048		;reserve stack space for mirrored cell buffer
	mov	di,sp		;point DI at start of mirrored cell buffer
	lodsw			;get cell's X/Y dimensions
	mov	bh,al		;save cell's X dimension
	xor	cx,cx
	mov	cl,ah		;use cell's Y dimension for loop counter
	lodsb			;get cell's skip color
	shl	al,1		;move skip color to high nibble
	shl	al,1
	shl	al,1
	shl	al,1
	mov	bl,al		;save cell's skip color
	push	si		;save the starting address of the cell data
	push	di		;save the starting address of the buffer

MCNewLine:	;Process cell data line
	xor	dx,dx

MCCntLS:	;Count leading skips
	lodsb			;get cell data code
	or	al,al		;is this the end of line code?
	jz	MCEOL		;yes

	mov	ah,al		;duplicate cell code
	and	ax,0F00Fh	;isolate color code (AH hi) & pixel count (AL lo)
	cmp	ah,bl		;is this a leading skip?
	jne	MCUpdPC		;no

	add	dl,al		;add leading skip count to pixel count total
	jmp	MCCntLS

MCCntNLP:	;Count non-leading pixels
	lodsb			;get cell data code
	and	al,0Fh		;isolate pixel count
	jz	MCMLine		;pixel count is zero -- end of line

MCUpdPC:	;Update pixel count
	add	dl,al		;add pixel count to pixel total
	inc	dh		;increment fill codes count
	jmp	MCCntNLP

MCMLine:	;Mirror line
	neg	dl
	add	dl,bh		;leading skips = X dimension - pixel total
	je	MCMirror	;no leading skips in this line

MCLSkips:	;Put leading skip pixels into cell
	mov	al,bl
	sub	dl,15		;are # of leading skip pixels > 15
	jbe	MCLSEnd		;no

	or	al,0Fh		;AL = 15 pixels of skip color
	stosb			;store leading skip code
	jmp	MCLSkips	;15 down and more to go

MCLSEnd:	;Handle last byte of leading skip pixels
	add	dl,15		;compute remaining leading skip pixels
	or	al,dl
	stosb			;store final leading skip code

MCMirror:	;Mirror fill codes
	mov	bp,si
	sub	si,2		;SI points at last byte of pixel data

MCReverse:	;Reverse fill codes
	std
	lodsb			;get cell data byte
	cld
	stosb			;store cell data byte
	dec	dh		;have all bytes been reversed?
	jne	MCReverse	;no

	mov	si,bp

MCEOL:		;End of cell data line
	mov	al,dh
	stosb			;store end of line code
	loop	MCNewLine	;loop until entire cell is mirrored

	pop	si		;get start of buffer address
	mov	cx,di
	sub	cx,si		;compute size of cell data
	pop	di		;get the address of the start of cell data
	rep	movsb		;copy mirrored data back into cell

	add	sp,2048		;clean up stack

MCXit:
	exit


xlatobjc:

;|-----------------------------------------------------------------------------|
;|      This subroutine translates object color codes in a view from the PCjr  |
;|      16 color mode to the PC composite or RGB modes.           	       |
;|-----------------------------------------------------------------------------|

	cmp	machtype,PC	;is the game running on a PC?
	retif	not_equal	;no - don't translate the object color codes

	cmp	monType,MONO	;is a Hercules board installed?
	retif	equal		;yes - don't translate the object color codes

	cmp	monType,EGA	;is an EGA board installed?
	retif	equal		;yes - don't translate the object color codes

	jmp	pcxo

code	ends


	end

