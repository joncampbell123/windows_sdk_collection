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

VDA_FileSysChange	EQU	(0400H+20+11)
MAXFSCINFOSIZE	EQU	256

VMDA_FileSysChng	STRUC
VMDA_FSC_Func	DW	?
VMDA_FSC_Buffer	DB	MAXFSCINFOSIZE DUP (?)
VMDA_FileSysChng	ENDS
VMDA_FSC_CREATE	EQU	0
VMDA_FSC_DELETE	EQU	1
VMDA_FSC_RENAME	EQU	2
VMDA_FSC_ATTRIBUTES	EQU	3
VMDA_FSC_NETCONNECT	EQU	4
VMDA_FSC_NETDISCONNECT	EQU	5
VMDA_FSC_REFRESH	EQU	6
VMDA_FSC_MKDIR	EQU	7
VMDA_FSC_RMDIR	EQU	8
