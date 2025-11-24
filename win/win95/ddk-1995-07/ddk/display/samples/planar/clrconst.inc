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

	.xlist
;----------------------------Module-Header------------------------------;
; Module Name: CLRCONST.BLT
;
; Color/monochrome-specific constants.
;
; This file is part of a set that makes up the Windows BitBLT function
; at driver-level.
;-----------------------------------------------------------------------;
	.list

ifdef	GEN_COLOR_BLT
if NUMBER_PLANES eq 3
PLANE_1			equ	00100001b
else
PLANE_1			equ	00010001b
endif
endif
