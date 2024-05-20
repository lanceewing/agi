;MISC
;Miscellaneous assembler routines

include	macro.ah
include	game.ah
include	trace.ah


public	SetMode
public	GetMode
public	GetChar
public	In
public	Out
public	MoveByte
public	FillMem
public	VerifyGameID


cgroup	group	code
dgroup	group	data


;BIOS video functions
video		equ	10h
bsetmod		equ	0		
bgetmod		equ	15


;BIOS keyboard stuff
kbd		equ	16h
getch		equ	0
chkkbd		equ	1
getshift	equ	2
scrollLock	equ	10h


data	segment public byte 'data'

extrn	var:byte
extrn	sysTimer:dword
extrn	gameID:near
extrn	traceStatus:word

time		dd	0
scrollState	db	0ffh

data	ends





code	segment byte public 'code'
assume	cs:cgroup, ds:dgroup, es:dgroup


extrn	Quit:near
extrn	_TraceOn:near
extrn	TraceOff:near




SetMode	proc	near
;Set video mode.

	enter	smode
	mov	al,byte ptr smode
	bios	video,bsetmod
	exit

SetMode	endp



GetMode	proc	near
;Return the current video mode.

	enter
	bios	video,bgetmod
	xor	ah,ah
	exit	

GetMode	endp




GetChar	proc	near
;Returns a char as Int, usIng the extended ASCII of the PC.  If no char is
;ready, returns NULL.

	enter

	;Check for the scroll-lock state changing -- if it does,
	;either start or stop tracing, depending on the state of tracing.
	bios	kbd,getshift
	and	al,scrollLock
	cmp	scrollState, 0ffh
	.if	equal
		mov	scrollState,al
	.else
		cmp	al,scrollState
		.if	not_equal
			mov	scrollState,al
			cmp	traceStatus,TRACE_OFF
			.if	equal
				call	_TraceOn
			.else
				call	TraceOff
				.end
			.end
		.end


	;Check for characters
	bios	kbd,chkkbd
	.if	zero
		xor	ax,ax		;no char available
	.else
		bios	kbd,getch
		test	al,al
		.if	nonzero
			xor	ah,ah
			.end
		.end
	exit	

GetChar	endp



In	proc	near
;Read the port 'Inport'

	enter	Inport

	mov	dx,Inport			;get the port number
	In	al,dx				;read the port
	xor	ah,ah

	exit

In	endp



Out	proc	near
;Send 'value' to 'Outport'.

	enter	Outport,value

	mov	dx,Outport
	mov	al,value
	Out	dx,al

	exit

Out	endp



MoveByte	proc	near
;Move 'movelen' bytes of memory from 'fromseg:from' to 'toseg:to'

	enter	toseg,to,fromseg,from,movelen

	push	ds
	push	es

	mov	ax,fromseg
	mov	ds,ax
	mov	si,from
	mov	ax,toseg
	mov	es,ax
	mov	di,to
	mov	cx,movelen
	rep	movsb
	mov	ax,movelen

	pop	es
	pop	ds

	exit

MoveByte	endp



FillMem		proc	near
;Fill memory with a certain value

	enter	fillStart, fillLen, fillVal

	mov	di,fillStart
	mov	cx,fillLen
	mov	al,fillVal
	rep	stosb

	;Return the start address
	mov	ax,fillStart

	exit

FillMem		endp




VerifyGameID	proc	near
;Verify that the gameID being set is that which this interpreter is designed
;for.  Quit if it is not.

	enter

	mov	si, offset dgroup:gameID
	mov	di, offset cgroup:checkID
	do
		lodsb			;get a byte of the ID being set.
		cmp	al,byte ptr cs:[di]
		.if	not_equal
			call	Quit
			.end
		inc	di
		or	al,al
	until	zero

	exit


checkID		db	"gameIDX"

VerifyGameID	endp


code	ends
	end

;VGJMPTBL
;Jump table for VGA graphics routines




cgroup	group	code

code	segment public byte 'code'
assume	cs:cgroup


extrn	vgig:near, vgts:near, vggs:near, vgsh:near, vgb2s:near, vgirt:near
extrn	vgdw:near

initgrfx:			;called from cmgraphx.asm
	jmp	vgig

textscrn:			;never called
	jmp	vgts

grafscrn:			;never called
	jmp	vggs

sethorz:			;never called
	jmp	vgsh

blk2Scrn:			;called from cmgraphx.asm
	jmp	vgb2s

initrows:			;called from cmgraphx.asm
	jmp	vgirt

DrawWndw:			;called from cmgraphx.asm
	jmp	vgdw

code	ends


end

