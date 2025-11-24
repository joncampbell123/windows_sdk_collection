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

;----------------------------Module-Header------------------------------;
; Module Name: DEVLOCAL.BLT
;
; Local variables for BitBLT used by device-specific code.
;
; The prefix "dl_" stands for "device-related local". Other prefixes
; are "cl_" for "color-related local" and "gl_" for "generic local".
;
; This file is part of a set that makes up the Windows BitBLT function
; at driver-level.
;-----------------------------------------------------------------------;

	localB	dl_moore_flags		;More flags

ifndef	FOUR_PLANE
	localW	dl_addr_brush_index	;Address of brush index in code
endif
