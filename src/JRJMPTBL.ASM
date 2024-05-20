;JRJMPTBL
;Jump table for PCjr graphics routines


cgroup	group	code

code	segment public byte 'code'
assume	cs:cgroup


extrn	jrig:near, jrts:near, jrgs:near, jrsh:near, jrb2s:near, jrirt:near
extrn	jrdw:near

initgrfx:
	jmp	jrig

textscrn:
	jmp	jrts

grafscrn:
	jmp	jrgs

sethorz:
	jmp	jrsh

blk2Scrn:
	jmp	jrb2s

initrows:
	jmp	jrirt

DrawWndw:
	jmp	jrdw

code	ends


end

