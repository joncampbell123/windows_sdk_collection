        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	EGAMEMD.INC
;
; This file contains the public definitions of the EGA memory
; locations which driver output routines need to reference.
;
; Created: 29-Apr-1987
; Author:  Walt Moore [waltm]
;
; Copyright (c) 1984-1987 Microsoft Corporation
;
; Restrictions:
;
;   Enter Ye into the valley of death if you change any of these
;   locations since we're in bed with Win386.
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
	public	shadow_mem_status
	public	known_word
	public	shadowed_mode
	public	saved_latches
	public	plane_index
	public	enable_test



EGAMem		segment at 0

screen_area	db	(SCAN_BYTES*SCREEN_HEIGHT) dup (?)
save_area	db	MASK_LENGTH dup (?)
screen_buf	db	(((MAX_BUF_HEIGHT+1) and 0FFFEh) * BUF_WIDTH) dup (?)


;	The following locations must reside in the last 16 bytes of
;	a 16K block of EGA memory (16K-16 or 32K-16).  Win386 expects
;	to find these locations there, and will save and restore
;	them when sharing the EGA with us.
;
;	The saved latches must be at 3FFF or 7FFF

		  org	(($-screen_area) or 3FFFh - 15)

current_brush	  db	8 dup (?)	;8 bytes for a brush
tonys_bar_n_grill db	?		;1 byte for a solid color
shadow_mem_status db	?		;Staus flags for upper 16/32K of mem
known_word	  dw	?		;Required locations for state
shadowed_mode	  db	?		;  detection code
plane_index	  db	?
enable_test	  db	?
saved_latches	  db	?
		  errnz <(($-screen_area) and 3FFFh)>


EGAMem	      ends
