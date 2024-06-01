;PCGRAPHX
;Picture drawing subroutines for the PC (CGA)

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file PCGRAPHX
;        Segment CODE.CODE, Addr = 93A5, Size = 297
;            PCB2S                                                 0:9545
;            PCDW                                                  0:95A5
;            PCGS                                                  0:94FB
;            PCIG                                                  0:946B
;            PCIRT                                                 0:9613
;            PCSH                                                  0:9510
;            PCTS                                                  0:94A9
;            PCXC                                                  0:93A5
;            PCXO                                                  0:93C7
;        Segment DATA.DATA, Addr = B916, Size = 62
;            COLORTBL                                            9BA:1D76
