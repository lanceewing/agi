;VGJMPTBL
;Jump table for VGA graphics routines




cgroup	group	code

code	segment public byte 'code'
assume	cs:cgroup


extrn	vgig:near, vgts:near, vggs:near, vgsh:near, vgb2s:near, vgirt:near
extrn	vgdw:near

initgrfx:			;called from cmgraphx.asm
	jmp	vgig

textscrn:			;never called
	jmp	vgts

grafscrn:			;never called
	jmp	vggs

sethorz:			;never called
	jmp	vgsh

blk2Scrn:			;called from cmgraphx.asm
	jmp	vgb2s

initrows:			;called from cmgraphx.asm
	jmp	vgirt

DrawWndw:			;called from cmgraphx.asm
	jmp	vgdw

code	ends


end

