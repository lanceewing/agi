/* LOGIC
**
*/

/* This file was assumed to exist. A real example on an original Sierra game disk
** has not been found. If this file is currently void of code, then it exists 
** merely as a placeholder to indicate that Siera's AGI interpreter had such a 
** file. If this file contains code, then it is because an attempt has been made
** to reconstruct what it might have looked like using a disassembly tool.
*/

/* The following is the section of the AGI.MAP file that relates to this module:

    Module LOGIC from file LOGIC
        Segment CODE.CODE, Addr = 10C0, Size = 2EE
            CALLLOGIC                                             0:1232
            CALLLOGICF                                            0:1258
            FINDLOG                                               0:10E7
            FLOADLOG                                              0:1131
            INITLOG                                               0:10C0
            LLOADLOG                                              0:1115
            LOADLOG                                               0:1172
            RESETLOG                                              0:10CF
            RESETSCANSTART                                        0:1322
            RESTORESCANOFS                                        0:137D
            SETSCANSTART                                          0:130D
            SETUPSCANOFS                                          0:133C
            _CALLLOGIC                                            0:1286
            _LLOADLOG                                             0:1155
        Segment CONST.CONST, Addr = BAB0, Size = 0
        Segment DATA.DATA, Addr = A50F, Size = 86
            CURLOG                                              9BA: 979
            LOGLIST                                             9BA: 96F
            PREVLOG                                             9BA: 97B
            SCANOFS                                             9BA: 97D

*/
