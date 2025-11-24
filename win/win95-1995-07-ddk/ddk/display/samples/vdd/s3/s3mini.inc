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
S3911	equ	080h			; Major chip id 911/924
S3924   equ     080h                    ; Major chip id 911/924
S3801   equ     0a0h                    ; Major chip id 801/805
S3805   equ     0a0h                    ; Major chip id 801/805
S3928   equ     090h                    ; Major chip id 928
S3928PCI equ    0b0h                    ; Major chip id 928 PCI
S3764	equ	0e0h			; Major chip id 764
S3864	equ	0c0h			; Major chip id 864
S3964	equ	0d0h			; Major chip id 964
S3732	equ	11h			; Major chip id 732 (in port 2EH)
S3866	equ	81h			; Major chip id 866 (in port 2EH)
S3868	equ	91h			; Major chip id 868 (in port 2EH)
S3968	equ	0b1h			; Major chip id 968 (in port 2EH)
S3968_B equ	0f1h			; Major chip id 968 (in port 2EH)
FOURTH_GEN	equ	01h		; OR'd into chip ID if 4th generation

;
;Chip running mode equates (from the S3 Mini-display-driver code):
;
ENGINE	equ	1			; chip in Engine mode
MEMORY	equ	0			; chip in MEMORY mode

;
;The PerVMData structure.  This structure represents the our reserved space
;in the so-called CB data structure.  The CB data structure is created for
;each VM started in the system.  We can therefore, save states that we need
;to remember on a per-VM basis.  This data will vary for every mini-VDD.
;The S3 chipset requires a few registers states for its extension registers
;to be saved.
;
PerVMData	struc
	Reg31HState	db	?
	Reg35HState	db	?
	Reg40HState	db	?
	Reg42HState	db	?
	Reg51HState	db	?
	Reg53HState	db	?
	Reg58HState	db	?
	Reg59HState	db	?
	Reg5AHState	db	?
	Reg6AHState	db	?
	BIOSVideoMode	dw	?
	Reg3C2State	db	?
	S3_Filler	db	3 dup(?)		;reclaim as needed
PerVMData	ends
.errnz	size PerVMData MOD 4
