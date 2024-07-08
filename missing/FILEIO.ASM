;FILEIO
;

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file FILEIO
;        Segment CODE.CODE, Addr = 58EC, Size = 1EE
;            CLOSE                                                 0:5991
;            CREAT                                                 0:58EC
;            DUPLICATE                                             0:59D3
;            FILEDT                                                0:5AB2
;            FINDFIRST                                             0:5A40
;            FINDNEXT                                              0:5A65
;            GETCURDIR                                             0:59F1
;            GETCURDRIVE                                           0:5A29
;            LSEEK                                                 0:59AA
;            OPEN                                                  0:590D
;            READ                                                  0:592E
;            UNLINK                                                0:5974
;            VALIDDRIVE                                            0:5A7D
;            WRITE                                                 0:5951
;        Segment DATA.DATA, Addr = AFA1, Size = 0

include	macro.ah

public  creat
public  open
public  read
public  write
public  unlink
public  close
public  lseek
public  Duplicate
public  GetCurDir
public  GetCurDrive
public  FindFirst
public  FindNext
public  ValidDrive
public  FileDT


cgroup	group	code
dgroup	group	data


NO_HANDLE       equ	-1
ERROR_CODE      equ	-1

;DOS function calls
dosSetDrive     equ	0eh
dosCurDrive     equ	19h
dosSetDTA       equ	1ah
dosCreat        equ	3ch
dosOpen         equ	3dh
dosClose        equ	3eh
dosRead         equ	3fh
dosWrite        equ	40h
dosUnlink       equ	41h
dosLSeek        equ	42h
dosDup          equ	45h
dosCurDir       equ	47h
dosFirst        equ	4eh
dosNext         equ	4fh
dosFileDT       equ	57h



data	segment public byte 'data'

extrn   dosError:word

data	ends



code	segment public byte 'code'
assume	cs:cgroup, ds:dgroup, es:dgroup

extrn   strlen:near


creat   proc    near
;Create a file with name pointed to by 'fname' with attributes set in 'mode'.

    enter   fname,mode

    call	ClearDosError
	mov	dx, fname
	mov	cx, mode
	dos	dosCreat
    .if carry
        mov	ax, NO_HANDLE
    .end

    exit

creat   endp



open    proc    near
;Open the file with name 'fname'.
;The access is defined by 'mode':
;       0 = _read
;       1 = _write
;       2 = _read/_write

    enter   fname,mode

	call	ClearDosError
	mov	dx, fname
	mov	al, byte ptr mode
	dos	dosOpen
	.if	carry
		mov	ax, NO_HANDLE
  	.end

    exit

open    endp



read    proc    near
;Read 'n' bytes from file 'fd' into 'buf'.

    enter   fd,buf,n

	call	ClearDosError
	mov	bx, fd
	mov	cx, n
	mov	dx, buf
	dos	dosRead
	.if	carry
		xor	ax, ax
	.end

    exit

read    endp



write   proc    near
;Write 'n' bytes from 'buf' to the file 'fd'.

    enter   fd,buf,n

	call	ClearDosError
	mov	bx, fd
	mov	cx, n
	mov	dx, buf
	dos	dosWrite
	.if	carry
		xor	ax, ax
	.end

    exit

write   endp



unlink  proc    near
;Unlink the file 'fname'

    enter   fname

	call	ClearDosError
	mov	dx, fname
	dos	dosUnlink
	.if	carry
		xor	ax, ax
	.end

    exit

unlink  endp



close   proc    near
;Close the file 'fd'.

    enter   fd

	call	ClearDosError
	mov	bx, fd
	dos	dosClose

    exit

close   endp



lseek   proc    near
;Input:
;       fd	file handle
;       ofs	double-word offset [ofsh, ofsl]
;       theorg  offset is from:
;               0 = offset from beginning of file
;               1 = current location + offset
;               2 = EOF + offset
;Output:
;       returns double-word file pointer

    enter   fd,ofs,theOrg

	call	ClearDosError
	mov	bx, fd
	mov	al, byte ptr theOrg
	mov	dx, word ptr ofs
	mov	cx, word ptr ofs[2]
	dos	dosLSeek
	.if	carry
		mov	ax, -1
		mov	dx, ax
	.end

    exit

lseek   endp



Duplicate   proc    near

	enter   fd

	call	ClearDosError
	mov	bx, fd
    dos dosDup
    .if carry
        mov	ax, NO_HANDLE
    .end

    exit

Duplicate   endp



GetCurDir   proc    near

    enter   dirStr

    mov	si, dirStr
	mov	byte ptr [si], 5Ch ; '\'
	inc	si
	xor	dl, dl
    dos dosCurDir
	push	dirStr
	call	strlen
	add	sp, 2
	mov	cx, ax
	mov	al, 2Fh	; '/'
	mov	di, dirStr
	repne scasb		    ; look for forward slash
	or	cx, cx		    ; if no	forward	slash found, exit
    .if nonzero
		mov	di, dirStr	; if forward slash found, replace start	of string with a forward slash
		mov	byte ptr [di], 2Fh ; '/'
    .end

    exit

GetCurDir   endp



GetCurDrive proc    near

    enter

    dos dosCurDrive
	xor	ah, ah
	add	al, 61h

    exit

GetCurDrive endp



FindFirst   proc    near

    enter   fname,attr,DTA

	mov	dx, DTA
	dos	dosSetDTA
	mov	dx, fname
	mov	cx, attr
	dos	dosFirst
    .if carry
        mov	ax, NO_HANDLE
    .end

    exit

FindFirst   endp



FindNext    proc    near

    enter

    dos dosNext
    .if carry
        mov	ax, NO_HANDLE
    .end

    exit

FindNext    endp



ValidDrive  proc    near
;Check for the validity of a particular drive by switching to
;it and then back.

    enter   theDrive

	call	ClearDosError

    ;Get and save the current drive
	dos	dosCurDrive
	mov	cl, al		    ; cl = current default drive number

    ;Try changing drives, and see if we get to the requested one.
	mov	dl, byte ptr theDrive
	sub	dl, 'a'
	
    dos dosSetDrive
    dos dosCurDrive     ;see if we got there

	xor	bx, bx		    ;assume false to start with
	cmp	al, dl		    ;compare desired drive to actual drive
    .if zero
        inc	bx          ;got there -- it's a valid drive
    .end

	mov	dl, cl		    ;restore the saved current drive
    dos dosSetDrive
	mov	ax, bx          ;return 0 if new drive is NOT valid, or 1 if new drive is valid
                    
    exit

ValidDrive  endp



FileDT  proc    near

    enter   fd

    mov bx, fd
    dos dosFileDT,0
	mov	ax, cx		; ax, cx = file	time; dx = file	date

    exit

FileDT  endp



ClearDosError	proc	

	push	ds
	mov	ax, dgroup
	mov	ds, ax
	mov	dosError, 0
	pop	ds
	return

ClearDosError	endp



code	ends
	end

