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
; SPECIAL.ASM
;----------------------------------------------------------------------------
        .xlist
DOS5=1
        include cmacros.inc
        include dibeng.inc
        include macros.inc
        .list

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
INCREASING      equ     +1
DECREASING      equ     -1
SIZE_PATTERN    equ     8
;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin  Code
assumes cs,Code
        .386

cProc   SpecialFrame,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
        include special.inc             ;include parms and locals
cBegin <nogen>
cEnd <nogen>

;----------------------------------------------------------------------------
; M A C R O S
;----------------------------------------------------------------------------
WRITE_MODE_0 macro
        mov     dx,3ceh
        mov     al,5
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0fch
        out     dx,al
        mov     dx,3c4h
        mov     al,4
        out     dx,al
        inc     dx
        in      al,dx
        and     al,11110111b
        out     dx,al
        endm

WRITE_MODE_1 macro
        mov     dx,3ceh
        mov     al,5
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0fch
        or      al,1
        out     dx,al
        mov     dx,3c4h
        mov     al,4
        out     dx,al
        inc     dx
        in      al,dx
        and     al,11110111b
        out     dx,al
        endm

WRITE_MODE_2 macro
        mov     dx,3ceh
        mov     al,5
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0fch
        or      al,2
        out     dx,al
        mov     dx,3c4h
        mov     al,4
        out     dx,al
        inc     dx
        in      al,dx
        and     al,11110111b
        out     dx,al
        endm

PACKED_PIXEL_MODE macro
        mov     dx,3ceh
        mov     al,5
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0fch
        out     dx,al
        mov     dx,3c4h
        mov     al,4
        out     dx,al
        inc     dx
        in      al,dx
        or      al,00001000b
        out     dx,al
        endm


;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
        externFP        DIB_BeginAccess
        externA         KernelsScreenSel        ;equates to a000:0000


;----------------------------------------------------------------------------
; PrepareForBlt
;----------------------------------------------------------------------------
PPROC PrepareForBlt near
        assumes ds,nothing
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing
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
PLABEL PFB_InputClipX
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
        js      PFB_ErrorExit           ;Wasn't enough, nothing to BLT
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
PLABEL PFB_InputClipRightEdge
        add     ax,si                   ;Compute right edge + 1
        sub     ax,bx                   ;Compute right edge overhang
        jbe     PFB_InputClipSaveXExt   ;No overhang
        sub     si,ax                   ;Subtract overhang from X extent
        js      PFB_ErrorExit           ;Wasn't enough, nothing to BLT

PLABEL PFB_InputClipSaveXExt
        mov     xExt,si                 ;Save new X extent

;----------------------------------------------------------------------------
; Now clip the Y coordinates.  The procedure is the same and all the
; above about positive and negative numbers still holds true.
;----------------------------------------------------------------------------
PLABEL PFB_InputClipY
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
        js      PFB_ErrorExit           ;Wasn't enough, nothing to BLT
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
PLABEL PFB_InputClipBtmEdge
        add     ax,di                   ;Compute bottom edge + 1
        sub     ax,bx                   ;Compute bottom edge overhang
        jbe     PFB_InputClipSaveYExt   ;No overhang
        sub     di,ax                   ;Subtract overhang from Y extent
        jns     PFB_InputClipSaveYExt
        jmp     PFB_ErrorExit           ;Wasn't enough, nothing to BLT

PLABEL PFB_InputClipSaveYExt
        mov     yExt,di                 ;Save new Y extent

PLABEL PFB_ChkNullBlt
        or      si,si
        jz      PFB_ErrorExit           ;X extent is 0
        or      di,di
        jz      PFB_ErrorExit           ;Y extent is 0

;----------------------------------------------------------------------------
; Cursor Exclusion
; A union of both rectangles must be performed to determine the
; exclusion area.
; Currently:
;       SI = X extent
;       DI = Y extent
;----------------------------------------------------------------------------
PLABEL PFB_CursorExclusion
        dec     si                      ;Make the extents inclusive of the
        dec     di                      ;  last point
        mov     cx,DestxOrg             ;Assume only a destination on the
        mov     dx,DestyOrg             ;  display
        test    SrcFlags,VRAM           ;Is the src a memory bitmap?
        jz      short PFB_CursorExcludeNoUnion ;Yes, go set right and bottom
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
        jle     short PFB_CursorExcludeY;CX is smallest
        xchg    ax,cx                   ;AX is smallest

PLABEL PFB_CursorExcludeY
        cmp     dx,bx                   ;Get smallest y
        jle     short PFB_CursorExcludeUnion ;DX is smallest
        xchg    dx,bx                   ;BX is smallest

PLABEL PFB_CursorExcludeUnion
        add     si,ax                   ;Set right
        add     di,bx                   ;Set bottom
        jmp     short PFB_CursorExcludeDoIt ;Go do exclusion

PLABEL PFB_CursorExcludeNoUnion
        add     si,cx                   ;Set right
        add     di,dx                   ;Set bottom

PLABEL PFB_CursorExcludeDoIt
        push    es
        push    lpDestDev               ;PDevice
        push    cx                      ;Left
        push    dx                      ;Top
        push    si                      ;Right
        push    di                      ;Bottom
        push    CURSOREXCLUDE           ;Flags
        call    DIB_BeginAccess
        pop     es

;----------------------------------------------------------------------------
; Some would have you believe there are TEN distinct cases that must be
; considered to determine the order in which source and destination
; bytes must be processed so that a byte isn't overwritten before it is
; read.
;
; Tain't so - if (and only if) the source and destination are the SAME,
; then if (and only if) the source area and destination area overlap,
; then bytes must either be moved from higher locations to lower OR
; vice versa.  FOUR cases, maximum:
;       1) Source and destination are different devices
;          (including NO source device)
;
;       2) Source and destination areas don't overlap
;
;       3) Source starts lower in memory than destination
;          (start at HIGH end, DECREASING)
;
;       4) Source starts higher in memory than destination
;          (start at LOW end, INCREASING)
;          (include exact overlap in this case)
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; assume most favorable case
;----------------------------------------------------------------------------
        mov     ah,INCREASING
;----------------------------------------------------------------------------
; Check if there is a source and if so, is it the same as the destination?
;----------------------------------------------------------------------------
        cmp     SrcFlags,0              ; Is there a src?
        je      PFB_StepDirFound        ; no.
        mov     edx,lpSrcDev            ; yes. Does lpSrcDev == lpDestDev?
        cmp     edx,lpDestDev
        jne     PFB_StepDirFound        ; no.
;----------------------------------------------------------------------------
;  check if rectangles overlap
;----------------------------------------------------------------------------
        mov     dx,xExt
        add     dx,SrcxOrg
        cmp     dx,DestxOrg             ; src.x + n.x <= dest.x ?
        jbe     PFB_StepDirFound        ; --yes
        mov     dx,xExt
        add     dx,DestxOrg
        cmp     dx,SrcxOrg              ; dest.x + n.x <= src.x ?
        jbe     PFB_StepDirFound        ; --yes
        mov     dx,yExt
        add     dx,SrcyOrg
        cmp     dx,DestyOrg             ; src.y + n.y <= dest.y ?
        jbe     PFB_StepDirFound        ; --yes
        mov     dx,yExt
        add     dx,DestyOrg
        cmp     dx,SrcyOrg              ; dest.y + n.y <= src.y ?
        jbe     PFB_StepDirFound        ; --yes
;----------------------------------------------------------------------------
; rectangles overlap
; determine which direction to process
;----------------------------------------------------------------------------
        mov     dx,DestyOrg
        cmp     dx,SrcyOrg              ; dest.y :: src.y ?
        jne     short @f                ; --not equal
        mov     dx,DestxOrg
        cmp     dx,SrcxOrg              ; dest.x :: src.x ?
@@:     jbe     PFB_StepDirFound        ; -- if dest.yx <= src.yx
;----------------------------------------------------------------------------
; overlap requires move start at high end
;----------------------------------------------------------------------------
        mov     ah,DECREASING
;----------------------------------------------------------------------------
; save the results
;----------------------------------------------------------------------------
PLABEL PFB_StepDirFound
        mov     bDirection,ah
        cmp     ah,INCREASING
        mov     ax,DestyOrg
        mov     bx,DestxOrg
        je      short @f
        add     ax,yExt
        add     bx,xExt
        dec     ax
        dec     bx
@@:     and     al,SIZE_PATTERN-1
        and     bl,SIZE_PATTERN-1
        mov     PatRow,al
        mov     PatCol,bl
        clc
        ret

PLABEL PFB_ErrorExit
        stc
        ret

PrepareForBlt   endp

;----------------------------------------------------------------------------
; SetSPointer
; Returns:
;   ds:esi-->starting point of source.
;----------------------------------------------------------------------------
PPROC SetSPointer       near
        lds     si,lpSrcDev
        mov     edx,[si].deDeltaScan
        mov     esi,dword ptr [si].deBits
        mov     ax,KernelsScreenSel     ;ds:esi-->screen.
        mov     ds,ax
        movzx   eax,SrcyOrg
        mul     edx                     ;eax = offset to start of source.
        add     esi,eax
        movzx   eax,SrcxOrg
        add     esi,eax
        ret
SetSPointer   endp

;----------------------------------------------------------------------------
; SetDPointer
; Returns:
;   es:edi-->starting point of destination.
;----------------------------------------------------------------------------
PPROC SetDPointer near
        les     di,lpDestDev
        mov     edx,es:[di].deDeltaScan
        mov     edi,dword ptr es:[di].deBits
        mov     ax,KernelsScreenSel     ;es:edi-->screen.
        mov     es,ax
        movzx   eax,DestyOrg
        mul     edx                     ;eax = offset to start of dest.
        add     edi,eax
        movzx   eax,DestxOrg
        add     edi,eax
        ret
SetDPointer   endp

;----------------------------------------------------------------------------
; SetDPointerDec
; Returns:
;   es:edi-->starting point of destination.
;----------------------------------------------------------------------------
PPROC SetDPointerDec near
        les     di,lpDestDev
        mov     edx,es:[di].deDeltaScan
        mov     edi,dword ptr es:[di].deBits
        mov     ax,KernelsScreenSel     ;es:edi-->screen.
        mov     es,ax
        movzx   eax,DestyOrg
        add     ax,yExt
        dec     ax
        mul     edx                     ;eax = offset to start of destination.
        add     edi,eax
        movzx   eax,DestxOrg
        add     ax,xExt
        dec     ax
        add     edi,eax
        ret
SetDPointerDec       endp

;----------------------------------------------------------------------------
; SetSPointerDec
; Returns:
;   ds:esi-->starting point of source.
;----------------------------------------------------------------------------
PPROC SetSPointerDec near
        lds     si,lpSrcDev
        mov     edx,[si].deDeltaScan
        mov     esi,dword ptr [si].deBits
        mov     ax,KernelsScreenSel     ;ds:esi-->screen.
        mov     ds,ax
        movzx   eax,SrcyOrg
        add     ax,yExt
        dec     ax
        mul     edx                     ;eax = offset to start of source.
        add     esi,eax
        movzx   eax,SrcxOrg
        add     ax,xExt
        dec     ax
        add     esi,eax
        ret
SetSPointerDec       endp


;----------------------------------------------------------------------------
; ScreenToScreen1
;   Calls pSetSBank and pSetDBank to set src and dest banks.
;----------------------------------------------------------------------------
PPROC ScreenToScreen1   near
        assumes ds,nothing
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing
        call    PrepareForBlt
        jc      STS1_Exit
        call    pSaveBank
        cmp     bDirection,INCREASING
        jne     STS1_Dec

;----------------------------------------------------------------------------
; Blt Downward.
;----------------------------------------------------------------------------
PLABEL STS1_Inc
        cld
        call    SetSPointer
        call    SetDPointer
        mov     edx,esi
        rol     edx,16
        call    pSetSBank
        mov     edx,edi
        rol     edx,16
        call    pSetDBank
        movzx   eax,si
        mov     cx,di
        shr     cx,1
        rcl     ax,1
        and     ax,3            ;bx = 000000sd  where s=0: src is word aligned
        shl     ax,1            ;                     d=0: dest is word aligned
        add     ax,offset DevCopy_Table ;add in base of inner loop table.
        mov     dx,cs:[eax]     ;Get jmp address.
        mov     cx,xExt
        sub     DestDeltaScan,cx
        inc     DestDeltaScan
        sub     SrcDeltaScan,cx
        inc     SrcDeltaScan
        cmp     cx,4
        jl      short @f
        mov     bx,yExt
        push    offset STS1_ReturnsHere1
        jmp     dx
@@:     mov     dx,cs:[DevCopy_Table][8]        ;5th word entry is always Tiny Blt code.
        mov     bx,yExt
        call    dx

PLABEL STS1_ReturnsHere1
        dec     di
        add     di,DestDeltaScan
        jc      short STS1_NextDestBank1

PLABEL STS1_DestBankSelected1
        dec     si
        add     si,SrcDeltaScan
        jc      short STS1_NextSrcBank1

PLABEL STS1_SrcBankSelected1
        mov     cx,xExt                 ;reload cx with xExt.
        dec     bx
        jz      short @f
        push    offset STS1_ReturnsHere1
        jmp     dx                      ;jmp back to inner loop.
@@:     call    pRestoreBank
        jmp     STS1_Exit

PLABEL STS1_NextDestBank1
        rol     edi,16
        inc     di
        mov     ax,dx
        mov     dx,di
        call    pSetDBank
        mov     dx,ax
        rol     edi,16
        jmp     STS1_DestBankSelected1

PLABEL STS1_NextSrcBank1
        rol     esi,16
        inc     si
        mov     ax,dx
        mov     dx,si
        call    pSetSBank
        mov     dx,ax
        rol     esi,16
        jmp     STS1_SrcBankSelected1

;----------------------------------------------------------------------------
; Blt Upward.
;----------------------------------------------------------------------------
PLABEL STS1_Dec
        std
        call    SetSPointerDec
        call    SetDPointerDec
        mov     edx,esi
        rol     edx,16
        call    pSetSBank
        mov     edx,edi
        rol     edx,16
        call    pSetDBank
        movzx   eax,si
        mov     cx,di
        shr     cx,1
        rcl     ax,1
        and     ax,3            ;bx = 000000sd  where s=0: src is word aligned
        shl     ax,1            ;                     d=0: dest is word aligned
        add     ax,10
        add     ax,offset DevCopy_Table ;add in base of inner loop table.
        mov     dx,cs:[eax]     ;Get jmp address.
        mov     cx,xExt
        sub     DestDeltaScan,cx
        inc     DestDeltaScan
        sub     SrcDeltaScan,cx
        inc     SrcDeltaScan
        cmp     cx,4
        jl      short @f
        mov     bx,yExt
        push    offset STS1_ReturnsHere2
        jmp     dx
@@:     mov     dx,cs:[DevCopy_Table][8]        ;5th word entry is always Tiny Blt code.
        mov     bx,yExt
        call    dx

PLABEL STS1_ReturnsHere2
        inc     di
        sub     di,DestDeltaScan
        jc      short STS1_PrevDestBank2

PLABEL STS1_DestBankSelected2
        inc     si
        sub     si,SrcDeltaScan
        jc      short STS1_PrevSrcBank2

PLABEL STS1_SrcBankSelected2
        mov     cx,xExt                 ;reload cx with xExt.
        dec     bx
        jz      short @f
        push    offset STS1_ReturnsHere2
        jmp     dx                      ;jmp back to inner loop.
@@:     call    pRestoreBank
        jmp     STS1_Exit

PLABEL STS1_PrevDestBank2
        rol     edi,16
        dec     di
        mov     ax,dx
        mov     dx,di
        call    pSetDBank
        mov     dx,ax
        rol     edi,16
        jmp     STS1_DestBankSelected2

PLABEL STS1_PrevSrcBank2
        rol     esi,16
        dec     si
        mov     ax,dx
        mov     dx,si
        call    pSetSBank
        mov     dx,ax
        rol     esi,16
        jmp     STS1_SrcBankSelected2

PLABEL STS1_Exit
        ret

ScreenToScreen1 endp

;----------------------------------------------------------------------------
; DevCopy_Table
; Entry:
;   cx = xExt of blt
;----------------------------------------------------------------------------
DevCopy_Table   label   word
        dw      DC_S0D0inc              ;Src  aligned, Dest  aligned
        dw      DC_S0D1inc              ;Src  aligned, Dest !aligned
        dw      DC_S1D0inc              ;Src !aligned, Dest  aligned
        dw      DC_S1D1inc              ;Src !aligned, Dest !aligned
        dw      DC_Tiny

        dw      DC_S0D0dec              ;Src  aligned, Dest  aligned
        dw      DC_S0D1dec              ;Src  aligned, Dest !aligned
        dw      DC_S1D0dec              ;Src !aligned, Dest  aligned
        dw      DC_S1D1dec              ;Src !aligned, Dest !aligned
        dw      DC_Tiny

PLABEL DC_Tiny
        rep     movsb
        ret

PLABEL DC_S0D1inc               ;assume cx =
                                ;loaded = 0  stored = 0
        lodsw                   ;loaded = 2  stored = 0
        stosb                   ;loaded = 2  stored = 1
        sub     cx,3            ;cx = 5
        rol     eax,16
@@:     lodsw                   ;loaded = 6  stored = 1
        rol     eax,8
        stosw                   ;loaded = 6  stored = 5
        rol     eax,8
        sub     cx,2            ;cx = 0
        jg      @b
        js      short @f
        lodsb                   ;loaded = 7  stored = 5
        rol     eax,8
        stosw                   ;loaded = 7  stored = 5
        ret
@@:     rol     eax,8
        stosb                   ;loaded = 6  stored = 6
        ret

PLABEL DC_S1D0inc               ;assume cx = 5
        sub     cx,2            ;cx = 3
                                ;loaded = 0  stored = 0
        lodsb                   ;loaded = 1  stored = 0
        ror     eax,8
@@:     lodsw                   ;loaded = 5  stored = 0
        rol     eax,8
        stosw                   ;loaded = 5  stored = 4
        rol     eax,8
        sub     cx,2            ;cx = 1
        jg      @b
        js      short @f
        lodsb                   ;loaded = 4  stored = 2
        rol     eax,8
        stosw                   ;loaded = 4  stored = 4
        ret
@@:     rol     eax,8
        stosb                   ;loaded = 5  stored = 5
        ret

PLABEL DC_S1D1inc
        movsb
        dec     cx
PLABEL DC_S0D0inc
        shr     cx,1
        rep     movsw
        adc     cx,cx
        rep     movsb
        ret

PLABEL DC_S1D0dec
        dec     si              ;cx = 5
                                ;loaded = 0  stored = 0
        lodsw                   ;loaded = 2  stored = 0
        mov     es:[di],ah      ;loaded = 2  stored = 1
        sub     di,2
        sub     cx,3            ;cx = 2
        rol     eax,16
@@:     lodsw                   ;loaded = 4  stored = 1
        ror     eax,8
        stosw                   ;loaded = 4  stored = 3
        ror     eax,8
        sub     cx,2            ;cx = 0
        jg      @b
        js      short @f
        mov     ah,[si+1]       ;loaded = 5  stored = 3
        ror     eax,8
        mov     es:[di],ax      ;loaded = 5  stored = 5
        dec     di
        ret
@@:     ror     eax,8
        mov     es:[di+1],ah    ;loaded = 4  stored = 4
        inc     si
        ret

PLABEL DC_S0D1dec
        dec     di              ;cx = 5
                                ;loaded = 0  stored = 0
        mov     al,[si]         ;loaded = 1  stored = 0
        rol     eax,16
        sub     si,2
        sub     cx,3            ;cx = 2
@@:     lodsw                   ;loaded = 3  stored = 0
        ror     eax,8
        stosw                   ;loaded = 3  stored = 2
        ror     eax,8
        sub     cx,2            ;cx = 0
        jg      @b
        js      short @f
        mov     ax,[si]         ;loaded = 5  stored = 2
        dec     si
        ror     eax,8
        stosw                   ;loaded = 4  stored = 4
        ror     eax,16
        mov     es:[di+1],ah
        ret
@@:     mov     ah,[si+1]       ;loaded = 5  stored = 3
        ror     eax,8
        mov     es:[di],ax      ;loaded = 5  stored = 5
        dec     di
        ret

PLABEL DC_S0D0dec
        movsb
        dec     cx

PLABEL DC_S1D1dec
        dec     di
        dec     si
        shr     cx,1
        rep     movsw
        adc     cx,cx
        inc     di
        inc     si
        rep     movsb
        ret

;----------------------------------------------------------------------------
; ScreenToScreen1L
; Uses VGA latches.
;----------------------------------------------------------------------------
PPROC ScreenToScreen1L near
        assumes ds,nothing
        assumes es,Data
        assumes fs,nothing
        assumes gs,nothing
        call    PrepareForBlt
        jc      STS1L_Exit
        push    dword ptr ss:[0]
        push    dword ptr ss:[4]
        mov     ax,pSetSBank
        mov     ss:[2],ax
        mov     ax,pSetDBank
        mov     ss:[4],ax
        call    ScreenCopyParams        ;ebx = LMR Index
        push    ax                      ;save Left, Right masks
        call    pSaveBank
        mov     cx,DestxOrg             ;
        and     cx,3                    ;cx = phase
        add     cx,xExt                 ;cx = xExt + phase
        add     cx,3                    ;cx = xExt + phase + 3
        shr     cx,2                    ;cx = # bytes to touch
        mov     al,bl                   ;al = 00000LMR (left,middle,right)
        shr     al,1
        sbb     cx,0                    ;cx -= 1 if (R)ight bit is set
        shr     al,2
        sbb     cx,0                    ;cx -= 1 if (L)eft bit is set

        cmp     bDirection,INCREASING
        jne     STS1L_Decreasing

PLABEL STS1L_Increasing
        cld
        mov     word ptr ss:[0],offset STSInc_SwitchBank
        call    SetDPointer
        shr     edi,2                   ;es:edi-->screen
        mov     edx,edi                 ;high word of edx is bank index
        shr     edx,16
        call    pSetDBank               ;dl = bank index. Set up the bank h/w.
        ror     edx,8                   ;edx.msb = dest bank index
        push    edx
        call    SetSPointer
        pop     edx
        shr     esi,2                   ;es:esi-->screen
        ror     esi,16
        mov     dx,si                   ;dl = src bank index
        call    pSetSBank
        ror     edx,8                   ;edx.msw = src,dest bank indices
        ror     esi,16
        WRITE_MODE_1
        pop     ax                      ;ax = Left, Right masks
        xchg    al,ah
        mov     dx,yExt                 ;dx = yExt
        push    bp
        mov     bp,DestDeltaScan        ;bp = width of display
        shr     bp,2
        call    STS1L_Table[ebx*2]
        pop     bp
        PACKED_PIXEL_MODE
        call    pRestoreBank
        pop     dword ptr ss:[4]
        pop     dword ptr ss:[0]
        jmp     STS1L_Exit

PLABEL STS1L_Decreasing
        std
        mov     ax,bx
        and     ax,5
        jz      short @f
        xor     ax,5
        jz      short @f
        xor     bx,5
@@:     mov     word ptr ss:[0],offset STSDec_SwitchBank
        call    SetDPointerDec
        shr     edi,2                   ;es:edi-->screen
        mov     edx,edi                 ;high word of edx is bank index
        shr     edx,16
        call    pSetDBank               ;dl = bank index. Set up the bank h/w.
        ror     edx,8                   ;edx.msb = dest bank index
        push    edx
        call    SetSPointerDec
        pop     edx
        shr     esi,2                   ;es:esi-->screen
        ror     esi,16
        mov     dx,si                   ;dl = src bank index
        call    pSetSBank
        ror     edx,8                   ;edx.msw = src,dest bank indices
        ror     esi,16
        WRITE_MODE_1
        pop     ax                      ;ax = Left, Right masks
        mov     dx,yExt                 ;dx = yExt
        push    bp
        mov     bp,DestDeltaScan        ;bx = width of display
        shr     bp,2
        call    STS1L_Table[ebx*2]
        pop     bp
        PACKED_PIXEL_MODE
        call    pRestoreBank
        pop     dword ptr ss:[4]
        pop     dword ptr ss:[0]

PLABEL STS1L_Exit
        ret

ScreenToScreen1L        endp

;----------------------------------------------------------------------------
; STS1L_Table
;----------------------------------------------------------------------------
STS1L_Table     label   word
        dw      0                       ;STS_xxx
        dw      STS_xxR
        dw      STS_xMx
        dw      STS_xMR
        dw      STS_Lxx
        dw      STS_LxR
        dw      STS_LMx
        dw      STS_LMR

;----------------------------------------------------------------------------
; STSInc_SwitchBank
;----------------------------------------------------------------------------
PPROC STSInc_SwitchBank near
        add     si,bp
        jc      short STSInc_SwitchSrcBank
PLABEL STSInc_SrcBankSwitched
        add     di,bp
        jc      short STSInc_SwitchDestBank
        ret

PLABEL STSInc_SwitchSrcBank
        rol     edx,8                   ;get src bank index
        inc     dl
        call    wptr ss:[2]             ;SetSBank
        ror     edx,8
        jmp     STSInc_SrcBankSwitched

PLABEL STSInc_SwitchDestBank
        rol     edx,16                  ;get dest bank index
        inc     dl
        call    wptr ss:[4]             ;SetDBank
        ror     edx,16
        ret

STSInc_SwitchBank       endp

;----------------------------------------------------------------------------
; STSDec_SwitchBank
;----------------------------------------------------------------------------
PPROC STSDec_SwitchBank near
        sub     si,bp
        jc      short STSDec_SwitchSrcBank
PLABEL STSDec_SrcBankSwitched
        sub     di,bp
        jc      short STSDec_SwitchDestBank
        ret

PLABEL STSDec_SwitchSrcBank
        rol     edx,8                   ;get src bank index
        dec     dl
        js      short @f
        call    wptr ss:[2]             ;SetSBank
@@:     ror     edx,8
        jmp     STSDec_SrcBankSwitched

PLABEL STSDec_SwitchDestBank
        rol     edx,16                  ;get dest bank index
        dec     dl
        js      short @f
        call    wptr ss:[4]             ;SetDBank
@@:     ror     edx,16
        ret
STSDec_SwitchBank       endp

;----------------------------------------------------------------------------
; STS_xMx
; Entry:
;  dx = yExt
;  cx = # of middle bytes
;  bx = LMR bits
;  ax = left, right masks
;  bp = delta scan
;  h/w set up in planar mode.
;----------------------------------------------------------------------------
PLABEL STS_xMx
PLABEL STSxMx_LoopTop
        push    si
        push    di
        mov     ax,cx
        rep     movsb
        mov     cx,ax
        pop     di
        pop     si
        call    ss:[0]                  ;call NextScan/Bank Switch Code
        dec     dx
        jnz     STSxMx_LoopTop
        ret

;----------------------------------------------------------------------------
; STS_LMx
; Entry:
;  dx = yExt
;  cx = # of middle bytes
;  bx = LMR bits
;  ax = left, right masks
;  bp = delta scan
;  h/w set up in planar mode.
;----------------------------------------------------------------------------
PLABEL STS_LMx
        mov     bx,dx                   ;bx = yExt
        dec     bx
        rol     ebx,16                  ;ebx.msw = yExt - 1
        mov     bx,ax                   ;ebx.lsw = masks
        mov     dx,3c4h
        mov     ax,2
        out     dx,al                   ;select bitplane mask register
        inc     dx                      ;dx = 3c5h

PLABEL STSLMx_LoopTop
        push    si
        push    di
        mov     al,bl                   ;al = 1st mask
        out     dx,al                   ;set it in the h/w
        movsb                           ;move up to 4 pels
        mov     al,0Fh                  ;restore mask to all 4 planes.
        out     dx,al
        push    cx
        rep     movsb                   ;do middle portion
        pop     cx
        pop     di
        pop     si
        call    ss:[0]                  ;call NextScan/Bank Switch Code
        sub     ebx,10000h
        jge     STSLMx_LoopTop
        mov     al,0Fh                  ;restore mask to all 4 planes.
        out     dx,al
        ret

;----------------------------------------------------------------------------
; STS_xMR
; Entry:
;  dx = yExt
;  cx = # of middle bytes
;  bx = LMR bits
;  ax = left, right masks
;  bp = delta scan
;  h/w set up in planar mode.
;----------------------------------------------------------------------------
PLABEL STS_xMR
        mov     bx,dx                   ;bx = yExt
        dec     bx
        rol     ebx,16                  ;ebx.msw = yExt - 1
        mov     bx,ax                   ;ebx.lsw = masks
        mov     dx,3c4h
        mov     ax,2
        out     dx,al                   ;select bitplane mask register
        inc     dx                      ;dx = 3c5h

PLABEL STSxMR_LoopTop
        push    si
        push    di
        push    cx
        rep     movsb                   ;do middle portion
        pop     cx
        mov     al,bh                   ;al = 2nd mask
        out     dx,al                   ;set it in the h/w
        movsb                           ;move up to 4 pels
        mov     al,0Fh                  ;restore mask to all 4 planes.
        out     dx,al
        pop     di
        pop     si
        call    ss:[0]                  ;call NextScan/Bank Switch Code
        sub     ebx,10000h
        jge     STSxMR_LoopTop
        mov     al,0Fh                  ;restore mask to all 4 planes.
        out     dx,al
        ret

;----------------------------------------------------------------------------
; STS_LMR
; Entry:
;  dx = yExt
;  cx = # of middle bytes
;  bx = LMR bits
;  ax = left, right masks
;  bp = delta scan
;  h/w set up in planar mode.
;----------------------------------------------------------------------------
PLABEL STS_LMR
        mov     bx,dx                   ;bx = yExt
        dec     bx
        rol     ebx,16                  ;ebx.msw = yExt - 1
        mov     bx,ax                   ;ebx.lsw = masks
        mov     dx,3c4h
        mov     ax,2
        out     dx,al                   ;select bitplane mask register
        inc     dx                      ;dx = 3c5h

PLABEL STSLMR_LoopTop
        push    si
        push    di
        mov     al,bl                   ;al = 1st mask
        out     dx,al                   ;set it in the h/w
        movsb                           ;move up to 4 pels
        mov     al,0Fh                  ;restore mask to all 4 planes.
        out     dx,al
        push    cx
        rep     movsb                   ;do middle portion
        pop     cx
        mov     al,bh                   ;al = 2nd mask
        out     dx,al                   ;set it in the h/w
        movsb                           ;move up to 4 pels
        pop     di
        pop     si
        call    ss:[0]                  ;call NextScan/Bank Switch Code
        sub     ebx,10000h
        jge     STSLMR_LoopTop
        mov     al,0Fh                  ;restore mask to all 4 planes.
        out     dx,al
        ret

;----------------------------------------------------------------------------
; STS_LxR
; Entry:
;  dx = yExt
;  cx = # of middle bytes
;  bx = LMR bits
;  ax = left, right masks
;  bp = delta scan
;  h/w set up in planar mode.
;----------------------------------------------------------------------------
PLABEL STS_LxR
        mov     bx,dx                   ;bx = yExt
        dec     bx
        rol     ebx,16                  ;ebx.msw = yExt - 1
        mov     bx,ax                   ;ebx.lsw = masks
        mov     dx,3c4h
        mov     ax,2
        out     dx,al                   ;select bitplane mask register
        inc     dx                      ;dx = 3c5h

PLABEL STSLxR_LoopTop
        push    si
        push    di
        mov     al,bl                   ;al = 1st mask
        out     dx,al                   ;set it in the h/w
        movsb                           ;move up to 4 pels
        mov     al,bh                   ;al = 2nd mask
        out     dx,al                   ;set it in the h/w
        movsb                           ;move up to 4 pels
        pop     di
        pop     si
        call    ss:[0]                  ;call NextScan/Bank Switch Code
        sub     ebx,10000h
        jge     STSLxR_LoopTop
        mov     al,0Fh                  ;restore mask to all 4 planes.
        out     dx,al
        ret

;----------------------------------------------------------------------------
; STS_xxR
; STS_Lxx
; Entry:
;  dx = yExt
;  cx = # of middle bytes
;  bx = LMR bits
;  ax = left, right masks
;  bp = delta scan
;  h/w set up in planar mode.
;----------------------------------------------------------------------------
PLABEL STS_xxR
        mov     al,ah

PLABEL STS_Lxx
        mov     bx,dx                   ;bx = yExt
        dec     bx
        rol     ebx,16                  ;ebx.msw = yExt - 1
        mov     bx,ax                   ;ebx.lsw = masks
        mov     dx,3c4h
        mov     ax,2
        out     dx,al                   ;select bitplane mask register
        inc     dx                      ;dx = 3c5h

PLABEL STSLxx_LoopTop
        push    si
        push    di
        mov     al,bl                   ;al = 1st mask
        out     dx,al                   ;set it in the h/w
        movsb                           ;move up to 4 pels
        pop     di
        pop     si
        call    ss:[0]                  ;call NextScan/Bank Switch Code
        sub     ebx,10000h
        jge     STSLxx_LoopTop
        mov     al,0Fh                  ;restore mask to all 4 planes.
        out     dx,al
        ret


;----------------------------------------------------------------------------
; MemoryToScreen1L -- src:no pattern:yes (solid -- Uses Latches)
; Entry:
; bx-->InnerLoop
;----------------------------------------------------------------------------
PPROC MemoryToScreen1L  near
        assumes ds,nothing
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing

        lfs     si,lpDestDev
        mov     di,BUSY
        xchg    fs:[si].deFlags,di
        mov     DeviceFlags,di

        push    eax                     ;save the color.
        push    bx                      ;preserve bx across call.
        call    PrepareForBlt
        pop     bx
        jc      MTS1L_Exit
        call    pSaveBank
        call    SetDPointer
        shr     edi,2                   ;es:edi-->screen
        mov     edx,edi                 ;high word of edx is bank index
        shr     edx,16
        mov     cl,bBankShiftCount
        shl     dl,cl
        pop     ecx                     ;ecx = color
        cld
        call    pSetBank                ;dl = bank index. Set up the bank h/w.
        shl     edx,16                  ;cache it in the high word.
        WRITE_MODE_0
        mov     eax,ecx                 ;eax = solid color
        mov     cx,xExt                 ;cx = xExt
        call    bx                      ;jmp to inner rop loop.
        PACKED_PIXEL_MODE
        call    pRestoreBank
PLABEL MTS1L_Exit
        ret

MemoryToScreen1L        endp

;----------------------------------------------------------------------------
; PatCopySL -- Solid pattern -- Uses the Latches
; Entry:
;   Assumes hardware setup in planar mode.
;   eax = solid color to blt.
;----------------------------------------------------------------------------
PPROC   PatCopySL       near
        assumes ds,nothing
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing
        mov     bx,ax                   ;bl = color
        mov     cx,DestxOrg
        and     cx,3                    ;cx = phase (0, 1, 2, or 3)
        jz      short PCSL_PhaseIsZero
        mov     ah,0fh                  ;ah = 0000 1111b - h/w needs backward pattern.
        shl     ah,cl                   ;ah = 0001 1110b (assuming cl = 1).
        add     cx,xExt                 ;cx = xExt + phase
        cmp     cx,4
        jg      short @f
        mov     al,1                    ;al = 0000 0001b
        shl     al,cl                   ;al = 0000 1000b (assuming cl+width=3)
        dec     al                      ;al = 0000 0111b
        and     ah,al                   ;ah = 0000 0110b
        jmp     PCSL_DoEdge
@@:     sub     cx,4                    ;cx = xExt + phase - 4
        mov     xExt,cx                 ;xExt = xExt - (4-phase)
        call    PCSL_DoEdge

PLABEL PCSL_PhaseIsZero
        mov     cx,xExt
        cmp     cx,4
        jg      short PCSL_DoMiddle
        mov     ah,1                    ;ah = 00000001b
        shl     ah,cl                   ;ah = 00001000b (assuming cl = 3).
        dec     ah                      ;ah = 00000111b
        jmp     PCSL_DoEdge

PLABEL PCSL_DoMiddle
        push    di
        mov     dx,yExt                 ;height of blt
        mov     ax,bx
        mov     bx,DestDeltaScan        ;bx = width of display
        shr     bx,2                    ;divide by 4 since we are dealing with quad pixels.
        mov     esi,edx                 ;save starting bank state.
        mov     si,cx                   ;save cx for ending edge check.
        shr     cx,2                    ;we are doing 4 bytes at a time.
        sub     bx,cx                   ;add on after scan.
        inc     bx                      ;adjust for bank detection

        test    di,1
        jnz     PCSL_MainLoop1

PLABEL PCSL_MainLoop0
        push    cx
        shr     cx,1
        rep     stosw                   ;do a scan blt -- 8 pixels at a time!
        adc     cx,cx
        rep     stosb
        pop     cx
        dec     di                      ;back off by one (stosb does not
        add     di,bx                   ; affect the carry flag).
        jc      short @f                ;wrap to next scan. jmp if bank switch.
        dec     dx                      ;loop for height of blt.
        jnz     PCSL_MainLoop0
        jmp     short PCSL_MainRegionDone
@@:     rol     edx,16                  ;get bank number into dl.
        add     dl,bNextBank            ;select next bank.
        call    pSetBank                ;tell the hardware.
        rol     edx,16                  ;put bank number back into high word.
        dec     dx                      ;This is the same loop logic as above.
        jnz     PCSL_MainLoop0
        jmp     short PCSL_MainRegionDone

PLABEL PCSL_MainLoop1
        push    cx
        stosb
        dec     cx
        shr     cx,1
        rep     stosw                   ;do a scan blt -- 8 pixels at a time!
        adc     cx,cx
        rep     stosb
        pop     cx
        dec     di                      ;back off by one (stosb does not
        add     di,bx                   ; affect the carry flag).
        jc      short @f                ;wrap to next scan. jmp if bank switch.
        dec     dx                      ;loop for height of blt.
        jnz     PCSL_MainLoop1
        jmp     short PCSL_MainRegionDone

@@:     rol     edx,16                  ;get bank number into dl.
        add     dl,bNextBank            ;select next bank.
        call    pSetBank                ;tell the hardware.
        rol     edx,16                  ;put bank number back into high word.
        dec     dx                      ;This is the same loop logic as above.
        jnz     PCSL_MainLoop1

PLABEL PCSL_MainRegionDone
        pop     di
        add     di,cx
        mov     cx,si
        and     cx,3                    ;any partial 4 pixel region?
        jnz     short @f                ;yes. Go do the right edge.
        ret                             ;Otherwise, we're done.
@@:     mov     bx,ax
        mov     ah,1                    ;ah = 00000001b
        shl     ah,cl                   ;ah = 00001000b (assuming cl = 3).
        dec     ah                      ;ah = 00000111b
                                        ;Fall into the edge code.
        mov     edx,esi                 ;reset the bank hardware.
        rol     edx,16                  ;get bank number into dl.
        call    pSetBank                ;tell the hardware.
        rol     edx,16                  ;put bank number back into high word.
;----------------------------------------------------------------------------
; Entry:
;  bl = color
;  ah = edge mask
;----------------------------------------------------------------------------
PLABEL PCSL_DoEdge
        mov     esi,edx                 ;save starting bank in upper word of esi.
        mov     dx,3c4h                 ;3c4h = sequencer
        mov     al,2                    ;set the bitplane (edge mask)
        out     dx,ax                   ; register.
        mov     si,di                   ;save starting di
        mov     cx,yExt                 ;
        mov     dx,DestDeltaScan        ;dx = width of display
        shr     dx,2                    ;convert to quadpixels.
PLABEL PCSL_DoEdgeLoop
        mov     es:[di],bl              ;store color to screen
        add     di,dx                   ;point to next scan line
        jc      short @f
        dec     cx
        jnz     PCSL_DoEdgeLoop         ;do this for yExt in height
        jmp     short PCSL_EdgeComplete
@@:     rol     edx,16
        add     dl,bNextBank            ;select next bank.
        call    pSetBank                ;tell the hardware.
        rol     edx,16
        dec     cx
        jnz     PCSL_DoEdgeLoop
PLABEL PCSL_EdgeComplete
        mov     di,si                   ;restore starting di.
        inc     di                      ;step over 4 pixel region
        mov     dx,3c4h
        mov     ax,0f02h                ;allow access to all planes
        out     dx,ax
        mov     edx,esi
        rol     edx,16                  ;restore the bank setting.
        call    pSetBank                ;tell the hardware.
        rol     edx,16
        ret
PatCopySL       endp

;----------------------------------------------------------------------------
; MemoryToScreen3L -- src:no pattern:yes (not solid -- Uses Latches)
; Entry:
; bx-->InnerLoop
;----------------------------------------------------------------------------
PPROC   MemoryToScreen3L        near
        assumes ds,nothing
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing
        lfs     si,lpDestDev
        mov     di,BUSY
        xchg    fs:[si].deFlags,di
        mov     DeviceFlags,di
        push    bx
        call    PrepareForBlt
        pop     bx
        jc      MTS3L_Exit
;----------------------------------------------------------------------------
; Set up pointer to brush in offscreen area.
;----------------------------------------------------------------------------
        call    pSaveBank
        mov     ax,KernelsScreenSel
        mov     es,ax                   ;es = ScreenSelector
        movzx   eax,DestHeight          ;get height of screen.
        movzx   edx,DestDeltaScan       ;dx = width of screen.
        mul     edx                     ;ax = height*width.
        push    eax                     ;eax= 1st non-visible scanline. Save it.
        mov     di,ax                   ;es:di-->where we store the brush.
        mov     edx,eax                 ;get bank number into dx.
        rol     edx,16                  ;roll it down to lsw for SetBank
        call    pSetBank                ;set bank hardware.
        mov     ax,ss
        mov     ds,ax                   ;ds = brush src selector.
;----------------------------------------------------------------------------
; Now rotate the brush and store into the offscreen area.
;----------------------------------------------------------------------------
        push    ebx                     ;save off ptr to inner loop routine.
        xor     ebx,ebx                 ;ebx = brush symmetry bits.
        xor     esi,esi
        lds     si,lpPBrush             ;ds:si-->lpPBrush
        lea     si,[si].dp8BrushBits    ;ds:si-->color part of brush
        mov     ch,8                    ;height of brush.
        cmp     PatCol,4                ;If PatCol is > 4, then fetch from
        jb      MTS3L_ZeroAligned       ; 2nd dword.

PLABEL MTS3L_FourAligned
@@:     mov     eax,[esi+4]
        stosd
        mov     edx,[esi]
        cmp     eax,edx
        setnz   bl
        ror     ebx,1
        mov     eax,edx
        stosd
        add     esi,8
        dec     ch
        jnz     @b
        jmp     MTS3L_PatternRotated

PLABEL MTS3L_ZeroAligned
@@:     mov     eax,[esi]
        stosd
        mov     edx,[esi+4]
        cmp     eax,edx
        setnz   bl
        ror     ebx,1
        mov     eax,edx
        stosd
        add     esi,8
        dec     ch
        jnz     @b

PLABEL MTS3L_PatternRotated
        mov     ecx,ebx                 ;get brush symmetry bits into ecx.high
        pop     ebx                     ;get back ptr to InnerLoop code.
        mov     ax,es
        mov     ds,ax                   ;ds = selector of brush in video ram.
        pop     edx                     ;get back offset to brush.
        shr     edx,2                   ;divide by 4 for planar mode.
        mov     si,dx                   ;get offset to brush bits.
        shr     edx,16                  ;mov bank value into low word.
        call    pSetSBank               ;Set src bank to point to brush.
        call    SetDPointer             ;set es:edi-->start point on the screen.
        WRITE_MODE_1                    ;set up planar mode.
        shr     edi,2                   ;divide by 4 for planar mode.
        mov     edx,edi                 ;high word is bank index of destination.
        shr     edx,16                  ;get into dl.
        call    pSetDBank               ;Set up the write bank h/w.
        shl     edx,16                  ;cache bank index in high word.
        mov     ax,DestyOrg             ;get y start.
        and     ax,0ffh                 ;convert to y offset in the write bank.
PLABEL MTS3L_BankLoop
        mov     StartScan,ax            ;Get starting dest scan in bank (0 to 255)
        mov     cx,256                  ;max number of scan in a bank is 256.
        sub     cx,ax                   ;compute cx = number of scans to do.
        sub     yExt,cx                 ;decrease yExt by number of scans in this bank.
        jg      short @f                ;if yExt is > 0, then cx is correct.
        add     cx,yExt                 ;else, cx is too big. Adjust by yExt (neg).
@@:     mov     nScans,cx               ;Pass nScans to inner loop code.
        movzx   cx,PatRow               ;get starting row of brush.
        mov     eax,ecx                 ;copy brush symmetry bits into eax.
        shl     al,1                    ;convert brush row to byte offset.
        push    si                      ;save base address of brush.
        add     si,ax                   ;si-->start of brush.
        rol     eax,8                   ;move brush symetry bits into lsb.
        ror     al,cl                   ;adjust based on brush start point.
        mov     bMirrorBrush,al         ;save in frame variable.
        add     cx,nScans               ;compute cx = brush row + height.
        and     cx,7                    ;insure brush row is in range (0-7).
        mov     PatRow,cl               ;update frame variable for next time
        mov     cx,xExt
        mov     nPixels,cx              ;Pass nPixels to inner loop code.
        mov     cx,DestxOrg
        mov     StartX,cx               ;Pass StartX to inner loop code.
        push    bx                      ;save off inner loop address.
        call    bx                      ;do the scans in this bank.
        pop     bx                      ;get back inner loop address.
        pop     si                      ;restore base address of brush.
        cmp     yExt,0                  ;if height is <= zero, we're done.
        jle     short @f
        rol     edx,16                  ;otherwise, we need to advance the
        inc     dl                      ;write bank.
        call    pSetDBank
        rol     edx,16                  ;cache current write bank in high word.
        xor     ax,ax                   ;ax = 0 is start scan of next write bank.
        jmp     MTS3L_BankLoop          ;go do next write bank.
@@:     PACKED_PIXEL_MODE               ;put adapter back into packed pixel mode.
        call    pRestoreBank
PLABEL MTS3L_Exit
        ret
MemoryToScreen3L        endp

;----------------------------------------------------------------------------
; PatCopyL -- non-solid pattern -- Uses the Latches
; Entry:
;   ds:si      -->pre-rotated brush in video memory.
;   bMirrorBrush contains side to side brush symmetry bits.
;   StartScan: Scan to start on.
;   StartX:    x offset into scan.
;   nPixels:   Number of pixels to do.
;   nScans:    Number of scans to do.
;   Assumes hardware setup in planar mode.
;   Assumes bank hardware setup appropriately.
; June 12, 1992 -by- [raypat]
;----------------------------------------------------------------------------
PPROC PatCopyL near
        assumes ds,nothing
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing
        mov     cx,StartX               ;get initial offset into scan.
        shr     StartX,2                ;convert StartX to quadpixels.
        and     cx,3                    ;cx = phase (0, 1, 2, or 3)
        jz      short PCL_PhaseIsZero   ;If phase zero, jmp to main loop.
        mov     ah,0fh                  ;ah = 0000 1111b - h/w needs backward pattern.
        shl     ah,cl                   ;ah = 0001 1110b (assuming cl = 1).
        add     cx,nPixels              ;cx = nPixels + phase
        cmp     cx,4                    ;if blt < 4 pixels, rhs mask needed.
        jg      short @f                ;otherwise, go to initial edge code.
        mov     al,1                    ;al = 0000 0001b
        shl     al,cl                   ;al = 0000 1000b (assuming cl+width=3)
        dec     al                      ;al = 0000 0111b
        and     ah,al                   ;ah = 0000 0110b
        jmp     PCL_DoEdge              ;Only one column to do.
@@:     sub     cx,4                    ;cx = nPixels + phase - 4
        mov     nPixels,cx              ;nPixels = nPixels - (4-phase)
        call    PCL_DoEdge              ;Do initial column.

PLABEL PCL_PhaseIsZero
        mov     cx,nPixels              ;cx = number of remaining pixels.
        cmp     cx,4                    ;if <= 4, then one column left.
        jg      short PCL_DoMiddle
        mov     ah,1                    ;ah = 00000001b
        shl     ah,cl                   ;ah = 00001000b (assuming cl = 3).
        dec     ah                      ;ah = 00000111b
        jmp     PCL_DoEdge

PLABEL PCL_DoMiddle
        push    nScans                  ;save number of scans.
        push    si                      ;save start of brush.
        mov     di,StartScan            ;di --> 1st scan to draw.
        shr     cx,2                    ;convert nPixels to quadpixels.
        mov     nQuadPixels,cx          ;update frame variable.
        mov     ax,nScans               ;Compute min(nScans,8).  Interleave loop
        min_ax  8                       ;will go 8 times, unless blt is < 8 in height.
        mov     dx,ax                   ;dx = number of interleave bases.

PLABEL PCL_VInterleaveLoop
        push    si                      ;save brush start scan.
        mov     cx,nQuadPixels          ;get number of quadpixels to do.
        mov     InterleaveBase,di       ;save off start of this interleave.
        mov     bx,nScans               ;get total number of scans to do.
        dec     bx                      ;make bx zero based.
        shr     bx,3                    ;bx = number of Eights this interation.
        inc     bx                      ;1-8 map to 1.  9-16 map to 2, etc.
        mov     al,ds:[si]              ;prime latches with 4 brush pixels.
        xor     si,1                    ;brush index alternates...0,1,0,1..,etc.
        shl     di,8                    ;convert scan to address (i.e., *256).
        add     di,StartX               ;add in x offset.
        shr     bMirrorBrush,1
        jc      PCL_NotMirrorBrush
@@:     push    di                      ;save start or row.
        shr     cx,1                    ;compute number of 8 pixels to draw.
        rep     stosw                   ;store 8 pixels at a time.
        adc     cx,cx                   ;carry will be set if 1 more quadpixel to do.
        rep     stosb                   ;draw final quadpixel if carry set.
        pop     di                      ;get back starting point.
        mov     cx,nQuadPixels          ;restore number of pixels to do.
        add     di,2048                 ;skip down 8 scans.
        dec     bx                      ;decrement number of Eights.
        jnz     @b                      ;Go do next Eight scan.
        jmp     PCL_VInterleaveBottom

PLABEL PCL_NotMirrorBrush
        push    di                      ;save starting point for HInterleave2.
        push    bx                      ;save number Eights.

PLABEL PCL_HInterleave1                 ;Do the "even" pixels, Eights iteration
        push    di                      ;save starting point.
@@:     stosb                           ;store latches to video ram.
        inc     di                      ;skip the next 4 pixels.
        sub     cx,2                    ;decrement count.
        jg      @b                      ;do alternating groups of 4 of entire scan.
        pop     di                      ;get back starting point.
        mov     cx,nQuadPixels          ;restore number of pixels to do.
        add     di,2048                 ;skip down 8 scans.
        dec     bx                      ;decrement number of Eights.
        jnz     PCL_HInterleave1        ;Go do next Eight scan.
        pop     bx                      ;restore number of Eights.

        pop     di                      ;Get back starting point of interleave.
        dec     cx                      ;if only 4 pixels wide, we don't
        jz      PCL_VInterleaveBottom   ; do the second horiz. interleave.
        inc     di                      ;point to 2nd horizontal interleave.
        mov     al,ds:[si]              ;prime latches with 4 brush pixels.
        xor     si,1                    ;advance brush index.

PLABEL PCL_HInterleave2
        push    di                      ;save starting point.
@@:     stosb                           ;store latches to video ram.
        inc     di                      ;skip the next 4 pixels.
        sub     cx,2                    ;decrement count.
        jg      @b                      ;do alternating groups of 4 of entire scan.
        pop     di                      ;get back starting point.
        mov     cx,nQuadPixels          ;restore number of pixels to do.
        dec     cx                      ;we do one fewer on HInterleave2.
        add     di,2048                 ;skip down 8 scans.
        dec     bx                      ;decrement number of Eights.
        jnz     PCL_HInterleave2        ;Go do next Eight scan.

PLABEL PCL_VInterleaveBottom
        pop     si                      ;get back start of brush.
        add     si,2                    ;advance to next row.
        and     si,0ff0fh               ;wrap to top of brush if necessary.
        mov     di,InterleaveBase       ;Get back base scan of interleave.
        inc     di                      ;Advance to next interleave.
        dec     nScans                  ;Decrement number of total scans done.
        dec     dl                      ;Decrement number of interleave bases done.
        jnz     PCL_VInterleaveLoop     ;Go do next interleave.

PLABEL PCL_MainRegionDone
        pop     si                      ;get back starting brush index.
        pop     nScans                  ;Get back number of total scans.
        mov     cx,nPixels              ;cx = number of pixels (not quadpixels!)
        and     cx,3                    ;any partial 4 pixel region?
        jnz     short @f                ;yes. Go do the right edge.
        ret                             ;Otherwise, we're done.
@@:     mov     ah,1                    ;ah = 00000001b
        shl     ah,cl                   ;ah = 00001000b (assuming cl = 3).
        dec     ah                      ;ah = 00000111b --- our edge mask.
        mov     cx,nQuadPixels          ;cx = number of quadpixels in middle section.
        add     StartX,cx               ;Advance start x by width of middle.
        and     cx,1                    ;reduce cx to an odd or even # of quadpixels.
        xor     si,cx                   ;alternate brush index by width of middle.

;----------------------------------------------------------------------------
; Entry:
;  ds:esi-->brush in video ram.
;  ah = edge mask
;----------------------------------------------------------------------------
PLABEL PCL_DoEdge
        push    si                      ;save brush index.
        push    nScans                  ;save total number of scans.
        mov     dx,3c4h                 ;3c4h = sequencer
        mov     al,2                    ;set the bitplane (edge mask)
        out     dx,ax                   ; register.
        mov     ax,nScans               ;Compute min(nScans,8).  Interleave loop
        min_ax  8                       ;will go 8 times, unless blt is < 8 in height.
        mov     dx,ax                   ;dx = number of interleave bases.
        mov     di,StartScan            ;di --> 1st scan to draw.

PLABEL PCL_DoEdgeLoop
        mov     ax,di                   ;save off start of this interleave.
        mov     bx,nScans               ;get total number of scans to do.
        dec     bx                      ;make bx zero based.
        shr     bx,3                    ;bx = number of Eights this interation.
        inc     bx                      ;1-8 map to 1.  9-16 map to 2, etc.
        mov     cl,ds:[si]              ;prime latches with 4 brush pixels.
        add     si,2                    ;advance to next brush row.
        and     si,0ff0fh               ;wrap to top if necessary.
        shl     di,8                    ;convert scan to address (i.e., *256).
        add     di,StartX               ;add in x offset.
@@:     mov     es:[di],bh              ;store latches to screen (masked of course).
        add     di,2048                 ;skip down 8 scans.
        dec     bx                      ;decrement number of Eights.
        jnz     @b                      ;Go do next Eight scan.
        mov     di,ax                   ;Get back interleave base.
        inc     di                      ;advance to next interleave.
        dec     nScans                  ;Decrement number of total scans done.
        dec     dx                      ;decrement number of interleave bases to do.
        jnz     PCL_DoEdgeLoop

PLABEL PCL_EdgeComplete
        inc     StartX                  ;Advance start x past column.
        pop     nScans                  ;get back total number of scans.
        pop     si                      ;get back brush base.
        xor     si,1                    ;advance over 4 pixels in brush.
        mov     dx,3c5h                 ;restore access to all planes
        mov     al,0fh                  ;0fh = all planes accessable.
        out     dx,al                   ;tell the hardware.
        ret
PatCopyL        endp

;----------------------------------------------------------------------------
; DPx_SL -- Solid pattern -- Uses the Latches
; Entry:
;   Assumes hardware setup in planar mode.
;   ax = solid pattern
;----------------------------------------------------------------------------
PPROC DPx_SL near
        mov     bx,ax                   ;get color into bx.
        mov     dx,3ceh                 ;grx controller
        mov     al,3                    ;data rotate/function select register
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0e0h                 ;clear rotate/function bits
        or      al,18h                  ;select XOR logic operation
        out     dx,al
        call    @f                      ;this code will work nicely.
        mov     dx,3ceh
        mov     al,3                    ;data rotate/function select register
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0e0h                 ;clear rotate/function bits
        out     dx,al
        ret

@@:     mov     cx,DestxOrg
        and     cx,3                    ;cx = phase (0, 1, 2, or 3)
        jz      short DPxSL_PhaseIsZero
        mov     ah,0fh                  ;ah = 0000 1111b - h/w needs backward pattern.
        shl     ah,cl                   ;ah = 0001 1110b (assuming cl = 1).
        add     cx,xExt                 ;cx = xExt + phase
        cmp     cx,4
        jg      short @f
        mov     al,1                    ;al = 0000 0001b
        shl     al,cl                   ;al = 0000 1000b (assuming cl+width=3)
        dec     al                      ;al = 0000 0111b
        and     ah,al                   ;ah = 0000 0110b
        jmp     DPxSL_DoEdge
@@:     sub     cx,4                    ;cx = xExt + phase - 4
        mov     xExt,cx                 ;xExt = xExt - (4-phase)
        call    DPxSL_DoEdge

PLABEL DPxSL_PhaseIsZero
        mov     cx,xExt
        cmp     cx,4
        jg      short DPxSL_DoMiddle
        mov     ah,1                    ;ah = 00000001b
        shl     ah,cl                   ;ah = 00001000b (assuming cl = 3).
        dec     ah                      ;ah = 00000111b
        jmp     DPxSL_DoEdge

PLABEL DPxSL_DoMiddle
        push    cx                      ;save cx for ending edge check.
        push    di
        mov     dx,yExt                 ;height of blt
        mov     ax,bx                   ;put color into si.
        mov     bx,DestDeltaScan        ;bx = width of display
        shr     bx,2                    ;divide by 4 since we are dealing with quad pixels.
        mov     esi,edx                 ;save starting bank state.
        shr     cx,2                    ;we are doing 4 bytes at a time.
        sub     bx,cx                   ;add on after scan.
        inc     bx                      ;adjust for bank detection
        mov     si,ax

PLABEL DPxSL_MainLoop
        push    cx
        shr     cx,1
        jz      short DPxSL_WordsDone
@@:     xchg    es:[di],al              ;combine ax with the latches.
        xchg    es:[di+1],ah
        mov     ax,si
        inc     di                      ;don't destroy the carry flag
        inc     di                      ;with an add,2 instruction.
        dec     cx
        jnz     @b
PLABEL DPxSL_WordsDone
        jnc     short @f
        mov     cl,es:[di]              ;prime the latches
        mov     es:[di],al              ;combine latches with ax.
        inc     di
@@:     pop     cx
        dec     di                      ;back off by one (stosb does not
        add     di,bx                   ; affect the carry flag).
        jc      short @f                ;wrap to next scan. jmp if bank switch.
        dec     dx                      ;loop for height of blt.
        jnz     DPxSL_MainLoop
        jmp     short DPxSL_MainRegionDone

@@:     rol     edx,16                  ;get bank number into dl.
        add     dl,bNextBank            ;next bank
        call    pSetBank                ;tell the hardware.
        rol     edx,16                  ;put bank number back into high word.
        dec     dx                      ;This is the same loop logic as above.
        jnz     DPxSL_MainLoop

PLABEL DPxSL_MainRegionDone
        pop     di
        add     di,cx
        pop     cx
        and     cx,3                    ;any partial 4 pixel region?
        jnz     short @f                ;yes. Go do the right edge.
        ret                             ;Otherwise, we're done.
@@:     mov     bx,ax
        mov     ah,1                    ;ah = 00000001b
        shl     ah,cl                   ;ah = 00001000b (assuming cl = 3).
        dec     ah                      ;ah = 00000111b
                                        ;Fall into the edge code.
        mov     edx,esi                 ;reset the bank hardware.
        rol     edx,16                  ;get bank number into dl.
        call    pSetBank                ;tell the hardware.
        rol     edx,16                  ;put bank number back into high word.
;----------------------------------------------------------------------------
; Entry:
;  bx = color
;  ah = edge mask
;----------------------------------------------------------------------------
PLABEL DPxSL_DoEdge
        mov     esi,edx                 ;save starting bank in upper word of esi.
        mov     dx,3c4h                 ;3c4h = sequencer
        mov     al,2                    ;set the bitplane (edge mask)
        out     dx,ax                   ; register.
        mov     si,di                   ;save starting di
        mov     cx,yExt                 ;
        mov     dx,DestDeltaScan        ;dx = width of display
        shr     dx,2                    ;convert to quadpixels.
PLABEL DPxSL_DoEdgeLoop
        xchg    es:[di],bl              ;store color to screen
        mov     bl,bh
        add     di,dx                   ;point to next scan line
        jc      short @f
        dec     cx
        jnz     DPxSL_DoEdgeLoop        ;do this for yExt in height
        jmp     short DPxSL_EdgeComplete
@@:     rol     edx,16
        add     dl,bNextBank
        call    pSetBank
        rol     edx,16
        dec     cx
        jnz     DPxSL_DoEdgeLoop
PLABEL DPxSL_EdgeComplete
        mov     di,si                   ;restore starting di.
        inc     di                      ;step over 4 pixel region
        mov     dx,3c4h
        mov     ax,0F02h                ;allow access to all planes
        out     dx,ax
        mov     edx,esi
        rol     edx,16                  ;restore the bank setting.
        call    pSetBank
        rol     edx,16
        ret
DPx_SL  endp

;----------------------------------------------------------------------------
; ScreenCopyParams
; Computes LMR bits and left,right masks for latched screen copy.
; Returns:
; bx = LMR jump table index.
; ah,al = left,right masks
;----------------------------------------------------------------------------
PPROC   ScreenCopyParams near
        xor     ebx,ebx
        mov     si,xExt
        mov     di,DestxOrg
        mov     cx,di
        and     cx,3                    ;cx = phase (0, 1, 2, or 3)
        jz      short SCP_PhaseIsZero
        mov     ah,0Fh                  ;ah = 0000 1111b - h/w needs backward pattern.
        shl     ah,cl                   ;ah = 0001 1110b (assuming cl = 1).
        add     cx,si                   ;cx = xExt + phase
        or      bx,4                    ;set L bit.
        cmp     cx,4
        jg      short @f
        mov     al,1                    ;al = 0000 0001b
        shl     al,cl                   ;al = 0000 1000b (assuming cl+width=3)
        dec     al                      ;al = 0000 0111b
        and     ah,al                   ;ah = 0000 0110b
        ret                             ;ah = Left Mask

@@:     sub     cx,4
        cmp     cx,4
        jge     SCP_DoMiddle
        or      bx,1                    ;set R bit.
        mov     al,1                    ;al = 0000 0001b
        shl     al,cl                   ;al = 0000 1000b (assuming cl+width=3)
        dec     al                      ;al = 0000 0111b
        ret                             ;al = Right Mask

PLABEL SCP_PhaseIsZero
        mov     cx,si                   ;cx = xExt + phase
        cmp     cx,4
        jge     short SCP_DoMiddle
        or      bx,4                    ;set L bit.
        mov     ah,1                    ;ah = 00000001b
        shl     ah,cl                   ;ah = 00001000b (assuming cl = 3).
        dec     ah                      ;ah = 00000111b
        ret                             ;ah = Left Mask

PLABEL SCP_DoMiddle
        or      bx,2                    ;set M bit.
        and     cx,3                    ;any partial 4 pixel region?
        jnz     short SCP_DoRight       ;yes. Go do the right edge.
        ret                             ;Otherwise, we're done.

PLABEL SCP_DoRight
        or      bx,1                    ;set R bit.
        mov     al,1                    ;al = 00000001b
        shl     al,cl                   ;al = 00001000b (assuming cl = 3).
        dec     al                      ;al = 00000111b
        ret                             ;al = Right Mask
ScreenCopyParams        endp


sEnd    Code
end
