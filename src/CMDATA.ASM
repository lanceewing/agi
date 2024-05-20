;CMDATA
;Common data areas for graphics modules

dgroup	group data
assume ds:dgroup, es:dgroup


public	startc, start_coords, finishc, finish_coords, ghrz_pos, thrz_pos
public	hsp_adjmt, drawMask, color, cPOddMsk, cPEvnMsk, picptr, row_table
public	s_ram, v_ram, h_ram, fontRam, priority, picReloc


data	segment byte public 'data'

ghrz_pos	db	?
thrz_pos	db	?
hsp_adjmt	dw	0
drawMask	dw	0
color		db	0
priority	db	0
cPOddMsk	db	0FFh
cPEvnMsk	db	0FFh
s_ram		dw	?
v_ram		dw	0B800h		;video memory segment: CGA, Tandy 1000, & Hercules
h_ram		dw	?
fontRam		dw	?
picptr		dw	?
picReloc	dw	0
row_table	label	word		;video ram row offset table
		dw	0ffffh
		dw	199 dup(?)

coord	struc
 y		db	0	;defined as Y,X so that access of the
 x		db	0	;  coord pair as a word has X as the high byte
coord	ends

start_coords	label	word
startc		coord	<>
finish_coords	label	word
finishc		coord	<>

data	ends


end
