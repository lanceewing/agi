;PCJOYRD
;Joystick read routine for IBM PC
;
;Change History:
;24/03/87  14:40:22	JAS
;	Added code to make certain that we don't get indications of a
;	joystick when one is not present.

include	macro.ah
include	game.ah

public	ReadJoy
public	ReadButtons


cgroup	group	code
dgroup	group	data



joyport	equ	201h
xbit	equ	01h
ybit	equ	02h
but1	equ	10h
but2	equ	20h

kbd	equ	16h
chkkey	equ	1


data	segment byte public 'data'

butstrc	struc
pressed	dw	0
down	dw	0
click	dw	0
dblclk	dw	0
timer	dw	2 dup(0)
butstrc	ends

BUTSIZE	equ	size butstrc

extrn	jx:word,jy:word
extrn	button:near

data	ends




code	segment public byte 'code'
assume	cs:cgroup,ds:dgroup



ReadJoy	proc	near

	enter
	
	cli
	
	bios	kbd,chkkey		;is there a keystroke waiting?
	.if	nonzero
		mov	ax,-1
		jmp	short JoyExit
		.end
	
	xor	bx,bx			;zero the counts (bh = x, bl = y)
	
	mov	ch,xbit			;test values for the bits in the port
	mov	cl,ybit
	
	mov	dx,joyport
	
	out	dx,al			;pulse the joystick
	do
		;The following instructions force the low bits of the
		;data bus to 1s, so that on machines with a floating
		;data bus (like the Tandy 3000), we can tell if a
		;joystick is present.
		flush
		aaa

		in	al,dx		;read the port
		test	al,ch		;see if the x bit has gone low
		.if	nonzero
			nop		;to equalize timing of both branches
			cli
			inc	bh	;nope -- increment the x count
			je	NotHere	;if rolled over, assume timeout
			.end
		test	al,cl		;see if the y bit has gone low
		.if	nonzero
			nop		;to equalize timing of both branches
			cli
			inc	bl	;nope -- increment the y count
			je	NotHere
			.end
		and	al,xbit or ybit	;have both x and y gone low?
	until	zero			;yep -- we're done
	
	bios	kbd,chkkey		;is there a keystroke waiting?
	.if	nonzero
		mov	ax,-1
		jmp	short JoyExit
		.end
	
	sti
	xor	ah,ah
	mov	al,bh
	mov	jx,ax		;report the x count
	xor	bh,bh
	mov	jy,bx		;report the y count
	xor	ax,ax
	jmp	short JoyExit
	
	
NotHere:
	xor	ax,ax
	mov	jx,ax
	mov	jy,ax
	
	
	
JoyExit:
	sti
	exit


ReadJoy	endp





ReadButtons	proc	near
;Read the joystick buttons.

	mov	dx,joyport
	in	al,dx		;read the port for the buttons
	
	xor	bx,bx
	test	al,but1
	.if	zero
		inc	bx
		.end
	mov	button.pressed,bx
	
	xor	bx,bx
	test	al,but2
	.if	zero
		inc	bx
		.end
	mov	button.pressed + BUTSIZE,bx
	
	return


ReadButtons	endp



code	ends
	end

