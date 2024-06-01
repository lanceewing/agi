;FILEIO
;

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file FILEIO
;        Segment CODE.CODE, Addr = 58EC, Size = 1EE
;            CLOSE                                                 0:5991
;            CREAT                                                 0:58EC
;            DUPLICATE                                             0:59D3
;            FILEDT                                                0:5AB2
;            FINDFIRST                                             0:5A40
;            FINDNEXT                                              0:5A65
;            GETCURDIR                                             0:59F1
;            GETCURDRIVE                                           0:5A29
;            LSEEK                                                 0:59AA
;            OPEN                                                  0:590D
;            READ                                                  0:592E
;            UNLINK                                                0:5974
;            VALIDDRIVE                                            0:5A7D
;            WRITE                                                 0:5951
;        Segment DATA.DATA, Addr = AFA1, Size = 0
