;RANDOM
;Return a random number between 0 and 255
;This uses the linear congruential method with a multiplier of 31821.

include	macro.ah

public	Random


cgroup	group	code
dgroup	group	data


MULT		equ	07c4dh

timer		equ	1ah
gettime		equ	0



data	segment byte public 'data'

public	randSeed
randSeed	dw	0

data	ends




code	segment byte public 'code'
assume	cs:cgroup,ds:dgroup



Random proc	near

	enter

	cmp	randSeed,0
	.if	equal
		bios	timer,gettime
		mov	randSeed,dx
	.end
	;generate a new random number
	mov	ax,MULT
	mul	randSeed		;multiply the seed by the multiplier
	inc	ax				;add one
	mov	randSeed,ax			;save the new seed

	xor	al,ah
	xor	ah,ah

	exit

Random endp


code	ends
	end

