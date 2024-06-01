;XTGRAPHX
;Picture drawing subroutines for the EGA graphics card

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file XTGRAPHX
;        Segment CODE.CODE, Addr = 93A5, Size = 240
;            XTB2S                                                 0:9415
;            XTDW                                                  0:949C
;            XTGS                                                  0:93FF
;            XTIG                                                  0:93A5
;            XTIRT                                                 0:9519
;            XTSH                                                  0:9414
;            XTSS                                                  0:954E
;            XTTS                                                  0:93C5
;        Segment DATA.DATA, Addr = B978, Size = 0
