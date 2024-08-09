;PRINTF.ASM
;

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file PRINTF
;        Segment CODE.CODE, Addr = 234A, Size = FE
;            PRINTF                                                0:2366
;            SPRINTF                                               0:234A
;        Segment DATA.DATA, Addr = A8C7, Size = 5


include	macro.ah


public  sprintf
public  printf


cgroup	group	code
dgroup	group	data



data	segment public byte 'data'

format_IP           dw 0
format_strbuff      dw 0
screen_or_string    db 0

data	ends



code	segment public byte 'code'
assume	cs:cgroup, ds:dgroup, es:dgroup


extrn   IToA:near
extrn   IToH:near
extrn   PutChar:near


sprintf     proc    near

	inc	screen_or_string
	pop	format_IP
	pop	format_strbuff
	call	printf
	push	format_strbuff
	push	format_IP
	dec	screen_or_string
	ret

sprintf     endp



printf  proc    near		

    enter   msg

	mov	si,msg
	mov	di,format_strbuff
	mov	bx,bp
	add	bx,0Ah

    do
        lodsb

		test    al,al
        breakif zero

        cmp	al,'%'
        .if not_equal
            call	AddChar
        .else
            lodsb

            cmp	al,'s'
            .if equal
                call	FormatString
            .else
                cmp	al,'d'
                .if equal
                    call	FormatDecimal
                .else
		            cmp	al,'u'
                    .if equal
                        call	FormatUnsigned
                    .else
		                cmp	al,'x'
                        .if equal
                            call	FormatHex
                        .else
		                    cmp	al,'c'
                            .if equal
                                mov	al,[bx]
		                        add	bx,2
		                        call	AddChar
                            .else
                                mov	al,'%'
		                        call	AddChar
		                        dec	si
                            .end
                        .end
                    .end
                .end
            .end
        .end
    repeat

    test	screen_or_string,0FFh
    .if not_equal
        xor	al,al
	    stosb           ; if not printing directly to screen, add a null char to end of string
    .end

	exit

printf	endp



FormatDecimal   proc    near

	mov	ax,[bx]
	test	ax,ax
    .if negative
	    push	ax
	    mov	al,2Dh	        ; if number is negative, add minus sign
	    call	AddChar
	    pop	ax
	    neg	ax
    .end
	push	bx
    pcall   IToA,ax
	pop	bx
	call	CopyString
	add	bx,2
	ret

FormatDecimal   endp



FormatUnsigned	proc near

	push	bx
    push	word ptr [bx]
    call    IToA
    add	sp, 2
	pop	bx
	call	CopyString
	add	bx,2
	ret

FormatUnsigned	endp



FormatHex   proc    near

	push	bx
    push	word ptr [bx]
    call    IToH
    add	sp, 2
	pop	bx
	call	CopyString
	add	bx,2
	ret

FormatHex   endp



FormatString    proc    near

	mov	ax,[bx]
	call	CopyString
	add	bx,2
	ret

FormatString    endp



CopyString  proc    near

	push	si
	mov	si,ax
    do
	    lodsb
		test	al,al
        breakif zero
		push	si
		call	AddChar
		pop	si
    repeat
	pop	si
	ret

CopyString  endp



AddChar     proc    near

	test	screen_or_string,0FFh
    .if not_equal
	    stosb			            ; add char to string output
    .else
	    push	bx
        pcall   PutChar,ax          ; add char to screen
	    pop	bx
    .end
	ret

AddChar     endp



code	ends
	end

