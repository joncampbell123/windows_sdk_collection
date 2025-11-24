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
; Module Name: CLRLOCAL.BLT
;
; Local variables for BitBLT used by color or monochrome-specific code.
;
; The prefix "cl_" stands for "color-related local". Other prefixes are
; "gl_" for "generic local" and "dl_" for "device-related locals".
;
; This file is part of a set that makes up the Windows BitBLT function
; at driver-level.
;-----------------------------------------------------------------------;
	.list

	localW	cl_both_colors		;Foreground and Background colors
	localB	cl_brush_accel		;Brush accelerator
	localV	cl_a_brush,SIZE_PATTERN	;Munged color ==> mono brush,

;	cl_a_brush is overloaded.  If going color ==> mono, it will
;	contain a brush processed against the given background
;	and foreground colors.	If going mono ==> color, it will
;	contain the AND and XOR masks for converting each plane.
;
;	2*NumberPlanes <= SIZE_PATTERN must hold true to overload
;	the cl_a_brush like this.

	errnz	SIZE_PATTERN-8		;Must be at least 6 bytes worth
