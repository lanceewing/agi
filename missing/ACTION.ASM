;ACTION.ASM
;

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file ACTION
;        Segment CODE.CODE, Addr = 29C, Size = 43
;            ACTION                                                0: 29C
;        Segment DATA.DATA, Addr = A1BD, Size = 2B8
;            ACTIONTBL                                           9BA: 61D


include	macro.ah
include	trace.ah


Public Action


cgroup	group	code
dgroup	group	data



; Note that NoAction is the first entry in the actionTbl, according to ETC.C
; Including entry 0, i.e. NoAction, there are 174 entries. Last action is HoldKey.
; actionTbl size is 0x2B8, which when divided by 4 is 174. That implies 4 bytes each.

actionNode	macro	name, numParms, varBits
extrn	name:near
dw	offset cgroup:name
db	numParms
db	varBits
endm



data	segment byte public 'data'

extrn	traceStatus:word

public  actionTbl

actionTbl	label	word
	actionNode	NoAction, 0, 0              ; In module ETC
	actionNode  Increment, 1, 10000000b     ; In module VAR
	actionNode  Decrement, 1, 10000000b     ; In module VAR
	actionNode  AssignN, 2, 10000000b       ; In module VAR
	actionNode  AssignF, 2, 11000000b       ; In module VAR
	actionNode  AddN, 2, 10000000b          ; In module VAR
	actionNode  AddF, 2, 11000000b          ; In module VAR
	actionNode  SubN, 2, 10000000b          ; In module VAR
	actionNode  SubF, 2, 11000000b          ; In module VAR
	actionNode  LIndirectF, 2, 11000000b    ; In module VAR
	actionNode  RIndirect, 2, 11000000b     ; In module VAR - 10
	actionNode  LIndirectN, 2, 10000000b    ; In module VAR
	actionNode  SetFlag, 1, 0               ; In module FLAG
	actionNode  ResetFlag, 1, 0             ; In module FLAG
	actionNode  ToggleFlag, 1, 0            ; In module FLAG
	actionNode  VSetFlag,	1, 10000000b    ; In module FLAG
	actionNode  VResetFlag, 1, 10000000b    ; In module FLAG
	actionNode  VToggleFlag, 1, 10000000b   ; In module FLAG
	actionNode  LNewRoom, 1, 0              ; In module NEWROOM
	actionNode  LNewRoomF, 1, 10000000b     ; In module NEWROOM
	actionNode  LLoadLog, 1, 0              ; In module LOGIC - 20
	actionNode  FLoadLog, 1, 10000000b      ; In module LOGIC
	actionNode  CallLogic,	1, 0            ; In module LOGIC
	actionNode  CallLogicF, 1, 10000000b    ; In module LOGIC
	actionNode  LLoadPic, 1, 10000000b      ; In module PICTURE
	actionNode  LPicture, 1, 10000000b      ; In module PICTURE
	actionNode  ShowPic, 0, 0               ; In module PICTURE
	actionNode  LDiscardPic, 1, 10000000b   ; In module PICTURE
	actionNode  LOverlayPic, 1, 10000000b   ; In module PICTURE
	actionNode  ShowPriScreen, 0, 0         ; In module DEBUG
	actionNode  LLoadView, 1, 0             ; In module VIEW - 30
	actionNode  LoadViewF, 1, 10000000b     ; In module VIEW
	actionNode  LDiscardView, 1, 0          ; In module VIEW
	actionNode  LAnimateObj, 1, 0           ; In module ANIMATE
	actionNode  UnAnimateAll, 0, 0          ; In module ANIMATE
	actionNode  LDraw, 1, 0                 ; In module DRAW
	actionNode  LErase, 1, 0                ; In module DRAW
	actionNode  Position, 3, 0              ; In module POSITION
	actionNode  FlagPosn, 3, 1100000b       ; In module POSITION
	actionNode  GetPosn, 3, 1100000b        ; In module POSITION
	actionNode  Reposition, 3, 1100000b     ; In module POSITION - 40
	actionNode  LSetView, 2, 0              ; In module VIEW
	actionNode  SetViewF, 2, 1000000b       ; In module VIEW
	actionNode  LSetLoop, 2, 0              ; In module VIEW
	actionNode  SetLoopF, 2, 1000000b       ; In module VIEW
	actionNode  FixLoop, 1, 0               ; In module ADVANCEL
	actionNode  ReleaseLoop, 1, 0           ; In module ADVANCEL
	actionNode  LSetCel, 2, 0               ; In module VIEW
	actionNode  SetCelF, 2, 1000000b        ; In module VIEW
	actionNode  LastCel, 2, 1000000b        ; In module VIEW
	actionNode  CurrentCel, 2, 1000000b     ; In module VIEW - 50
	actionNode  CurrentLoop, 2, 1000000b    ; In module VIEW
	actionNode  CurrentView, 2, 1000000b    ; In module VIEW
	actionNode  NumberLoops, 2, 1000000b    ; In module VIEW
	actionNode  SetPriority, 2, 0           ; In module PRIORITY
	actionNode  FSetPriority,	2, 1000000b ; In module PRIORITY
	actionNode  ReleasePriority, 1, 0       ; In module PRIORITY
	actionNode  GetPriority, 2, 1000000b    ; In module PRIORITY
	actionNode  LStopUpdate, 1, 0           ; In module ANILIST
	actionNode  LStartUpdate, 1, 0          ; In module ANILIST
	actionNode  ForceUpdate, 1, 0           ; In module ANILIST - 60
	actionNode  IgnoreHorizon, 1, 0         ; In module POSITION
	actionNode  ObserveHorizon,	1, 0        ; In module POSITION
	actionNode  SetHorizon, 1, 0            ; In module POSITION
	actionNode  OnWater, 1, 0               ; In module POSITION
	actionNode  OnLand, 1, 0                ; In module POSITION
	actionNode  OnAnything, 1, 0            ; In module POSITION
	actionNode  IgnoreObjs, 1, 0            ; In module COLLIDE
	actionNode  ObserveObjs, 1, 0           ; In module COLLIDE
	actionNode  Distance, 3, 100000b        ; In module COLLIDE
	actionNode  StopCycling, 1, 0           ; In module CYCLE - 70
	actionNode  StartCycling, 1, 0          ; In module CYCLE
	actionNode  NormalCycle, 1, 0           ; In module CYCLE
	actionNode  EndOfLoop, 2, 0             ; In module CYCLE
	actionNode  ReverseCycle, 1, 0          ; In module CYCLE
	actionNode  ReverseLoop, 2, 0           ; In module CYCLE
	actionNode  SetCycleFreq, 2, 1000000b   ; In module CYCLE
	actionNode  StopMotion, 1, 0            ; In module MOTION
	actionNode  StartMotion, 1, 0           ; In module MOTION
	actionNode  SetStepSize, 2, 1000000b    ; In module MOTION
	actionNode  SetStepTime, 2, 1000000b    ; In module MOTION - 80
	actionNode  LMoveObj, 5, 0              ; In module MOTION
	actionNode  MoveObjF, 5, 1110000b       ; In module MOTION
	actionNode  FollowEgo, 3, 0             ; In module MOTION
	actionNode  StartWander, 1, 0           ; In module MOTION
	actionNode  NormalMotion, 1, 0          ; In module MOTION
	actionNode  SetObjDir, 2, 1000000b      ; In module MOTION
	actionNode  GetObjDir, 2, 1000000b      ; In module MOTION
	actionNode  IgnoreBlocks, 1, 0          ; In module BLOCK
	actionNode  ObserveBlocks, 1, 0         ; In module BLOCK
	actionNode  Block, 4, 0                 ; In module BLOCK - 90
	actionNode  UnBlock, 0, 0               ; In module BLOCK
	actionNode  Get, 1, 0                   ; In module OBJACT
	actionNode  GetF, 1, 10000000b          ; In module OBJACT
	actionNode  Drop, 1, 0                  ; In module OBJACT
	actionNode  Put, 2, 0                   ; In module OBJACT
	actionNode  PutF,	2, 1000000b         ; In module OBJACT
	actionNode  GetRoomF, 2, 11000000b      ; In module OBJACT
	actionNode  LLoadSnd, 1, 0              ; In module SOUND
	actionNode  Sound, 2, 0                 ; In module SOUND
	actionNode  LStopSnd, 0, 0              ; In module SOUND - 100
	actionNode  LPrintMsg, 1, 0             ; In module PRINT
	actionNode  FPrintMsg, 1, 10000000b     ; In module PRINT
	actionNode  Display, 3, 0               ; In module PRINT
	actionNode  DisplayF, 3, 11100000b      ; In module PRINT
	actionNode  LClearLines, 3, 0           ; In module SCRACT
	actionNode  LTextScrn, 0, 0             ; In module SCRACT
	actionNode  LGrafScrn, 0, 0             ; In module SCRACT
	actionNode  SetCursorChar, 1, 0         ; In module USERIN
	actionNode  SetTextAtr, 2, 0            ; In module SCRACT
	actionNode  ShakeScreen, 1, 0           ; In module SHAKE - 110
	actionNode  ConfigureScreen, 3, 0       ; In module SCRACT
	actionNode  StatusLineOn, 0, 0          ; In module STATUS
	actionNode  StatusLineOff, 0, 0         ; In module STATUS
	actionNode  SetString, 2, 0             ; In module STRING
	actionNode  GetString, 5, 0             ; In module STRING
	actionNode  WordToString, 2, 0          ; In module STRING
	actionNode  ParseString, 1, 0           ; In module PARSE
	actionNode  GetNum, 2, 1000000b         ; In module DEBUG
	actionNode  PreventInput, 0, 0          ; In module USERIN
	actionNode  AcceptInput, 0, 0           ; In module USERIN - 120
	actionNode  SetControl, 3, 0            ; In module CONTROL
	actionNode  LAddToPic, 7, 0             ; In module PICOBJ
	actionNode  AddToPicF, 7, 11111110b     ; In module PICOBJ
	actionNode  Status, 0, 0                ; In module STATUS
	actionNode  SaveGame, 0, 0              ; In module SAVEGAME
	actionNode  RestoreGame, 0, 0           ; In module RESTGAME
	actionNode  NoAction, 0, 0              ; In module ETC - (was init.disk() but now does nothing)
	actionNode  Restart, 0, 0               ; In module RESTART
	actionNode  ShowObj, 1, 0               ; In module SHOWOBJ
	actionNode  Random, 3, 100000b          ; In module RANDOM - 130
	actionNode  ProgramControl, 0, 0        ; In module MOTION
	actionNode  PlayerControl, 0, 0         ; In module MOTION
	actionNode  ObjStatus, 1, 10000000b     ; In module DEBUG
	actionNode  QuitGame, 1, 0              ; In module MAIN
	actionNode  ShowMem, 0, 0               ; In module MEMMGR
	actionNode  Pause, 0, 0                 ; In module MAIN
	actionNode  EchoLine, 0, 0              ; In module USERIN
	actionNode  CancelLine, 0, 0            ; In module USERIN
	actionNode  InitJoy, 0, 0               ; In module JOYDRV
	actionNode  ToggleMonitor, 0, 0         ; In module SCRACT - 140
	actionNode  Version, 0, 0               ; In module DEBUG
	actionNode  SetScriptSize, 1, 0         ; In module SCRIPT
	actionNode  SetGameID, 1, 0             ; In module STRING
	actionNode  Log, 1, 0                   ; In module LOG 
	actionNode  SetScanStart, 0, 0          ; In module LOGIC
	actionNode  ResetScanStart, 0, 0        ; In module LOGIC
	actionNode  RepositionTo, 3, 0          ; In module POSITION
	actionNode  VRepositionTo, 3, 1100000b  ; In module POSITION
	actionNode  TraceOn, 0, 0               ; In module TRACE
	actionNode  TraceInfo, 3, 0             ; In module TRACE - 150
	actionNode  PrintMsgAt, 4, 0            ; In module PRINT
	actionNode  FPrintMsgAt, 4, 10000000b   ; In module PRINT
	actionNode  VDiscardView, 1, 10000000b  ; In module VIEW
	actionNode  LClearRect, 5, 0            ; In module SCRACT
	actionNode  SetUpperLeft, 2, 0          ; In module PICTURE
	actionNode  SetMenu, 1, 0               ; In module MENU
	actionNode  SetMenuItem, 2, 0           ; In module MENU
	actionNode  SubmitMenu, 0, 0            ; In module MENU
	actionNode  EnableMenu, 1, 0            ; In module MENU
	actionNode  DisableMenu, 1, 0           ; In module MENU - 160
	actionNode  MenuInput, 0, 0             ; In module MENU
	actionNode  VShowObj, 1, 1              ; In module SHOWOBJ
	actionNode  OpenDialogue, 0, 0          ; In module HDIALOG 
	actionNode  CloseDialogue, 0, 0         ; In module HDIALOG
	actionNode  MulN, 2, 10000000b          ; In module VAR
	actionNode  MulV, 2, 11000000b          ; In module VAR
	actionNode  DivN, 2, 10000000b          ; In module VAR
	actionNode  DivV, 2, 11000000b          ; In module VAR
	actionNode  CloseWindow, 0, 0           ; In module PRINT
	actionNode  SetSimple, 1, 0             ; In module SAVEGAME - 170
	actionNode  PushScript, 0, 0            ; In module SCRIPT
	actionNode  PopScript, 0, 0             ; In module SCRIPT
	actionNode  HoldKey, 0, 0               ; In module KBDINT


data	ends



code	segment public byte 'code'
assume	cs:cgroup,ds:dgroup


extrn	Error:near
extrn	TraceAction:near



Action	proc near
;Action number is passed in the al
;

    do
		cmp	al, 0FCh
		breakif not_below

		cmp	al, 0
		breakif zero

		cmp	al, 173
		.if above
			xor	ah, ah
			push	ax
			mov 	ax, 16
			push	ax
			call	Error	;bad action number
		.end

		cmp	traceStatus,TRACE_ON
		.if	equal
			xor	ah,ah
			push	ax
			pcall	TraceAction,ax,si
			pop	ax
		.end

		xor	bh,bh
		mov	bl,al
		shl	bx,1
		shl	bx,1
		push	si
		call	actionTbl[bx]
		add	sp, 2
		
		mov	si, ax		; ax is	set to next command byte in this logic stream
		or	si, si		; if zero, means new.room encountered
		breakif zero    ; quit right now if new.room found

		lodsb			; get next cmd value, store it in ax, then return

	repeat

	return

Action	endp



code	ends
	end
