;INTRPT
;Grab/restore various interrupts.

include	macro.ah
include	game.ah
include cmequ8s.ah

public	TimerIntHandler
public	SetInterrupts
public	ResetInterrupts
public	TimerDispatch
public	DeadStack
public	NullInterrupt
public	CriticalError


cgroup	group	code
dgroup	group	data


SetInt		equ	25h
GetInt		equ	35h

prtScrInt	equ	5
timeInt		equ	8
kbdInt		equ	9
videoInt	equ	10h
userTimeInt	equ	1ch
charInt		equ	1fh
ctrlBrkInt	equ	23h
critErrInt	equ	24h


timerPort	equ	40h	;address of counter port for interval timer
EOI		equ	20h	;end-of-interrupt to 8259


;This is the counter to load the programmable interval timer with in order
;to get interrupts at a rate of 60/second.  Clock to the PIT is 1.19318 MHz.
TIMER_DIVISOR	equ	19886
TIMER_COUNT	equ	3	;issue BIOS interrupt every 3 interrupts



data	segment	public byte 'data'

extrn	stackBase:word
extrn	playSnd:word
extrn	monType:word
extrn	extraChar:byte

public	oldKbd, oldVideo, dosError

oldTimer	dd	?
oldUserTimer	dd	?
oldCtrlBrk	dd	?
oldCritErr	dd	?
oldPrtScr	dd	?
oldKbd		dd	?
oldVideo	dd	?
oldChar		dd	?

dosError	dw	?

timerCounter	db	TIMER_COUNT

badStackMsg	db	"Stack blown.",LF
		db	"Press ESC to exit.",0

data	ends




code	segment	public byte 'code'
assume	cs:cgroup,ds:dgroup

extrn	PlayIt:near
extrn	ErrBeep:near
extrn	WindowNoWait:near
extrn	GetChar:near
extrn	Quit:near
extrn	TimerInt:near
extrn	KbdHandler:near
extrn	HVideoInt:near



SetInterrupts	proc	near
;Set the address of our various interrupt handlers.

	;First, save the address of the current handlers
	push	es

	dos	GetInt,prtScrInt
	mov	word ptr oldPrtScr,bx
	mov	word ptr oldPrtScr[2],es

	dos	GetInt,kbdInt
	mov	word ptr oldKbd,bx
	mov	word ptr oldKbd[2],es

	dos	GetInt,timeInt
	mov	word ptr oldTimer,bx
	mov	word ptr oldTimer[2],es

	dos	GetInt,userTimeInt
	mov	word ptr oldUserTimer,bx
	mov	word ptr oldUserTimer[2],es

	dos	GetInt,ctrlBrkInt
	mov	word ptr oldCtrlBrk,bx
	mov	word ptr oldCtrlBrk[2],es

	dos	GetInt,critErrInt
	mov	word ptr oldCritErr,bx
	mov	word ptr oldCritErr[2],es

	dos	GetInt,charInt
	mov	word ptr oldChar,bx
	mov	word ptr oldChar[2],es

	cmp	monType,MONO
	.if	equal
		dos	GetInt,videoInt
		mov	word ptr oldVideo,bx
		mov	word ptr oldVideo[2],es
		.end

	pop	es

	;Point the interrupts to our routines
	push	ds

	mov	dx,offset cgroup:NullInterrupt
	mov	ax,cs
	mov	ds,ax
	dos	SetInt,prtScrInt
	dos	SetInt,ctrlBrkInt

	mov	dx,offset cgroup:KbdHandler
	mov	ax,cs
	mov	ds,ax
	dos	SetInt,kbdInt

	;Change the timer interrupt frequency to 60/second
	mov	al,36h			;timer 0, LSB, MSB, mode 3
	out	timerPort+3,al
	mov	ax,TIMER_DIVISOR
	out	timerPort,al
	xchg	ah,al
	out	timerPort,al

	mov	dx,offset cgroup:TimerIntHandler
	mov	ax,cs
	mov	ds,ax
	dos	SetInt,timeInt

	mov	dx,offset cgroup:TimerDispatch
	mov	ax,cs
	mov	ds,ax
	dos	SetInt,userTimeInt

	mov	dx,offset cgroup:CriticalError
	mov	ax,cs
	mov	ds,ax
	dos	SetInt,critErrInt

	mov	dx,offset dgroup:extraChar
	mov	ax,es
	mov	ds,ax
	dos	SetInt,charInt

	cmp	es:monType,MONO
	.if	equal
		mov	dx,offset cgroup:HVideoInt
		mov	ax,cs
		mov	ds,ax
		dos	SetInt,videoInt
		.end

	pop	ds

	return

SetInterrupts	endp





ResetInterrupts	proc	near
;Set all interrupts back to their original handlers

	lds	dx,oldChar
	dos	SetInt,charInt
	mov	ax,es
	mov	ds,ax

	lds	dx,oldPrtScr
	dos	SetInt,prtScrInt
	mov	ax,es
	mov	ds,ax

	;Set the timer frequency back to normal (18.2/second)
	mov	al,36h			;timer 0, LSB, MSB, mode 3
	out	timerPort+3,al
	xor	al,al
	out	timerPort,al
	out	timerPort,al

	lds	dx,oldTimer
	dos	SetInt,timeInt
	mov	ax,es
	mov	ds,ax

	lds	dx,oldUserTimer
	dos	SetInt,userTimeInt
	mov	ax,es
	mov	ds,ax

	lds	dx,oldKbd
	dos	SetInt,kbdInt
	mov	ax,es
	mov	ds,ax

	lds	dx,oldCtrlBrk
	dos	SetInt,ctrlBrkInt
	mov	ax,es
	mov	ds,ax

	lds	dx,oldCritErr
	dos	SetInt,critErrInt
	mov	ax,es
	mov	ds,ax

	cmp	monType,MONO
	.if	equal
		lds	dx,oldVideo
		dos	SetInt,videoInt
		mov	ax,es
		mov	ds,ax
		.end

	return

ResetInterrupts	endp




TimerIntHandler	proc	near
;This routines gets the interrupt from the timer chip, deals with
;any sounds that need playing, and passes the interrupt to the normal
;BIOS interrupt handler every third interrupt (a rate of 20/second).

	push	ax
	push	ds
	mov	ax,dgroup
	mov	ds,ax

	cld
	cmp	playSnd,0
	.if	not_equal
		call	PlayIt
		.end

	dec	byte ptr timerCounter
	.if	nonzero
		;Nothing to do -- issue the end-of-interrupt
		mov	al,EOI
		out	20h,al
	.else
		;Time to call the BIOS routine (which will issue EOI for us)
		mov	timerCounter,TIMER_COUNT
		pushf
		call	oldTimer
		.end
	pop	ds
	pop	ax
	iret


TimerIntHandler	endp




TimerDispatch	proc	near

	;save all registers
	cli
	pusha

	mov	ax,dgroup
	mov	ds,ax			;get addressability to our data
	mov	es,ax
	call	TimerInt		;go to our interrupt handler
	pushf				;simulate an interrupt
	call	oldUserTimer		;call the previous handler

	;Check for stack overflow.
	mov	si,stackBase
	cmp	word ptr [si],STACKFLAG
	.if	not_equal
		mov	word ptr [si],STACKFLAG
		;Monkey the stack so that the return is to DeadStack
		mov	bx,sp
		mov	word ptr [bx+18],offset cgroup:DeadStack
		mov	word ptr [bx+20],cs
		.end

	popa
	sti
	iret				;return

TimerDispatch	endp



DeadStack	proc	near

	mov	ax,dgroup
	mov	ds,ax			;get addressability to our data
	mov	es,ax

	mov	ax,stackBase
	add	ax,STACKSIZE
	mov	sp,ax
	call	ErrBeep
	call	ErrBeep
	pcall	WindowNoWait,&badStackMsg
	do
		call	GetChar
		cmp	ax,ESC
	until	equal
	call	Quit

DeadStack	endp




NullInterrupt	proc	near
;Nullify this interrupt by just returning.

	iret

NullInterrupt	endp



CriticalError	proc	near
;Handle a critcal DOS error by setting the dosError code, restoring
;our registers from the stack, setting the carry, and returning to
;the original DOS call.

	;Pop DOS's return address off the stack
	add	sp,6

	;Get back data addressability and put the error code into dosError
	mov	ax,dgroup
	mov	ds,ax
	and	di,0ffh			;mask off undefined portion
	.if	zero
		mov	di,100h		;use non-zero value for write-protect
		.end
	mov	dosError,di

	;Set the carry flag in the flags which will be returned to the caller
	mov	bp,sp
	mov	ax,[bp+22]	;get the original flags
	or	ax,1		;set the carry
	mov	[bp+22],ax	;and put the flags back

	;Restore all registers from the stack
	pop	ax
	pop	bx
	pop	cx
	pop	dx
	pop	si
	pop	di
	pop	bp
	pop	ds
	pop	es

	;And return to the orignal caller to DOS
	iret


CriticalError	endp




code	ends
	end

