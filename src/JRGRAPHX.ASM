;JRGRAPHX
;Picture drawing subroutines for the PCjr


cgroup	group code
dgroup	group data
assume	cs:cgroup, ds:dgroup, es:dgroup

include	macro.ah
include	cmequ8s.ah
include	jrequ8s.ah


public	jrig, jrts, jrgs, jrsh, jrb2s, jrirt, jrdw


extrn	hsp_adjmt:word, ghrz_pos:byte, thrz_pos:byte, machtype:word
extrn	s_ram:word, v_ram:word, row_table:word, textAtr:word
extrn	BackToScrn:near, getShdwOfst:near, getVidOfst:near



data	segment	byte public 'data'
data	ends		;dummy segment

;masm 'segment at' bug - see scrout.asm
;;;romdata		segment at 40h
;;;	org	65h
;;;crt_mode_set	db	?
;;;romdata		ends
romdata		equ	0040h
crt_mode_set	equ	0065h




code	segment byte public 'code'

jrig:

	mov	ax,89h		;initialize graphics: 320x200 16 color mode
	int	10h
	mov	ah,11		;set background color to black
	xor	bx,bx
	int	10h
	mov	al,0bh		;turn on color burst (BIOS error in TANDY 1000)
	mov	dx,3d8h
	out	dx,al
	call	jrsh		;set horizontal sync position

	push	es
	mov	es,v_ram	;video segment
	xor	ax,ax
	mov	di,ax		;clear entire graphics screen to black
	mov	cx,4000h
	rep	stosw
	pop	es

	ret


jrts:				;set video screen to text mode
	enter
	mov	ax,81h		;set text to 40x25 color mode
	int	10h
	mov	dx,machtype	;get machine type while DS points to DGROUP

;;;assume	ds:romdata
	push	ds
	mov	ax,romdata
	mov	ds,ax
	cmp	dx,JR		;is this a PCjr?

	.if	equal		;yes
		mov	dx,3dah	;Video Gate Array
		in	al,dx	;set to address state
		mov	al,3
		out	dx,al	;select mode control 2
		xor	al,al
		out	dx,al	;change blink to intensity
	.else
		mov	al,8
		mov	ds:crt_mode_set,al
		mov	dx,3d8h
		out	dx,al	;change blink to intensity
	.end

	pop	ds
assume	ds:dgroup

	call	jrsh		;align text screen
	mov	ah,1		;select set cursor type function
	mov	cx,1000h	;invisible cursor
	int	10h
	mov	ah,2		;select set cursor position function
	xor	bh,bh
	xor	dx,dx		;row 0, column 0
	int	10h

	mov	al,byte ptr textAtr	;get character attribute
	mov	ah,6		;scroll window
	xor	al,al		;0 lines -> clear the window
	xor	cx,cx		;upper left of screen
	mov	dx,1827h	;lower right of screen
	int	10h
	exit


jrgs:				;set video screen to graphics mode
	enter
	call	jrig		;set graphics mode
	call	BackToScrn
	exit


jrsh:				;set horizontal sync position
	cmp	machtype,JR	;is this a PCjr?

	.if	equal
		mov	ah,5	;read CRT/CPU page registers
		mov	al,80h
		int	10h	;BH = CRT page
		mov	bl,bh
		xor	bh,bh	;BX = CRT page
		mov	ax,400h	;set AX to segment length per page
		mul	bx	;compute video memory segment
		mov	v_ram,ax
	.end

	mov	ah,15		;get current video mode
	int	10h
	xor	bx,bx
	cmp	al,9		;is screen in graphics mode?

	.if	equal		;set graphics horizontal sync position
		mov	bl,ghrz_pos	  
		add	bx,hsp_adjmt
		mov	ghrz_pos,bl
	.else			;set text horizontal sync position
		mov	bl,thrz_pos
		add	bx,hsp_adjmt
		mov	thrz_pos,bl
	.end

	mov	hsp_adjmt,0	;clear horizontal sync position adjustment
	mov	dx,3d4h		;point to video chip index register
	mov	al,2		;select horiz. sync position register
	out	dx,al		;load video chip index register
	inc	dx		;point to video chip data register
	mov	al,bl		;set horiz. sync position value
	out	dx,al		;shift the screen

	ret


jrb2s:

;|-----------------------------------------------------------------------------|
;|      This subroutine translates a rectangular block of shadow ram into a    |
;|  block of color data in video ram.  The input parameters are: 	       |
;|  			AH = bottom left corner X coordinate		       |
;|  			AL = bottom left corner Y coordinate		       |
;|  			BH = height (# of lines)			       |
;|  			BL = length (# of shadow bytes)			       |
;|-----------------------------------------------------------------------------|

	test	ah,1		;is base X coord even?
	jz	b2s_lnthck	;yes
	dec	ah		;set base X coord to even coord
	inc	bl		;adjust length accordingly

b2s_lnthck:
	inc	bl		;force length to even number of pixels
	and	bl,0FEh
	mov	cx,bx		;save block height & length
	call	getShdwOfst	;get shadow ram offset of block base X,Y
	mov	bx,cx		;restore block height & length
	mov	bp,bx		;compute next shadow line offset factor (nslof)
	and	bp,0FFh		;isolate block length
	add	bp,shdw_lnth	;nslof = shadow line length + shadow block length
	xor	dx,dx		;compute next video line offset factor (nvlof)
	mov	dl,bl		;get video block length
	add	dx,next_vbnk	;nvlof = next bank offset + video block length
	shr	bl,1		;convert shadow block length to words
	mov	si,di		;put shadow ram offset into SI
	mov	cx,bx		;save block height & length
	call	getVidOfst	;put video ram offset into DI
	mov	bx,cx		;restore block height & length
	push	ds		;save data addressing
	push	es
	mov	es,v_ram	;destination is video ram
	mov	ds,s_ram	;source is shadow ram
	xor	cx,cx

b2s_lop1:
	mov	cl,bl		;set object length
	push	dx		;save nvlof

b2s_lop2:
	lodsw			;get 2 shadow bytes: AH=|PL|CL|, AL=|PH|CH|
	and	ax,0F0Fh	;isolate color nibbles: AH=|00|CL|, AL=|00|CH|
	mov	dx,ax
	shl	dx,1		;shift high color nibbles into high nibbles
	shl	dx,1
	shl	dx,1
	shl	dx,1		;DX=|CL|00|CH|00|
	or	ax,dx		;combine low and high color nibbles: AX=|CL|CL|CH|CH|
	stosw			;put color pixels into video ram
	loop	b2s_lop2	;loop until entire block line has been moved
	pop	dx		;restore nvlof
	dec	bh		;are there more lines to move?
	jz	b2s_xit		;no
	sub	si,bp		;move to next shadow line
	sub	di,dx		;move to next video line
	jnb	b2s_lop1	;no bank underflow
	add	di,vadjstmt	;adjust DI to point into next bank
	jmp	b2s_lop1

b2s_xit:
	pop	es		;restore data addressing
	pop	ds
	ret			;return control to calling routine


jrdw:

;|-----------------------------------------------------------------------------|
;|      This subroutine draws a rectangular window in video ram.  The input    |
;| parameters are:							       |
;|  			AH = bottom left corner X coordinate		       |
;|  			AL = bottom left corner Y coordinate		       |
;|  			BH = height (# of lines)			       |
;|  			BL = length (# of shadow bytes)			       |
;|			DL = color ;