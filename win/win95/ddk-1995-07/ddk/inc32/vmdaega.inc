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

;******************************************************************************
;
;   Title:	VMDAEGA.INC
;
;   Version:	1.00
;
;==============================================================================



;VDD virtual mode services for the grabber
GRB_Get_Version EQU 0
GRB_Get_Mem	EQU GRB_Get_Version + 1
GRB_Get_State	EQU GRB_Get_Mem + 1
GRB_Get_Mod	EQU GRB_Get_State + 1
GRB_Clear_Mod	EQU GRB_Get_Mod + 1
GRB_Free_Mem	EQU GRB_Clear_Mod + 1
GRB_Get_GrbMem	EQU GRB_Free_Mem + 1
GRB_Free_Grab	EQU GRB_Get_GrbMem + 1
GRB_Get_GrbState EQU GRB_Free_Grab + 1
GRB_Unlock_APP  EQU GRB_Get_GrbState + 1

Grabber_VerNum	EQU	0300h		; version 3, 0

;*******************
;	Controller state structure
;
VDA_EGA_State	STRUC
    VDA_EGA_Mode    DB	?		; Display mode(0-A)
    VDA_EGA_Colr    DB	?		; Border color
    VDA_EGA_Flags   DB	?		; Flags
    VDA_EGA_Rows    DB	?		; Height of display in lines (text mode)
    VDA_EGA_CurX    DW	?		; Cursor horizontal position
    VDA_EGA_CurY    DW	?		; Cursor vertical position
    VDA_EGA_CurBeg  DW	?		; Cursor start scan line
    VDA_EGA_CurEnd  DW	?		; Cursor end scan line
    VDA_EGA_Pal     DB	16 DUP (?)	; Palettes(6 bits of color mapping)
VDA_EGA_State	ENDS

;*******
;
;   Controller modes
;
;Name...............Value.......Resol'n...color.....Adap....Pages
;----------------------------------------------------------------
VMode40x25BW16	    EQU 0	; 40x25   16	grey   CGA/EGA 8/8
VMode40x25Co16	    EQU 1	; 40x25   16	color  CGA/EGA 8/8
VMode80x25BW16	    EQU 2	; 80x25   16	grey   CGA/EGA 4/8
VMode80x25Co16	    EQU 3	; 80x25   16	color  CGA/EGA 4/8
VModeLoResCo4	    EQU 4	; 320x200 4/8	color  CGA/EGA 1/1
VModeLoResBW4	    EQU 5	; 320x200 4/8	grey   CGA/EGA 1/1
VModeMdResBW2	    EQU 6	; 640x200 2	BW     CGA/EGA 1/1
VModeMono	    EQU 7	; 80x25   4	mono   Mono/EGA 4/8
VModeLoResCo16	    EQU 0Dh	; 320x200 16	color  EGA     2or4or8(mem)
VModeMdResCo16	    EQU 0Eh	; 640x200 16	color  EGA     1or2or4(mem)
VModeHiResBW4M	    EQU 0Fh	; 640x350 4	mono   EGA     8
VModeHiResCo4E	    EQU 10h	; 640x350 4/16	color  EGA     1or2(mem)
;					or16/64(mem)
;
; Controller flags
;
fVDA_V_ScOff	    EQU 0001h		; Screen off
fVDA_V_ScOffBit     EQU 0
fVDA_V_HCurTrk	    EQU 0002h		; Horizontal cursor track - keep
fVDA_V_HCurTrkBit   EQU 1		;   cursor position in window
; GEM Ventura programs DAC 0 as white - set if DAC 0 is non-zero
fVDA_V_InvDAC	    EQU 0004h		
fVDA_V_InvDACBit    EQU 2


BeginDoc
;***************
;   EGA/VGA VDD modifications state structure ::= <flag>,<count>,<list>
;
;   <flag> indicates type, list valid and controller change and cursor change.
;   <count> is count of entries(not bytes or words) in list.
;   <list> is type dependent as follows:
;	Type 0 - list is modified pages bit map, 1 bit per displayed 4k page,
;	    count is page count. Bit map is one byte per 32k linear address
;	    space, where byte 0's bits are <page 0 bit>,<page 1 bit>,
;	    ...,<page 7 bit>. byte 1's bits are <page 8 bit>,<page 9 bit>.
;	    A set bit indicates a change in that page.
;
;	    This type is not used for text modes.
;
;	    Modes D and E display is less than two pages long and so the
;	    page count will be two. Mode 10 page count is 7. Modes
;	    11 and 12 page count is 10.
;
;	Type 1 - modified enclosing rectangles, count is rectangle count,
;	    (less than four) , rectangle is two points: <left>,<top> and
;	    <right>,<bottom>. Each point is relative to left, upper
;	    corner which is 0,0 and each value is stored in one word.
;
;	    This type may be used for all modes.
;
;	    For text modes, modes 0-3, there may be a special, scroll rectangle.
;	    If the high bit of R_Left is set in one of these RECTs, then it is
;	    not a rect but a scroll specification.
;		The structure then becomes: <scroll count>,<scroll flags>,
;		<right column>,<left column>,<bottom line>,<top line>,
;		<fill character>,<fill attribute>
;	    Note that all items are byte values and that the scroll event
;		specification takes the same space as an update rectangle.
;
ModListMax	EQU	64		    ; Max size of VDD_Mod_List field
EndDoc
VDD_Mod_State	STRUC

    VDD_Mod_Flag    DW	?		    ; Modified state flag
    VDD_Mod_Count   DW	?		    ; Count of entries in modification list
    VDD_Mod_List    DB	ModListMax dup(?)   ; The modification list(type dependent)

VDD_Mod_State	ENDS

;
; Masks and bit numbers for VDD_Mod_Flag
;
fVDD_M_Type	EQU 0000000000000111B	; Mask for value field
fVDD_M_TypeBit	EQU 0			; start bit for value field
fVDD_M_Ctlr	EQU 0000000000001000B	; flag indicating controller state chg
fVDD_M_CtlrBit	EQU 3			; bit for controller state change
fVDD_M_VRAM	EQU 0000000000010000B	; flag indicating Video RAM change
fVDD_M_VRAMBit	EQU 4			; bit for Video RAM change
fVDD_M_Curs	EQU 0000000000100000B	; flag indicating Cursor change
fVDD_M_CursBit	EQU 5			; bit for Cursor change
fVDD_M_ScOff	EQU 0000000001000000B	; flag indicating screen is off
fVDD_M_ScOffBit EQU 6			; bit for screen off
fVDD_M_Err	EQU 1000000000000000B	; flag indicating error, can't pass mods
fVDD_M_ErrBit	EQU 15			; bit for error
;
; Values for fVDD_M_TypeMsk field
;
fVDD_M_Type_Page	EQU    0	; Bit field of modified pages
fVDD_M_Type_Rect	EQU    1	; Array of RECT structures

;*******
;
; Structure for accessing RECTs
;
ifndef Rect
    ??$need_rect equ 1
endif

ifdef ??$need_rect
Rect    struc
        rcLeft          dw      ?
        rcTop           dw      ?
        rcRight         dw      ?
        rcBottom        dw      ?
Rect    ends
endif

;********
; If the high bit of R_Left is set in one of these RECTs, then it is
;  not a rect but a scroll event specification.
;  THIS APPLIES TO TEXT MODE ONLY (modes 0-3)!!
;  A scroll event spec looks like this:
;
ScrollRect STRUC
    ScrCnt  DB	?	    ; Count of lines to scroll in scroll region
    ScrFlgs DB	?	    ; Flags
    ScrRgt  DB	?	    ; Right Col of scroll (0 indexed)(ignore if full width bit set)
    ScrLft  DB	?	    ; Left Col of scroll (0 indexed)(ignore if full width bit set)
    ScrBot  DB	?	    ; Bottom line of scroll (0 indexed)
    ScrTop  DB	?	    ; Top line of scroll (0 indexed)
    ScrFch  DB	?	    ; Fill character for blank lines of scroll
    ScrFatt DB	?	    ; Fill attribute for blank lines of scroll
ScrollRect ENDS

; Masks for ScrFlgs
Scr_M_Scroll	    EQU 10000000B   ; High bit of flags indicates scroll
Scr_M_ScrollBit     EQU 7
Scr_M_FullWid	    EQU 00000001B   ; Set if scroll is full width (ignore Rgt Lft)
Scr_M_FullWidBit    EQU 0
Scr_M_Up	    EQU 00000010B   ; Set if scroll is UP, clear if DOWN
Scr_M_UpBit	    EQU 1

IFNDEF MASM6
IF2
    IF (SIZE ScrollRect) NE (SIZE Rect)
	%out ERROR -- ScrollRect is wrong size!!!!!
    ENDIF
ENDIF
ELSE    ; MASM6
    IF (SIZE ScrollRect) NE (SIZE Rect)
	%out ERROR -- ScrollRect is wrong size!!!!!
    ENDIF
ENDIF


;*******************
;	Memory state structure
;
VDA_Mem_State	STRUC
    VDA_Mem_Addr    DD	?		; Address of memory in video mem handle
    VDA_Mem_PgMap   DB	32 DUP (?)	; Page map 1st 32k
    VDA_Mem_Pg2Map  DB	32 DUP (?)	; Page map 2nd 32k
    VDA_Mem_DPagOff DD	?		; 32 bit offset from page start
    VDA_Mem_DispPag DB	?		; Page number of start of display
    VDA_Mem_Resvd   DB	?, ?, ? 	; Reserved
VDA_Mem_State	ENDS

;
; ADDRESSING VIDEO DISPLAY
;
; Start of the display memory = VDA_Mem_Addr + VDA_Mem_DPagOffset +
;	(1000h * VDA_Mem_PgMap[VDA_Mem_DspPag + (8 * <plane>)])
;
;   For modes 0-6, the <plane> is always 0
;
;   Displayed memory within a plane is always contiguous from the start
;	address as computed according to the formula above and the macro below.
;	Addressing the display memory from the computed start is as follows,
;	    where X is 0 based column and Y is 0 based row.
;
;	Mode 0-1: Even byte is char, odd byte is attr, 40 cols, 25 rows
;		   one plane.
;	    ChrByteOff=(X/2)+(Y*80); AttrByteOff=ChrByteOff+1
;
;	Mode 2-3,7: Even byte is char, odd byte is attr, 80 cols, 25 rows,
;		   one plane.
;	    ChrByteOff=(X/2)+(Y*160); AttrByteOff=ChrByteOff+1
;
;	Mode "43": Even byte is char, odd byte is attr, 80 cols,
;		    43 rows on EGA, 52 rows on VGA, one plane.
;	    ChrByteOff=(X/2)+(Y*160); AttrByteOff=ChrByteOff+1
;
;	Mode 4-5: 2 bits per PEL, packed into bytes: bits 7-6 is PEL 0,
;		    bits 5-4 is PEL 1, etc. Where PEL = X MOD 4. Odd rows
;		    offset by 8k from even rows. 320 cols, 200 rows, one plane.
;	    PELByteOff=(X/4)+((Y/2)*80)+((Y MOD 2)*8192)
;
;	Mode 6: 1 bit per PEL, packed into bytes: bit 7 is PEL 0,
;		    bit 6 is PEL 1, bit 5 is PEL 2, etc., where PEL = X MOD 8.
;		    Odd rows offset by 8k from even rows. 640 cols, 200 rows,
;		    one plane.
;	    PELByteOff=(X/8)+((Y/2)*160)+((Y MOD 2)*8192)
;
;	Mode D: 1 bit per PEL in each plane, packed into bytes: bit 7 is PEL 0,
;		    bit 6 is PEL 1, etc., where PEL = X MOD 8. 320 cols,
;		    200 rows, 4 planes. Note that the start of each plane
;		    must be calculated according to the GetStartDispAddr macro.
;	    PELByteOff=(X/8)+(Y*80)
;
;	Mode E: 1 bit per PEL in each plane, packed into bytes: bit 7 is PEL 0,
;		    bit 6 is PEL 1, etc., where PEL = X MOD 8. 640 cols,
;		    200 rows, 4 planes. Note that the start of each plane
;		    must be calculated according to the GetStartDispAddr macro.
;	    PELByteOff=(X/8)+(Y*160)
;
;	Mode 10: 1 bit per PEL in each plane, packed into bytes: bit 7 is PEL 0,
;		    bit 6 is PEL 1, etc., where PEL = X MOD 8. 640 cols,
;		    350 rows, 4 planes. Note that the start of each plane
;		    must be calculated according to the GetStartDispAddr macro.
;	    PELByteOff=(X/8)+(Y*160)
;
;
;	DestReg     = 32 bit register to place video memory address
;	MemStateReg = 32 bit register which -> VDA_Mem_State
;	MemStateSeg = SEGMENT REGISTER for MemStateReg
;	PlaneReg    = 32 bit register holding plane number desired
;
; ALL REGISTERS MUST BE SEPARATE!!!!!
;
;
GetStartDispAddr MACRO	 DestReg,MemStateReg,PlaneReg,MemStateSeg
	movzx	DestReg,MemStateSeg:[MemStateReg.VDA_Mem_DispPag]
	lea	DestReg,[DestReg][PlaneReg*8]
	movzx	DestReg,MemStateSeg:[MemStateReg.VDA_Mem_PgMap][DestReg]
	shl	DestReg,12
	add	DestReg,MemStateSeg:[MemStateReg.VDA_Mem_Addr]
	add	DestReg,MemStateSeg:[MemStateReg.VDA_Mem_DPagOff]
	ENDM
