;VGGRAPHX
;Picture drawing subroutines for the VGA graphics card

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file VGGRAPHX
;        Segment CODE.CODE, Addr = 93A5, Size = 102
;            VGB2S                                                 0:9416
;            VGDW                                                  0:9469
;            VGGS                                                  0:9400
;            VGIG                                                  0:93A5
;            VGIRT                                                 0:9494
;            VGSH                                                  0:94A6
;            VGTS                                                  0:93C7
;        Segment DATA.DATA, Addr = BA1C, Size = 0
