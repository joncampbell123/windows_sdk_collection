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

;
;Include file for MiniVDD definitions:
;

;
;Chip ID equates:
;
ET4000		equ	10h
TL_W32		equ	20h

;
;The PerVMData structure.  This structure represents the our reserved space
;in the so-called CB data structure.  The CB data structure is created for
;each VM started in the system.  We can therefore, save states that we need
;to remember on a per-VM basis.  This data will vary for every mini-VDD.
;
;The structure must have Reg3CBH in the byte 0 and Reg3CDH in byte 2!!!
;
PerVMData		struc
	Reg3CBH 	db	?
	Filler_1	db	?		;reserved, use as needed
	Reg3CDH 	db	?
	Filler_2	db	?		;reserved, use as needed
PerVMData		ends
;
;
