        TITLE FLAT.ASM
        page 60,132

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; FLAT.ASM - Returns a selector to the frame buffer memory
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


?PLM=1	    ; PASCAL Calling convention is DEFAULT
?WIN=0      ; Windows calling convention

        .286
        .xlist
        include cmacros.inc
        include vcap.inc
        .list

        externFP    AllocSelector           ; in KERNEL
        externFP    FreeSelector            ; in KERNEL
        externFP    SetSelectorBase         ; in KERNEL
        externFP    SetSelectorLimit        ; in KERNEL

public CreatePhysicalSelector

; -------------------------------------------------------
;               DATA SEGMENT DECLARATIONS
; -------------------------------------------------------

ifndef SEGNAME
        SEGNAME equ <_TEXT>
endif

createSeg %SEGNAME, CodeSeg, word, public, CODE

sBegin Data
        GlobalD         glpFrameBuffer, 0
sEnd Data

sBegin CodeSeg
        assumes cs,CodeSeg
        assumes ds,Data
        assumes es,nothing

;----------------------------------------------------------------
;
;   returns DX:AX       --> selector:offset to video buffer
;
;----------------------------------------------------------------
        assumes ds,Data
        assumes es,nothing

cProc CreatePhysicalSelector,<FAR,PASCAL,PUBLIC>,<si,di>
        ParmD   base
        ParmD   limit
cBegin
        mov     cx,base.lo      ; BX:CX = physical base of memory
        mov     bx,base.hi

        mov     di,limit.lo     ; SI:DI = extent of memory
        mov     si,limit.hi

        mov     ax,0800h        ; call DPMI
        int     31h
        jnc     dpmi_no_error

dpmi_error:
        xor     si,si
        jmp     exit

dpmi_no_error:                  ; BX:CX contains linear base
        mov     di,cx           ; save it in SI:DI
        mov     si,bx

        ;
        ;   now create a selector that points to the memory
        ;
        cCall   AllocSelector, <ds>
        xchg    si,ax               ; si = selector, ax = base.hi

        cCall   SetSelectorBase,<si, ax, di>

        ;we should be able to use the following, but for some
        ;reason, Kernel limits size to only 1 Meg.
        ;Therefore, call DPMI directly to set the size!!!
        ;cCall   SetSelectorLimit,<si, limit>  ; doesn't work > 1 Meg.

        mov     ax,0008h        ; call SetSegmentLimit
        mov     bx,si           ; selector in bx
        mov     cx,limit.hi     ; CX:DX = physical extent of memory
        mov     dx,limit.lo
        int     31h
exit:
        mov     dx,si           ; DX:AX points to memory
        xor     ax,ax
cEnd


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GetFrameBufferPointer	 Gets a linear pointer to frame memory.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

cProc GetFrameBufferPointer,<FAR,PASCAL,PUBLIC,WIN>,<si,di>
        ParmB		bAddress        ; bAddress is (1-14)
cBegin
	; Get a selector to display memory
	xor	ax, ax
	xor	bx, bx
	mov	bl, bAddress
	mov	cl, 04h
	shl	bx, cl
	mov	cx, 0010h
        ; (1 Meg - 1) (must change following for larger buffer sizes)
	cCall	CreatePhysicalSelector,<bx,ax,000Fh,0FFFFh>
        mov     glpFrameBuffer.sel, dx
        mov     glpFrameBuffer.off, ax  
cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; FreeFrameBufferSelector
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

cProc FreeFrameBufferSelector,<FAR,PASCAL,PUBLIC,WIN>,<si,di>
cBegin
        ; Free the selector after setting its length to zero.
        mov     ax, glpFrameBuffer.sel
        or      ax,ax
        jz      NoSelector
        mov     bx, ax          ; BX is the selector
        mov     ax,0008h        ; call SetSegmentLimit
        xor     cx,cx           ; CX:DX = physical extent of memory
        xor     dx,dx
        int     31h

        cCall   FreeSelector, <bx>
NoSelector:
        xor     ax,ax
cEnd

sEnd
end
