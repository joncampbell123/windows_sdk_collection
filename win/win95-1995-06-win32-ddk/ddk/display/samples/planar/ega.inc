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
; Module Name:	EGA.INC
;
; This file contains the definitions of the EGA registers used by
; the driver.
;
; Exported Functions:	none
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
; Restrictions:
;
;	The following rules apply to the use of the EGA registers
;	for the drivers (excluding pointer code and state detection
;	code):
;
;
;	1)  Routines are allowed to alter the following registers:
;
;	    Sequencer:
;
;		SEQ_DATA		SEQ_MAP_MASK
;
;	    Graphics Controller:
;
;		GRAF_ADDR
;		GRAF_DATA		GRAF_DATA_ROT
;		GRAF_SET_RESET		GRAF_READ_MAP
;		GRAF_ENAB_SR		GRAF_MODE
;		GRAF_COL_COMP		GRAF_BIT_MASK
;		GRAF_CDC
;
;
;	2)  The following registers will always contain the indicated
;	    value.  Routines are not allowed to alter them:
;
;	    Sequencer:
;
;		SEQ_ADDR	set to point SEQ_MAP_MASK
;
;
;	3)  Rotation counts for GRAF_DATA_ROT are not allowed.
;
;
;	4)  The following EGA registers will contain the indicated
;	    value upon entry to a routine.  Any routine which alters
;	    them must restore them to the indicated state.
;
;		Map Mask	    all planes enabled
;		Enable Set/Reset    all planes disabled
;		Data Rotate	    Set, no rotation
;		Mode		    Processor write, data read
;		Bit Mask	    all bits enabled
;
;
;	5)  The Mode Register is always shadowed in EGA memory for
;	    the state detection code.  At interrupt time, the mode
;	    will be restored from the location shadowed_mode. Any
;	    routine altering the Mode Register must keep this
;	    location current.  Upon exit from the routine, this
;	    location must be set to the default along with the
;	    contents of the Mode register.
;
;
;	6)  Pointer drawing will save and restore the contents of
;	    the processor latches.
;
;
;	7)  Pointer drawing will leave GRAF_ADDR set to GRAF_BIT_MASK.
;
;
;   Enter Ye into the valley of death if you violate any of these
;   rules.  WIN386 and this driver are in bed with each other.
;
;-----------------------------------------------------------------------;


;	Miscellaneous Registers used only at EGA initialization time

MISC_OUTPUT	equ	0C2h		;Miscellaneous Output Register
CRTC_ADDR	equ	0D4h		;CRTC Address Register for color mode
CRTC_DATA	equ	0D5h		;CRTC Data    Register for color mode
GRAF_1_POS	equ	0CCh		;Graphics 1 Address Register
GRAF_2_POS	equ	0CAh		;Graphics 2 Address Register
ATTR_READ	equ	0DAh		;Attribute Controler Read  Address
ATTR_WRITE	equ	0C0h		;Attribute Controler Write Address
IN_STAT_0	equ	0C2h		;Input Status Register 0
IN_STAT_1	equ	0DAh		;Input Status Register 1



;	EGA Register Definitions.
;
;	The following definitions are the EGA registers and values
;	used by this driver.  All other registers are set up when
;	the EGA is placed into graphics mode and never altered
;	afterwards.
;
;	All unspecified bits in the following registers must be 0.


EGA_BASE	equ	300h		;Base address of the EGA (3xx)



;	Sequencer Registers Used

SEQ_ADDR	equ	0C4h		;Sequencer Address Register
SEQ_DATA	equ	0C5h		;Sequencer Data    Register

SEQ_MAP_MASK	equ	02h		;Write Plane Enable Mask
MM_C0		equ	00000001b	;  C0 plane enable
MM_C1		equ	00000010b	;  C1 plane enable
MM_C2		equ	00000100b	;  C2 plane enable
MM_C3		equ	00001000b	;  C3 plane enable
MM_ALL		equ	00001111b	;  All planes



;	Graphics Controller Registers Used

GRAF_ADDR	equ	0CEh		;Graphics Controller Address Register
GRAF_DATA	equ	0CFh		;Graphics Controller Data    Register

GRAF_SET_RESET	equ	00h		;  Set/Reset Plane Color
GRAF_ENAB_SR	equ	01h		;  Set/Reset Enable
GRAF_COL_COMP	equ	02h		;  Color Compare Register

GRAF_DATA_ROT	equ	03h		;  Data Rotate Register
DR_ROT_CNT	equ	00000111b	;    Data Rotate Count
DR_SET		equ	00000000b	;    Data Unmodified
DR_AND		equ	00001000b	;    Data ANDed with latches
DR_OR		equ	00010000b	;    Data ORed	with latches
DR_XOR		equ	00011000b	;    Data XORed with latches

GRAF_READ_MAP	equ	04h		;  Read Map Select Register
RM_C0		equ	00000000b	;    Read C0 plane
RM_C1		equ	00000001b	;    Read C1 plane
RM_C2		equ	00000010b	;    Read C2 plane
RM_C3		equ	00000011b	;    Read C3 plane

GRAF_MODE	equ	05h		;  Mode Register
M_PROC_WRITE	equ	00000000b	;    Write processor data rotated
M_LATCH_WRITE	equ	00000001b	;    Write latched data
M_COLOR_WRITE	equ	00000010b	;    Write processor data as color
M_AND_WRITE	equ	00000011b	;    Write (procdata AND bitmask)
M_DATA_READ	equ	00000000b	;    Read selected plane
M_COLOR_READ	equ	00001000b	;    Read color compare

GRAF_CDC	equ	07h		;  Color Don't Care Register
GRAF_BIT_MASK	equ	08h		;  Bit Mask Register
