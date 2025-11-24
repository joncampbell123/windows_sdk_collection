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

        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	EGAMEMD.INC
;
; This file contains the public definitions of the EGA memory
; locations which driver output routines need to reference.
;
; Restrictions:
;
;   This file is to be included by the cursor code only.
;
;-----------------------------------------------------------------------;


;	The cursor save area and screen buffer will be allocated in
;	unused screen RAM.  This will allow the EGA's special hardware
;	to be used for moving all planes of a byte with a single movsb
;	instruction.  It will also allow the EGA's drawing modes to
;	be used for writing to all planes with a single mov instruction.
;	Besides, it also saves a lot of space!
;
;	Special EGA locations will be made public by this routine since
;	it knows exactly what must be where better than anybody else.


	public	current_brush
	public	tonys_bar_n_grill
IF MASMFLAGS and EGA
	public	shadow_mem_status
	public	known_word
ELSE
	public	VDD_latch_byte
	public	wm3_detection
	public	VDD_latch_byte
ENDIF
	public	shadowed_mode
	public	saved_latches
	public	plane_index
	public	enable_test



EGAMem		segment at 0

screen_area	db	(SCAN_BYTES*SCREEN_HEIGHT) dup (?)
save_area_0	db	MASK_LENGTH dup (?)
save_area_1	db	MASK_LENGTH dup (?)
save_area_2	db	MASK_LENGTH dup (?)
save_area_3	db	MASK_LENGTH dup (?)
save_area_4	db	MASK_LENGTH dup (?)
save_area_5	db	MASK_LENGTH dup (?)
save_area_6	db	MASK_LENGTH dup (?)
save_area_7	db	MASK_LENGTH dup (?)


;	The following locations must reside in the last 16 bytes of
;	a 16K block of EGA memory (16K-16 or 32K-16).  Win386 expects
;	to find these locations there, and will save and restore
;	them when sharing the EGA with us.
;
;	The saved latches must be at 3FFF or 7FFF

if MASMFLAGS and EGA
if (SCREEN_HEIGHT-450)
		  org	(7FFFh - 15)
else
		  org	(3FFFh - 15)
endif
endif
current_brush	  db	8 dup (?)	;8 bytes for a brush
tonys_bar_n_grill db	?		;1 byte for a solid color
if MASMFLAGS and EGA
shadow_mem_status db	?		;Staus flags for upper 16/32K of mem
known_word	  dw	?		;Required locations for state
else
		  db	?		; pad byte (place holder for shadow_mem_status)
VDD_latch_byte	  db	?		; reserved for Windows 3.1 VDDVGA
wm3_detection	  db	?		; ds.asm: detect write mode 3 compatibility
endif
shadowed_mode	  db	?		;  detection code
plane_index	  db	?
enable_test	  db	?
saved_latches	  db	?

if MASMFLAGS and EGA
if SCREEN_HEIGHT-450
		  errnz <(($-screen_area) and 3FFFh)>
else
		  errnz <(($-screen_area) and 0FFFh)>
endif
.errnz $ - shadow_mem_status - 7 	; VDD assumes this relationship
endif

EGAMem	      ends
