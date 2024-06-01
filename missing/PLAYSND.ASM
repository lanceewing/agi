;PLAYSND
;Assembly level routines for playing a SOUND

;This file was assumed to exist. A real example on an original Sierra game disk
;has not been found. If this file is currently void of code, then it exists 
;merely as a placeholder to indicate that Siera's AGI interpreter had such a 
;file. If this file contains code, then it is because an attempt has been made
;to reconstruct what it might have looked like using a disassembly tool.

;The following is the section of the AGI.MAP file that relates to this module:
; 
;    Module A from file PLAYSND
;        Segment CODE.CODE, Addr = 7BCA, Size = 1B1
;            CONVERTFREQTOPC                                       0:7D60
;            CONVERTTDVOLUME                                       0:7D43
;            PLAYIT                                                0:7C2C
;            PLAYNOTE                                              0:7CEA
;            SOUNDOFF                                              0:7CB9
;            STARTSOUND                                            0:7BCA
;        Segment DATA.DATA, Addr = B3C4, Size = 23
