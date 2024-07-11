;SCANLOG
;

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file SCANLOG
;        Segment CODE.CODE, Addr = 28E8, Size = BA
;            CONDTEST                                              0:2918
;            SCANLOG                                               0:28E8
;            SKIPIF                                                0:2976
;            SKIPOR                                                0:2950
;        Segment DATA.DATA, Addr = AA08, Size = 0

include	macro.ah
include logic.ah

public  ScanLog
public  CondTest
public  SkipOr
public  SkipIf


cgroup	group	code
dgroup	group	data



data	segment public byte 'data'

tblEntry    struc
offset      dw  ?
numParms    db  ?
varBits     db  ?
tblEntry    ends

extrn	testTbl:near

data	ends



code	segment public byte 'code'
assume	cs:cgroup, ds:dgroup, es:dgroup


extrn   Action:near
extrn   DoTest:near


ScanLog     proc    near

    enter   logaddr

	mov	si, logaddr
	mov	si, [si+scan]

NextAction:				
	lodsb			        ;load AL with next action command byte

    do
		test	al, al      ;action cmd 'return' has value of 0
        breakif zero

		cmp	al, 0FFh
		jz	CondTest

        cmp	al, 0FEh
        .if zero
            lodsw			;for goto command, adjust position by value of next word
		    add	si, ax
		    jmp	NextAction
        .end

		call	Action
		test	si, si		;si is set to 0 if new.room encountered
    until zero

	mov	ax, si

	exit

ScanLog     endp



CondTest:		
	xor	bx, bx

NextTest:				
	lodsb

	cmp	al, 0FCh
    .if above_equal
        .if equal
	        test	bh, bh
	        jne	SkipIf
	        inc	bh
	        jmp	NextTest
        .end

	    cmp	al, 0FFh
        .if equal
            add	si, 2	    ;if end of if found, skip offset, and switch to action processing
	        jmp	NextAction
        .end

	    cmp	al, 0FDh        ;not
        .if equal
            xor	bl, 1       ;set not status in bl
	        jmp	NextTest
        .end
    .end

	push	bx		        ;save 'or' status (bh) and 'not' status (bl)
	call	DoTest	        ;returns value in al (0=false; 1=true)
	pop	bx		            ;restore status

	xor	al, bl		        ;apply 'not' (if it was set)
	mov	bl, 0		        ;reset 'not' status
	.if equal
	    test	bh, bh		;if result is false, check 'or' level
	    jz  SkipIf          ;if 'or' level is zero, false is result
	    jmp	NextTest        ;if 'or' level =1, try next cmd
    .end

	test	bh, bh
    .if not_equal           ;if 'or' level is 0, get next byte



SkipOr:                     ;otherwise skip to the end of the 'or'
	    xor	bh, bh
	    xor	ah, ah

        do
		    lodsb

		    cmp	al, 0FCh
            breakif equal           ;closing 'or' found, so break out
            continueif above

		    cmp	al, 0Eh
            .if equal
                lodsb               ;need to read 'said' cmd arg count,
		        shl	ax, 1           ;as it has a variable num of args
		        add	si, ax
                continue
            .end
			
		    mov	di, ax
		    shl	di, 1
		    shl	di, 1
		    mov	al, testTbl.numParms[di]
		    add	si, ax
        repeat

    .end

	jmp	NextTest



SkipIf:	
	xor	bh, bh
	xor	ah, ah

    do
	    lodsb

	    cmp	al, 0FFh
        breakif equal       ;'if' found; break out of skip if loop

	    cmp	al, 0FCh
        continueif not_below

	    cmp	al, 0Eh		    ;for 'said' command, need to read in number of bytes to skip
        .if equal
            lodsb			;number of args in this 'said' cmd
	        shl	ax, 1
	        add	si, ax
	        continue
        .end
			
	    mov	bl, al
	    shl	bx, 1
	    shl	bx, 1
	    mov	al, testTbl.numParms[bx] ; number of args for	this test cmd
	    add	si, ax
    repeat

	lodsw			        ;if block was false; retrieve offset amount
	add	si, ax		        ;add offset to pointer

	jmp	NextAction



code	ends
	end

