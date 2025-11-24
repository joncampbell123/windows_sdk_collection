        page 60, 132

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1991 - 1995	Microsoft Corporation.	All Rights Reserved.
;
;   ASSEMU.ASM
;
;   General Description:
;      Contains non-fixed assembly routines.
;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .286

        .xlist
        include cmacros.inc
        include windows.inc
        include mmsystem.inc
        include mpu401.inc
        .list

        ?PLM=1                          ; Pascal calling convention
        ?WIN=0                          ; NO! Windows prolog/epilog code


;===========================================================================;
;   extrn declarations
;===========================================================================;

        externFP GlobalAlloc            ; KERNEL
        externFP GlobalFree             ; KERNEL
        externFP AllocSelector          ; KERNEL
        externFP FreeSelector           ; KERNEL
        externFP PrestoChangoSelector   ; KERNEL

        externFP MPU401InterruptHandler

;===========================================================================;
;   segmentation
;===========================================================================;

IFNDEF SEGNAME
        SEGNAME equ <_TEXT>
ENDIF

createSeg %SEGNAME, CodeSeg, word, public, CODE


;===========================================================================;
;   code segment
;===========================================================================;

sBegin CodeSeg

        assumes cs, CodeSeg
        assumes ds, Data
        assumes es, nothing

;---------------------------------------------------------------------------;
;
;   BOOL SetInterruptMask( bIRQ, fMask )
;
;   DESCRIPTION:
;       This function sets or unsets interrupt vector mask.
;
;   ENTRY:
;       ParmB   bIRQ        :   The IRQ (0 - 15) to mask/unmask
;       ParmB   fMask       :   TRUE if should be masked--FALSE if unmask
;
;   EXIT:
;       AX    :   The return value is the previous interrupt mask.
;
;   USES:
;       Flags, AX, CX, DX
;
;
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc SetInterruptMask <NEAR, PUBLIC> <>
        ParmB   bIRQ
        ParmB   fMask
cBegin

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   see if we need to talk to the slave or master PIC
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     cl, bIRQ
        mov     dx, PIC_IMR_MASTER
        cmp     cl, 8
        jb      SetIntMask_Master
        and     cl, 07h
        mov     dx, PIC_IMR_SLAVE

        public SetIntMask_Master
SetIntMask_Master:

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   compute the interrupt mask.
;       DX = slave or master mask register
;       CL = 0-7 bit to set/clear
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ch, 1                   ; CH = 1
        shl     ch, cl                  ; CH = int mask

        mov     cl, fMask               ; get mask
        or      cl, cl
        jz      SetIntMask_UnMask
        mov     cl, ch

        public SetIntMask_UnMask
SetIntMask_UnMask:

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   CH  = PIC mask         (1 << (bInt&7))
;   CL  = wanted mask      fMask ? ch : 0
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        not     ch                      ; we need inverse of mask

        EnterCrit                       ; !!! Trashes BX !!!
        in      al, dx                  ; grab current mask
        mov     ah, al                  ; save it
        and     al, ch                  ; clear bit
        or      al, cl                  ; clear or set based on fMask
        cmp     al, ah                  ; don't set the same state again!
        je      SetIntMask_Same
        out     dx, al                  ; enable/disable ints...

        public SetIntMask_Same
SetIntMask_Same:

        LeaveCrit                       ; !!! Trashes BX !!!

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   we have set/cleared the PIC, now return the old state.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        not     ch                      ; return previous mask state
        mov     al, ah
        and     al, ch
        xor     ah, ah

cEnd


;---------------------------------------------------------------------------;
;
;   LPFUNC SetInterruptVector( bIRQ, lpNewISR )
;
;   DESCRIPTION:
;       This function takes the IRQ and sets the appropriate interrupt
;       vector; and returns the pointer to the previous handler of the
;       IRQ.
;
;   ENTRY:
;       ParmB   bIRQ        :   The IRQ (0 - 15) to install handler for.
;       ParmD   lpNewISR    :   The handler
;
;   EXIT:
;       DX:AX   :   The return value is the previous interrupt handler.
;
;   USES:
;       Flags, AX, BX, DX, ES
;
;
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc SetInterruptVector <NEAR, PUBLIC> <>
        ParmB   bIRQ
        ParmD   lpNewISR
cBegin

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   convert IRQ to interrupt vector...
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     al, bIRQ
        mov     ah, 08h
        cmp     al, ah                  ; Q: slave or master IRQ?
        jl      isv_Continue

        mov     ah, (70h - 08h)         ;   slave

        public isv_Continue
isv_Continue:

        add     al, ah                  ; AL = interrupt vector

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   get old interrupt vector (AL == interrupt vector)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ah, 35h
        int     21h                     ; get the old vector in es:bx

        push    es                      ; save for a bit
        push    bx


;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   set new interrupt vector (AL == interrupt vector)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ah, 25h
        push    ds
        lds     dx, lpNewISR
        assumes ds, nothing
        int     21h                     ; set the new vector
        pop     ds

        pop     ax                      ; restore old ISR for return value
        pop     dx                      ; ... DX:AX is old handler

cEnd

;---------------------------------------------------------------------------;
;
;   ISR_Stub
;
;   DESCRIPTION:
;
;   This function is duplicated for the actual interrupt service
;   routine for a given IRQ.  The code is modified to load SI
;   with the appropriate offset of the HARDWAREINSTANCE associated
;   with the IRQ.
; 
;
;---------------------------------------------------------------------------;

	assumes ds, nothing
	assumes es, nothing

cProc ISR_Stub <FAR, PASCAL, PUBLIC> <>
cBegin nogen

        push    ds
        push    ax
        mov     ax, DGROUP                      ; set up local DS
        mov     ds, ax
        assumes ds, Data
        push    si

        mov     si, 1234h

ref_phwi        equ     ($-ISR_Stub) - 2

        ;
        ; See if MPU401 is what caused the interrupt
        ;

        push    ax
        push    dx

        mov     dx, [si.hwi_wIOAddressMPU401]   ; get the base port address
        inc     dx                              ; point to status port
        in      al, dx                          ; read status
        test    al, DSR

        pop     dx
        pop     ax
        jnz     isr_End_Of_Interrupt            ; just EOI

        push    es
        db      66h                             ; pushad
        pusha

        cCall   MPU401InterruptHandler, <si>

        db      66h                     
        popa                                    ; popad
        pop     es

isr_End_Of_Interrupt:

        mov     ax, [si.hwi_wEOICommands] 
        or      al, al                  ; Q: need to EOI slave? (IRQ's 8-15)
        jz      isr_EOI_Master          ;   N: just do master (IRQ's 0-7)

        out     PIC_EOI_SLAVE, al       ; EOI the slave first--then master

isr_EOI_Master:

        mov     al, ah                  ; master EOI command in AL
        out     PIC_EOI_MASTER, al

        pop     si
        pop     ax
        pop     ds
        assumes ds, nothing

        iret

ISR_Stub_Len    equ     $-ISR_Stub

cEnd    nogen

;---------------------------------------------------------------------------;
;
;   Create_ISR
;
;   DESCRIPTION:
;
; 
;
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc Create_ISR <NEAR, PASCAL, PUBLIC> <es, si, di>
        ParmW   phwi
cBegin
        mov     si, phwi
        mov     ax, [si.hwi_uISRCodeSel]
        or      ax, ax
        jnz     SHORT ci_Exit_Failure

        cCall   GlobalAlloc, <GMEM_FIXED+GMEM_ZEROINIT+GMEM_SHARE, 0, ISR_Stub_Len>
        or      ax, ax
        jz      SHORT ci_Exit_Failure
        mov     [si.hwi_uISRDataSel], ax
        mov     dx, ax
        xor     ax, ax
        cCall   AllocSelector, <ax>
        cCall   PrestoChangoSelector, <dx, ax>
        mov     [si.hwi_uISRCodeSel], ax

        push    ds
        push    si
        push    cs
        pop     ds

        Assumes ds, Code

        mov     si, offset ISR_Stub 
        mov     es, dx
        xor     di, di
        mov     cx, ISR_Stub_Len
        rep     movsb
        pop     si

        pop     ds
        Assumes ds, Data

        mov     di, ref_phwi
        mov     word ptr es:[di], si

        mov     ax, 1
        jmp     SHORT ci_Exit

ci_Exit_Failure:
        xor     ax, ax

ci_Exit:
        
cEnd

sEnd CodeSeg

        end
