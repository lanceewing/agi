;BEEP
;Beep the speaker to signal various actions.

public	Beep
public	ErrBeep

include	macro.ah
include	game.ah


cgroup	group	  code


port_b	equ	61h
timer	equ	40h



code	segment public byte 'code'
assume	cs:cgroup


Beep	proc	near
;Friendly beep ('press ENTER to see more input, buddy')

	mov	bx,1000h	;frequency divisor
	mov	cx,8000h	;duration
	jmp	BeepIt

Beep	endp


ErrBeep	proc	near
;Error beep

	mov	bx,500h		;frequency divisor
	xor	cx,cx		;duration
	jmp	BeepIt

ErrBeep	endp



BeepIt:
	mov	al,0b6h
	out	timer+3,al
	mov	ax,bx
	out	timer+2,al
	mov	al,ah
	out	timer+2,al
	in	al,port_b
	mov	ah,al
	or	al,3
	out	port_b,al
	do
	dloop
	mov	al,ah
	out	port_b,al
	return



code	ends
	end

