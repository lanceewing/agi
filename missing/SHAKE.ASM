;SHAKE
;Assembly language routine for shaking the screen

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file SHAKE
;        Segment CODE.CODE, Addr = 7641, Size = 73
;            SHAKESCREEN                                           0:7641
;        Segment DATA.DATA, Addr = B3B5, Size = B


include	macro.ah
include game.ah
include	cmequ8s.ah

public  ShakeScreen


cgroup	group	code
dgroup	group	data



data	segment public byte 'data'

extrn	monType:word
extrn   machtype:word
extrn   sysTimer:dword
extrn   ghrz_pos:byte

vert_pos	db    0

shakeTbl	db    1     ;pos 1 - horz
		    db    1     ;pos 1 - vert
		    db 0FFh     ;pos 2 - horz
		    db    1     ;pos 2 - vert
		    db    1     ;pos 3 - horz
		    db 0FFh     ;pos 3 - vert
		    db 0FFh     ;pos 4 - horz
		    db 0FFh     ;pos 4 - vert
		    db    0     ;pos 5 - end; original horiz position
		    db    0     ;pos 5 - end; original vert position

data	ends



code	segment public byte 'code'
assume	cs:cgroup, ds:dgroup, es:dgroup


extrn   XTSS:near
extrn   HSS:near


ShakeScreen	proc near		

	enter   count

	mov	di, count
	xor	ch, ch
	mov	cl, [di]	; cl has shake value
	inc	di
	cmp	monType,EGA
    .if equal
        call	XTSS    ;EGA shake screen
    .else
        cmp	monType,MONO
        .if equal
            call	HSS     ;Hercules Shake Screen
        .else
            cmp	monType,VGA
            .if equal
                ;call	VGSS    ; TODO: Find actual name. VGSS is not a symbol in VGGRAPHX module!
                nop             ; TODO: Remove nops after finding VGSS procedure, if it existed.
                nop
                nop
            .else
                cmp	machtype, PC
                .if equal
                    mov	vert_pos, 70h       ;CGA vertical sync position
                .else
                    mov	vert_pos, 38h       ;JR vertical sync position
                .end

                mov	dx, 3D5h                ;6845 (CRT controller) data register

                do
                    lea	si, shakeTbl

                    do
                        mov	al, 2           ;2 = horizontal sync position
                        dec	dx              ;point DX at 6845 index register (port 3D4h)
                        out	dx, al		    ;
                        lodsb               ;load next byte from shake table
                        add	al, ghrz_pos    ;add graphics mode horizontal position
                        inc	dx              ;point DX at 6845 data register (port 3D5h)
                        out	dx, al		    ;

                        mov	al, 7           ;7 = vertical sync position
                        dec	dx              ;point DX at 6845 index register (port 3D4h)
                        out	dx, al          ;
                        lodsb               ;load next byte from shake table
                        add	al, vert_pos    ;
                        inc	dx              ;point DX at 6845 data register (port 3D5h)
                        out	dx, al		    ;

		                mov	bx, word ptr sysTimer
                        do
                            cmp	bx, word ptr sysTimer
                        while equal

                        cmp	al, vert_pos    ;0 indicates the end of the shake table
                    until zero
                dloop

            .end
        .end
    .end

    mov	ax, di

    exit
    
ShakeScreen	endp



code	ends
	end

