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
; XGABLT.ASM
;----------------------------------------------------------------------------
        .xlist
	DOS5 = 1			;so we don't get INC BP in <cBegin>
        include cmacros.inc
        include macros.inc
	include dibeng.inc
	include xga.inc 		;contains coprocessor register offsets
	.list
;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
BS_SOLID        equ     0
BS_HOLLOW       equ     1
BS_HATCHED      equ     2
BS_PATTERN      equ     3
ROP_S           equ     11001100b
ROP_DSon        equ     00010001b
ROP_DSna        equ     00100010b
ROP_Sn          equ     00110011b
ROP_SDna        equ     01000100b
ROP_DSx         equ     01100110b
ROP_DSan        equ     01110111b
ROP_DSa         equ     10001000b
ROP_DSno        equ     10111011b
ROP_SDno        equ     11011101b
ROP_DSo         equ     11101110b
ROP_DSxn        equ     10011001b

ROP_Dn          equ     01010101b
ROP_DDx         equ     00000000b
ROP_DDxn        equ     11111111b

ROP_P           equ     11110000b
ROP_Pn          equ     00001111b
ROP_DPo         equ     11111010b
ROP_PDno        equ     11110101b
ROP_DPno        equ     10101111b
ROP_DPa         equ     10100000b
ROP_DPan        equ     01011111b
ROP_DPx         equ     01011010b
ROP_PDna        equ     01010000b
ROP_DPna        equ     00001010b
ROP_DPon        equ     00000101b

INCREASING      equ     +1
DECREASING      equ     -1
SIZE_PATTERN    equ     8
;----------------------------------------------------------------------------
; M A C R O S
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externFP DIB_BitBlt		;in DIBENG.DLL
	externFP DIB_BeginAccess	;in DIBENG.DLL
	externFP DIB_EndAccess		;in DIBENG.DLL
	externFP BeginAccess		;in ACCESS.ASM
	externFP EndAccess		;in ACCESS.ASM

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
        globalW CachePatM,0
        globalW CachePatD,0
        globalW LRCached,0
PatCacheBufr	db	1024 dup (0)

	externB WindowsEnabledFlag		;in VGA.ASM
	externB DIBAccessCallFlag		;in ACCESS.ASM
	externD XGARegs 			;in VGA.ASM
	externD OffScreenStartAddr		;in VGA.ASM
	externD ColorFormatMask 		;in VGA.ASM
;
	externW wChipId 			;in VGA.ASM
	externW wBpp				;in INIT.ASM
	externW wScreenWidth			;in VGA.ASM
	externW wScreenWidthBytes		;in VGA.ASM
	externB CursorDefinedFlag		;in CURSOR.ASM
	externD lpDriverPDevice 		;in ENABLE.ASM
	externB AGXChipID			;in VGA.ASM
sEnd	Data

;----------------------------------------------------------------------------
;			       XGA
;----------------------------------------------------------------------------
sBegin  Code
assumes cs,Code

;----------------------------------------------------------------------------
; BltSpecial_XGA
;----------------------------------------------------------------------------
cProc	BltSpecial_XGA,<FAR,PUBLIC,PASCAL,NODATA>
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
	parmD	lpDrawMode		;--> to a drawmode
;
        localW  SrcDeltaScan
        localW  DestDeltaScan
        localW  DestHeight
        localW  SrcFlags
	localW	RopFunction		;Translated ROP function
;
cBegin
.386
	push	esi			;save 32 bit versions of these
	push	edi			;
	mov	ax,DGROUP
	mov	gs,ax
        assumes ds,nothing
	assumes es,nothing
        assumes fs,nothing
	assumes gs,Data
	cmp	WindowsEnabledFlag,0	;are we in Windows HiRes mode?
	jne	BSXCallDIBEngine	;nope, let DIB engine worry about it
	cmp	wBpp,8			;running 8 BPP?
	je	@F			;yes, we support BLTer for 8 BPP
	cmp	wChipId,AGX_ID		;running on the IIT AGX?
	jne	BSXCallDIBEngine	;we only support 8 BPP on XGA
	cmp	wBpp,16 		;running 16 BPP on the AGX?
	jne	BSXCallDIBEngine	;nope, let the DIB engine do it!
	cmp	wScreenWidth,800	;running 800x600x16 on the AGX?
	je	BSXCallDIBEngine	;yes, the BLTer isn't supported!
	cmp	AGXChipID,15h		;running on an AGX 15 or greater?
	jb	BSXCallDIBEngine	;nope, on the 14, we can't BLT
@@:	mov	al,bptr Rop+2		;Get the raster op code.

	mov	bx,03h			;assume it's SrcReplace
        cmp     al,ROP_S                ;is it SrcReplace?
	je	BSXDoDSBlt
;
	mov	bl,0eh			;assume it's DSon
        cmp     al,ROP_DSon             ;is it DSon?
	je	BSXDoDSBlt
;
	mov	bl,04h			;assume it's DSna
        cmp     al,ROP_DSna             ;is it DSna?
	je	BSXDoDSBlt
;
	mov	bl,0ch			;assume it's Sn
        cmp     al,ROP_Sn               ;is it Sn?
	je	BSXDoDSBlt
;
	mov	bl,02h			;assume it's SDna
        cmp     al,ROP_SDna             ;is it SDna?
	je	BSXDoDSBlt
;
	mov	bl,06h			;assume it's DSx
        cmp     al,ROP_DSx              ;is it DSx?
	je	BSXDoDSBlt
;
        mov     bl,08h                  ;assume it's DSan
        cmp     al,ROP_DSan             ;is it DSan?
	je	BSXDoDSBlt
;
	mov	bl,01h			;assume it's DSa
        cmp     al,ROP_DSa              ;is it DSa?
	je	BSXDoDSBlt
;
	mov	bl,0dh			;assume it's DSno
        cmp     al,ROP_DSno             ;is it DSno?
	je	BSXDoDSBlt
;
	mov	bl,0bh			;assume it's SDno
        cmp     al,ROP_SDno             ;is it SDno?
	je	BSXDoDSBlt
;
	mov	bl,07h			;assume it's DSo
        cmp     al,ROP_DSo              ;is it DSo?
	jne	BSXTryBrushBlts

PLABEL BSXDoDSBlt
	mov	RopFunction,bx		;save XGA ROP function locally
	lfs	bx,lpSrcDev		;fs:bx-->src pdevice
        mov     ax,fs:[bx].deFlags      ;ax = src pdevice flags.
        mov     SrcFlags,ax             ;save it.
        test    ax,VRAM                 ;is this my PDevice?
	jz	BSXCallDIBEngine	;No.
	call	PrepareForBlt		;Do clipping
	jc	BSXDone 		;exit if null blt.

	lfs	bx,lpDestDev
;
;We need to set the BUSY bit in the destination PDevice structure so that
;the DIB engine doesn't try to asynchronously draw the cursor while we're
;using the hardware to BLT.
;
	or	fs:[bx].deFlags,BUSY	;
	call	ScrScrBlt		;go do scr to screen blt
	jmp	BSXUnexcludeCursor	;
;;
PLABEL BSXTryBrushBlts
	mov	bx,03h			;assume it's Pattern Replace
        cmp     al,ROP_P                ;is it Pattern Replace?
	je	BSXDoBrushBlt		;yes, go do the BLT
;
	mov	bl,0ch			;assume it's Pn
        cmp     al,ROP_Pn               ;is it Pn?
	je	BSXDoBrushBlt		;yes, go do the BLT
;
	mov	bl,07h			;assume it's DPo
        cmp     al,ROP_DPo              ;is it DPo?
	je	BSXDoBrushBlt		;yes, go do the BLT
;
	mov	bl,0bh			;assume it's PDno
        cmp     al,ROP_PDno             ;is it PDno?
	je	BSXDoBrushBlt		;yes, go do the BLT
;
	mov	bl,0dh			;assume it's DPno
        cmp     al,ROP_DPno             ;is it DPno?
	je	BSXDoBrushBlt		;yes, go do the BLT
;
	mov	bl,01h			;assume it's DPa
        cmp     al,ROP_DPa              ;is it DPa?
	je	BSXDoBrushBlt		;yes, go do the BLT
;
	mov	bl,0eh			;assume it's DPan
        cmp     al,ROP_DPan             ;is it DPan?
	je	BSXDoBrushBlt		;yes, go do the BLT
;
	mov	bl,06h			;assume it's DPx
        cmp     al,ROP_DPx              ;is it DPx?
	je	BSXDoBrushBlt		;yes, go do the BLT
;
	mov	bl,02h			;assume it's PDna
        cmp     al,ROP_PDna             ;is it PDna?
	je	BSXDoBrushBlt		;yes, go do the BLT
;
	mov	bl,04h			;assume it's DPna
        cmp     al,ROP_DPna             ;is it DPna?
	je	BSXDoBrushBlt		;yes, go do the BLT
;
	mov	bl,08h			;assume it's DPon
        cmp     al,ROP_DPon             ;is it DPon?
	je	BSXDoBrushBlt		;yes, go do the BLT
;
	mov	bl,0ah			;assume it's Dn
        cmp     al,ROP_Dn               ;is it Dn?
	je	BSXDoBrushBlt		;yes, go do the BLT
;
	mov	bl,0			;assume it's DDx - 0s
        cmp     al,ROP_DDx              ;is it DDx?
	je	BSXDoBrushBlt		;yes, go do the BLT
;
	mov	bl,0fh			;assume it's DDxn
        cmp     al,ROP_DDxn             ;is it DDxn?
	jne	BSXCallDIBEngine	;no. DIB Engine can do this blt.

PLABEL BSXDoBrushBlt
	mov	RopFunction,bx		; save XGA ROP function locally
        mov     SrcFlags,0              ;say no source
	call	PrepareForBlt		;Do clipping
	jc	BSXDone 		;exit if null blt.

	lfs	bx,lpDestDev
;
;We need to set the BUSY bit in the destination PDevice structure so that
;the DIB engine doesn't try to asynchronously draw the cursor while we're
;using the hardware to BLT.
;
	or	fs:[bx].deFlags,BUSY	;
	call	BrushBlt		;go do brush blt
	jmp	BSXUnexcludeCursor	;and we're done
;
PLABEL	BSXCallDIBEngine
	pop	edi
        pop     esi
        lea     sp,[bp-2]
        pop     ds
        pop     bp
	jmp	DIB_BitBlt

PLABEL BSXUnexcludeCursor
;
;We need to unset the BUSY bit in the destination PDevice structure which
;we previously set in order to prevent the DIB engine from asynchonously
;drawing the cursor
;
	lfs	bx,lpDestDev		;
	and	fs:[bx].deFlags,NOT BUSY;
;
;When we get to this point, we've finished doing a BitBLT which used hardware
;acceleration.	If we previously called the DIB engine to exclude a software
;cursor owned by it, we must call the DIB engine to unexclude its software
;cursor.
;
	cmp	DIBAccessCallFlag,0	;did we call DIB eng to exclude cursor?
	je	BSXDone 		;nope, skip the following!
	push	lpDestDev		;
	push	CURSOREXCLUDE		;
	call	DIB_EndAccess		;Let DIB Engine unexclude cursor.
	mov	DIBAccessCallFlag,0	;clear the flag
;
PLABEL BSXDone
	pop	edi			;
	pop	esi			;
.286c
cEnd

;----------------------------------------------------------------------------
; PrepareForBlt
;----------------------------------------------------------------------------
PPROC	PrepareForBlt	near
.386
        assumes ds,nothing
	assumes es,nothing
        assumes fs,nothing
	assumes gs,Data
        lds     bx,lpDestDev
        mov     eax,[bx].deDeltaScan
        mov     DestDeltaScan,ax
        mov     ax,[bx].deHeight
        mov     DestHeight,ax
        mov     si,xExt                 ;X extent will be used a lot
        mov     di,yExt                 ;Y extent will be used a lot
        cmp     SrcFlags,0              ;Is there a src?
        je      short PFB_ChkNullBlt    ;no.
        lds     bx,lpSrcDev             ;yes.
        mov     eax,[bx].deDeltaScan
        mov     SrcDeltaScan,ax
;----------------------------------------------------------------------------
; Input clipping.  The source device must be clipped to the device
; limits.  The destination X and Y, and the extents have been clipped
; by GDI and are positive numbers (0-7FFFh).  The source X and Y could
; be negative.  The clipping code will have to check constantly for
; negative values.
;----------------------------------------------------------------------------
PLABEL  PFB_InputClipX
        mov     ax,SrcxOrg              ;Will need source X org
        mov     bx,[bx].deWidth         ;Maximum allowable is width_bits-1
        or      ax,ax                   ;Any left edge overhang?
        jns     PFB_InputClipRightEdge  ;  No, left edge is on the surface

;----------------------------------------------------------------------------
; The source origin is off the left hand edge of the device surface.
; Move both the source and destination origins right by the amount of
; the overhang and also remove the overhang from the extent.
;
; There is no need to check for the destination being moved off the
; right hand edge of the device's surface since the extent would go
; zero or negative were that to happen.
;----------------------------------------------------------------------------
        add     si,ax                   ;Subtract overhang from X extent
        js      PFB_NullBlt             ;Wasn't enough, nothing to BLT
        sub     DestxOrg,ax             ;Move destination left
        xor     ax,ax                   ;Set new source X origin
        mov     SrcxOrg,ax

;----------------------------------------------------------------------------
; The left hand edge has been clipped.  Now clip the right hand edge.
; Since both the extent and the source origin must be positive numbers
; now, any sign change from adding them together can be ignored if the
; comparison to bmWidth is made as an unsigned compare (maximum result
; of the add would be 7FFFh+7FFFh, which doesn't wrap past zero).
;----------------------------------------------------------------------------
PLABEL  PFB_InputClipRightEdge
        add     ax,si                   ;Compute right edge + 1
        sub     ax,bx                   ;Compute right edge overhang
        jbe     PFB_InputClipSaveXExt   ;No overhang
        sub     si,ax                   ;Subtract overhang from X extent
        js      PFB_NullBlt             ;Wasn't enough, nothing to BLT

PLABEL  PFB_InputClipSaveXExt
        mov     xExt,si                 ;Save new X extent

;----------------------------------------------------------------------------
; Now clip the Y coordinates.  The procedure is the same and all the
; above about positive and negative numbers still holds true.
;----------------------------------------------------------------------------
PLABEL  PFB_InputClipY
        mov     ax,SrcyOrg              ;Will need source Y org
        mov     bx,word ptr lpSrcDev
        mov     bx,[bx].deHeight        ;Maximum allowable is height-1
        or      ax,ax                   ;Any top edge overhang?
        jns     PFB_InputClipBtmEdge    ;  No, top is on the surface

;----------------------------------------------------------------------------
; The source origin is off the top edge of the device surface.  Move
; both the source and destination origins down by the amount of the
; overhang, and also remove the overhang from the extent.
;
; There is no need to check for the destination being moved off the
; bottom of the device's surface since the extent would go zero or
; negative were that to happen.
;----------------------------------------------------------------------------
        add     di,ax                   ;Subtract overhang from Y extent
        js      PFB_NullBlt             ;Wasn't enough, nothing to BLT
        sub     DestyOrg,ax             ;Move destination down
        xor     ax,ax                   ;Set new source Y origin
        mov     SrcyOrg,ax

;----------------------------------------------------------------------------
; The top edge has been clipped. Now clip the bottom edge. Since both
; the extent and the source origin must be positive numbers now, any
; sign change from adding them together can be ignored if the
; comparison to bmWidth is made as an unsigned compare (maximum result
; of the add would be 7FFFh+7FFFh, which doesn't wrap thru 0).
;----------------------------------------------------------------------------
PLABEL  PFB_InputClipBtmEdge
        add     ax,di                   ;Compute bottom edge + 1
        sub     ax,bx                   ;Compute bottom edge overhang
        jbe     PFB_InputClipSaveYExt   ;No overhang
        sub     di,ax                   ;Subtract overhang from Y extent
        jns     PFB_InputClipSaveYExt

PLABEL  PFB_NullBlt
        stc
        ret

PLABEL  PFB_InputClipSaveYExt
        mov     yExt,di                 ;Save new Y extent

PLABEL  PFB_ChkNullBlt
        or      si,si
        jz      PFB_NullBlt             ;X extent is 0
        or      di,di
	jz	PFB_NullBlt		;Y extent is 0

;----------------------------------------------------------------------------
; Cursor Exclusion
; A union of both rectangles must be performed to determine the
; exclusion area.
; Currently:
;       SI = X extent
;       DI = Y extent
;----------------------------------------------------------------------------
PLABEL  PFB_CursorExclusion
	cmp	CursorDefinedFlag,02h	;running with a software cursor?
	jne	PFB_Done		;nope, no need to exclude hdw cursor
	dec	si			;Make the extents inclusive of the
	dec	di			;last point
	mov     cx,DestxOrg             ;Assume only a destination on the
	mov     dx,DestyOrg             ;  display
	test    SrcFlags,VRAM           ;Is the src a memory bitmap?
	jz	PFB_CursorExcludeNoUnion;Yes, go set right and bottom
	xchg    ax,cx                   ;  No, prepare for the union
	mov     bx,dx
	mov     cx,SrcxOrg              ;Set source org
	mov     dx,SrcyOrg

;----------------------------------------------------------------------------
; The union of the two rectangles must be performed.  The top left
; corner will be the smallest x and smallest y.  The bottom right
; corner will be the largest x and the largest y added into the extents
;----------------------------------------------------------------------------
	cmp     cx,ax                   ;Get smallest x
	jle	PFB_CursorExcludeY	;CX is smallest
	xchg    ax,cx                   ;AX is smallest

PLABEL  PFB_CursorExcludeY
	cmp     dx,bx                   ;Get smallest y
	jle	PFB_CursorExcludeUnion	;DX is smallest
	xchg    dx,bx                   ;BX is smallest

PLABEL  PFB_CursorExcludeUnion
	add     si,ax                   ;Set right
	add     di,bx                   ;Set bottom
	jmp	PFB_CursorExcludeDoIt	;Go do exclusion

PLABEL  PFB_CursorExcludeNoUnion
	add     si,cx                   ;Set right
	add     di,dx                   ;Set bottom

PLABEL  PFB_CursorExcludeDoIt
	push	gs			;save GS --> Data
	push    lpDestDev               ;PDevice
	push    cx                      ;Left
	push    dx                      ;Top
	push    si                      ;Right
	push    di                      ;Bottom
	push    CURSOREXCLUDE           ;Flags
	call	DIB_BeginAccess 	;returns with flags in ax.
	pop	gs			;restore GS --> Data
	mov	DIBAccessCallFlag,0ffh	;set the DIBAccessCallFlag

PLABEL PFB_Done
	clc	
	ret
.286c
PrepareForBlt   endp

;--------------------------------------------------------------------------
; ScrScrBlt
;--------------------------------------------------------------------------
PPROC	ScrScrBlt	near
.386
        assumes ds,nothing
	assumes es,nothing
        assumes fs,nothing
	assumes gs,Data
	lfs	di,XGARegs		;FS:DI --> XGA memory mapped BLTer regs
	mov	bx,xExt 		;get the width into BX
        dec     bx                      ;decrement it as per manual
        js      SSBLeave                ;if nothing to BLT, get out
        mov     cx,yExt                 ;get the height into CX
        dec     cx                      ;decrement it as per manual
        jns     SSBlSetFunction         ;if not 0, continue

PLABEL SSBLeave
        ret

PLABEL SSBlSetFunction
	MakeHardwareNotBusy	fs,di
	mov	ax,RopFunction		;restore saved function
	mov	fs:[di].FgdMix,al	;set it into the hardware
	mov	fs:[di].BgdMix,al	;

PLABEL SSBlSetExtents
;
;At this point:
;       BX has X-extent.
;       CX has Y-extent.
;
	mov	fs:[di].OperationDimension1,bx
					;set the X-extent
	mov	fs:[di].OperationDimension2,cx
					;set the Y-extent
;
PLABEL SSBlDetermineDirection
;
;Now we construct the block move command taking into account the direction of
;the move:
;
	mov	esi,0a8118000h		;start with default PixBLT command
	mov	dx,SrcxOrg		;get the source X-coordinate
	mov	ax,DestxOrg		;get the destination X-coordinate
	cmp	dx,ax			;do we want to increment X as we move?
        jge     SSBlSetStartingX        ;yes! we're set up OK
	or	si,100b 		;no, set bits to step left
        add     dx,bx                   ;and make our starting X the right side
	add	ax,bx			;

PLABEL SSBlSetStartingX
	mov	fs:[di].SourceMapX,dx	;set starting source X
	mov	fs:[di].DestMapX,ax	;set starting destination X
;
	mov	dx,SrcyOrg		;get the source Y-coordinate
	mov	ax,DestyOrg		;get the destination Y-coordinate
	cmp	dx,ax			;do we want to increment Y as we move?
        jge     SSBlSetStartingY        ;yes! we're set up OK
	or	si,010b 		;no, set bits to step up
        add     dx,cx                   ;and make our starting Y the bottom
	add	ax,cx			;

PLABEL SSBlSetStartingY
	mov	fs:[di].SourceMapY,dx	;set starting source Y
	mov	fs:[di].DestMapY,ax	;and starting destination Y
;
;Now, send the command:
;
	mov	fs:[di].PixelOperation,esi
;
SSBExit:
	ret				;and return to caller
.286c
ScrScrBlt       endp
;--------------------------------------------------------------------------
; BrushBlt
;
;--------------------------------------------------------------------------
PPROC	BrushBlt	near
.386
        assumes ds,nothing
	assumes es,nothing
        assumes fs,nothing
	assumes gs,Data
        lds     si,lpPBrush             ;ds:si points to the physical brush
        mov     ax,ds:[si].dp8BrushStyle;
        mov     ah,ds:[si].dp8BrushFlags;al style, ah flag
        cmp     al,BS_PATTERN           ;pattern brush?
        jne     @f                      ;no:
        test    ah,PATTERNMONO          ;mono pattern?
	jz	HBColorPattern		;no: colored
        mov     ax,OFFSET dp8BrushMono  ;save offset to mono bits
	jmp	HBMonoPattern		;Yes: mono
@@:
        cmp     al,BS_SOLID             ;solid brush
        jne     @f                      ;no:
        test    ah,COLORSOLID           ;solid color?
        jnz     HBSolid                 ;yes
	jmp	HBColorPattern		;no: dithered
@@:
        cmp     al,BS_HATCHED           ;hatched brush
        mov     ax, OFFSET dp8BrushMask ;hatched - offset to mask bits in ax
	je	HBMonoPattern		;yes:
        mov     al,bptr Rop+2           ;no: assume HOLLOW brush
        cmp     al,ROP_DDx              ;if 0s
	je	HBSolid 		;
        cmp     al,ROP_DDxn             ;if 1s
	je	HBSolid 		;
        cmp     al,ROP_Dn               ;Not destination
	je	HBSolid 		;
        jmp     HBExit                  ;else quit

PLABEL	HBColorPattern
;DS:SI--> PBrush
;
;Copy pattern to off screen memory first
;       DS:SI->PBRUSH
;
	lfs	di,XGARegs		;FS:DI --> XGA Coprocessor registers
	MakeHardwareNotBusy	fs,di
	les	ecx,fword ptr OffScreenStartAddr
					;ES:ECX --> flat address of off-screen
	push	si			;save pointer to PBrush
	lea	si,[si].dp8BrushBits	;DS:SI --> brush bitmap
	mov	dl,8			;there are 8 lines in a brush
	movzx	ebx,wScreenWidthBytes	;
;
public	HBCopyColourBrushLoop
HBCopyColourBrushLoop:
;
;This loop will copy the 8x8 brush in memory to an 8 pixel wide by 16 pixel
;high tile (so we can "auto-rotate" brush in Y) at the start of our
;off-screen area.
;
	mov	eax,dword ptr [si]	;
	mov	es:[ecx],eax		;put it in next line of off-screen area
	mov	es:[ecx+(ebx*8)],eax	;make a copy 8 lines down for rotate
	mov	eax,dword ptr [si+4]	;
	mov	es:[ecx+4],eax		;
	mov	es:[ecx+(ebx*8)+4],eax	;make a copy 8 lines down for rotate
	cmp	wBpp,16 		;running 16 BPP?
	jne	@F			;nope, skip the following
	mov	eax,dword ptr [si+8]	;
	mov	es:[ecx+8],eax		;
	mov	es:[ecx+(ebx*8)+8],eax	;make a copy 8 lines down for rotate
	mov	eax,dword ptr [si+12]	;
	mov	es:[ecx+12],eax 	;
	mov	es:[ecx+(ebx*8)+12],eax ;make a copy 8 lines down for rotate
@@:	add	si,wBpp 		;bump to next line of brush bitmap
	add	ecx,ebx 		;bump to next line of screen
	dec	dl			;done copying?
	jnz	HBCopyColourBrushLoop	;nope, go copy next line
	pop	si			;restore SI --> PBrush structure
;
;Now the 8x8 brush is copied to an 8x16 tile in off-screen memory.  We need to
;setup a loop to tile it across the screen for the width of the BLT.
;
	mov	cx,xExt 		;this is width of tile that we need
	mov	bx,DestxOrg		;get the amount we have to rotate in X
	and	bx,07h			;
	add	cx,bx			;need to have this much extra for rotate
	mov	bx,8			;this is current width of our tile
	sub	cx,bx			;is our tile big enough already?
	jle	HBSetupBrushBLTLoop	;yes, no need to do this again
	mov	fs:[di].FgdMix,3	;our MixMode is always REPLACE
	mov	fs:[di].BgdMix,3	;
;
public	HBTileBrushLoop
HBTileBrushLoop:
;
;Setup the XGA co-processor to expand the tile in the X-direction:
;
	dec	bx			;decrement width of tile as per manual
	mov	fs:[di].OperationDimension1,bx
					;set the X-extent
	mov	fs:[di].OperationDimension2,15
					;set the Y-extent
	inc	bx			;correct for previous DEC BX
;
;Now just do the BLT, always left to right and top to bottom:
;
	mov	fs:[di].SourceMapX,0	;starting source X is always 0
	mov	fs:[di].DestMapX,bx	;set destination X
	mov	fs:[di].SourceMapY,0	;set starting source Y
	mov	fs:[di].DestMapY,0	;and starting destination Y
;
;Now, send the command, using PixMap 1:
;
	mov	fs:[di].PixelOperation,0a8228000h
;
;Check to see if we've done enough and update the xExt in CX
;
	sub	cx,bx			;is our tile big enough yet?
	jle	HBSetupBrushBLTLoop	;yes, go setup to strip it onto dest
;
;OK, the tile has been duplicated, so the new size of our source tile is
;double what it was:
;
	shl	bx,1			;double size of source tile
	MakeHardwareNotBusy	fs,di
	jmp	HBTileBrushLoop 	;go do another tile
;
public	HBSetupBrushBLTLoop
HBSetupBrushBLTLoop:
;
;At this point:
;	FS:DI --> XGA memory mapped co-processor registers.
;
;We now have to stripe our colour pattern contained in the off-screen
;memory area onto the destination rectangle using PixBLT's:
;
	MakeHardwareNotBusy	fs,di
	mov	cx,RopFunction		;set the MixMode to the XGA
	mov	fs:[di].FgdMix,cl	;
	mov	fs:[di].BgdMix,cl	;
	mov	cx,yExt 		;get yExt to do
	push	DestyOrg		;we modify this in the loop
;
public	HBBrushBLTLoop
HBBrushBLTLoop:
;
;Setup the XGA co-processor to stripe the tile onto the destination:
;
	mov	bx,xExt 		;get width of rectangle to BLT
	dec	bx			;decrement it as per manual
	mov	fs:[di].OperationDimension1,bx
					;set the X-extent
	mov	bx,7			;assume we have at least 8 lines left
	cmp	cx,8			;do we have 8 lines left to do?
	jae	@F			;yes, go do 8 lines
	mov	bx,cx			;nope, go do only lines left
	dec	bx			;
@@:	mov	fs:[di].OperationDimension2,bx
					;set the Y-extent
;
;Now just do the BLT, always left to right and top to bottom.  We set the
;SourceMapX and SourceMapY according to the X & Y rotation factors so that
;the BLT will "auto-rotate" the colour brush to the correct origins.
;
	mov	bx,DestxOrg		;get offset so we "auto-rotate" in X
	mov	fs:[di].DestMapX,bx	;set destination X
	and	bx,07h			;
	mov	fs:[di].SourceMapX,bx	;setting source-X will "auto-rotate"
;
	mov	bx,DestyOrg		;starting destination Y is DestyOrg
	mov	fs:[di].DestMapY,bx	;
	and	bx,07h			;get rotation factor in Y
	mov	fs:[di].SourceMapY,bx	;setting source-Y will "auto-rotate"
;
;Now, send the command, using PixMap 1 as source and PixMap 0 as destination:
;
	mov	fs:[di].PixelOperation,0a8218000h
;
;Check to see if we've done enough and update the yExt in CX and DestyOrg:
;
	add	DestyOrg,8		;bump to next stripe in destination
	sub	cx,8			;have we striped enough?
	jle	HBDoneWithColourBrush	;yes, we're done!
	MakeHardwareNotBusy	fs,di
	jmp	HBBrushBLTLoop		;not yet, go do another stripe
;
public	HBDoneWithColourBrush
HBDoneWithColourBrush:
	pop	DestyOrg		;restore saved variable
	jmp	HBExit
;
;
PLABEL  HBSolid
;We have a solid brush.  Setup the XGA to do it in hardware:
;DS:SI-->PBrush
;
	lfs	di,XGARegs		;FS:DI --> XGA Coprocessor registers
	MakeHardwareNotBusy	fs,di
	mov	cx,RopFunction		;get Rop to set
	mov	fs:[di].FgdMix,cl	;set MixMode to the XGA
	mov	fs:[di].BgdMix,cl	;
	cmp	cx,0ah			;is this a NOT operation?
	je	@F			;yes, skip setting the colour
;
;Set the colour onto the XGA:
;
	mov	eax,dword ptr [si].dp8BrushBits
	and	eax,ColorFormatMask	;mask to 8 or 16 BPP
	mov	fs:[di].ForegroundColorReg,eax
;
;Set the coordinates:
;
@@:	mov	ax,DestxOrg		;get left coordinate
	mov	fs:[di].DestMapX,ax	;
	mov	ax,DestyOrg		;get top coordinate
	mov	fs:[di].DestMapY,ax	;
;
;And the extents:
;
	mov	ax,xExt 		;get the width
        dec     ax                      ;decrement it (as per manual)
	mov	fs:[di].OperationDimension1,ax
					;set the X-extent
	mov	ax,yExt 		;get the height
        dec     ax                      ;
	mov	fs:[di].OperationDimension2,ax
					;set the Y-extent
;
;Send the solid coloured rectangle command out:
;
	mov	fs:[di].PixelOperation,08118000h
;
HBSolidExit:
        jmp     HBExit                  ;and we're done with our solid brush!

PLABEL	HBMonoPattern
;Entry:
;	AX contains offset from start of PBrush of mono brush bits to use.
;	DS:SI points at our PBrush.
;	GS --> Data.
;
;First, cache the monochrome pattern to the off-screen area using the
;processor to write the 8x8 monochrome brush to the off-screen area
;
	lfs	di,XGARegs		;FS:DI --> XGA Coprocessor registers
	MakeHardwareNotBusy	fs,di
;
;Setup the dimensions of the MapMask PixMap to be 8x8:
;
	mov	fs:[di].PixmapIndex,3	;make sure we're pointing at index 3
	mov	fs:[di].PixmapWidth,7	;set PixMap's dimensions to 8x8
	mov	fs:[di].PixmapHeight,7	;
;
public	HBCopyMonoBrush
HBCopyMonoBrush:
;
;This loop will copy the 8x8 brush in memory to an 8 pixel wide by 8 pixel
;high tile at the start of our off-screen area.
;
	push	si			;save pointer to PBrush
	add	si,ax			;now DS:SI --> our mono brush bits
	les	ecx,fword ptr OffScreenStartAddr
					;ES:ECX --> flat address of off-screen
;
;Each line of the PBrush is duplicated 4 times.  Therefore, we skip 3
;bytes between fetching each line of the brush
;
	mov	al,[si] 		;
	mov	ah,[si+4]		;
	mov	es:[ecx],ax		;
	mov	al,[si+8]		;
	mov	ah,[si+12]		;
	mov	es:[ecx+2],ax		;
	mov	al,[si+16]		;
	mov	ah,[si+20]		;
	mov	es:[ecx+4],ax		;
	mov	al,[si+24]		;
	mov	ah,[si+28]		;
	mov	es:[ecx+6],ax		;
	pop	si			;restore SI --> PBrush structure
;
public	HBPerformMonoBrushBLT
HBPerformMonoBrushBLT:
;
;At this point:
;	FS:DI --> XGA memory mapped co-processor registers.
;
;The XGA can perform the monochrome brush expansion for us, tiling the
;8x8 monochrome brush to the entire destination rectangle, expanding
;it to colour as it goes.
;
;Set the MixMode that we determined:
;
	mov	cx,RopFunction		;set the MixMode to the XGA
	mov	fs:[di].FgdMix,cl	;
	mov	fs:[di].BgdMix,cl	;
;
;Set the foreground and background colours to expand to.  We use the
;colours contained in the brush if the brush is hatched.  Otherwise,
;we use the colours contained in the DrawMode data structure:
;
	mov	eax,[si].dp1FgColor	;assume it's a hatched brush
	mov	ebx,[si].dp1BgColor	;
	cmp	[si].dp1BrushStyle,BS_HATCHED
					;is it a hatched brush?
	je	HBPBSetColors		;yes! we've got the colours
;
public	HBPBSetMono
HBPBSetMono:
	push	ds			;save DS:SI --> PBrush
	push	si			;
	lds	si,lpDrawMode		;get the DrawMode into DS:SI
	mov	ebx,dword ptr [si+4]	;get background colour (1s->bc color)
	mov	eax,dword ptr [si+8]	;get foreground colour (0s->fg color)
	pop	si			;restore DS:SI --> PBrush
	pop	ds
;
public	HBPBSetColors
HBPBSetColors:
	and	eax,ColorFormatMask
	and	ebx,ColorFormatMask
	mov	fs:[di].BackgroundColorReg,eax
	mov	fs:[di].ForegroundColorReg,ebx
;
public	HBPBSetCoordinates
HBPBSetCoordinates:
;
;Set the xExt and yExt to the XGA:
;
	mov	ax,xExt 		;
	dec	ax			;needed for XGA
	mov	fs:[di].OperationDimension1,ax
	mov	ax,yExt 		;
	dec	ax			;needed for XGA
	mov	fs:[di].OperationDimension2,ax
;
;The XGA can automatically rotate our mono patterned brush.  Set the
;pattern offset registers to do this
;
	mov	ax,DestxOrg		;
	and	ax,07h			;this is amount to rotate by
	mov	fs:[di].PatternMapX,ax	;
	mov	ax,DestyOrg		;
	and	ax,07h			;this is amount to rotate by
	mov	fs:[di].PatternMapY,ax	;
;
;And finally, set the destination coordinates:
;
	mov	ax,DestxOrg		;
	mov	fs:[di].DestMapX,ax	;set destination X
	mov	ax,DestyOrg		;starting destination Y is DestyOrg
	mov	fs:[di].DestMapY,ax	;and starting destination Y
;
;Now, send the command, using the PixMap 3 as the pattern and
;PixMap 1 (the visible screen) as our destination:
;
	mov	fs:[di].PixelOperation,08113000h
;
PLABEL HBExit				;
	ret				;
.286c
BrushBlt	endp
;
;
cEnd
;
;
sEnd    Code
end
