;START
;Entry point to start the AGI interpreter.

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file START
;        Segment BASE.CODE, Addr = 0, Size = 78
;        Segment CODE.CODE, Addr = 78, Size = B0
;            $STRT$                                                0:  78
;            EXIT                                                  0:  AE
;            GETARGS                                               0:  C4
;            START                                                 0:  78
;        Segment DATA.DATA, Addr = 9BA0, Size = 2
;            STACKBASE                                           9BA:   0

;NOTE: The $STRT$ symbol has been identified as originating with the PLINK86
;      Overlay Manager. PLINK86 must have added that during the linking 
;      process.
