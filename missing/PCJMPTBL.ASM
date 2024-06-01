;PCJMPTBL
;Jump table for PC (CGA) graphics routines

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file PCJMPTBL
;        Segment CODE.CODE, Addr = 9390, Size = 15
;            BLK2SCRN                                              0:939C
;            DRAWWNDW                                              0:93A2
;            GRAFSCRN                                              0:9396
;            INITGRFX                                              0:9390
;            INITROWS                                              0:939F
;            SETHORZ                                               0:9399
;            TEXTSCRN                                              0:9393
