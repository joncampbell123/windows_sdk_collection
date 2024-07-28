        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	EGAMEM.INC
;
; This file contains the external definitions of the EGA memory
; locations which driver output routines need to reference.
;
; Created: 06-Jan-1987
; Author:  Walt Moore [waltm]
;
; Copyright (c) 1987 Microsoft Corporation
;
; Restrictions:
;
;   Enter Ye into the valley of death if you change any of these
;   locations since we're in bed with Win386.
;
;-----------------------------------------------------------------------;


;       EGAMem defines offsets of locations in the EGA's address
;       space used both to determine and save the state of the EGA.
;
;	The actual address within the EGA's Regen RAM is determined
;	at link time, and is based on the number of vertical scans
;	and cursor shape work areas.




EGAMem	segment at 0


;	The following locations are valid only for the duration of the
;	current function.  Their contents cannot be guaranteed after
;	exiting from the routine which sets them up.

	externB current_brush		;One 8x8 brush may be stored here.
	externB tonys_bar_n_grill	;For output routine use.



;	shadow_mem_status contains flags controlling access to unused
;	video memory which will be used for saving the memory underneath
;	menus and dialogue boxes.  Win386 knows about these locations
;	in EGA memory, and will enable/disable the use of the memory
;	as needed.  Win386 will set the SHADOW_TRASHED bit whenever
;	it takes the memory away from us.
;
;	For the EGAHires driver, the unused memory will reside at
;	A000:8000-A000:FFFF.  For the EGALores driver, it will be
;	at A000:4000-A000:7FFF.  If memory exists in the proper
;	range, then SHADOW_EXISTS will be set.



	externB shadow_mem_status	;Staus flags for unused video ram

SHADOW_EXISTS	equ	00000001b	;Shadow memory exists
SHADOW_IN_USE	equ	00000010b	;Shadow contains valid image
SHADOW_TRASHED	equ	00000100b	;Shadow contents have been destroyed


;	shadowed_mode must always match the contents of the mode register.
;	Whenever a cursor shape is drawn, the mode register is restored
;	from here.

	externB shadowed_mode		;Mode register shadow


;	The following locations are used by the state detection code.
;	They are initialized whenever the driver is enabled, and
;	maintained thereafter by the state detection code.  No one is
;	to alter these locations but the SD code.


	externW known_word		;Known value in this word
	externB enable_test		;Used for Data Rotate test
                                        ;     and Bit Mask test
					;     and Enable Set/Reset test
	externB plane_index		;Plane index goes here
	externB saved_latches		;Saved processor latches

EGAMem	ends
