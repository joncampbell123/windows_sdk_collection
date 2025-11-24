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

;----------------------------------------------------------------------------
; BITBLT.ASM
;----------------------------------------------------------------------------
        .xlist
        include cmacros.inc
        include dibeng.inc
        .list

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externFP	BltSpecial_XGA
	externFP	DIB_BitBlt

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin	Code
	assumes cs,Code
	.386

;------------------------------------------------------------------------
; BB_JumpToDibEngineX - jump to DIBENG unless screen<->screen blt
; BB_JumpToDibEngine  - jump to DIBENG
;------------------------------------------------------------------------

BB_JumpToDibEngineX:
        mov     bx,sp
        mov     eax,ss:[bx][32]         ;edx = lpDestDev
        cmp     eax,ss:[bx][24]         ;does lpSrcDev==lpDestDev?
        je      BitBltHW                ;yes, let us handle screen<==>screen
BB_JumpToDibEngine:
        jmp     DIB_BitBlt              ;Let DIB Engine handle it

;------------------------------------------------------------------------
; BitBlt entry point, check for special cases and either
; jmp to DIB_BitBlt or BitBltHW
;------------------------------------------------------------------------

cProc   BitBlt,<FAR,PUBLIC,WIN,PASCAL>
	parmD	lpDestDev               ;--> to destination bitmap descriptor
	parmW	DestxOrg                ;Destination origin - x coordinate
	parmW	DestyOrg                ;Destination origin - y coordinate
	parmD	lpSrcDev                ;--> to source bitmap descriptor
	parmW	SrcxOrg                 ;Source origin - x coordinate
	parmW	SrcyOrg                 ;Source origin - y coordinate
	parmW	xExt                    ;x extent of the BLT
	parmW	yExt                    ;y extent of the BLT
	parmD	Rop                     ;Raster operation descriptor
	parmD	lpPBrush                ;--> to a physical brush (pattern)
	parmD	lpDrawMode              ;--> to a drawmode
cBegin <nogen>
        mov     bx,sp                   ;ss:[bx]-->stack
	lfs	bx,ss:[bx][32]		;fs:bx-->lpDestDev
	mov	ax,fs:[bx].deFlags	;need many times
	test	ax,VRAM			;is destination screen ?
        jz      BB_JumpToDibEngine      ;no. Don't draw. Let DIB Engine handle it
        test    ax,BUSY                 ;Is the screen busy
        jnz     BB_JumpToDibEngine      ;yes, let DIB Engine handle it
	test	ax,PALETTE_XLAT		; do we need background palette translation ?
        jnz     BB_JumpToDibEngineX     ;yes, let DIB Engine handle it (mabey)
        errn$   BitBltHW                ;no, let us give it a try
cEnd   <nogen>

;----------------------------------------------------------------------
; Destination is video memory. Our blter can handle all cases of 8 & 16BPP.
; We would reject all cases of 4BPP, and DSP Rops of 24BPP.
;----------------------------------------------------------------------
cProc   BitBltHW,<FAR,PUBLIC,WIN,PASCAL>
        parmD   lpDestDev               ;--> to destination bitmap descriptor
        parmW   DestxOrg                ;Destination origin - x coordinate
        parmW   DestyOrg                ;Destination origin - y coordinate
        parmD   lpSrcDev                ;--> to source bitmap descriptor
        parmW   SrcxOrg                 ;Source origin - x coordinate
        parmW   SrcyOrg                 ;Source origin - y coordinate
        parmW   xExt                    ;x extent of the BLT
        parmW   yExt                    ;y extent of the BLT
        parmD   Rop                     ;Raster operation descriptor
        parmD   lpPBrush                ;--> to a physical brush (pattern)
        parmD   lpDrawMode              ;--> to a drawmode
cBegin <nogen>
	jmp	BltSpecial_XGA
cEnd   <nogen>
sEnd	Code
end
