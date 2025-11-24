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
; TSENGBLT.ASM
;----------------------------------------------------------------------------
        .xlist
DOS5 = 1
        include cmacros.inc
	incLogical = 1
	include	gdidefs.inc
        include macros.inc
        include dibeng.inc
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
        externNP        ScreenToScreen1
        externNP        ScreenToScreen1L
        externNP        PrepareForBlt
        externNP        SetSPointer
        externNP        SetDPointer
        externNP        SetSPointerDec
        externNP        SetDPointerDec
        externNP        ScreenToScreen1L
        externNP        MemoryToScreen1L
        externNP        MemoryToScreen3L
        externNP        PatCopyL
        externNP        PatCopySL
        externNP        DPx_SL

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
        externW wBpp
sEnd    Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin  Code
assumes cs,Code
        .386

PLABEL BS_LetDIBEngineDoIt
        assumes es,Data
        pop     edi
        pop     esi
        lea     sp,[bp-2]
        pop     ds
        pop     bp
        jmp     DIB_BitBlt

;----------------------------------------------------------------------------
; BltSpecial_Tseng
;----------------------------------------------------------------------------
cProc   BltSpecial_Tseng,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
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

        lea     ax,SetTsengBank
        mov     pSetBank,ax
        lea     ax,SetTsengSBank
        mov     pSetSBank,ax
        lea     ax,SetTsengDBank
        mov     pSetDBank,ax
        lea     ax,SaveTsengBank
        mov     pSaveBank,ax
        lea     ax,RestoreTsengBank
        mov     pRestoreBank,ax
        mov     bNextBank,1
        mov     bBankShiftCount,0

        mov     al,bptr Rop+2           ;Get the raster op code.
        cmp     al,ROP_S                ;Yes. Check for special rops.
        jz      BS_S

        cmp     wBpp,16
        je      BS_LetDIBEngineDoIt

        test    fs:[bx].deFlags,PALETTE_XLAT    ;palette xlate active?
        jnz     BS_LetDIBEngineDoIt             ;yes.

        cmp     al,ROP_P
        jz      short BS_P
        cmp     al,ROP_DDx
        jz      short BS_0
        cmp     al,ROP_DDxn
        jz      short BS_1
        cmp     al,ROP_DPx
        jz      BS_DPx
        cmp     al,ROP_Dn
        jz      short BS_Dn
        cmp     al,ROP_Pn
        jz      BS_Pn
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
        jz      short BS_P_NotSolid             ; get brush color and jmp to
        push    BS_Exit
        mov     eax,dword ptr [si].dp8BrushBits ; the SolidColor code.
        mov     bx,offset PatCopySL
        jmp     MemoryToScreen1L

BS_P_NotSolid:
        test    [si].dp8BrushFlags,PATTERNMONO
        jnz     BS_LetDIBEngineDoIt             ;DIB Engine can do this blt.
        push    BS_Exit
        mov     bx,offset PatCopyL
        jmp     MemoryToScreen3L

BS_Dn:
        push    BS_Exit
        mov     eax,-1                  ;we'll xor with FF to invert the bckgnd.
        mov     bx,offset DPx_SL
        jmp     MemoryToScreen1L

BS_DPx:
        lds     si,lpPBrush
	cmp	[si].dp8BrushStyle,BS_HOLLOW
        je      BS_LetDIBEngineDoIt		;Let the DIB Engine handle it.
        test    [si].dp8BrushFlags,COLORSOLID
        jz      short BS_DPx_NotSolid           ; get brush color and jmp to the
        push    BS_Exit
        mov     eax,dword ptr [si].dp8BrushBits ; SolidColor code.
        mov     bx,offset DPx_SL
        jmp     MemoryToScreen1L

BS_DPx_NotSolid:
        jmp     BS_LetDIBEngineDoIt             ;DIB Engine can do this blt.

BS_Pn:
        lds     si,lpPBrush
	cmp	[si].dp8BrushStyle,BS_HOLLOW
        je      BS_LetDIBEngineDoIt		;Let the DIB Engine handle it.
        test    [si].dp8BrushFlags,COLORSOLID
        jz      BS_LetDIBEngineDoIt             ;DIB Engine can do this blt.
        push    BS_Exit
        mov     eax,dword ptr [si].dp8BrushBits ; SolidColor code.
        not     eax
        mov     bx,offset PatCopySL
        jmp     MemoryToScreen1L

BS_S:
        lfs     bx,lpSrcDev
        mov     ax,fs:[bx].deFlags         ;ax = src pdevice flags.
        mov     SrcFlags,ax                ;save it.
        test    ax,VRAM                    ;is this my PDevice?
        jz      BS_LetDIBEngineDoIt        ;No.
        mov     ax,BUSY
        xchg    fs:[bx].deFlags,ax
        mov     DeviceFlags,ax
        push    BS_Exit                    ;will return to here.
        cmp     wBpp,16
        je      ScreenToScreen16
        mov     al,byte ptr SrcxOrg
        mov     ah,byte ptr DestxOrg
        and     ax,0303H
        cmp     al,ah
        je      ScreenToScreen1L
        jmp     ScreenToScreen1

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
; SaveTsengBank
;----------------------------------------------------------------------------
PPROC   SaveTsengBank   near
        push    dx
        push    ax
        mov     dx,3cdh
        in      al,dx
        mov     bCurrentBankSetting,al
        pop     ax
        pop     dx
        ret
SaveTsengBank   endp


;----------------------------------------------------------------------------
; RestoreTsengBank
;----------------------------------------------------------------------------
PPROC   RestoreTsengBank        near
        push    dx
        push    ax
        mov     al,bCurrentBankSetting
        mov     dx,3cdh
        out     dx,al
        pop     ax
        pop     dx
        ret
RestoreTsengBank        endp

;----------------------------------------------------------------------------
; SetTsengBank
;----------------------------------------------------------------------------
PPROC   SetTsengBank    near
        push    ax
        push    dx
        mov     al,dl
        shl     dl,4
        or      al,dl
        mov     dx,3cdh
        out     dx,al
        pop     dx
        pop     ax
        ret
SetTsengBank    endp


;----------------------------------------------------------------------------
; SetTsengSBank
;----------------------------------------------------------------------------
PPROC   SetTsengSBank   near
        push    ax
        push    dx
        mov     ah,dl                   ;Save read bank #
        mov     dx,3cdh
        in      al,dx                   ;get bank states
        and     al,0Fh                  ;mask off read bank bits
        shl     ah,4                    ;adjust desired bank value
        or      al,ah                   ;merge with exiting bank state.
        out     dx,al                   ;send it to the hardware
        pop     dx
        pop     ax
        ret
SetTsengSBank   endp

;----------------------------------------------------------------------------
; SetTsengDBank
;----------------------------------------------------------------------------
PPROC   SetTsengDBank   near
        push    ax
        push    dx
        mov     ah,dl                   ;Save desired write bank
        mov     dx,3cdh
        in      al,dx                   ;get bank states
        and     al,0F0h                 ;mask off write bank bits
        or      al,ah                   ;merge with exiting bank state.
        out     dx,al                   ;send it to the hardware
        pop     dx
        pop     ax
        ret
SetTsengDBank   endp

;----------------------------------------------------------------------------
; ScreenToScreen16 -- for 16 bpp modes
; Entry:
; Assumes: fs = Data
;----------------------------------------------------------------------------
PPROC ScreenToScreen16	near
        assumes ds,nothing
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing
	call	PrepareForBlt
        jc      STS16_Exit
	call	pSaveBank
        cmp     bDirection,INCREASING
        jne     STS16_Dec

;----------------------------------------------------------------------------
; Blt Downward.
;----------------------------------------------------------------------------
PLABEL STS16_Inc
	cld
	call	SetSPointer
	movzx	eax,SrcxOrg
        add     esi,eax
	call	SetDPointer
	movzx	eax,DestxOrg
        add     edi,eax
	mov	edx,esi
	rol	edx,16
	call	pSetSBank
	mov	edx,edi
	rol	edx,16
	call	pSetDBank
        mov     bx,xExt
	add	bx,bx			;bx = xExt*2
        sub     DestDeltaScan,bx
        sub     SrcDeltaScan,bx

PLABEL STS16_IncCopyLoop
        mov     cx,bx			;cx = xExt*2
	movzx	eax,di			;ax = dest ptr
	add	ax,bx			;ax = dest ptr + xExt*2
	rcr	eax,1			;Carry set if dest scan crosses 64k.
	mov	ax,si			;ax = src ptr	
	add	ax,bx			;ax = src ptr + xExt*2
	rcr	eax,1			;Carry set if dest scan crosses 64k.
	shr	eax,29			;prepare jump index.
	shr	cx,1			;cx = xExt
	jmp	word ptr STS16_DevCopyTableInc[eax];jump to copy routine.

PLABEL STS16_IncCopyLoopBottom
	add	di,DestDeltaScan
	jc	short STS16_NextDestBank1

PLABEL STS16_DestBankSelected1
	add	si,SrcDeltaScan
	jc	short STS16_NextSrcBank1

PLABEL	STS16_SrcBankSelected1
	dec	yExt
	jnz	STS16_IncCopyLoop
	call	pRestoreBank

PLABEL STS16_Exit
	ret

PLABEL STS16_NextDestBank1
	rol	edi,16
	inc	di
	mov	dx,di
	call	pSetDBank
	rol	edi,16
        jmp     STS16_DestBankSelected1

PLABEL STS16_NextSrcBank1
	rol	esi,16
	inc	si
	mov	dx,si
	call	pSetSBank
	rol	esi,16
        jmp     STS16_SrcBankSelected1

;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
PLABEL STS16_DevCopyTableInc
;             sd crossing 64k boundary.
	dw	STS16_00Inc
	dw	STS16_01Inc
	dw	STS16_10Inc
	dw	STS16_11Inc

;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
PLABEL STS16_00Inc
	rep	movsw			;copy the scan.
	jmp	STS16_IncCopyLoopBottom

;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
PLABEL STS16_01Inc
@@:	lodsw				;get a pel
	mov	es:[di],ax		;store it.
	add	di,2			;advance di.
	jc	short @f		;if carry, we need to do a bank switch.
	dec	cx
	jnz	@b
	jmp	STS16_IncCopyLoopBottom
@@:	rol	edi,16
	inc	di
	mov	dx,di
	call	pSetDBank
	rol	edi,16
	dec	cx
	jnz	STS16_01Inc

	jmp	STS16_IncCopyLoopBottom

;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
PLABEL STS16_10Inc
@@:	mov	ax,[si]			;get a pel.
	stosw				;store it.
	add	si,2			;advance si.
	jc	short @f		;if carry, we need to do a bank switch.
	dec	cx
	jnz	@b
	jmp	STS16_IncCopyLoopBottom	
@@:	rol	esi,16
	inc	si
	mov	dx,si
	call	pSetSBank
	rol	esi,16
	dec	cx
	jnz	STS16_10Inc
	jmp	STS16_IncCopyLoopBottom

;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
PLABEL STS16_11Inc
@@:	mov	ax,[si]
	mov	es:[di],ax		;store it.
	add	si,2
	jc	short STS16_11IncSi	;if carry, we need to do a bank switch.

PLABEL STS16_11IncStore
	add	di,2			;advance di.
	jc	short STS16_11IncDi	;if carry, we need to do a bank switch.
	dec	cx
	jnz	STS16_11Inc
	jmp	STS16_IncCopyLoopBottom

PLABEL STS16_11IncSi
	rol	esi,16
	inc	si
	mov	dx,si
	call	pSetSBank
	rol	esi,16
	jmp	STS16_11IncStore

PLABEL STS16_11IncDi
	rol	edi,16
	inc	di
	mov	dx,di
	call	pSetDBank
	rol	edi,16
	dec	cx
	jnz	STS16_11Inc
	jmp	STS16_IncCopyLoopBottom

;----------------------------------------------------------------------------
; Blt Upward.
;----------------------------------------------------------------------------
PLABEL STS16_Dec
	std
	call	SetSPointerDec
	movzx	eax,SrcxOrg
	add	ax,xExt
	dec	ax
        add     esi,eax
	call	SetDPointerDec
	movzx	eax,DestxOrg
	add	ax,xExt
	dec	ax
        add     edi,eax
	mov	edx,esi
	rol	edx,16
	call	pSetSBank
	mov	edx,edi
	rol	edx,16
	call	pSetDBank
        mov     bx,xExt
	add	bx,bx			;bx = xExt*2
        sub     DestDeltaScan,bx
        sub     SrcDeltaScan,bx

PLABEL STS16_DecCopyLoop
        mov     cx,bx			;cx = xExt*2
	movzx	eax,di			;ax = dest ptr
	sub	ax,bx			;ax = dest ptr - xExt*2
	rcr	eax,1			;Carry set if dest scan crosses 64k.
	mov	ax,si			;ax = src ptr	
	sub	ax,bx			;ax = src ptr - xExt*2
	rcr	eax,1			;Carry set if dest scan crosses 64k.
	shr	eax,29			;prepare jump index.
	shr	cx,1			;cx = xExt
	jmp	word ptr STS16_DevCopyTableDec[eax];jump to copy routine.

PLABEL STS16_DecCopyLoopBottom
	sub	di,DestDeltaScan
	jc	short STS16_NextDestBank2

PLABEL STS16_DestBankSelected2
	sub	si,SrcDeltaScan
	jc	short STS16_NextSrcBank2

PLABEL	STS16_SrcBankSelected2
	dec	yExt
	jnz	STS16_DecCopyLoop
	call	pRestoreBank
	jmp     STS16_Exit

PLABEL STS16_NextDestBank2
	rol	edi,16
	dec	di
	mov	dx,di
	call	pSetDBank
	rol	edi,16
        jmp     STS16_DestBankSelected2

PLABEL STS16_NextSrcBank2
	rol	esi,16
	dec	si
	mov	dx,si
	call	pSetSBank
	rol	esi,16
        jmp     STS16_SrcBankSelected2

;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
PLABEL STS16_DevCopyTableDec
;             sd crossing 64k boundary.
	dw	STS16_00Dec
	dw	STS16_01Dec
	dw	STS16_10Dec
	dw	STS16_11Dec

;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
PLABEL STS16_00Dec
	rep	movsw			;copy the scan.
	jmp	STS16_DecCopyLoopBottom

;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
PLABEL STS16_01Dec
@@:	lodsw				;get a pel
	mov	es:[di],ax		;store it.
	sub	di,2			;advance di.
	jc	short @f		;if carry, we need to do a bank switch.
	dec	cx
	jnz	@b
	jmp	STS16_DecCopyLoopBottom
@@:	rol	edi,16
	dec	di
	mov	dx,di
	call	pSetDBank
	rol	edi,16
	dec	cx
	jnz	STS16_01Dec
	jmp	STS16_DecCopyLoopBottom

;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
PLABEL STS16_10Dec
@@:	mov	ax,[si]			;get a pel.
	stosw				;store it.
	sub	si,2			;advance si.
	jc	short @f		;if carry, we need to do a bank switch.
	dec	cx
	jnz	@b
	jmp	STS16_DecCopyLoopBottom	
@@:	rol	esi,16
	dec	si
	mov	dx,si
	call	pSetSBank
	rol	esi,16
	dec	cx
	jnz	STS16_10Dec
	jmp	STS16_DecCopyLoopBottom

;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
PLABEL STS16_11Dec
@@:	mov	ax,[si]
	mov	es:[di],ax		;store it.
	sub	si,2
	jc	short STS16_11DecSi	;if carry, we need to do a bank switch.

PLABEL STS16_11DecStore
	sub	di,2			;advance di.
	jc	short STS16_11DecDi	;if carry, we need to do a bank switch.
	dec	cx
	jnz	STS16_11Dec
	jmp	STS16_DecCopyLoopBottom

PLABEL STS16_11DecSi
	rol	esi,16
	dec	si
	mov	dx,si
	call	pSetSBank
	rol	esi,16
	jmp	STS16_11DecStore

PLABEL STS16_11DecDi
	rol	edi,16
	dec	di
	mov	dx,di
	call	pSetDBank
	rol	edi,16
	dec	cx
	jnz	STS16_11Dec
	jmp	STS16_DecCopyLoopBottom

ScreenToScreen16	endp

sEnd    TsengSeg
end
