;CMGRAPHX
;Common picture drawing subroutines


public	clrShdwRam, drwHLine, drwVLine, plotPoint, colorFill, setDisp
public	BackToScrn, getVidOfst, getShdwOfst, xlatColor, OpenWndw, CloseWndw


cgroup	group code
dgroup	group data
assume	cs:cgroup, ds:dgroup, es:dgroup
	
extrn	start_coords:word, startc:byte, finish_coords:word, finishc:byte
extrn	color:byte, priority:byte, cPOddMsk:byte, cPEvnMsk:byte, s_ram:word
extrn	row_table:word, machtype:word, monType:word, showPri:word
extrn	drawMask:word, picReloc:word
extrn	blk2Scrn:near, initrows:near, initpri:near, initgrfx:near, pcxc:near
extrn	DrawWndw:near, HBS:near

include macro.ah
include	cmequ8s.ah

data	segment byte public 'data'

coord	struc
 y		db	0
 x		db	0
coord	ends

endCoords		label	word
 rightEnd	db	?
 leftEnd	db      ?
dirFlag_and_YAdj 	label	word
 YAdj		db	?
 dirFlag	db	?
dupFlag_and_Filler 	label 	word
 dupFlag	db	?
		db	?
prevStartCoords 	label	word
 YPrev		db	?
 XPrev		db	?
prevEndCoords	label	word
 rEPrev       	db	?
 lEPrev		db	?
dFPrev_and_Filler	label	word
		dw	?
shdwEndCoords 	label	word
 shdwRE		db	?
 shdwLE		db	?

data	ends


code	segment byte public 'code'

clrShdwRam:			;clear shadow ram
	push	es
	mov	es,s_ram	;establish shadow ram addressing
	xor	di,di		;set DI to start of shadow ram
	mov	cx,PAGELEN	;# of words in shadow ram
	rep	stosw		;clear shadow ram
	pop	es		;restore data addressing
	cmp	monType,MONO	;is game running on a monochrome monitor?
	.if	equal
		call	HBS	;clear Hercules shadow ram
		.end
	ret


drwHLine:			;draw a horizontal line
	mov	dx,start_coords	;load horizontal line's start & end points
	mov	bx,finish_coords
	push	bx		;save end point (new start point)
	cmp	dh,bh		;is start X to the left of finish X?
	jbe	dHL1		;yes
	xchg	dx,bx		;reverse start and finish points
	mov	startc.x,dh
	mov	finishc.x,bh

dHL1:
	xor	ch,ch		;compute line length
	mov	cl,bh
	sub	cl,dh
	inc	cx
	call	plotPoint	;plot starting pixel
	dec	cx		;are there more pixels to plot?
	jz	dHL_xit		;no
	push	es		;save data addressing
	mov	es,s_ram	;establish shadow ram addressing

dHL2:
	inc	di		;move to next pixel
	mov	al,es:[di]	;get shadow pixel from shadow ram
	or	al,bh		;apply draw mask to shadow pixel
	and	al,bl		;apply color/priority mask to shadow pixel
	mov	es:[di],al	;put updated shadow pixel into shadow ram
	loop	dHL2		;loop until line is complete
	pop	es		;restore data addressing

dHL_xit:
	pop	start_coords	;set new start point (old end point)
	ret


drwVLine:			;draw a vertical line in shadow ram
	mov	dx,start_coords	;load vertical line's start point
	mov	bx,finish_coords ;load vertical line's end point
	push	bx		;save end point (new start point)
	cmp	dl,bl		;is start Y above finish Y?
	jb	dVL1		;yes
	xchg	dx,bx		;reverse start and finish points
	mov	startc.y,dl
	mov	finishc.y,bl

dVL1:
	xor	ch,ch		;compute line length
	mov	cl,bl
	sub	cl,dl
	inc 	cx
	call	plotPoint	;plot starting pixel
	dec	cx		;are there more pixels to plot?
	jz	dVL_xit		;no
	push	es		;save data addressing
	mov	es,s_ram	;establish shadow ram addressing

	mov	bl,cPOddMsk	;sync RGB odd/even pattern with next line
	mov	dh,cPEvnMsk	;assume next line's number is odd
	test	dl,1		;is current line's number even?
	jz	dVL3		;yes


dVL2:
	xchg	bl,dh		;alternate RGB odd/even patterns

dVL3:
	add	di,shdw_lnth	;move down 1 line
	mov	al,es:[di]	;get shadow pixel from shadow ram
	or	al,bh		;apply draw mask to shadow pixel
	and	al,bl		;apply color/priority mask to shadow pixel
	mov	es:[di],al	;put updated shadow pixel into shadow ram
	loop	dVL2		;loop until line is complete
	pop	es		;restore data addressing

dVL_xit:
	pop	start_coords	;set new start point (old end point)
	ret


plotPoint:			;plot point in shadow ram
	push	es		;save data addressing
	mov	es,s_ram	;establish shadow ram addressing
	mov	ax,start_coords	;get starting X/Y coords

	xor	bh,bh		;compute shadow ram offset to point X,Y
	mov	bl,al		;Y coordinate
	mov	di,bx
	shl	di,1
	shl	di,1
	shl	di,1		;DI = Y coordinate * 8
	shl	bx,1		;BX = Y coordinate * 2
	add	di,bx		;DI = Y coordinate * 10
	shl	di,1
	shl	di,1
	shl	di,1
	shl	di,1		;DI = Y coordinate * 160
	xor	bh,bh
	mov	bl,ah		;X coordinate
	add	di,bx		;DI = (Y coordinate * 160) + X coordinate

	mov	bl,cPEvnMsk	;get the even line color pattern
	test	al,1		;is the Y coordinate odd?
	jz	pP1		;no
	mov	bl,cPOddMsk	;get the odd line color pattern

pP1:
	mov	bh,byte ptr drawMask 	;BH = draw mask
	mov	al,es:[di]	;get shadow pixel from shadow ram
	or	al,bh		;apply draw mask to shadow pixel
	and	al,bl		;apply color/priority mask to shadow pixel
	mov	es:[di],al	;put updated shadow pixel into shadow ram
	pop	es		;restore screen addressing
	ret


colorFill:				;fill area with color/priority
	push	si

	call	getShdwOfst	;point DI at color fill seed point

	push	es		;save data addressing
	mov	es,s_ram	;establish shadow ram addressing

	mov	bh,byte ptr drawMask	;BH = color/priority pixel drawing mask
	mov	bl,low WHITEBKGD ;BL = color/priority background pattern (4Fh)

	test	bh,0Fh 		;is this a color fill?
	jz	priChk	  	;no
	mov	dl,0Fh		;set filled-color-pixel mask value
	cmp	color,0Fh	;is this a fill with background color?
	je	dropIt		;yes
	jmp short cFInit

priChk:
	test	bh,0F0h 	;is this a priority fill?
	jz	dropIt  	;no - neither color nor priority is enabled
	mov	dl,0F0h		;set filled-priority-pixel mask value
	cmp	priority,40H	;is this a fill with background priority?
	jne	cFInit		;no

dropIt:
	jmp	cFXit		;ignore this color fill

cFInit:
	and	bl,dl		;apply filled-pixel mask to background pattern
	mov	al,es:[di]	;get seed pixel

	and	al,dl		;isolate appropriate nibble (color or priority)
	cmp	al,bl		;is seed pixel already filled?
	jne	dropIt		;yes

	push	bp		;save BP
	mov	ax,0FFFFh
	push	ax
	push	ax
	push	ax
	push	ax		;push end of parameters flag onto stack
	mov	leftEnd,161 	;initialize the variable which holds the
				;	X coord. of the left end of the
				;	previous line
	mov	rightEnd,0	;initialize the variable which holds the
				;	X coord. of the right end of the
				;	previous line
	mov	YAdj,1		;set Y coord. adjustment factor for 1 line down
	mov	dupFlag,0	;clear duplicate line flag
 
fillLine:
	mov	ax,endCoords	;save current line's parameters
	mov	prevEndCoords,ax
	mov	ax,dupFlag_and_Filler
	mov	dFPrev_and_Filler,ax
	mov	ax,start_coords	;AH = starting X coord, AL = starting Y coord
	mov	XPrev,ah

	mov	cl,ah		;CL = starting X coordinate

	mov	ah,cPOddMsk
	test	al,1		;is Y coordinate odd?
	jnz	initScan	;yes
	mov	ah,cPEvnMsk

initScan:
	mov	bp,di		;save DI starting value
	xor	ch,ch
	inc	cx		;max scan length = starting X coord. + 1
	mov	al,es:[di]	;get seed pixel	from shadow ram
	std			;set fill direction for right to left

fillLeft:
	or	al,bh		;apply draw mask to shadow pixel
	and	al,ah		;apply color/priority mask to shadow pixel
	stosb			;return filled pixel to shadow ram
	mov	al,es:[di]	;get pixel from shadow ram
	mov	dh,al		;duplicate pixel
	and	dh,dl		;isolate appropriate nibble (color or priority)
	cmp	dh,bl		;is pixel already filled?
	loope	fillLeft	;no - continue filling to the left

	inc	di		;point DI at left end byte
	mov	cx,di		;compute left end X coordinate
	sub	cx,bp
	mov	al,startc.x
	add	cl,al
	mov	leftEnd,cl	;set left end X coordinate
	mov	startc.x,cl	;save next line's seed pixel X coordinate
	xchg	bp,di		;save left end DI, and restore seed pixel DI
	inc	di             	;begin filling 1 shadow ram pixel to the right
	mov	cx,shdw_lnth-1	;compute maximum scan length
	sub	cl,al
	cld			;set fill direction for left to right
	jcxz	filled		;this is the right edge

fillRight:
	mov	al,es:[di]	;get pixel from shadow ram
	mov	dh,al		;duplicate pixel
	and	dh,dl		;isolate appropriate nibble (color or priority)
	cmp	dh,bl		;is pixel already filled?
	jne	filled		;yes - line is complete
	or	al,bh		;apply draw mask to shadow pixel
	and	al,ah		;apply color/priority mask to shadow pixel
	stosb			;return filled pixel to shadow ram
	loop	fillRight	;continue filling to the right

filled:
	mov	ax,di		;compute right end X coordinate
	sub	ax,bp
	dec	al
	add	al,leftEnd
	mov	rightEnd,al
				;line is now filled - find next line
	cmp	lEPrev,161	;is this the seed line?
	je	setDir		;yes - no need to save params
	cmp	al,rEPrev	;is this line's right end X coord < the
				;	previous line's right end X coord?
	je	dupLineCk	;no
	ja	notADup		;no
	mov	XPrev,al	;set previous line's starting pixel X coord to
				;	this line's right end X coord
	mov	dupFlag,0	;clear duplicate lines flag
	jmp short saveParams

dupLineCk:
	mov	al,leftEnd
	cmp	al,lEPrev	;is this line's left end X coord == the
				;	previous line's left end X coord?
	jne	notADup		;no
	cmp	dupFlag,1	;was the previous line also a duplicate?
	je	setDir		;yes - no need to save params.
	mov	dupFlag,1	;mark this line as a duplicate of the previous one
	mov	al,rightEnd	;set previous line's starting pixel X coord to
				;	this line's right end X coord
	mov	XPrev,al
	jmp short saveParams

notADup:
	mov	dupFlag,0	;clear duplicate lines flag
	mov	al,rEPrev	;set previous line's starting pixel X coord to
				;	that line's right end X coordinate
	mov	XPrev,al

saveParams:
	push	dFPrev_and_Filler ;save parameters from the previous line
	push	dirFlag_and_YAdj
	push	prevStartCoords
	push	prevEndCoords

setDir:
	mov	al,YAdj	;match direction flag to the Y coord adjustment direction
	mov	dirFlag,al
	mov	al,startc.y	;save current Y coordinate
	mov	YPrev,al

findNxtLine:
	add	al,YAdj	;Add Y coord adjustment to Y coord to move to next line
	mov	startc.y,al
	cmp	al,y_max	;is Y coordinate within limits (0 - 167)?
	ja	phaseIICk	;no

pixelFilledCk:
	mov	ax,start_coords
	xor	ch,ch
	mov	cl,al		;Y coordinate
	mov	di,cx
	shl	di,1
	shl	di,1
	shl	di,1		;DI = Y coordinate * 8
	shl	cx,1		;CX = Y coordinate * 2
	add	di,cx		;DI = Y coordinate * 10
	shl	di,1
	shl	di,1
	shl	di,1
	shl	di,1		;DI = Y coordinate * 160
	xor	ch,ch
	mov	cl,ah		;X coordinate
	add	di,cx		;DI = (Y coordinate * 160) + X coordinate
	mov	al,es:[di]	;get pixel from shadow ram
	mov	dh,al		;duplicate pixel
	and	dh,dl		;isolate appropriate nibble (color or priority)
	cmp	dh,bl		;is pixel already filled?
	jne	alreadyFilled	;yes
	jmp	fillLine	;no

alreadyFilled:
	mov	al,startc.x
	mov	ah,YAdj
	cmp	ah,dirFlag	;is this phase 1?
	je	phaseEndCk	;yes
	cmp	dupFlag,1	;is this line a duplicate?
	je	phaseEndCk	;yes
	cmp	al,shdwLE	;is this line's current X coord to the left of
				;	the shadow of the line which seeded this line?
	jb	phaseEndCk	;yes
	mov	ah,shdwRE
	cmp	al,ah		;is it to the right?
	ja	phaseEndCk	;yes
	mov	al,ah
	cmp	al,rightEnd	;is this line totally within the shadow?
	jae	phaseIICk	;yes
	inc	al		;move this line's current X coord to the right
				;	 of the shadow
	mov	startc.x,al

phaseEndCk:
	cmp	al,rightEnd	;is this the right end?
	jae	phaseIICk	;yes
	inc	al		;move 1 pixel to the right
	mov	startc.x,al
	jmp short pixelFilledCk

phaseIICk:
	mov	al,YAdj
	cmp	al,dirFlag	;have both top & bottom been searched?
	jne	endFillCk	;yes
	cmp	dupFlag,0	;is this line a duplicate?
	jne	endFillCk	;yes - no need to test in other direction then
	neg	al		;reverse Y coord adjustment
	mov	YAdj,al
	mov	al,leftEnd
	mov	startc.x,al	;return current X coord of this line's left end
	mov	al,YPrev
	mov	startc.y,al	;return current Y coord of this line
	jmp short getShdwCoords

endFillCk:
	pop	endCoords	;get next set of paramaters
	pop	start_coords
	pop	dirFlag_and_YAdj
	pop	dupFlag_and_Filler
	mov	al,startc.y
	cmp	al,0ffh		;is this the marker for the end of the parameters?
	je	endFill		;yes
	mov	YPrev,al	;save this line's Y coord

getShdwCoords:
	mov	bp,sp		;get end points of "shadow" line
	mov	cx,[bp]
	mov	shdwEndCoords,cx
	jmp	findNxtLine

endFill:
	pop	bp

cFXit:
	pop	es		;restore data addressing
	pop	si
	ret


setDisp:			;set display to graphics mode
	enter

	mov	ax,BLACKBKGD
	call	clrShdwRam	;clear shadow ram to black
	call	initrows	;build video row offset table
	call	initpri		;build priority table
	call	initgrfx	;set screen to graphics mode

	exit


BackToScrn:			;create the video ram from shadow ram
	enter

	test	showPri,1
	.if	nonzero
		push	es
		push	ds
		mov	ax,s_ram
		mov	ds,ax
		mov	es,ax
		xor	si,si
		mov	di,si
		mov	cx,168*160
		do
			lodsb
			ror	al,1
			ror	al,1
			ror	al,1
			ror	al,1
			stosb
		dloop

		pop	ds
		pop	es
		cmp	monType,MONO
		.if	equal
			call	HBS ;build HGC shadow ram from PC shadow ram
			.end
		.end
	mov	al,y_max
	mov	ah,x_min
	mov	bl,shdw_lnth
	mov	bh,PICROWS
	call	blk2Scrn

	exit


OpenWndw:			;open a window on the video screen
	enter	startXY, dimensions, wndwColors
	mov	ax,startXY
	mov	bx,dimensions
	mov	dx,wndwColors
	call	DrawWndw	;draw full window
	mov	ax,startXY
	mov	bx,dimensions
	mov	dx,wndwColors
	mov	bh,1		;segment height = 1 (horizontal line)
	sub	bl,2		;segment length = window length - 2
	inc	ah		;X + 1
	dec	al		;Y - 1
	mov	dl,dh		;border color
	call	DrawWndw	;draw window border segment
	mov	ax,startXY
	mov	bx,dimensions
	mov	dx,wndwColors
	add	ah,bl
	sub	ah,2 		;X + window length - 2
	sub	al,2		;Y - 2
	sub	bh,4		;segment height = window height - 4
	mov	bl,1		;segment length = 1 (vertical line)
	mov	dl,dh		;border color
	call	DrawWndw	;draw window border segment
	mov	ax,startXY
	mov	bx,dimensions
	mov	dx,wndwColors
	inc	ah		;X + 1
	sub	al,bh
	add	al,2		;Y - window height + 2
	mov	bh,1		;segment height = 1 (horizontal line)
	sub	bl,2		;segment length = window length - 2
	mov	dl,dh		;border color
	call	DrawWndw	;draw window border segment
	mov	ax,startXY
	mov	bx,dimensions
	mov	dx,wndwColors
	inc	ah		;X + 1
	sub	al,2		;Y - 2
	sub	bh,4		;segment height = window height - 4
	mov	bl,1		;segment length = 1 (vertical line)
	mov	dl,dh		;border color
	call	DrawWndw	;draw window border segment
	exit


CloseWndw:			;close a window on the video screen
	enter	startXY, dimensions
	mov	ax,startXY
	mov	bx,dimensions
	call	blk2Scrn
	exit


getVidOfst:
	lea	di,row_table	;set DI to base of video ram row table
	mov	bx,picReloc 	;relocate block's Y coordinate by picture relocation factor
	add	bl,al		;Y coord
	shl	bx,1		;Y coord * 2 = row table offset
	mov	di,[di+bx]	;set base offset = row Y offset value
	xor	bh,bh
	mov	bl,ah		;X coord
	cmp	monType,VGA
	.if	equal
		shl	bx,1	;eight bits per pixel
		.end
	cmp	machtype,PC	;is the game running on a PC?
	.if	equal		;yes -- convert X coord to CGA/HGC pixel offset
		shr	bx,1
		cmp	monType,EGA 	;is an EGA board active?
		.if	equal		;yes -- convert X coord to EGA pixel offset
			shr	bx,1
			.end
		.end
	add	di,bx		;add X coord pixel offset to row Y offset
	ret


getShdwOfst:

;|-----------------------------------------------------------------------------|
;|      This subroutine calculates the offset into the shadow ram represented  |
;|  by the X/Y coordinate pair passed in the AX register.                      |
;|                                                                             |
;|      On entry:       AH = shadow ram X coordinate                           |
;|                      AL = shadow ram Y coordinate                           |
;|      On exit:        DI = offset into shadow ram                            |
;|                      BX = shadow ram X coordinate                           |
;|-----------------------------------------------------------------------------|

	xor	bh,bh
	mov	bl,al		;Y coordinate
	mov	di,bx
	shl	di,1
	shl	di,1
	shl	di,1		;DI = Y coordinate * 8
	shl	bx,1		;BX = Y coordinate * 2
	add	di,bx		;DI = Y coordinate * 10
	shl	di,1
	shl	di,1
	shl	di,1
	shl	di,1		;DI = Y coordinate * 160
	xor	bh,bh
	mov	bl,ah		;X coordinate
	add	di,bx		;DI = (Y coordinate * 160) + X coordinate

	ret


xlatColor:			;xlate PCjr color code to PC code
	mov	ah,al		;duplicate color code
	cmp	machtype,PC	;is the game running on a PC?
	retif	not_equal	;no - don't translate the color codes

	cmp	monType,MONO	;is a Hercules board installed?
	retif	equal		;yes - don't translate the color codes

	cmp	monType,EGA	;is an EGA board installed?
	retif	equal		;yes - don't translate the color codes

	jmp	pcxc



code	ends


end

