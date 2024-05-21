;IOJMPTBL
;Jump table for IBM object subroutines

public	saveback, restback, InsertObj


cgroup	group	code

code	segment public byte 'code'
assume	cs:cgroup


extrn	ISSB:near, IRSB:near, IIO:near

saveback:
	jmp	ISSB

restback:
	jmp	IRSB

InsertObj:
	jmp	IIO


code	ends

end
