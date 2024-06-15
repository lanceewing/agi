;PLAYSND
;Assembly level routines for playing a SOUND

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file PLAYSND
;        Segment CODE.CODE, Addr = 7BCA, Size = 1B1
;            CONVERTFREQTOPC                                       0:7D60
;            CONVERTTDVOLUME                                       0:7D43
;            PLAYIT                                                0:7C2C
;            PLAYNOTE                                              0:7CEA
;            SOUNDOFF                                              0:7CB9
;            STARTSOUND                                            0:7BCA
;        Segment DATA.DATA, Addr = B3C4, Size = 23
;

include	macro.ah
include game.ah

public	StartSound
public	PlayIt
public	SoundOff
public	PlayNote
public	ConvertTDVolume
public	ConvertFreqToPC


cgroup	group	code
dgroup	group	data


sndPort		equ	0c0h
portB		equ	061h

NUM_CHANNELS	equ	4
CHAN1_SNDOFF	equ	0fh



data	segment public byte 'data'

extrn	machType:word
extrn	playSnd:word
extrn	sndEndFlag:word


;Definition of the sound node
soundNode	struc

sNext		dw	?
sNum		dw	?
sPtr		dw	?
sChan		dw	NUM_CHANNELS dup(?)

soundNode	ends

channel			dw	NUM_CHANNELS dup(?)
count			dw	NUM_CHANNELS dup(?)
channelActive	dw	NUM_CHANNELS dup(?)

;Codes which turn off the various channels
terminateCode	dw	09fh, 0bfh, 0dfh, 0ffh

maxChannel		dw	?
channelsPlaying	db	0

data	ends



code	segment	public byte 'code'
assume	cs:cgroup,ds:dgroup


extrn        SetFlag:near


StartSound	proc	near
;Start the sound whose node is pointed to by sNode.

	enter	sNode

	;Move the channel pointers into our local pointers
	mov	si,sNode
	add	si,sChan
	mov	di,offset dgroup:channel
	mov	cx,NUM_CHANNELS
	rep	movsw
	
	;Clear all the channel counters
	mov	ax,1
	mov	di,offset dgroup:count
	mov	cx,NUM_CHANNELS
	rep	stosw		

	;Set all channels active
	mov	di, offset dgroup:channelActive
	mov	cx,NUM_CHANNELS
	rep	stosw

	;Set the maximum channel # for the machine_
	cmp	machType,PC
	.if	equal
		mov	maxChannel,2
	.else
		mov	maxChannel,NUM_CHANNELS * 2
		.end

	;Gate sound chip to audio if a pcjr
	cmp	machType,PC
	.if	not_equal
		in	al,portB
		or	al,60h
		out	portB,al
		.end

	mov	playSnd,1

	exit

StartSound	endp




PlayIt		proc	near
;Play the next set of frequencies in the sound pointed to by 'sndPtr'.
;This is entered as interrupt code with only ax and ds saved.

	push	bx
	push	cx
	push	dx
	push	si

	xor	bx,bx			;bx will be the channel index
	mov	channelsPlaying,bl	;initialize channels playing count
	do
		;Don't play this channel if it's finished
		cmp	channelActive[bx],1
		.if	not_equal
			add	bx,2
			continue
			.end

		;See if it's time to send a new note out on this channel
		dec	count[bx]
		.if	zero
			;Get the pointer to channel data
			mov	si,channel[bx]

			;Load the count for this channel and see if we're done
			lodsw
			cmp	ax,0ffffh
			.if	equal
				;Turn the channel off
				dec	channelActive[bx]
				mov	al,byte ptr terminateCode[bx]
				call	PlayNote
				break
				.end
			mov	count[bx],ax

			;Get the note and volume
			lodsw
			mov	dx,ax
			lodsb
			mov	cl,al

			mov	channel[bx],si	;save ptr to channel data

			push	bx
			call	PlayNote
			pop	bx
			.end

		;This channel is playing -- increment the number
		;of channels which are
		inc	channelsPlaying

		add	bx,0002h
		cmp	bx,maxChannel
	while	less

	cmp	channelsPlaying,0
	.if	equal
		call	SoundOffSub
		.end

	pop	si
	pop	dx
	pop	cx
	pop	bx

	return

PlayIt		endp



SoundOff	proc	near

	enter
	call	SoundOffSub
	exit



SoundOffSub:
	cmp	machType,PC
	.if	equal
		mov	cl,CHAN1_SNDOFF
		call	PlayNote
	.else
		mov	al,09fh
		out	sndPort,al
		mov	al,0bfh
		out	sndPort,al
		mov	al,0dfh
		out	sndPort,al
		mov	al,0ffh
		out	sndPort,al
		.end

    mov	playSnd,0
    mov al,byte ptr sndEndFlag
    call SetFlag

	return

SoundOff		endp



PlayNote	proc	near
;Note is in dx, volume in cl.

	cmp	machType,PC
	.if	not_equal			;PCjr/Tandy sounds
		cmp	machType,TD
		.if	equal
			call	ConvertTDVolume
			.end

		;Send the notes out
		cmp	dh,00h
		.if	not_equal
			mov	al,dh
			out	sndPort,al
			mov	al,dl
			out	sndPort,al
		.end
		mov	al,cl
		out	sndPort,al
	.else					;PC sounds
		and	cl,0fh
		cmp	cl,CHAN1_SNDOFF
		.if	equal
			in	al,portB
			and	al,0fch
			out	portB,al
		.else
			call	ConvertFreqToPC		;returns freq in bx
	
			;Start the note playing.
			mov	al,0b6h
			out	43h,al
	
			mov	ax,bx
			out	42h,al
			mov	al,ah
			out	42h,al
	
			in	al,portB
			or	al,3
			out	portB,al
			.end
		.end

	return


PlayNote	endp



ConvertTDVolume	proc	near
;Convert the volume in the cl to something a little lower for the loud
;Tandy sound channel.

	mov	ch,cl
	and	ch,90h
	cmp	ch,90h
	.if	equal
		mov	ch,cl
		and	ch,0fh
		cmp	ch,8
		.if	less
			and	cl,0f0h
			add	ch,3
			or	cl,ch
			.end
		.end
	return

ConvertTDVolume	endp



ConvertFreqToPC	proc	near
;Convert the note codes in the dx to a frequency divisor in the bx

	;Get low bits into bl
	and	dh,00fh
	mov	bl,dh

	;Add the high bits
	xor	dh,dh
	and	dl,03fh
	mov	cl,4
	shl	dx,cl
	add	bx,dx

	;Multiply by 12 to get note for the PC
	shl	bx,1
	shl	bx,1		;* 4
	mov	dx,bx
	shl	bx,1		;* 8
	add	bx,dx		;+ -> * 12

	return

ConvertFreqToPC	endp





code	ends
	end
