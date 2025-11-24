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

FontSeg		segment at 0
fsVersion		dw	?
fsSize			dd	?
fsCopyright		db	60 dup (?)
fsType			dw	?	; Type field for the font
fsPoints		dw	?	; Point size of font
fsVertRes		dw	?	; Vertical digitization
fsHorizRes		dw	?	; Horizontal digitization
fsAscent		dw	?	; Baseline offset from char cell top
fsInternalLeading	dw	?	; Internal leading included in font
fsExternalLeading	dw	?	; Prefered extra space between lines
fsItalic		db	?	; Flag specifying if italic
fsUnderline		db	?	; Flag specifying if underlined
fsStrikeOut		db	?	; Flag specifying if struck out
fsWeight		dw	?	; Weight of font
fsCharSet		db	?	; Character set of font
fsPixWidth		dw	?	; Width field for the font
fsPixHeight		dw	?	; Height field for the font
fsPitchAndFamily	db	?	; Flag specifying pitch and family
fsAvgWidth		dw	?	; Average character width
fsMaxWidth		dw	?	; Maximum character width
fsFirstChar		db	?	; First character in the font
fsLastChar		db	?	; Last character in the font
fsDefaultChar		db	?	; Default character for out of range
fsBreakChar		db	?	; Character to define wordbreaks
fsWidthBytes		dw	?	; Number of bytes in each row
fsDevice		dd	?	; Offset to device name
fsFace			dd	?	; Offset to face name
fsBitsPointer		dd	?	; Bits pointer
fsBitsOffset		dd	?	; Offset to the begining of the bitmap
			even		; Word alignment for the offset table
fsFlags 		dd	?	; Bit Flags
fsAspace		dw	?	; Global A space, if any
fsBspace		dw	?	; Global B space, if any
fsCspace		dw	?	; Global C space, if any
fsColorPointer		dd	?	; offset to color table, if any
fsReserved		dd	4 dup(?)
fsCharOffset		db	?	; Area for storing the character offsets
FontSeg		ends
