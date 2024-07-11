;MACRO.AH2
;

;A macro file like this was assumed to exist for the following reason: The 
;ASM files are using macros with the names return, exit, enter, pcall, do, 
;dloop, until, while, repeat, break, breakif, continue, .if, .else and .end,
;but the MACRO.AH file does not define those macros, even though MACRO.AH is 
;assumed to be complete. When we say complete, what we mean is that it was 
;complete at the time that it was stored on the physical floppy disk that 
;ended up being used for KQ3 version 2.14, which was at least 6 months 
;before the date associated with the AGI interpreter source code extracted
;from the SQ2 2.0D floppy disk. It is certainly possible, even probable,
;that a newer version of MACRO.AH was in use at the time that the original
;AGI interpreter source was stored on the physical floppy disk that ended
;up being used for SQ2 version 2.0D. For now we will treat it as a separate
;macro file, although that seems unlikely. We may merge this with the main
;MACRO.AH file later on.


.xlist

if1	       ;******************************	  macros only on 1st pass

;Constants for pcall argument prefixes.
AMPERSAND_STR	equ		<&>
HASH_STR		equ		<#>

;Constants for pcall argument types.
ADDRESS_ARG		equ		1
IMMEDIATE_ARG	equ		2

;Block type constants
$NoType 		equ		0
$LoopType		equ		11
$IfType 		equ		12
$ElseType		equ		13
$RetIfType		equ		14

;These are the possible test types for .if, until, while and breakif.
zero			equ		zero
nonzero			equ		nonzero
negative		equ		negative
plus			equ		plus
equal			equ		equal
not_equal		equ		not_equal
less			equ		less
greater			equ		greater
above			equ		above
above_equal		equ		above_equal
below			equ		below
below_equal		equ		below_equal
carry			equ		carry
no_carry		equ		no_carry

return	macro
	ret
	endm

exit	macro
	add	sp, 2
	pop	bp
	pop	di
	pop	si
	ret
	endm

enter	macro	p1,p2,p3,p4,p5
	ifnb	<p1>
		p1&	equ	[bp+8]
	endif
	ifnb	<p2>
		p2&	equ	[bp+0Ah]
	endif
	ifnb	<p3>
		p3&	equ	[bp+0Ch]
	endif
	ifnb	<p4>
		p4&	equ	[bp+0Eh]
	endif
	ifnb	<p5>
		p5&	equ	[bp+10h]
	endif
	push	si
	push	di
	push	bp
	mov	bp, sp
	sub	sp, 2
	endm

$concat		macro	symbol,text,char
    symbol equ <text&char>
	endm

$set_pcall_arg_type		macro	char,amp_s,hash_s
	pcallArgType=0
	ifidn <char>,<amp_s>
		pcallArgType=ADDRESS_ARG
	else
		ifidn <char>,<hash_s>
			pcallArgType=IMMEDIATE_ARG
		endif
	endif
	endm

$decode_pcall_arg	macro	arg
	foundArgType=0
	pcallArg equ < >
	.xcref foundArgType
	irpc aChar,arg
		if foundArgType
			$concat pcallArg,%pcallArg,aChar
		else
			$set_pcall_arg_type aChar,%AMPERSAND_STR,%HASH_STR
			if pcallArgType eq 0
				$concat pcallArg,%pcallArg,aChar
			endif
			foundArgType=1
		endif
    endm
	endm

$add_sp		macro	offset
	add	sp,	&offset
	endm

$mov_var2_arg	macro	arg
	mov		[bp-2],word ptr &arg
	endm

$lea_ax_arg		macro	arg
	lea		ax, &arg
	endm

$push_arg	macro	arg
	push	&arg
	endm

pcall	macro	fn,p1,p2,p3,p4,p5
	paramOffset = 0
	irp arg,<p5,p4,p3,p2,p1>
		ifnb <arg>
			$decode_pcall_arg arg
			if pcallArgType eq IMMEDIATE_ARG
				$mov_var2_arg	%pcallArg
				push	[bp-2]
			else
				if pcallArgType eq ADDRESS_ARG
					push	ax
					$lea_ax_arg	%pcallArg
					mov		[bp-2], ax
					pop		ax
					push	[bp-2]
				else
					$push_arg	%pcallArg
				endif
			endif
			paramOffset = paramOffset + 2
		endif
	endm
	call	fn
	$add_sp	%paramOffset
	endm

$poke	macro	n,m
	$st&n	= m
	.xcref	$st&n
	endm

$peek	macro	x,n
	x	= $st&n
	endm

$push	macro	n
	$st	= $st+1
	$poke	%$st,n
	endm

$pop	macro	x
	$peek   x,%$st
	$st     = $st-1
	endm

$newloop	macro
	$push	BlockType
	$push	BlockNum
	BlockType = $LoopType
	BlockCount = BlockCount+1
	BlockNum = BlockCount
	DoBlkNum = BlockNum
	endm

$newelse	macro
	BlockType = $ElseType
	endm

$newif		macro
	$push	BlockType
	$push	BlockNum
	BlockType = $IfType
	BlockCount = BlockCount+1
	BlockNum = BlockCount
	endm

$newretif	macro
	$push	BlockType
	$push	BlockNum
	BlockType = $RetIfType
	BlockCount = BlockCount+1
	BlockNum = BlockCount
	endm

$popblock	macro
	$pop	BlockNum
	$pop	BlockType
	if	BlockType eq $LoopType
		DoBlkNum = BlockNum
	endif
	endm

$label	macro	prefix,num
&prefix&num:
	endm

$loop	macro	prefix,num
	loop	&prefix&num
	endm

do	macro
	$newloop
	$label	DoStart,%BlockNum
	endm

dloop	macro
	$loop	DoStart,%BlockNum
	$label	DoEnd,%BlockNum
	$popblock
	endm

$jmp_zero	macro	prefix,num
	jz	&prefix&num
	endm

$jmp_not_zero	macro	prefix,num
	jnz	&prefix&num
	endm

$jmp_sign	macro	prefix,num
	js	&prefix&num
	endm

$jmp_not_sign	macro	prefix,num
	jns	&prefix&num
	endm

$jmp_equal	macro	prefix,num
	je	&prefix&num
	endm

$jmp_not_equal	macro	prefix,num
	jne	&prefix&num
	endm

$jmp_less	macro	prefix,num
	jl	&prefix&num
	endm

$jmp_greater	macro	prefix,num
	jg	&prefix&num
	endm

$jmp_less_equal	macro	prefix,num
	jle	&prefix&num
	endm

$jmp_greater_equal	macro	prefix,num
	jge	&prefix&num
	endm

$jmp_above	macro	prefix,num
	ja	&prefix&num
	endm

$jmp_not_above	macro	prefix,num
	jna	&prefix&num
	endm

$jmp_above_equal	macro	prefix,num
	jae &prefix&num
	endm

$jmp_below	macro	prefix,num
	jb	&prefix&num
	endm

$jmp_not_below	macro	prefix,num
	jnb	&prefix&num
	endm

$jmp_not_below_equal	macro	prefix,num
	jnbe	&prefix&num
	endm

$jmp_below_equal	macro	prefix,num
	jbe	&prefix&num
	endm

$jmp_carry	macro	prefix,num
	jc	&prefix&num
	endm

$jmp_no_carry	macro	prefix,num
	jnc	&prefix&num
	endm

$jmp_uncond		macro	prefix,num
	jmp	&prefix&num
	endm

$jmp_if_not		macro	test_type,prefix,num
	ifidn <test_type>,<equal>
		$jmp_not_equal prefix,num
	endif
	ifidn <test_type>,<not_equal>
		$jmp_equal prefix,num
	endif
	ifidn <test_type>,<zero>
		$jmp_not_zero prefix,num
	endif
	ifidn <test_type>,<nonzero>
		$jmp_zero prefix,num
	endif
	ifidn <test_type>,<below>
		$jmp_not_below prefix,num
	endif
	ifidn <test_type>,<above>
		$jmp_not_above prefix,num
	endif
	ifidn <test_type>,<below_equal>
		$jmp_above prefix,num
	endif
	ifidn <test_type>,<above_equal>
		$jmp_below prefix,num
	endif
	ifidn <test_type>,<negative>
		$jmp_not_sign prefix,num
	endif
	ifidn <test_type>,<plus>
		$jmp_sign prefix,num
	endif
	ifidn <test_type>,<carry>
		$jmp_no_carry prefix,num
	endif
	ifidn <test_type>,<no_carry>
		$jmp_carry prefix,num
	endif
	ifidn <test_type>,<greater>
		$jmp_less_equal prefix,num
	endif
	ifidn <test_type>,<less>
		$jmp_greater_equal prefix,num
	endif
	endm

$jmp_if		macro	test_type,prefix,num
	ifidn <test_type>,<equal>
		$jmp_equal prefix,num
	endif
	ifidn <test_type>,<not_equal>
		$jmp_not_equal prefix,num
	endif
	ifidn <test_type>,<zero>
		$jmp_zero prefix,num
	endif
	ifidn <test_type>,<nonzero>
		$jmp_not_zero prefix,num
	endif
	ifidn <test_type>,<below>
		$jmp_below prefix,num
	endif
	ifidn <test_type>,<not_below>
		$jmp_not_below prefix,num
	endif
	ifidn <test_type>,<above>
		$jmp_above prefix,num
	endif
	ifidn <test_type>,<above_equal>
		$jmp_above_equal prefix,num
	endif
	ifidn <test_type>,<below_equal>
		$jmp_below_equal prefix,num
	endif
	ifidn <test_type>,<negative>
		$jmp_sign prefix,num
	endif
	ifidn <test_type>,<plus>
		$jmp_not_sign prefix,num
	endif
	ifidn <test_type>,<carry>
		$jmp_carry prefix,num
	endif
	ifidn <test_type>,<no_carry>
		$jmp_no_carry prefix,num
	endif
	ifidn <test_type>,<greater>
		$jmp_greater prefix,num
	endif
	ifidn <test_type>,<less>
		$jmp_less prefix,num
	endif
	endm

until	macro	test_type
	$jmp_if_not	test_type,DoStart,%BlockNum
	$label	DoEnd,%BlockNum
	$popblock
	endm

while	macro	test_type
	$jmp_if	test_type,DoStart,%BlockNum
	$label	DoEnd,%BlockNum
	$popblock
	endm

repeat	macro
	$jmp_uncond DoStart,%BlockNum
	$label	DoEnd,%BlockNum
	$popblock
	endm

break	macro
	$jmp_uncond DoEnd,%DoBlkNum
	endm

breakif		macro	test_type
	$jmp_if	test_type,DoEnd,%DoBlkNum
	endm

continue	macro
	$jmp_uncond DoStart,%DoBlkNum
	endm

continueif	macro	test_type
	$jmp_if	test_type,DoStart,%DoBlkNum
	endm

retif		macro	test_type
	$newretif
	$jmp_if_not	test_type,RetIfEnd,%BlockNum
	ret
	$label RetIfEnd,%BlockNum
	$popblock
	endm

.if		macro	test_type
	$newif
	$jmp_if_not	test_type,IfEnd,%BlockNum
	endm

.else	macro
	;; An .else indicates the end of an .if, so we add a jmp over the else.
	$jmp_uncond ElseEnd,%BlockNum
	;; Then add a label for the start of the .else for .if to skip over to when the .if condition is false.
	$label	IfEnd,%BlockNum
	$newelse
	endm

.end	macro
	if	BlockType eq $IfType
		$label	IfEnd,%BlockNum
	else
		if	BlockType eq $ElseType
			$label	ElseEnd,%BlockNum
		endif
	endif
	$popblock
	endm


endif	       ;******************************	  macros only on 1st pass

$st = 0
BlockCount = 0
BlockNum = 0
BlockType = 0
DoBlkNum = 0

.list
