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
;The PerVMData structure.  This structure represents the our reserved space
;in the so-called CB data structure.  The CB data structure is created for
;each VM started in the system.  We can therefore, save states that we need
;to remember on a per-VM basis.  This data will vary for every mini-VDD.
;
PerVMData	struc
	V7Ex80		db	?		;Video 7/Cirrus 5420 register
	V7Ex81		db	?		;Video 7/Cirrus 5420 register
	V7Ex82		db	?		;Video 7/Cirrus 5420 register
	V7Ex83		db	?		;Video 7/Cirrus 5420 register
	V7Ex84		db	?		;Video 7/Cirrus 5420 register
	V7Ex85		db	?		;Video 7/Cirrus 5420 register
	V7Ex86		db	?		;Video 7/Cirrus 5420 register
	V7Ex87		db	?		;Video 7/Cirrus 5420 register
	V7Ex88		db	?		;Video 7/Cirrus 5420 register
	V7Ex89		db	?		;Video 7/Cirrus 5420 register
	V7Ex8A		db	?		;Video 7/Cirrus 5420 register
	V7Ex8B		db	?		;Video 7/Cirrus 5420 register
	V7Ex8C		db	?		;Video 7/Cirrus 5420 register
	V7Ex8D		db	?		;Video 7/Cirrus 5420 register
	V7Ex8E		db	?		;Video 7/Cirrus 5420 register
	V7Ex8F		db	?		;Video 7/Cirrus 5420 register
	V7Ex90		db	?		;Video 7/Cirrus 5420 register
	V7Ex91		db	?		;Video 7/Cirrus 5420 register
	V7Ex92		db	?		;Video 7/Cirrus 5420 register
	V7Ex93		db	?		;Video 7/Cirrus 5420 register
	V7Ex94		db	?		;Video 7/Cirrus 5420 register
	V7Ex95		db	?		;Video 7/Cirrus 5420 register
	V7Ex96		db	?		;Video 7/Cirrus 5420 register
	V7Ex97		db	?		;Video 7/Cirrus 5420 register
	V7Ex98		db	?		;Video 7/Cirrus 5420 register
	V7Ex99		db	?		;Video 7/Cirrus 5420 register
	V7Ex9A		db	?		;Video 7/Cirrus 5420 register
	V7Ex9B		db	?		;Video 7/Cirrus 5420 register
	V7Ex9C		db	?		;Video 7/Cirrus 5420 register
	V7Ex9D		db	?		;Video 7/Cirrus 5420 register
	V7Ex9E		db	?		;Video 7/Cirrus 5420 register
	V7Ex9F		db	?		;Video 7/Cirrus 5420 register
	V7ExA0		db	?		;Video 7/Cirrus 5420 register
	V7ExA1		db	?		;Video 7/Cirrus 5420 register
	V7ExA2		db	?		;Video 7/Cirrus 5420 register
	V7ExA3		db	?		;Video 7/Cirrus 5420 register
	V7ExA4		db	?		;Video 7/Cirrus 5420 register
	V7ExA5		db	?		;Video 7/Cirrus 5420 register
	V7ExA6		db	?		;Video 7/Cirrus 5420 register
	V7ExA7		db	?		;Video 7/Cirrus 5420 register
	V7ExA8		db	?		;Video 7/Cirrus 5420 register
	V7ExA9		db	?		;Video 7/Cirrus 5420 register
	V7ExAA		db	?		;Video 7/Cirrus 5420 register
	V7ExAB		db	?		;Video 7/Cirrus 5420 register
	V7ExAC		db	?		;Video 7/Cirrus 5420 register
	V7ExAD		db	?		;Video 7/Cirrus 5420 register
	V7ExAE		db	?		;Video 7/Cirrus 5420 register
	V7ExAF		db	?		;Video 7/Cirrus 5420 register
	V7ExB0		db	?		;Video 7/Cirrus 5420 register
	V7ExB1		db	?		;Video 7/Cirrus 5420 register
	V7ExB2		db	?		;Video 7/Cirrus 5420 register
	V7ExB3		db	?		;Video 7/Cirrus 5420 register
	V7ExB4		db	?		;Video 7/Cirrus 5420 register
	V7ExB5		db	?		;Video 7/Cirrus 5420 register
	V7ExB6		db	?		;Video 7/Cirrus 5420 register
	V7ExB7		db	?		;Video 7/Cirrus 5420 register
	V7ExB8		db	?		;Video 7/Cirrus 5420 register
	V7ExB9		db	?		;Video 7/Cirrus 5420 register
	V7ExBA		db	?		;Video 7/Cirrus 5420 register
	V7ExBB		db	?		;Video 7/Cirrus 5420 register
	V7ExBC		db	?		;Video 7/Cirrus 5420 register
	V7ExBD		db	?		;Video 7/Cirrus 5420 register
	V7ExBE		db	?		;Video 7/Cirrus 5420 register
	V7ExBF		db	?		;Video 7/Cirrus 5420 register
	V7ExC0		db	?		;Video 7/Cirrus 5420 register
	V7ExC1		db	?		;Video 7/Cirrus 5420 register
	V7ExC2		db	?		;Video 7/Cirrus 5420 register
	V7ExC3		db	?		;Video 7/Cirrus 5420 register
	V7ExC4		db	?		;Video 7/Cirrus 5420 register
	V7ExC5		db	?		;Video 7/Cirrus 5420 register
	V7ExC6		db	?		;Video 7/Cirrus 5420 register
	V7ExC7		db	?		;Video 7/Cirrus 5420 register
	V7ExC8		db	?		;Video 7/Cirrus 5420 register
	V7ExC9		db	?		;Video 7/Cirrus 5420 register
	V7ExCA		db	?		;Video 7/Cirrus 5420 register
	V7ExCB		db	?		;Video 7/Cirrus 5420 register
	V7ExCC		db	?		;Video 7/Cirrus 5420 register
	V7ExCD		db	?		;Video 7/Cirrus 5420 register
	V7ExCE		db	?		;Video 7/Cirrus 5420 register
	V7ExCF		db	?		;Video 7/Cirrus 5420 register
	V7ExD0		db	?		;Video 7/Cirrus 5420 register
	V7ExD1		db	?		;Video 7/Cirrus 5420 register
	V7ExD2		db	?		;Video 7/Cirrus 5420 register
	V7ExD3		db	?		;Video 7/Cirrus 5420 register
	V7ExD4		db	?		;Video 7/Cirrus 5420 register
	V7ExD5		db	?		;Video 7/Cirrus 5420 register
	V7ExD6		db	?		;Video 7/Cirrus 5420 register
	V7ExD7		db	?		;Video 7/Cirrus 5420 register
	V7ExD8		db	?		;Video 7/Cirrus 5420 register
	V7ExD9		db	?		;Video 7/Cirrus 5420 register
	V7ExDA		db	?		;Video 7/Cirrus 5420 register
	V7ExDB		db	?		;Video 7/Cirrus 5420 register
	V7ExDC		db	?		;Video 7/Cirrus 5420 register
	V7ExDD		db	?		;Video 7/Cirrus 5420 register
	V7ExDE		db	?		;Video 7/Cirrus 5420 register
	V7ExDF		db	?		;Video 7/Cirrus 5420 register
	V7ExE0		db	?		;Video 7/Cirrus 5420 register
	V7ExE1		db	?		;Video 7/Cirrus 5420 register
	V7ExE2		db	?		;Video 7/Cirrus 5420 register
	V7ExE3		db	?		;Video 7/Cirrus 5420 register
	V7ExE4		db	?		;Video 7/Cirrus 5420 register
	V7ExE5		db	?		;Video 7/Cirrus 5420 register
	V7ExE6		db	?		;Video 7/Cirrus 5420 register
	V7ExE7		db	?		;Video 7/Cirrus 5420 register
	V7ExE8		db	?		;Video 7/Cirrus 5420 register
	V7ExE9		db	?		;Video 7/Cirrus 5420 register
	V7ExEA		db	?		;Video 7/Cirrus 5420 register
	V7ExEB		db	?		;Video 7/Cirrus 5420 register
	V7ExEC		db	?		;Video 7/Cirrus 5420 register
	V7ExED		db	?		;Video 7/Cirrus 5420 register
	V7ExEE		db	?		;Video 7/Cirrus 5420 register
	V7ExEF		db	?		;Video 7/Cirrus 5420 register
	V7ExF0		db	?		;Video 7/Cirrus 5420 register
	V7ExF1		db	?		;Video 7/Cirrus 5420 register
	V7ExF2		db	?		;Video 7/Cirrus 5420 register
	V7ExF3		db	?		;Video 7/Cirrus 5420 register
	V7ExF4		db	?		;Video 7/Cirrus 5420 register
	V7ExF5		db	?		;Video 7/Cirrus 5420 register
	V7ExF6		db	?		;Video 7/Cirrus 5420 register
	V7ExF7		db	?		;Video 7/Cirrus 5420 register
	V7ExF8		db	?		;Video 7/Cirrus 5420 register
	V7ExF9		db	?		;Video 7/Cirrus 5420 register
	V7ExFA		db	?		;Video 7/Cirrus 5420 register
	V7ExFB		db	?		;Video 7/Cirrus 5420 register
	V7ExFC		db	?		;Video 7/Cirrus 5420 register
	V7ExFD		db	?		;Video 7/Cirrus 5420 register
	V7ExFE		db	?		;Video 7/Cirrus 5420 register
	V7ExFF		db	?		;Video 7/Cirrus 5420 register
	GCRReg6State	db	?		;needed for screwy BIOS check
	SeqReg2State	db	?		;needed for screwy BIOS check
PerVMData	ends
