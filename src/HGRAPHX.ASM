;HGRAPHX
;Graphics subroutines for the Hercules graphics card


public	HIG, HTS, HSH, HB2S, HIRT, HDW, HBS, HCVB, HVideoInt, HSS

public	patrnMasks


cgroup	group code
dgroup	group data
assume	cs:cgroup, ds:dgroup, es:dgroup
	
extrn	monType:word, s_ram:word, v_ram:word, h_ram:word, row_table:word,
extrn	textAtr:word, picReloc:word, fontRam:word, oldVideo:dword
extrn	halfTone:word
extrn	BackToScrn:near, getShdwOfst:near, getVidOfst:near

include macro.ah
include	cmequ8s.ah
include	pcequ8s.ah

NORMAL		equ	07h
INVERSE		equ	70h
HPG0OFFSET	equ	0BAh		;offset into page #0 for HGC screen shake
HPG1OFFSET	equ	5		;offset into page #1 for HGC screen shake


data	segment byte public 'data'

hGCTbl 		db      035h, 02Dh, 02Eh, 007h, 05bh, 002h, 057h, 057h
        	db      002h, 003h, 000h, 000h, 000h, 000h, 000h, 000h

patrnMasks	db	000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h	;black
		db	088h, 000h, 000h, 000h, 022h, 000h, 000h, 000h	;dark blue
		db	080h, 010h, 002h, 020h, 001h, 008h, 040h, 004h	;dark green
		db	0AAh, 000h, 0AAh, 000h, 0AAh, 000h, 0AAh, 000h	;dark cyan
		db	022h, 088h, 022h, 088h, 022h, 088h, 022h, 088h	;red
		db	088h, 000h, 088h, 000h, 088h, 000h, 088h, 000h	;purple
		db	011h, 022h, 044h, 088h, 011h, 022h, 044h, 088h	;brown
		db	055h, 0AAh, 055h, 0AAh, 055h, 0AAh, 055h, 0AAh	;light gray
		db	022h, 000h, 088h, 000h, 022h, 000h, 088h, 000h	;dark gray
		db	0D7h, 0FFh, 07Dh, 0FFh, 0D7h, 0FFh, 07Dh, 0FFh	;light blue
		db	0DDh, 055h, 077h, 0AAh, 0DDh, 055h, 077h, 0AAh	;light green
		db	07Fh, 0EFh, 0FDh, 0DFh, 0FEh, 0F7h, 0BFh, 0FBh	;light cyan
		db	0AAh, 0FFh, 0AAh, 0FFh, 0AAh, 0FFh, 0AAh, 0FFh	;pink
		db	077h, 0BBh, 0DDh, 0EEh, 077h, 0BBh, 0DDh, 0EEh	;orange
		db	077h, 0FFh, 0FFh, 0FFh, 0DDh, 0FFh, 0FFh, 0FFh	;yellow
		db	0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh	;white

data	ends


code	segment byte public 'code'

HIG:
	enter

	call	HCVB		;clear HGC display buffer

        mov     dx,CONFIGSW	;set configuration switch for graphics mode
        mov     al,3
        out     dx,al
        mov     dx,DSPYMODE	;set display mode control port for blank screen
        mov     al,82h		;display buffer starts @B800h
        out     dx,al
        mov     dx,MONOINDX	;re-program the 6845 for graphics mode
        mov     ah,00		;start 6845 index register @0
        lea     si,hGCTbl	;point SI at 6845 parameter table
        mov     cx,16		;all 16 parameters must be re-programmed

	do
		mov     al,ah
        	out     dx,al	;set 6845 index register
        	inc     dx   	;point DX at 6845 data register (port 3B5h)
        	mov     al,[si]
        	out     dx,al	;write parameter to 6845 data register
        	inc     ah   	;increment 6845 index register
        	dec     dx   	;point DX at 6845 index register (port 3B4h)
        	inc     si   	;point SI at next 6845 parameter
		dec 	cx
	until	zero

        mov     dx,DSPYSTAT	;point DX at display status (port 3BAh)
	mov	cx,5

	do
		do
			in      al,dx
        		or      al,al
		until	negative

		do
			in      al,dx
        		or      al,al
		until	plus

		dec	cx
	until	zero

        mov     dx,DSPYMODE	;set display mode control port for active screen
        mov     al,8Ah
        out     dx,al

	call	BackToScrn

	exit


HTS:				;set video screen to text mode
	enter

	call	HCVB

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


HSH:				;set horizontal sync position
	ret			;If a Hercules board is active, don't shift





;|----------------------------------------------------------------------------|
;|									      |
;|        	   Convert IBM PC Graphics to Hercules Graphics	      	      |
;|									      |
;|----------------------------------------------------------------------------|

HBS:
	push	es
	mov	es,h_ram

	push	ds
	mov	ds,s_ram

	xor	cx,cx		;starting PC line number
	mov	si,cx
	mov	di,cx

NxtLine:
	push	cx		;save current PC line number
	mov	dx,cx
	and	dx,3
	shl	dx,1		;DX = primary pattern offset
	mov	cx,shdw_lnth/2	;# of pixel pairs in each PC shadow ram line

NxtPair:
	push	cx		;save # of pixel pairs remaining to be processed
	lodsw			;get 2 shadow bytes: AH=|PO|CO|, AL=|PE|CE|
	and	ax,0F0Fh	;isolate color nibbles: AH=|00|CO|, AL=|00|CE|
	cmp	al,ah		;are both pixels the same color?
	jne	Nibblize	;no -- pattern has to be nibblized

	shl	al,1
	shl	al,1
	shl	al,1		;AL = offset of pattern mask group
	mov	bx,dx
	add	bl,al		;BX = offset of pattern mask pair
	mov	al,ss:patrnMasks[bx+1]
	mov	es:[di+HGCSLNTH],al ;put secondary line pattern into hercules ram
	mov	al,ss:patrnMasks[bx]
	stosb			;put primary line pattern into hercules ram
	jmp short NewLineCk

Nibblize:
	shl	al,1
	shl	al,1
	shl	al,1		;AL = offset of pattern mask group
	mov	bx,dx
	add	bl,al		;BX = offset of pattern mask pair
	mov	cl,ss:patrnMasks[bx+1] ;get secondary pattern
	and	cl,0F0h		;isolate even X coordinate nibble
	mov	al,ss:patrnMasks[bx] ;get primary pattern
	and	al,0F0h		;isolate even X coordinate nibble
	shl	ah,1
	shl	ah,1
	shl	ah,1		;AH = offset of pattern mask group
	mov	bx,dx
	add	bl,ah		;BX = offset of pattern mask pair
	mov	ch,ss:patrnMasks[bx+1] ;get secondary pattern
	and	ch,0Fh		;isolate odd X coordinate nibble
	mov	ah,ss:patrnMasks[bx] ;get primary pattern
	and	ah,0Fh		;isolate odd X coordinate nibble
	or	cl,ch		;combine secondary odd & even nibbles
	mov	es:[di+HGCSLNTH],cl ;put secondary line pattern into hercules ram
	or	al,ah		;combine primary odd & even nibbles
	stosb			;put primary line pattern into hercules ram
	
NewLineCk:
	pop	cx
	loop	NxtPair		;loop until entire line is processed

	add	di,HGCSLNTH	;increment to next HGC shadow ram line
	pop	cx
	inc	cx		;increment to next line number
	cmp	cx,y_max	;have all lines been processed?
	jbe	NxtLine		;no -- process next line

	pop	ds		;restore data addressing
	pop	es

	ret



;|-----------------------------------------------------------------------------|
;|      This subroutine translates a rectangular block of shadow ram into a    |
;|  block of color data in video ram.  The input parameters are: 	       |
;|  			AH = bottom left corner X coordinate		       |
;|  			AL = bottom left corner Y coordinate		       |
;|  			BH = height (# of lines)			       |
;|  			BL = length (# of PC shadow bytes)			       |
;|-----------------------------------------------------------------------------|

HB2S:
	test	ah,1		;is base X coord even?
	jz	B2SLnthCk	;yes

	dec	ah		;set base X coord to even coord
	inc	bl		;adjust length accordingly

B2SLnthCk:
	inc	bl		;force length to even number of pixels
	and	bl,0FEh
	mov	cx,bx		;save block height & length
	call	getShdwOfst	;get shadow ram offset of block base X,Y
	sub	di,bx		;adjust offset for HGC 80 byte lines (BX = X)
	shr	bx,1
	add	di,bx
	mov	si,di		;put shadow ram offset into SI
	call	getVidOfst	;put video ram offset into DI
	mov	bx,cx		;restore block height & length
	shr	bl,1		;convert PC shadow block length to HGC shadow block length
	mov	bp,bx		;compute next shadow line offset factor (nslof)
	and	bp,0FFh		;isolate HGC shadow block length
	add	bp,shdw_lnth	;nslof = shadow line length + shadow block length
	xor	dx,dx		;compute next video line offset factor (nvlof)
	mov	dl,bl		;get video block length
	add	dx,HNEXTVBNK	;nvlof = next bank offset + video block length
	push	ds		;save data addressing
	push	es
	mov	es,v_ram	;destination is video ram
	mov	ds,h_ram	;source is HGC shadow ram
	xor	cx,cx

B2SLoop1:
	mov	cl,bl		;set object video length (in bytes)

B2SLoop2:
	mov	al,[si+HGCSLNTH] ;move one byte of Hercules secondary shadow ram
	mov	es:[di+HNEXTVBNK/2],al
	movsb			;move one byte of Hercules primary shadow ram
	loop	B2SLoop2	;loop until entire object line has been moved

	dec	bh		;are there more lines to move?
	jz	B2SXit		;no

	sub	si,bp		;move to next shadow line
	sub	di,dx		;move to next video line
	jnb	B2SLoop1	;no bank underflow

	add	di,HVADJSTMT	;adjust DI to point into bank 2 of Hercules video ram
	jmp	B2SLoop1

B2SXit:
	pop	es		;restore data addressing
	pop	ds

	ret			;return control to calling routine


;|-----------------------------------------------------------------------------|
;|      This subroutine draws a rectangular window in video ram.  The input    |
;| parameters are:							       |
;|  			AH = bottom left corner X coordinate		       |
;|  			AL = bottom left corner Y coordinate		       |
;|  			BH = height (# of lines)			       |
;|  			BL = length (# of shadow bytes)			       |
;|			DL = color (black, blue, green, red, white, or yellow) |
;|				Note: black is substituted for all non-white   |
;|				      colors.				       |
;|-----------------------------------------------------------------------------|

HDW:
	push	es		;save data addressing
	mov	es,v_ram	;establish video ram addressing

	cmp	dl,0Fh		;is color white?

	.if	not_equal	;no
		xor	dl,dl	;substitue black for non-white colors
	.else
		mov	dl,0FFh	;put white into both nibbles of DL
		.end

DW1:
	mov	cx,ax
	call	DWPP		;plot start point
	mov	ax,cx
	xor	ch,ch
	mov	cl,bl
	test	ah,1		;is start X odd?
	jz	DW2		;no

	dec	cl		;odd nibble completes a byte -- reduce nibble cnt
	jz	DW5		;nibble count == 0 -- line is complete

	inc	di		;move to next byte

DW2:
	push	ax
	add	ah,bl		;compute ending X coordinate
	dec	ah
	test	ah,1		;is ending X coord even?
	jnz	DW3		;no

	push	di		;save DI (left end of line)
	call	DWPP		;plot end point
	dec	cl
	pop	di

DW3:
	jcxz	DW4		;if CX == 0 line is complete

	shr	cx,1		;convert window length from nibbles to bytes
	mov	al,dl		;set line color
	mov	dl,cl		;save window length
	mov	si,di		;save primary line starting offset
	add	di,HNEXTVBNK/2	;compute secondary line starting offset
	rep	stosb		;draw secondary line
	mov	di,si		;restore primary line starting offset
	mov	cl,dl		;restore window length
	rep	stosb		;draw primary line
	mov	dl,al		;put line color back into DL

DW4:
	pop	ax

DW5:
	dec	bh		;have all lines been drawn?
	jz	DWXit		;yes

	dec	al		;move Y coord up one line
	jmp	DW1

DWXit:
	pop	es		;restore data addressing
	ret

DWPP:				;plot point in video ram
	mov	si,bx
	call	getVidOfst	;convert X/Y coords into a video ram offset
	mov	bx,si
	mov	dh,0Fh		;load even nibble plot mask
	test	ah,1		;is X coord even?
	jz	DWPP1		;yes

	not	dh		;set plot mask to odd nibble

DWPP1:
	mov	al,es:[di]	;get byte from primary line of video ram
	and	al,dh		;clear odd/even video nibble
	not	dh
	mov	ah,dl		;set color mask to odd/even nibble
	and	ah,dh
	or	al,ah		;put color nibble into odd/even video nibble
	mov	es:[di],al	;put updated primary byte into video ram
	mov	al,es:[di+HNEXTVBNK/2] ;get byte from secondary line of video ram
	mov	ah,dl		;set color mask to odd/even nibble
	and	ah,dh
	not	dh
	and	al,dh		;clear odd/even video nibble
	or	al,ah		;put color nibble into odd/even video nibble
	mov	es:[di+HNEXTVBNK/2],al ;put updated secondary byte into video ram

	ret


HIRT:
	lea	di,row_table
	mov	ax,HGCSRO	;set starting row offset
	cmp	ax,es:[di]	;has the table been initialized already?
	je	IRT_XIT		;yes - no need to do it again

	mov	dx,HSCRNROWS/BANKS ;# of rows/bank in HGC video ram

IRT_RWLOP:
	mov	cx,BANKS 	;one loop for each bank

IRT_LNLOP:
	stosw			;put row starting offset in the table
	add	ax,HNEXTVBNK	;increment to bank 2
	loop	IRT_LNLOP

	sub	ax,HVADJSTMT	;get offset of next row in bank 0
	dec	dx		;is this the last row?
	jnz	IRT_RWLOP	;no

	call	HIG		;initialize graphics mode & clear screen

IRT_XIT:
	ret


HCVB:
	push	es
	mov	ax,v_ram	;establish HGC addressing
	sub	ax,HGCBUFSZ/16	;AX = HGC page 0 segment address (B000h)
	mov	es,ax
	xor	ax,ax
	mov	di,ax
	mov	cx,HGCBUFSZ	;# of words in HGC video buffer (pages 0 & 1)
	rep	stosw
	pop	es

	ret


HVideoInt:
	cmp	ah,WrtChar		;is this a write character request?
	.if	not_equal		;no
		cmp	ah,ScrollUp	;is this a text scroll up request?
		.if	not_equal	;no -- pass request on to the BIOS
			jmp	oldVideo
			.end
		.end

	cli				;disable interrupts
	pusha
	cld
	cmp	ah,WrtChar		;is this a write character request?
	.if	equal
		call	HWrtChar	;yes -- write character
	.else
		call	HScrollUp	;no -- must be a scroll up then
		.end

	popa
	sti				;enable interrupts

	iret


HWrtChar:
	cbw			;AX = ASCII character
	mov	cl,3
	shl	ax,cl
	mov	si,ax		;SI = ASCII character * 8
	neg	si		;SI = -(ASCII character * 8)
	shl	ax,1
	shl	ax,1		;AX = ASCII character * 32
	add	si,ax		;SI = ASCII character * 24
	or	dh,dh		;DH = cursor row number
	.if	nonzero
		mov	cx,7	;CX = number of screen line pairs in rows 1-24
	.else
		mov	cx,6	;CX = number of screen line pairs in row 0
		.end
	call	HGetCharXY
	call	getVidOfst	;put video ram offset of cursor X,Y into DI
	test	textAtr,INVERSE	;set normal/inverse mask
	.if	nonzero
		mov	bx,0FFFFh ;inverse -- dark characters on light background
	.else
		xor	bx,bx	;normal -- light characters on dark background
		.end
	cmp	halfTone,0
	.if	not_equal
		mov	dx,1010101010101010b
	.else
		xor	dx,dx
		.end
	push	es		;establish video ram addressing
	mov	es,v_ram
	push	ds
	mov	ds,fontRam	;establish font ram addressing
	cmp	cx,7		;clear top two lines in rows 1-24 only
	.if	equal
		xor	ax,ax
		xor	ax,bx	;apply normal/inverse mask to pattern
		or	ax,dx	;apply full/half tone mask to pattern
		mov	es:[di+HNEXTVBNK/2],ax ;clear secondary video line
		jmp	WCPLine
		.end

WCLoop:
	lodsw				;get secondary line font pattern
	xor	ax,bx			;apply normal/inverse mask to pattern
	or	ax,dx			;apply full/half tone mask to pattern
	mov	es:[di+HNEXTVBNK/2],ax	;write to secondary video line
	lodsw				;get primary line font pattern
	xor	ax,bx			;apply normal/inverse mask to pattern
	or	ax,dx			;apply full/half tone mask to pattern

WCPLine:
	stosw				;write to primary video line
	add	di,HNEXTVBNK-2		;move to next line
	.if	negative
		sub	di,HVADJSTMT
		.end
	loop	WCLoop

	pop	ds			;restore data addressing
	pop	es

	ret


HScrollUp:			;NOTE: row 0 is 1 line pair shorter than rows 1-24
	push	ds		;save data addressing
	push	es
	mov	bl,ch		;BL = row number of upper left corner
	push	bx		;save attribute for blank lines (BH)
	push	ax
	push	dx
	mov	dx,cx		;DH = row, DL = column number of upper left corner
	add	dh,al		;DH = row number to be scrolled from
	call	HGetCharXY	;convert column,row to X,Y coordinates
	or	ch,ch		;is text row 0 the top row of the window?
	.if	zero
		inc	al	;yes -- adjust Y coordinate by 1
		.end
	call	getVidOfst	;DI = video ram offset of X,Y
	mov	si,di		;SI = offset of 1st byte to be scrolled from
	mov	dx,cx		;DH = row number of upper left corner
	call	HGetCharXY	;convert column,row to be scrolled into to X,Y
	call	getVidOfst	;DI = offset of 1st byte to be scrolled into
	pop	dx
	pop	ax
	mov	bx,v_ram	;establish video ram addressing
	mov	es,bx
	mov	ds,bx
	xor	bx,bx
	mov	bl,dl
	inc	bl
	sub	bl,cl
	shl	bl,1		;BX = width of window in bytes
	mov	dl,dh
	inc	dl
	sub	dl,ch		;DL = # of text rows in window
	cmp	dl,al		;is line count >= window size?
	.if	below_equal
		mov	ch,-1	;CH = row 0 adjusment factor
		jmp	HSUClrWndw ;yes -- clear entire window
		.end
	or	al,al		;is entire window to be scrolled?
	.if	zero
		mov	ch,-1	;CH = row 0 adjusment factor
		jmp	HSUClrWndw ;yes -- clear entire window
		.end
	mov	ah,dl
	mov	dl,al		;DL = # of text rows to clear at bottom of window
	sub	ah,al		;AH = # of text rows to be scrolled up
	mov	al,ah
	mov	cl,3
	shl	al,cl
	sub	al,ah		;AL = # of screen line pairs to be scrolled up
	or	ch,ch		;is text row 0 the top row of the window?
	.if	zero
		dec	al	;yes -- compensate for row 0
		.end
	xor	cx,cx
	mov	cl,al		;CX = # of screen line pairs to be scrolled up
	mov	ax,HNEXTVBNK/2
	sub	ax,bx		;AX = next line pair offset factor


	do
		push	cx	;save # of line pairs left to scroll up
		mov	cx,bx	;CX = width of window in words
		rep	movsb	;move primary line
		add	si,ax	;move to source secondary line
		add	di,ax	;move to destination secondary line
		mov	cx,bx	;CX = width of window in bytes
		rep	movsb	;move secondary line
		add	si,ax	;move to next source line pair
		.if	negative
			sub	si,HVADJSTMT
			.end
		add	di,ax	;move to next destination line pair
		.if	negative
			sub	di,HVADJSTMT
			.end
		pop	cx	;restore # of line pairs left to scroll up
	dloop

	mov	ch,1		;CH = row 0 adjusment factor

HSUClrWndw:
	mov	dh,dl
	mov	cl,3
	shl	dl,cl
	sub	dl,dh		;DL = # of line pairs to clear
	pop	ax		;AL = row number of upper left corner
	or	al,al		;is the upper left corner row number == 0?
	.if	zero
		add	dl,ch	;yes -- adjust DL for shorter height of row 0
		.end
	xor	ch,ch
	mov	cl,dl		;CX = # of line pairs to clear
	mov	dx,HNEXTVBNK/2
	sub	dx,bx		;DX = next line pair offset factor
	cmp	ah,INVERSE	;is text background white?
	.if	equal
		mov	al,0FFh	;yes -- set clear pattern to white
	.else
		cmp	ah,NORMAL ;is text background black?
		.if	equal
			xor	al,al	;yes -- set clear pattern to black
		.else
			mov	al,ah	;use pattern passed in AH
			.end
		.end

	do
		push	cx	;save # of lines left to clear
		mov	cx,bx	;CX = width of window in bytes
		rep	stosb	;clear primary line
		add	di,dx	;move to secondary line
		mov	cx,bx	;CX = width of window in bytes
		rep	stosb	;clear secondary line
		add	di,dx	;move to next line pair to be cleared
		.if	negative
			sub	di,HVADJSTMT
			.end
		pop	cx	;restore # of line pairs left to clear
	dloop	

	mov	bh,ah		;restore blank line attribute
	pop	es		;restore data addressing
	pop	ds

	ret


HGetCharXY:
	mov	ah,dl		;AH = cursor column
	shl	ah,1
	shl	ah,1		;AH = X coordinate of cursor column
	mov	al,dh		;AL = cursor row
	shl	al,1
	shl	al,1
	shl	al,1		;AL = cursor row * 8
	.if	nonzero
		sub	al,dh	;AL = cursor row * 7
		dec	al	;AL = (cursor row * 7) - 1
		.end
	mov	bx,picReloc	;neutralize effect of picReloc in getVidOfst
	sub	al,bl

	ret


HSS:
	push	bx		;save registers
	push	si
	push	di

	push	cx		;save # of times to shake screen

	push	es		;save data addressing
	push	ds

	mov	ax,v_ram	;establish HGC addressing
	sub	ax,HGCBUFSZ/16	;AX = HGC page 0 segment address (B000h)
	mov	es,ax
	mov	ds,v_ram	;AX = HGC page 1 segment address (B800h)
	mov	si,HPG1OFFSET
	mov	di,HPG0OFFSET
	mov	cx,HSCRNROWS-4	;only shake 170 line pairs each time
	mov	ax,(HNEXTVBNK/2)-(HGCVLNTH-10) 	;AX = next line pair offset factor

	do
		push	cx
		mov	cx,(HGCVLNTH-10)/2	;move 40 words on each line
		rep	movsw			;move primary line
		add	si,ax			;move to secondary line
		add	di,ax
		mov	cx,(HGCVLNTH-10)/2	;move 40 words on each line
		rep	movsw			;move secondary line
		add	si,ax			;move to next line pair
		add	di,ax
		.if	negative
			sub	si,HVADJSTMT
			sub	di,HVADJSTMT
			.end
		pop	cx
	dloop

	pop	ds		;restore data addressing
	pop	es

	pop	cx		;restore shake counter

	do
		push	cx
		mov	cx,4		;switch 4 times to screen page 0 and back to page 1

		do
			push	cx		;# of times left to switch pages
			mov	bl,0Ah		;BL = page # to switch to

			do
        			mov     dx,DSPYMODE	;set screen page #
        			mov     al,bl
        			out     dx,al
			
				mov	dx,DSPYSTAT	;video status register
				mov	cx,4		;wait for 4 vertical retrace periods

				do
					do			;wait for start of vertical retrace
						in	al,dx	;read video status register
						or	al,al 	;test for start of vertical retrace
					until	negative

					do			;wait for end of vertical retrace
						in	al,dx	;read video status register
						or	al,al 	;test for end of vertical retrace
					until	plus
				dloop	

				xor	bl,80h		;toggle screen page number
			until	plus

			pop	cx		;restore switch counter
		dloop

		pop	cx		;restore shake counter
	dloop
			
	pop	di		;restore registers
	pop	si
	pop	bx

	ret

code	ends


end
