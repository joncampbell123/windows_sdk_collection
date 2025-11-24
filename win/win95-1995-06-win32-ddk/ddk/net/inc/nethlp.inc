;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

RIOCData	STRUC
iod_magic	DD	?
iod_addr	DD	?
RIOCData	ENDS
RIOCMAGICINL	EQU	0E970H
RIOCMAGICINH	EQU	3734H
RIOCMAGICOUTL	EQU	3734H
RIOCMAGICOUTH	EQU	0EF70H
RIOCMAGICIN	EQU	3734E970H
RIOCMAGICOUT	EQU	0EF703734H
RDRHNDGETVARS	EQU	0
RDRHNDINSTALL	EQU	1
RDRHNDUNINSTALL	EQU	2
RDRHNDSETLPT	EQU	3
RDRHNDREMOVELPT	EQU	4
RDRHNDTRANSITION	EQU	5
RDRTINYINSTALL	EQU	6
RDRTINYUNINSTALL	EQU	7
RDRGETINSTALLSTATE	EQU	8
SVINSTALL	EQU	9
SVSETHOOK	EQU	10
SVUNINSTALL	EQU	11
SVGETINFO	EQU	12
RDRTINYINSTALLED	EQU	1
RDRENHINSTALLED	EQU	2
SERVERINSTALLED	EQU	4
VNBGETINFO	EQU	6
VNBWRITEMCBT	EQU	7
VNBWRITEHOOK	EQU	8
VNBHOOKSONOFF	EQU	9
