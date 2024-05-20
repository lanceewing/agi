;EQUIPCHK
;Determine the equipment which we're operating on and set up memory
;management.
;
;Change History:
;87 Aug	pmk
;	Added test for VGA (PS/2)
;20/02/87  10:42:33	JAS
;	Added code to allow forcing machine type and monitor type
;	from the command line.

include	macro.ah
include	game.ah
include cmequ8s.ah
include pcequ8s.ah

public	EquipChk
public	WhichMachine
public	InitMem

USE_VGA	equ	1	;turn on added code

cgroup	group	code
dgroup	group	data



VIDEO		equ	10h
VIDMODE		equ	0Fh
EGAINFO		equ	10h
ALTSELECT	equ	12h
SWSETTINGS	equ	0Ch
EGAMEMORY	equ	3
VMODEMASK	equ	0000000000110000b
TIMEOFDAY	equ	1Ah
INPUT0		equ	0
GRAPHMODE	equ	0
TEXTMODE	equ	1
WHITE		equ	0FFh
BLACK		equ	0

FONTFILESIZE	equ	FONTSIZE*16

equip		equ	11h
driveMask	equ	0000000011000000b

atID		equ	0fch	;PC/AT, PS/2 mod 40,50
jrID		equ	0fdh
ps30ID		equ	0fah	;PS/2 mod 30

AllocMem	equ	48h
FreeMem		equ	49h
SetBlock	equ	4ah
Terminate	equ	4ch		;Terminate process


;	The 'segment at' bug bites again - see scrout.asm
;;;ROMSeg	segment at 0f000h
;;;org	0fffeh
;;;sysID		db	?
;;;ROMSeg	ends
ROMSeg	equ	0f000h
sysID	equ	0fffeh


data	segment public byte 'data'

extrn	var:byte
extrn	pspSeg:word
extrn	heapBase:word
extrn	memPtr:word
extrn	maxMem:word
extrn	s_ram:word
extrn	h_ram:word
extrn	fontRam:word
extrn	noMemMsg:near
extrn	noFontMsg:near
extrn	endSeg:word
extrn	screenMode:word
extrn	winBot:word
extrn	charHeight:word


public	twoDrive, machType, monType
twoDrive	dw	0
progSize	dw	0
heapSize	dw	0
machType	dw	-1
monType		dw	-1

fontFile	db	"HGC_Font",0
tdStr		db	"Tandy"


data	ends




code	segment public byte 'code'
assume	cs:cgroup,ds:dgroup,es:dgroup


extrn	printf:near
extrn	ErrBeep:near
extrn	open:near
extrn	read:near
extrn	close:near
extrn	_SetTextAtr:near




EquipChk	proc	near

	enter
	
	;If machine type was not specified, find out what we're running on.
	cmp	machType,-1
	.if	equal
		call	WhichMachine
		.end
	
	;See if the system has two drives
	bios	equip		;keep ax reg for GetMonType
	test	ax,driveMask
	.if	nonzero			;then more than one drive
		mov	twoDrive,1
		.end

	;If monitor type is not specified, determine what's active.
	cmp	monType,-1
	.if	equal
		call	GetMonType	;needs ax reg from bios equip call
		.end

	;See if the machine has enough memory -- if it does, initialize it
	call	InitMem

	exit


EquipChk	endp





WhichMachine	proc	near
;Determine the type of machine on which we're running.

	mov	machType,PC		;default is a PC or compatible
	
	;Point to the ROM
;;;	assume	es:ROMSeg
	push	es
	mov	ax,ROMSeg
	mov	es,ax
	
	;See if we're on a PCjr
	mov	bl,es:sysID		;get system id byte
	cmp	bl,jrID			;is it a PCjr?
	.if	equal
		mov	machType,JR	;yes -- return;
		jmp	WhichExit
		.end
	
	;Test for the Tandy 1000 by scanning from f000:c000 to f000:c400 for the
	;word 'Tandy'.
	mov	di,0c000h
	do
		lea	si,tdStr
		push	di
		mov	cx,5
		rep	cmpsb
		pop	di
		.if	equal			;found 'Tandy',
			mov	machType,TD	;  so set up for Tandy 1000
			break
			.end
		inc	di
		cmp	di,0c400h
	while	less
	
WhichExit:
	pop	es
	assume	es:dgroup
	return


WhichMachine	endp




GetMonType	proc	near
	;See if a monochrome display is active
	and	ax,VMODEMASK		;ax reg comes from bios equip call
	cmp	ax,VMODEMASK
	.if	equal
       		mov     dx,DSPYSTAT	;point DX at display status (port 3BAh)

		do
			in      al,dx
       			or      al,al
		until	negative

		xor	ah,ah
		int	TIMEOFDAY	;get current clock count
		add	dx,2		;set delay for 2 clock ticks
		mov	ax,dx

		do
			push	ax
	        	mov     dx,DSPYSTAT
			in      al,dx
       			or      al,al
			.if	plus
				pop	ax
				break
				.end

			xor	ah,ah
			int	TIMEOFDAY
			pop	ax
			cmp	ax,dx
			.if	below
				pcall	ErrBeep
				mov	al,1
				dos	Terminate
				.end

		repeat

		mov	  monType,MONO
	.else
		;See what type of color adapter is available
		mov	monType,COMP		;default: CGA/composite
ifdef USE_VGA
		;check for PS/2	- yes, Virginia, there IS a legal way to find out!
		push	es		;int 15h will clobber it!
		mov	ah,0c0h		;return sys config params
		int	15h
		.if	no_carry	;	(some systems don't support it)
			mov	al,es:[bx+2]	;fetch model byte
			cmp	al,ps30ID	;test for PS/2 mod 30
			.if	equal
				mov	machType,PS2
				mov	monType,VGA	;yep - VGA
				pop	es
				jmp	GotIt
				.end
			cmp	al,atID		;PC/AT or PS/2 mods 50, 60
			.if	equal
				mov	al,es:[bx+3]	;submodel byte
				cmp	al,3	;PS/2 mod 50 or 60
				.if	greater
					mov	machType,PS2
					mov	monType,VGA	;yep - VGA
					pop	es
					jmp	GotIt
					.end
				.end
			.end
		pop	es	;restore reg clobbered by int 15h
		;no sys config support, or not PS/2 - doit hardway

ifdef NONO ; this stuff is NOT a good idea - screws up my STD 286!!
		;Do we have Mode 13 capability ? (PS/2 etc)
		mov	ax,0f00h	;See Bios I/F Tech Ref pg 2-19,
		int	VIDEO		;	Read Current Video State
		cmp	al,13h		;are we in mode 13 ? (256-color)
		.if	equal
			mov	monType,VGA	;yes - VGA
			jmp	GotIt
			.end
		;NOTE - there is a nasty rumor that some early
		;cheap clones will parrot back any mode you
		;ask for, whether or not they actually support it.
		;Anybody with one of these can override this test 
		;with an appropriate '-x' on command line.
		push	ax		;save current mode
		mov	ax,0093h	;try to set mode 13 (w/o buffer clear)
		int	VIDEO
		mov	ax,0f00h	;ask again
		cmp	al,13h		;did it take?
		.if	equal
			mov	monType,VGA	;yes
			pop	ax		;balance stack
			jmp	GotIt
			.end
		pop	ax
		int	VIDEO		;restore original mode
endif ; NONO !
		;Keep looking - not VGA
endif ; USE_VGA
		xor	al,al
		mov	bl,EGAINFO		;request Return EGA Information option
		mov	bh,0FFh			;precondition BH with bogus value
		mov	cl,0Fh			;precondition CL with bogus value
		bios	VIDEO,ALTSELECT		;call video BIOS Alternate Select function
		cmp	cl,SWSETTINGS		;is Switch Setting valid for an EGA?
		.if	below			;yes
			cmp	bl,EGAMEMORY	;is Memory Value valid for an EGA?
			.if	below_equal	;yes
				or	bh,bh	;is Mode == color mode?
				.if	zero	;yes
					mov	monType,EGA	;EGA is present
					.end
				.end
			.end
GotIt:
		.end
	return

GetMonType	endp



InitMem		proc	near

	enter
	
	mov	screenMode,TEXTMODE
	pcall	_SetTextAtr,#WHITE,#BLACK
	;Shrink allocated memory to the program size
	mov	bx,endSeg	;get address of end of program
	sub	bx,pspSeg	;total program size
	mov	progSize,bx
	push	es
	mov	ax,pspSeg
	mov	es,ax
	dos	SetBlock	;set memory block to indicated size
	pop	es
	
	;Allocate heap space
	mov	bx,1000h
	add	bx,dgroup
	sub	bx,endSeg
	mov	heapSize,bx
	dos	AllocMem
	.if	carry
		jmp	NoMem		;can't get the memory
		.end
	
	;Convert the heap address to an offset within the data segment
	sub	ax,dgroup
	mov	cl,4
	shl	ax,cl
	mov	memPtr,ax	;set the heap pointer
	mov	heapBase,ax
	mov	ax,heapSize
	mov	cl,4
	shl	ax,cl		;heap size * 16
	add	ax,heapBase
	mov	maxMem,ax
	
	;Allocate space for the PC shadow ram and if necessary, the space for
	;the Hercules shadow ram.
	cmp	monType,MONO

	.if	equal
		;Allocate space for both shadow ram areas
		mov	bx,2*SHADOWSIZE
	.else
		;Allocate space only for PC shadow ram area
		mov	bx,SHADOWSIZE
		.end

	dos	AllocMem
	.if	carry
		jmp	NoMem		;can't get the memory
		.end

	mov	s_ram,ax
	cmp	monType,MONO

	.if	not_equal
		jmp	IMXit
		.end
	add	ax,SHADOWSIZE
	mov	h_ram,ax
	mov	bx,FONTSIZE
	dos	AllocMem
	.if	carry
		jmp	NoMem		;can't get the memory
		.end

	mov	fontRam,ax
	pcall	open,&fontFile,#INPUT0
	or	ax,ax
	.if	negative
		pcall	printf,&noFontMsg
		jmp	ErrXit		;can't find the HGC font file
		.end

	push	ax
	pcall	read,ax,heapBase,#FONTFILESIZE
	pop	ax
	pcall	close,ax
	mov	si,heapBase
	push	es
	mov	es,fontRam
	xor	di,di
	mov	cx,FONTSIZE*16
	rep	movsb
	mov	winBot,24
	mov	charHeight,7
	pop	es

IMXit:
	mov	screenMode,GRAPHMODE

	exit
	
	
NoMem:
	pcall	printf,&noMemMsg

ErrXit:
	pcall	ErrBeep
	mov	al,1
	dos	Terminate



InitMem		endp





code	ends
	end
