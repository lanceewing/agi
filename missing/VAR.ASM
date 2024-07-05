;VAR
;Actions on vars

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
;
;    Module A from file VAR
;        Segment CODE.CODE, Addr = 6F96, Size = 12F
;            ADDF                                                  0:6FE9
;            ADDN                                                  0:6FDC
;            ASSIGNF                                               0:6FC9
;            ASSIGNN                                               0:6FBC
;            DECREMENT                                             0:6FA9
;            DIVN                                                  0:708D
;            DIVV                                                  0:70A6
;            INCREMENT                                             0:6F96
;            LINDIRECTF                                            0:701C
;            LINDIRECTN                                            0:7035
;            MULN                                                  0:705F
;            MULV                                                  0:7072
;            RINDIRECT                                             0:7046
;            SUBF                                                  0:7009
;            SUBN                                                  0:6FFC
;        Segment DATA.DATA, Addr = B393, Size = 0


include	macro.ah

public  Increment
public  Decrement
public  AssignN
public  AssignF
public  AddN
public  AddF
public  SubN
public  SubF
public  LIndirectF
public  LIndirectN
public  RIndirect
public  MulN
public  MulV
public  DivN
public  DivV


cgroup	group	code
dgroup	group	data



data	segment public byte 'data'

extrn	var:byte

data	ends



code	segment public byte 'code'
assume	cs:cgroup, ds:dgroup, es:dgroup


Increment   proc    near

	xor	bh, bh
	lodsb
	mov	bl, al
	cmp	var[bx], 0FFh
    .if below
		inc	var[bx]
    .end		
	mov	ax, si
	return

Increment   endp



Decrement   proc    near

	xor	bh, bh
	lodsb
	mov	bl, al
	cmp	var[bx], 0
    .if nonzero
		dec	var[bx]
    .end			
	mov	ax, si
	return

Decrement   endp



AssignN     proc    near

	xor	bh, bh
	lodsb
	mov	bl, al
	lodsb
	mov	var[bx], al
	mov	ax, si
	return

AssignN     endp



AssignF     proc    near

	xor	ah, ah
	lodsb
	mov	di, ax
	lodsb
	mov	bx, ax
	mov	al, var[bx]
	mov	var[di], al
	mov	ax, si
	return

AssignF     endp



AddN        proc    near

	xor	bh, bh
	lodsb
	mov	bl, al
	lodsb
	add	var[bx], al
	mov	ax, si
	return

AddN        endp



AddF        proc    near

	xor	ah, ah
	lodsb
	mov	di, ax
	lodsb
	mov	bx, ax
	mov	al, var[bx]
	add	var[di], al
	mov	ax, si
	return

AddF        endp



SubN        proc    near

	xor	bh, bh
	lodsb
	mov	bl, al
	lodsb
	sub	var[bx], al
	mov	ax, si
	return

SubN        endp



SubF        proc    near

	xor	ah, ah
	lodsb
	mov	di, ax
	lodsb
	mov	bx, ax
	mov	al, var[bx]
	sub	var[di], al
	mov	ax, si
	return

SubF        endp



LIndirectF  proc    near

	xor	ah, ah
	lodsb
	mov	di, ax
	mov	al, var[di]
	mov	di, ax
	lodsb
	mov	bx, ax
	mov	al, var[bx]
	mov	var[di], al
	mov	ax, si
	return

LIndirectF  endp



LIndirectN  proc    near

	xor	bh, bh
	lodsb
	mov	bl, al
	mov	bl, var[bx]
	lodsb
	mov	var[bx], al
	mov	ax, si
	return

LIndirectN  endp


RIndirect   proc    near

	xor	ah, ah
	lodsb
	mov	di, ax
	lodsb
	mov	bx, ax
	mov	al, var[bx]
	mov	bx, ax
	mov	al, var[bx]
	mov	var[di], al
	mov	ax, si
	return

RIndirect   endp



MulN        proc    near

	xor	bh, bh
	lodsb
	mov	bl, al
	mov	cl, var[bx]
	lodsb
	mul	cl
	mov	var[bx], al
	mov	ax, si
	return

MulN        endp



MulV        proc    near

	xor	bh, bh
	lodsb
	mov	bl, al
	mov	cl, var[bx]
	push	bx
	lodsb
	mov	bl, al
	mov	al, var[bx]
	mul	cl
	pop	di
	mov	var[di], al
	mov	ax, si
	return

MulV        endp



DivN        proc    near

	xor	ah, ah
	xor	bh, bh
	lodsb
	mov	bl, al
	mov	ch, var[bx]
	lodsb
	mov	cl, al
	mov	al, ch
	div	cl
	mov	var[bx], al
	mov	ax, si
	return

DivN        endp



DivV        proc    near

	xor	ah, ah
	xor	bh, bh
	lodsb
	mov	bl, al
	mov	ch, var[bx]
	push	bx
	lodsb
	mov	bl, al
	mov	cl, var[bx]
	mov	al, ch
	div	cl
	pop	bx
	mov	var[bx], al
	mov	ax, si
	return

DivV        endp



code	ends
	end

