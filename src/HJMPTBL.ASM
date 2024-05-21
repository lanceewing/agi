;HJMPTBL
;Jump table for Hercules Graphics Card graphics routines


cgroup	group	code

code	segment public byte 'code'
assume	cs:cgroup


extrn	HIG:near, HTS:near, HSH:near, HB2S:near, HIRT:near, HDW:near

initgrfx:
	jmp	HIG

textscrn:
	jmp	HTS

grafscrn:
	jmp	HIG

sethorz:
	jmp	HSH

blk2Scrn:
	jmp	HB2S

initrows:
	jmp	HIRT

DrawWndw:
	jmp	HDW

code	ends


end
