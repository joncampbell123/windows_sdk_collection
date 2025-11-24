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
; Module Name: GENLOCAL.BLT
;
; Parameters and generic local variables for BitBLT.
;
; This file contains the parameters passed to BitBLT, as well as local
; variables that all of its code expects.
;
; The prefix "gl_" stands for "generic local". Other prefixes are "cl_"
; for "color-related local" and "dl_" for "device-related local".
;
; This file is part of a set that makes up the Windows BitBLT function
; at driver-level.
;-----------------------------------------------------------------------;

	parmD	lpDestDev		;--> to destination bitmap descriptor
	parmW	DestxOrg		;Destination origin - x coordinate
	parmW	DestyOrg		;Destination origin - y coordinate
	parmD	lpSrcDev		;--> to source bitmap descriptor
	parmW	SrcxOrg 		;Source origin - x coordinate
	parmW	SrcyOrg 		;Source origin - y coordinate
	parmW	xExt			;x extent of the BLT
	parmW	yExt			;y extent of the BLT
	parmD	Rop			;Raster operation descriptor
	parmD	lpPBrush		;--> to a physical brush (pattern)
	parmD	lpDrawMode		;--> to a drawmode



;	Generic locals (i.e. used by all BitBLT code)

ifdef	THIS_IS_DOS_3_STUFF
else
	localW	ScreenSelector
endif
	localB	gl_phase_h		;Horizontal phase (rotate count)
	localB	gl_pat_row		;Current row for patterns [0..7]
	localB	gl_direction		;Increment/decrement flag

	localB	local_enable_flag	;Local copy of the enable flag
;	localB	mask_flags		;Number of first fetches needed
;
;MF_PHASE		equ	00000111b
;MF_PHASE_POSITIVE	equ	00001000b
;MF_STEP_RIGHT		equ	00010000b
;MF_WORD_FETCH		equ	00100000b
;MF_BIG_PHASE		equ	01000000b ; phase > 4
;MF_UN_USED		equ	10000000b

	localB	gl_the_flags
	localB	gl_first_fetch		;Number of first fetches needed
	localB	gl_step_direction	;Direction of move (left right)
	localW	gl_start_mask		;Mask for first dest byte
	localW	gl_last_mask		;Mask for last	dest byte
	localW	gl_mask_p		;Horizontal phase mask
	localW	gl_inner_loop_count	;# of entire bytes to BLT in innerloop
	localW	gl_operands		;Operand string
	localW	gl_start_fl 		;Start of fetch/logic operation
	localW	gl_end_fl		;End   of fetch/logic operation
	localW	gl_end_fls		;End   of fetch/logic/store operation
	localD	gl_blt_addr 		;BLT offset address
	localW	WorkSelector		; selector to use
	localW	cFetchCode		;size of the fetch code alone

	localV	gl_src,%(SIZE DEV) 	;Source device data
	localV	gl_dest,%(SIZE DEV)	;Destination device data
