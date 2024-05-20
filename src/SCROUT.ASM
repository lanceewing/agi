;SCROUT
;Character output to the screen.

include	macro.ah
include	game.ah
include	cmequ8s.ah

public	PutChar
public	ClrLines
public	ClearLine
public	ClearRect
public	SetCur
public	SaveCurPos
public	RestoreCurPos
public	Scroll
public	InvertChar
public	HalfToneChar


cgroup	group	code
dgroup	group	data


Video		equ	10h
bSetCur		equ	2
bGetCur		equ	3

;bounds of text window
TEXTTOP		equ	21
TEXTLEFT	equ	0
TEXTBOT		equ	24
TEXTRGHT	equ	39
MAXLINES	equ	TEXTBOT-TEXTTOP+1

LINEDELAY	equ	180


BACKSPACE	equ	8
RETURN		equ	13


EGA_CHARS	equ	43h*4		;interrupt vector addr of EGA chars



;;;romSeg	segment at 0f000h
;	MASM bug: if any storage is allocated under a 'segment at',
;	references to the segment base address are assembled wrong.  
;	This ACKNOWLEDGED bug has persisted since version 2.0!!!
;;;	org	0fa6eh
;;;romChars	db	?
;;;romSeg	ends
romSeg		equ	0f000h
romChars	equ	0fa6eh




data	segment byte public 'data'
extrn	playSnd:word
extrn	textAtr:word
extrn	backColor:word
extrn	monType:word
extrn	screenMode:word

public	winCol, savedCurPos, extraChar, halfTone

winCol		dw	0

MAXSAVED	equ	5
savedCurPos	dw	MAXSAVED dup(0)
savedCurIndex	dw	0

;These are for building half-tone characters
EGAChars	dd	?		;space to save EGA char ptr
halfTone	dw	0
extraChar	db	8 dup(0)

data	ends





code	segment public byte 'code'
assume	cs:cgroup,ds:dgroup,es:dgroup

extrn	EraseCur:near
extrn	Error:near




PutChar	proc	near
;Put the character 'pc' to the screen.  Interpret carriage returns and
;backspaces.

	enter	_pc
	
	xor	bh,bh				;set up for page 1
	mov	al,_pc				;get the character
	
	cmp	al,BACKSPACE
	.if	equal
		bios	Video,bGetCur		;get current position
		cmp	dl,TEXTLEFT
		.if	not_equal		;if not at left edge of window,
			dec	dl		;  move back one column
		.else
			cmp	dh,TEXTTOP		;if at left edge of
			.if	above			;  window and not at
				mov	dl,TEXTRGHT	;  top of window, move
				dec	dh		;  to right edge of
				.end			;  previous line
			.end

		push	dx
		xor	al,al		;0 lines -> clear window
		mov	cx,dx
		mov	bh,byte ptr backColor
		bios	Video, ScrollUp
		pop	dx

		xor	bh,bh
		bios	Video,bSetCur
		
		jmp	PCEnd
		.end
	
	cmp	al,RETURN
	.if	not_equal
		cmp	al,LF
		.end
	.if	equal
		bios	Video,bGetCur		;get current position
		cmp	dh,TEXTBOT		;at bottom of screen?
		.if	below			;no
			inc	dh		;move to next row
			.end
		mov	dl,byte ptr winCol
		bios	Video,bSetCur		;set cursor at left edge
		jmp	PCEnd
		.end
	
	;Get and save the current cursor position.
	bios	Video,bGetCur
	push	dx

	;Get the character attribute.
	mov	bl,byte ptr textAtr

	;Do some special handling of graphics characters
	cmp	monType,MONO
	.if	not_equal
		cmp	screenMode,GRAPHMODE
		.if	equal
			;Invert the character if the high bit of the text attribute is set.
			test	textAtr,80h
			.if	nonzero
				call	InvertChar
				mov	_pc,al
				.end
		
			;Halftone the character if the halfTone flag is set.
			cmp	halfTone,0
			.if	not_equal
				call	HalfToneChar
				mov	_pc,al
				.end
	
			;If we're on an EGA, save the pointer to the character set
			;definition.  If the char is 128, set the pointer to the char
			;set so that the EGA will read 'extraChar'
			cmp	monType,EGA
			.if	equal
				push	ds
				xor	cx,cx
				mov	ds,cx
				mov	si,EGA_CHARS
				mov	di,offset dgroup:EGAChars
				movsw			;save old EGA char ptr
				movsw
				cmp	al,128
				.if	equal
					mov	si,EGA_CHARS
					mov	cx,offset dgroup:extraChar
					sub	cx,8*128
					mov	[si],cx
					mov	[si+2],es
					.end
				pop	ds
				.end
	
			and	bl,7fh			;mask off any xor bit
			.end
		.end

	mov	cx,1
	mov	al,_pc			;get the character
	bios	Video,WrtChar
	pop	dx
	inc	dl			;advance the cursor column
	cmp	dl,TEXTRGHT
	.if	below_equal
		bios	Video,bSetCur	;if in the window, set it
	.else
		pcall	PutChar,#RETURN	;out of window -- recurse on
		.end			;  ourself to go to next line

	;If we're on an EGA, restore the character definition pointer.
	cmp	monType,EGA
	.if	equal
		push	es
		xor	cx,cx
		mov	es,cx
		mov	si,offset dgroup:EGAChars
		mov	di,EGA_CHARS
		movsw
		movsw
		pop	es
		.end
	
PCEnd:
	exit


PutChar	endp




SetCur	proc	near
;Set the cursor to the given row and column

	enter	row,col

	mov	dh,row
	mov	dl,col
	xor	bh,bh
	bios	Video,bSetCur

	exit

SetCur	endp



SaveCurPos	proc	near

	enter

	xor	bh,bh
	bios	Video,bGetCur
	mov	bx,savedCurIndex
	cmp	bx,2*MAXSAVED
	.if	below
		mov	savedCurPos[bx],dx
		add	savedCurIndex,2
		.end

	exit

SaveCurPos	endp



RestoreCurPos	proc	near

	enter

	mov	bx,savedCurIndex
	cmp	bx,0
	.if	above
		sub	bx,2
		mov	savedCurIndex,bx
		mov	dx,savedCurPos[bx]
		xor	bh,bh
		bios	Video,bSetCur
		.end

	exit

RestoreCurPos	endp





ClrLines	proc	near
;Clear a group of lines on the text screen

	enter	topline, bottomline, attribute

	pcall	ClearRect,topline,#TEXTLEFT,bottomline,#TEXTRGHT,attribute

	exit

ClrLines	endp


ClearLine	proc	near

	enter	theLine, clrAtr
	pcall	ClrLines, theLine, theLine, clrAtr
	exit

ClearLine	endp



ClearRect	proc	near

	enter	topRow, leftCol, botRow, rightCol, rectAtr
	
	;Save the current cursor position
	xor	bh,bh
	bios	Video,bGetCur
	push	dx

	xor	al,al		;0 lines -> clear window
	mov	ch,byte ptr topRow
	mov	cl,byte ptr leftCol
	mov	dh,byte ptr botRow
	mov	dl,byte ptr rightCol
	mov	bh,byte ptr rectAtr
	bios	Video, ScrollUp

	;Restore the cursor position
	pop	dx
	xor	bh,bh
	bios	Video,bSetCur

	exit

ClearRect	endp



Scroll		proc	near

	enter	ulRow, ulCol, lrRow, lrCol, scAtr

	;Scroll the window
	mov	al,1
	mov	ch,byte ptr ulRow
	mov	cl,byte ptr ulCol
	mov	dh,byte ptr lrRow
	mov	dl,byte ptr lrCol
	mov	bh,scAtr
	bios	Video, ScrollUp

	mov	dh, lrRow
	mov	dl, ulCol
	xor	bh,bh
	bios	Video,bSetCur

	exit

Scroll		endp




InvertChar	proc	near
;Create an inverse character in 'extraChar'

	call	GetRomChar
	mov	ax,0ffffh
	do
		xor	[di],ax
		inc	di
		inc	di
	dloop

	mov	al,128
	return

InvertChar	endp




HalfToneChar	proc	near
;Create a special character in 'extraChar' which is a half-toned '_pc'.

	call	GetRomChar
	mov	ax,1010101001010101b
	do
		or	[di],ax
		inc	di
		inc	di
	dloop

	mov	al,128
	return



HalfToneChar	endp



GetRomChar	proc	near
;Copy the character generation bytes for the character in al from rom
;to 'extraChar'.

	;If the char is 128, we've already got what we want in extraChar.
	xor	ah,ah
	cmp	al,128
	je	GRCExit

	push	ds
	mov	cx,romSeg
	mov	ds,cx

	;Point to our extraChar and the char to get from rom.
	mov	di,offset dgroup:extraChar
	mov	si,romChars
	mov	cl,3
	shl	ax,cl			;char * 8 for index into table
	add	si,ax

	;Move the rom char to our char.
	mov	cx,4
	rep	movsw

	pop	ds

GRCExit:
	;Point di to the resulting char and load cx with the number
	;of words to operate on.
	mov	di,offset dgroup:extraChar
	mov	cx,4

	return

GetRomChar	endp




code	ends
	end

