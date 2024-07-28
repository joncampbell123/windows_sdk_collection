	page	,132
;----------------------------Module-Header------------------------------;
; Module Name: polyline.inc
;
; Brief Description: Continas equates used by Polyline.
;
; Created: 3/30/87
; Author: Kent Settle	(kentse)
;
; Copyright (c) 1983 - 1987  Microsoft Corporation
;-----------------------------------------------------------------------;

SECOND_PASS	equ	00000010b	; Second pass flag.

DEST_IS_DEVICE	equ	00000001b	;Device is the display
DEST_IS_BITMAP	equ	00000000b	;Device is bitmap.
DEST_IS_HUGE	equ	01000000b	;Device is a huge bitmap

;	The following bit indices are combined to index into the previous
;	axis update tables.  The placement of the entries are dependent
;	on these indices.  If one changes, so must the others.

Y_MAJOR 	equ	00000001b	;Y is the major axis
DIAGONAL	equ	00000010b	; stepping along a diagonal
STEP_UP 	equ	00000100b	;The DDA is stepping up
STEP_LEFT	equ	00001000b	;The DDA is stepping right
HUGE_UPDATE	equ	01000000b	;Huge bitmap update

CASE_TABLE_MASK equ	00000011b	; only worry about diagonal + y major.

BITMAP_DO_NOTHING   equ 00h

RASTER_OP_MASK	equ	000Fh
PARTIAL_BYTE_MASK equ	00000111b

