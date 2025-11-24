;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1994 - 1995  Microsoft Corporation.  All Rights Reserved.
;
;   OEMJOY.INC
;   File:	oemjoy.inc
;   Content:	oemjoy include file
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   The following equates can either be used by pmode apps, rmode apps,
;   or VxD's.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

MULTIMEDIA_OEM_ID	EQU	0440H
OEMJOY_Device_ID	EQU	MULTIMEDIA_OEM_ID+8

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   The following equates are only useful in a VxD.  DDB_Sys_Crit_Init_Done
;   is defined by VMM.INC iff 'Not_Vxd' is NOT defined.  In this way, if
;   VMM.INC is included in a VxD before this include file, you will have
;   access to this information.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

ifdef DDB_Sys_Crit_Init_Done

OEMJOY_Init_Order              equ UNDEFINED_INIT_ORDER

ENDIF   ; DDB_Sys_Crit_Init_Done (VxD's only)
