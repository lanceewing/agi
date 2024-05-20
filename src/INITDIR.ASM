;INITDIR
;Initial directory for KQ II.  This is read from a given sector to tell
;the interpreter where the various necessary directories are on the disk.

;typedef   struct {
;     byte volume;
;     byte head;
;     int  track;
;     int  sector;
;     int  offset;
;     }    SEGMENT;


data	segment public byte 'data'

aniobj		db	1,0
		dw	10,3,0
object		db	1,0
		dw	10,1,0
words		db	1,0
		dw	10,8,0
logdir		db	1,0
		dw	7,6,0
picdir		db	1,0
		dw	7,8,0
viewdir		db	1,0
		dw	8,1,0
snddir		db	1,0
		dw	8,3,0
ovl1dir		db	1,0
		dw	9,1,0		    ;pcgraf.ovl
ovl2dir		db	1,0
		dw	8,5,0		    ;jrgraf.ovl
ovl3dir		db	1,0
		dw	6,1,0		    ;kqdata.ovl

data	ends

	end
