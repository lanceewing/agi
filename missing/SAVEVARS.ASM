;SAVEVARS
;

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file SAVEVARS
;        Segment DATA.DATA, Addr = 9BA2, Size = 5E1
;            BACKCOLOR                                           9BA: 5CF
;            BLOCKING                                            9BA: 13D
;            CURCHAR                                             9BA: 5D7
;            FLAG                                                9BA: 109
;            FORECOLOR                                           9BA: 5CD
;            GAMEID                                              9BA:   2
;            GETINPUT                                            9BA: 5D3
;            HORIZON                                             9BA: 12D
;            INPUTROW                                            9BA: 5D5
;            KEYDIR                                              9BA: 12F
;            KEYMAP                                              9BA: 145
;            LRX                                                 9BA: 135
;            LRY                                                 9BA: 137
;            MAXDRAWN                                            9BA: 13F
;            PICBOTTOM                                           9BA: 5DF
;            PICNO                                               9BA: 13B
;            PICTOP                                              9BA: 5DD
;            SAVEDSCRIPT                                         9BA: 5E1
;            SAVEEND                                             9BA: 5E3
;            SAVESTART                                           9BA:   2
;            SCRIPTENTRIES                                       9BA: 143
;            SCRIPTSIZE                                          9BA: 141
;            SHOWSTATUS                                          9BA: 5D9
;            STATUSROW                                           9BA: 5DB
;            SYSTIMER                                            9BA: 129
;            TEXTATR                                             9BA: 5D1
;            ULX                                                 9BA: 131
;            ULY                                                 9BA: 133
;            USERCTRL                                            9BA: 139
;            USERSTR                                             9BA: 20D
;            VAR                                                 9BA:   9


dgroup	group data
assume ds:dgroup, es:dgroup


public  saveStart, gameID, var, flag, sysTimer, horizon, keyDir, ulx, uly
public  lrx, lry, userCtrl, picNo, blocking, maxDrawn, scriptSize
public  scriptEntries, keyMap, userStr, foreColor, backColor, textAtr
public  getInput, inputRow, curChar, showStatus, statusRow, picTop
public  picBottom, savedScript, saveEnd


data	segment byte public 'data'

saveStart       label   word
gameID          db  7   dup(0)

var             db  256 dup(0)
flag            db  20h dup(0)
sysTimer        dd  0
horizon         dw  0
keyDir          dw  0
ulx             dw  0
uly             dw  0
lrx             dw  0
lry             dw  0
userCtrl        dw  0
picNo           dw  0
blocking        dw  0
maxDrawn        dw  0Fh
scriptSize      dw  50
scriptEntries   dw  0
keyMap          db  200 dup(0)
userStr         db  960 dup(0)
foreColor       dw  0
backColor       dw  0
textAtr         dw  0
getInput        dw  0
inputRow        dw  23
curChar         dw  0
showStatus      dw  0
statusRow       dw  15h
picTop          dw  0
picBottom       dw  0

savedScript     dw  0

saveEnd         label   word


data	ends


end
