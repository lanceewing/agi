;PICDRAW
;Assembly level routines for drawing a PICTURE

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file PICDRAW
;        Segment CODE.CODE, Addr = 6081, Size = 337
;            DRAWPIC                                               0:6086
;            DRWPICNC                                              0:6081
;            POINT                                                 0:6140
;            _DRAWBRUSH                                            0:616B
;        Segment DATA.DATA, Addr = B20D, Size = DA
