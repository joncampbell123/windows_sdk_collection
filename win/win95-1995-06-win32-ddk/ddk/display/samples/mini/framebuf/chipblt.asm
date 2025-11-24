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
; CHIPBLT.ASM
;----------------------------------------------------------------------------
        .xlist
DOS5 = 1
        include cmacros.inc
	incLogical = 1
	include	gdidefs.inc
        include macros.inc
        include dibeng.inc
        include device.inc
        .list
;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
ROP_P           equ     11110000b
ROP_Pn          equ     00001111b
ROP_S           equ     11001100b       ;SRCCOPY
ROP_Sn          equ     00110011b
ROP_DDx         equ     00000000b       ;BLACKNESS
ROP_DDxn        equ     11111111b       ;WHITENESS
ROP_Dn          equ     01010101b       ;DSTINVERT
ROP_DPx         equ     01011010b       ;PATINVERT

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
INCREASING      equ     +1
DECREASING      equ     -1

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
        externA         KernelsScreenSel        ;equates to a000:0000
        externFP        DIB_BitBlt
        externFP        DIB_BeginAccess
        externFP        DIB_EndAccess
        externNP        PrepareForBlt
        externNP        SetSPointer
        externNP        SetDPointer
        externNP        MemoryToScreen1L
        externNP        PatCopySL

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
        externW wBpp
        externW wDeviceType
sEnd    Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin  Code
assumes cs,Code
        .386

BS_LetDIBEngineDoIt:
        assumes es,Data
        pop     edi
        pop     esi
        lea     sp,[bp-2]
        pop     ds
        pop     bp
        jmp     DIB_BitBlt

;----------------------------------------------------------------------------
; BltSpecial_Chip
;----------------------------------------------------------------------------
cProc   BltSpecial_Chips,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
        include special.inc             ;include parms and locals
cBegin
        assumes ds,nothing
        assumes es,Data
        assumes fs,nothing
        assumes gs,nothing
        lfs     bx,lpDestDev            ;fs:bx-->dest pdevice
        test    fs:[bx].deFlags,VRAM    ;is destination the screen?
        jz      BS_LetDIBEngineDoIt     ;no.
        test    fs:[bx].deFlags,BUSY    ;Is the screen busy?
        jnz     BS_LetDIBEngineDoIt     ;yes.
        xor     ax,ax
        mov     SrcFlags,ax             ;initialize Src pdevice Flags.

        cmp     wBpp,24
        je      BS_LetDIBEngineDoIt
        cmp     wBpp,16
        je      BS_LetDIBEngineDoIt
	
        lea     ax,SetChipBank452
	cmp	wDeviceType,CHIPS452
	je	short @f
        lea     ax,SetChipBank
@@:     mov     pSetBank,ax
        lea     ax,SaveChipBank
        mov     pSaveBank,ax
        lea     ax,RestoreChipBank
        mov     pRestoreBank,ax
        mov     bNextBank,4
        mov     bBankShiftCount,2

        test    fs:[bx].deFlags,PALETTE_XLAT    ;palette xlate active?
        jnz     BS_LetDIBEngineDoIt             ;yes.

        mov     al,bptr Rop+2           ;Get the raster op code.
        cmp     al,ROP_P
        jz      short BS_P
        cmp     al,ROP_DDx
        jz      short BS_0
        cmp     al,ROP_DDxn
        jz      short BS_1
        jmp     BS_LetDIBEngineDoIt     ;DIB Engine can do this blt.

BS_1:
        push    BS_Exit
        mov     eax,-1                  ;Blt solid white.
        mov     bx,offset PatCopySL
        jmp     MemoryToScreen1L

BS_0:
        push    BS_Exit
        xor     eax,eax                 ;Blt solid black.
        mov     bx,offset PatCopySL
        jmp     MemoryToScreen1L

BS_P:
        lds     si,lpPBrush
	cmp	[si].dp8BrushStyle,BS_HOLLOW
        je      BS_LetDIBEngineDoIt		;Let the DIB Engine handle it.
        test    [si].dp8BrushFlags,COLORSOLID
	jz      BS_LetDIBEngineDoIt	        ;DIB Engine can do this blt.
        push    BS_Exit
        mov     eax,dword ptr [si].dp8BrushBits ; the SolidColor code.
        mov     bx,offset PatCopySL
        jmp     MemoryToScreen1L

BS_Exit:
        lfs     bx,lpDestDev
        mov     ax,DeviceFlags
        xchg    fs:[bx].deFlags,ax
        push    lpDestDev
        push    CURSOREXCLUDE
        call    DIB_EndAccess
        cld
cEnd


;----------------------------------------------------------------------------
; SaveChipBank
;----------------------------------------------------------------------------
PPROC   SaveChipBank   near
        push    dx
        push    ax
        mov     dx,3d6h
	mov	al,10h
	out	dx,al
	in	ax,dx
        mov     bCurrentBankSetting,ah
        pop     ax
        pop     dx
        ret
SaveChipBank   endp


;----------------------------------------------------------------------------
; RestoreChipBank
;----------------------------------------------------------------------------
PPROC   RestoreChipBank        near
        push    dx
        push    ax
	mov	al,10h
        mov     ah,bCurrentBankSetting
        mov     dx,3d6h
        out     dx,ax
        pop     ax
        pop     dx
        ret
RestoreChipBank        endp

;----------------------------------------------------------------------------
; SetChipBank
;----------------------------------------------------------------------------
PPROC   SetChipBank    near
        push    ax
        push    dx
        shl     dl,4
        mov     ah,dl
	mov	al,10h
        mov     dx,3d6h
        out     dx,ax
        pop     dx
        pop     ax
        ret
SetChipBank    endp


;----------------------------------------------------------------------------
; SetChipBank452
;----------------------------------------------------------------------------
PPROC   SetChipBank452    near
        push    ax
        push    dx
        shl     dl,2
        mov     ah,dl
	mov	al,10h
        mov     dx,3d6h
        out     dx,ax
        pop     dx
        pop     ax
        ret
SetChipBank452    endp


sEnd    Code
end
