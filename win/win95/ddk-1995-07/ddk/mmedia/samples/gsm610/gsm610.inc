;*==========================================================================;
;*
;*  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
;*  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
;*  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
;*  A PARTICULAR PURPOSE.
;*
;*  Copyright (C) 1993 - 1995 Microsoft Corporation. All Rights Reserved.
;*
;*--------------------------------------------------------------------------;
;*
;*  gsm610.inc
;*
;*  Description:
;*      This file contains definitions borrowed from gsm610.h and codec.h,
;*      as needed.
;*
;*==========================================================================;

tagSTREAMINSTANCE	STRUC
	; This GSM610 codec requires the following parameters
	; per stream instance.  These parameters are used by
	; the encode and decode routines.
fnConvert	DD ?
fdwConfig       DD ?
dp	DW	120 DUP (?)
drp	DW	160 DUP (?)
z1	DW	?
l_z2	DD	?
mp	DW	?
OldLARpp	DW	9 DUP (?)
u	DW	8 DUP (?)
nrp	DW	?
OldLARrpp	DW	9 DUP (?)
msr	DW	?
v	DW	9 DUP (?)
tagSTREAMINSTANCE	ENDS
STREAMINSTANCE STRUC
	DB	SIZE tagSTREAMINSTANCE DUP (?)
STREAMINSTANCE ENDS
PSTREAMINSTANCE STRUC
	DW	?
PSTREAMINSTANCE ENDS
NPSTREAMINSTANCE STRUC
	DW	?
NPSTREAMINSTANCE ENDS
FPSTREAMINSTANCE STRUC
	DD	?
FPSTREAMINSTANCE ENDS
