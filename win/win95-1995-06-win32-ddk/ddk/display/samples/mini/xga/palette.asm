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
	include xga.inc
        .list
;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
        externFP DIB_SetPaletteTranslate
        externFP DIB_GetPaletteTranslate
        externFP DIB_UpdateColorsExt
        externFP DIB_SetPaletteExt
        externFP DIB_GetPaletteExt

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
        externD lpColorTable
	externW wBpp
	externW wChipID 			;in VGA.ASM
	externD lpDriverPDevice
	externW CRTCRegisterBase
sEnd    Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin  Code
        assumes cs,Code
        .386

;---------------------------------------------------------------------------
; SetRAMDAC
;   Initialize the XGA's DACs to the values stored in a color table
;   composed of triples.
; Entry:
;       ax  - index to 1st palette entry
;       cx  - count of indices to program
;    ds:si --> array of RGBs
;---------------------------------------------------------------------------
PPROC	SetRAMDAC near
	assumes ds,nothing
	movzx	ebx,ax			;save starting index in EBX
	movzx	esi,si			;make sure top word of ESI is 0
	push	ds			;save DS over this
	mov	ax,_DATA		;make DS --> Data
	mov	ds,ax			;
	assumes ds,Data
	mov	dx,CRTCRegisterBase	;DX --> XGA CRTC index register
	cmp	wChipID,AGX_ID		;running on an IIT AGX?
	pop	ds			;(restore saved DS)
	assumes ds,nothing
	je	SetAGXPalette		;on an AGX, use alternate method
;
;First initialize the XGA's Palette Sequencer Register (index 66H) to
;RGB ordering and starting with the RED index
;
	mov	ax,0066h		;initialize palette sequence reg
	out	dx,ax			;
;
public	SRDLoop
SRDLoop:
;
;First, set the index:
;
	mov	ah,bl			;get index into AH
	mov	al,60h			;this is CRTC index for palette index
	out	dx,ax			;
;
;Now for the RED:
;
	mov	al,65h			;this is CRTC index for palette data
	mov	ah,[esi+(ebx*4)+2]	;get RED value into AH
        out     dx,ax                   ;
;
;Now for the GREEN:
;
	mov	ah,[esi+ebx*4+1]	;get GREEN value into AH
        out     dx,ax                   ;
;
;Lastly, the BLUE:
;
	mov	ah,[esi+ebx*4]		;get BLUE value into AH
        out     dx,ax                   ;
	inc	bx			;bump to next colour index
	dec	cx			;perform a loop maneuver
	jnz	SRDLoop 		;
	jmp	SRDExit 		;
;
public	SetAGXPalette
SetAGXPalette:
;
;The AGX uses the standard VGA DAC.
;
	mov	dx,3c8h 		;DX --> DAC Write Index register
;
public	SRDLoop_AGX
SRDLoop_AGX:
;
;First, set the index:
;
	mov	al,bl			;get index into AL
	out	dx,al			;
	inc	dl			;DX --> DAC Data register
;
;Now for the RED:
;
	mov	al,[esi+(ebx*4)+2]	;get RED value into AL
	shr	al,2			;get it into low 6 bits
	out	dx,al			;
;
;Now for the GREEN:
;
	mov	al,[esi+ebx*4+1]	;get GREEN value into AL
	shr	al,2			;get it into low 6 bits
	out	dx,al			;
;
;Lastly, the BLUE:
;
	mov	al,[esi+ebx*4]		;get BLUE value into AL
	shr	al,2			;get it into low 6 bits
	out	dx,al			;
	dec	dl			;DX --> DAC Write Index register
	inc	bx			;bump to next colour index
	dec	cx			;perform a loop maneuver
	jnz	SRDLoop_AGX		;
;
SRDExit:
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
cProc   SetPalette,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
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
