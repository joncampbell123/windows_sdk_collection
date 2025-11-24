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
; PALETTE.ASM
;   This file contains palette specific functions.  These are required if
;   the driver is palette capable.
;----------------------------------------------------------------------------
        .xlist
DOS5 = 1
        include cmacros.inc
        include macros.inc
        include dibeng.inc
        .list
;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
        externFP DIB_SetPaletteExt

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
        externD lpColorTable
        externW wBpp
        externD lpDriverPDevice
sEnd    Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin  Code
        assumes cs,Code
        .386

;---------------------------------------------------------------------------
; SetRAMDAC
;   Initialize the vga's DACs to the values stored in a color table
;   composed of triples.
; Entry:
;       ax  - index to 1st palette entry
;       cx  - count of indices to program
;    ds:si --> array of RGBs
;---------------------------------------------------------------------------
PPROC   SetRAMDAC near
        push    esi
        mov     dx,3c8h                 ;Color palette write mode index reg.
        out     dx,al
	movzx	ebx,ax
	movzx	esi,si
        mov     dx,3c9h                 ;Color palette data reg.
PLABEL SR_DACLoop
	mov	al,[esi+ebx*4+2]	;Red
	shr	al,2			;map from 0-255 to 0-63
	out	dx,al

	mov	al,[esi+ebx*4+1]	;Green
	shr	al,2			;map from 0-255 to 0-63
	out	dx,al

	mov	al,[esi+ebx*4]		;Blue
	shr	al,2			;map from 0-255 to 0-63
	out	dx,al
	inc	ebx
	dec	cx
        jnz     SR_DACLoop
        pop     esi
        ret

SetRAMDAC  endp

PPROC   SetRAMDAC_far	far
	call	SetRAMDAC
	ret
SetRAMDAC_far	endp

;---------------------------------------------------------------------------
; SetPalette
;  Set the hardware palette from GDI.  This palette is also copied into
;  the screen DIB Color table.
; Entry:
;       start       - first palette index to set
;       count       - count of palette indices
;       lpPalette   -->array of RGBQUADs containing colors
;---------------------------------------------------------------------------
cProc   SetPalette,<FAR,PUBLIC,PASCAL,NODATA>,<si,di>
        parmW   wStartIndex             ;starting index in the Palette
        parmW   wNumEntries             ;no of indexes in the Palette
        parmD   lpPalette               ;long pointer to color Palette
cBegin
        mov     ax,DGROUP
        mov     ds,ax
        assumes ds,Data
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing
	push	wStartIndex		;Let the DIB Engine move the
	push	wNumEntries		; palette into the color table.
	push	lpPalette
	push	lpDriverPDevice
	call	DIB_SetPaletteExt
	les	si,lpDriverPDevice
	test	es:[si].deFlags,BUSY	;Is the screen busy?
	jnz	SP_Exit			;Yes, can't touch the hardware.
	lds	si,lpColorTable		;ds:si-->device color table
	mov	ax,wStartIndex
	mov	cx,wNumEntries
	call	SetRAMDAC		;Set new colors into the h/w palette.

PLABEL SP_Exit
        xor     ax,ax                   ;show success
        cwd
cEnd
sEnd  
end
