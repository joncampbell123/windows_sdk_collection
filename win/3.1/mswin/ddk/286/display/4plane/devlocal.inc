;----------------------------Module-Header------------------------------;
; Module Name: DEVLOCAL.BLT
;
; Local variables for BitBLT used by device-specific code.
;
; Created: In Windows' distant past (c. 1983)
;
; Copyright (c) 1983 - 1987  Microsoft Corporation
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
