;FLAG
;Actions on flags


include	macro.ah
include	game.ah

public	Set
public	Reset
public	Toggle
public	IsSet
public	SetFlag
public	ResetFlag
public	ToggleFlag
public	VSetFlag
public	VResetFlag
public	VToggleFlag
public	IsFlagSet
public	GetFlag
public	SetIt
public	ResetIt
public	ToggleIt
public	ClearFlags


cgroup	group	code
dgroup	group	data


flagParm	equ	ss:[bx+2]


data	segment public byte 'data'

extrn	flag:byte
extrn	var:byte

data	ends



code	segment public byte 'code'
assume	cs:cgroup, ds:dgroup, es:dgroup


SetFlag		proc	near

	lodsb
	call	SetIt
	mov	ax,si
	return

SetFlag		endp



ResetFlag	proc	near

	lodsb
	call	ResetIt
	mov	ax,si
	return

ResetFlag	endp



ToggleFlag	proc	near

	lodsb
	call	ToggleIt
	mov	ax,si
	return

ToggleFlag	endp



VSetFlag	proc	near

	lodsb
	xor	bh,bh
	mov	bl,al
	mov	al,var[bx]
	call	SetIt
	mov	ax,si
	return

VSetFlag	endp


VResetFlag	proc	near

	lodsb
	xor	bh,bh
	mov	bl,al
	mov	al,var[bx]
	call	ResetIt
	mov	ax,si
	return

VResetFlag	endp



VToggleFlag	proc	near

	lodsb
	xor	bh,bh
	mov	bl,al
	mov	al,var[bx]
	call	ToggleIt
	mov	ax,si
	return

VToggleFlag	endp




Set		proc	near

	mov	bx,sp
	mov	al,byte ptr flagParm
	call	SetIt
	return

Set		endp



Reset		proc	near

	mov	bx,sp
	mov	al,byte ptr flagParm
	call	ResetIt
	return

Reset		endp



Toggle		proc	near

	mov	bx,sp
	mov	al,byte ptr flagParm
	call	ToggleIt
	return

Toggle		endp



IsSet		proc	near

	mov	bx,sp
	mov	al,byte ptr flagParm
	call	IsFlagSet
	return

IsSet		endp



SetIt		proc	near

	call	GetFlag
	or	[bx],al
	return

SetIt		endp



ResetIt		proc	near

	call	GetFlag
	xor	al,0ffh
	and	[bx],al
	return

ResetIt		endp



ToggleIt	proc	near

	call	GetFlag
	xor	[bx],al
	return

ToggleIt	endp



IsFlagSet	proc	near

	call	GetFlag
	test	[bx],al
	.if	zero
		xor	ax,ax
	.else
		xor	ax,ax
		inc	ax
		.end
	return
	
IsFlagSet	endp		




GetFlag		proc	near
;Flag number is in al.  Return with bx pointing to the flag byte and ax
;with the appropriate bit set.

	xor	ah,ah

	mov	bx,ax
	shr	bx,1		;divide number by 8 to get byte offset
	shr	bx,1
	shr	bx,1
	add	bx,offset dgroup:flag

	mov	cx,ax
	and	cx,7		;get offset in the byte
	mov	al,80h
	shr	al,cl		;shift the bit to the proper position

	return

GetFlag		endp



ClearFlags	proc	near

	push	di
	xor	ax,ax
	mov	cx,NUMFLAGS/16
	lea	di,flag
	rep	stosw
	pop	di
	return

ClearFlags	endp



code	ends
	end

