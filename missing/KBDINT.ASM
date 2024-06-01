;KBDINT
;The keyboard interrupt handler

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file KBDINT
;        Segment CODE.CODE, Addr = 5C6E, Size = 10F
;            HOLDKEY                                               0:5C6E
;            KBDHANDLER                                            0:5C76
;        Segment DATA.DATA, Addr = B155, Size = 18
