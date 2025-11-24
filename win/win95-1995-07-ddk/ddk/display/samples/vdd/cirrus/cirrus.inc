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
; Note: these match those in cirrus.inc in the cirrus.drv mini display driver.
;
CL_6420         equ     1
CL_6440         equ     2
CL_5420         equ     4
CL_5422         equ     8
CL_5426         equ     10h
CL_5428         equ     20h
CL_5429         equ     40h
CL_5430         equ     80h
CL_5432         equ     100h
CL_5434         equ     200h
CL_62x5         equ     400h
CL_6235         equ     800h
CL_6245         equ     1000h
CL_7541         equ     2000h
CL_7542         equ     4000h
CL_7543         equ     8000h
  
IS_BLTTER       equ     CL_5426+CL_5428+CL_5429+CL_5430+CL_5432+CL_5434+CL_7541+CL_7542+CL_7543

; register definitions
; first, the obvious
SEQ_INDEX       equ     3c4h
SEQ_DATA        equ     3c5h
GRAPHICS_INDEX  equ     3ceh
GRAPHICS_DATA   equ     3cfh
CRTC_INDEX      equ     3d4h
CRTC_DATA	equ	3d5h

;
; 5426 extended register equates
BLT_STATUS_REGISTER     equ     31h
BLT_BUSY_BIT		equ	1	; bit in GR31 that is bltter busy


;----------------------------------------------------------------------------
unlock_extension_regs_542x	macro

        mov     edx,SEQ_INDEX           ; EDX -> SR index register
	in	al,dx			; get current SR index
	ror	eax,8			; save it

	mov	ax,1206h		; this will unlock the extension regs
	out	dx,ax			;
;
	rol	eax,8			; get the index
	out	dx,al			; restore SR index
        endm

unlock_extension_regs_64xx	macro

        mov     edx,GRAPHICS_INDEX      ; EDX -> GR index register
	in	al,dx			; get current GR index
	ror	eax,8			; save it

	mov	ax,0ec0ah		;this will unlock the extension regs
	out	dx,ax			;
;
	rol	eax,8			; get the index
	out	dx,al			; restore GR index
        endm


;
;The PerVMData structure.  This structure represents the our reserved space
;in the so-called CB data structure.  The CB data structure is created for
;each VM started in the system.  We can therefore, save states that we need
;to remember on a per-VM basis.  This data will vary for every mini-VDD.
;The Cirrus Logic chipsets requires a few registers states for its extension registers
;to be saved.
;
PerVMData_54xx	struc
	GCRReg0 	db	?
	GCRReg1 	db	?
	GCRReg5 	db	?
	GCRReg9 	db	?
	GCRRegA 	db	?
	GCRRegB 	db	?
	SeqReg2 	db	?
	SeqReg6 	db	?
	SeqReg9 	db	?
	SeqRegA 	db	?
	SeqReg12 	db	?
	SeqReg14 	db	?
	SeqReg15 	db	?
	CRTCReg11	db	?
	CRTCReg1D	db	?
        DACIndex        db      ?
        DACPixelIndex   dd      ?
        DACPixelMode    db      ?               ; READ_MODE or WRITE_MODE
        HiddenDAC       db      4*10h   dup(?)
PerVMData_54xx	ends
;
WRITE_MODE      equ     0               ; DAC write mode flag
READ_MODE       equ     3               ; DAC read mode flag

;
PerVMData_64xx	struc
	GCRReg0A 	db	?               ; only 0 or 1 valid! 
	GCRRegD 	db	?
	GCRRegA1	db	?
	GCRRegAE	db	?
	GCRRegB0	db	?
PerVMData_64xx	ends
