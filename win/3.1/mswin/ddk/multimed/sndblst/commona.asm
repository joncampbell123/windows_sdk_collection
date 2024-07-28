        page 60, 132
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   commona.asm
;
;   Copyright (c) 1991-1992 Microsoft Corporation.  All rights reserved.
;
;   General Description:
;      Contains wave and midi support routines that don't need to be fixed.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .286

        .xlist
        include cmacros.inc                   
        include windows.inc
        include mmsystem.inc
        include mmddk.inc
        include sndblst.inc
        include vsbd.inc
        .list

        ?PLM=1                          ; Pascal calling convention
        ?WIN=0                          ; NO! Windows prolog/epilog code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   extrn declarations
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 

        externW  <gwPort>
        externFP <dspWrite>             ; sndblst.asm
        externFP <dspRead>              ; sndblst.asm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   segmentation
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IFNDEF SEGNAME
        SEGNAME equ <_TEXT>
ENDIF

createSeg %SEGNAME, CodeSeg, word, public, CODE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   data segment
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

sBegin Data

    globalD glpVSBDEntry,       0   ; the VSBD api entry point is stored here
    globalW gwAcquireCount,     0   ; acquire count for sound blaster

sEnd Data

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   code segment
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

sBegin CodeSeg

        assumes cs, CodeSeg
        assumes ds, DATA

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; CritEnter
;
;   Saves the current state of the interrupt flag on the stack and
;   then disables interrupts.
;
; Registers Destroyed:
;       BX, FLAGS
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, nothing
        assumes es, nothing

cProc CritEnter <FAR, PASCAL, PUBLIC>, <>
cBegin nogen
        pop     dx                      ; get return address
        pop     ax
        pushf
        pushf
        pop     bx
        test    bh, 2                   ; if interrupts are already off, don't
        jz      no_cli                  ; ... blow ~300 clocks doing the cli
        cli
no_cli:
        push    ax
        push    dx
        ret
cEnd nogen

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; CritLeave
;
;   Restores the interrupt state saved by CritEnter
;
; Registers Destroyed:
;       BX, FLAGS
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, nothing
        assumes es, nothing

cProc CritLeave <FAR, PASCAL, PUBLIC>, <>
cBegin nogen
        pop     dx                      ; get return address
        pop     ax

        pop     bx
        test    bh, 2
        jz      no_sti
        sti
no_sti:
        push    ax
        push    dx
        ret
cEnd nogen

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm dspSpeakerOn | Turn on the speaker.
;
; @rdesc There is no return value.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, Data
        assumes es, nothing

cProc dspSpeakerOn <FAR, PASCAL, PUBLIC> <>
cBegin

        push    ax

        D2 <dspSpeakerOn>

; The Media Vision Thunder Board does not properly support the speaker
; status command, so we're not using it here.

        mov     al, SPKRON
        call    dspWrite

dspSpeakerOnExit:

        pop     ax

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm dspSpeakerOff | Turn off the speaker.
;
; @rdesc There is no return value.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, Data
        assumes es, nothing

cProc dspSpeakerOff <FAR, PASCAL, PUBLIC> <>
cBegin

        push    ax

        D2 <dspSpeakerOff>

; The Media Vision Thunder Board does not properly support the speaker
; status command, so we're not using it here.

        mov     al, SPKROFF
        call    dspWrite

dspSpeakerOffExit:

        pop     ax

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; WORD FAR PASCAL vsbdAcquireSoundBlaster(void)
;
; EXIT:
;     IF success
;         AX = 0, go ahead and open
;     ELSE
;         AX = non-zero error code:
;             SB_API_ASB_Err_Bad_Sound_Blaster    equ 01h
;                 The SB base specified is not being virtualized by VSBD.
;
;             SB_API_ASB_Err_Already_Owned        equ 02h
;                 The SB is currently owned by another VM.
;
; USES:
;     Flags, AX, BX, DX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, Data
        assumes es, nothing

cProc vsbdAcquireSoundBlaster <FAR, PASCAL, PUBLIC> <>
cBegin

        xor     ax, ax                      ; make sure this is zero
        cmp     [gwAcquireCount], ax        ; Q: do we already own it?
        jne     vsbd_Acquire_Success        ;   Y: then succeed

        mov     ax, [glpVSBDEntry].off      ; Q: is VSBD installed?
        or      ax, [glpVSBDEntry].sel
        jz      vsbd_Acquire_Success        ;   N: then leave (return success)


;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   AX = Base of SB to acquire (for example, 0240h)
;   BX = Flags: fSB_ASB_Acquire_DSP, fSB_ASB_Acquire_AdLib_Synth
;   DX = SB_API_Acquire_Sound_Blaster (1)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ax, [gwPort]                ; base port to acquire
        mov     bx, fSB_ASB_Acquire_DSP+fSB_ASB_Auto_Reset_DSP
        mov     dx, SB_API_Acquire_Sound_Blaster
        call    [glpVSBDEntry]

        test    ax, not SB_API_ASB_Err_State_Unknown
        jnz     vsbd_Acquire_Exit           ; fail? vamoose with error code!


;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   If we owned it last, we know what state we left it in, so don't waste
;   time or turn the speaker on, etc.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        test    ax, SB_API_ASB_Err_State_Unknown
        jz      vsbd_Acquire_Success

        cCall   dspSpeakerOn
        xor     ax, ax                      ; success

vsbd_Acquire_Success:

        inc     [gwAcquireCount]            ; increment acquire count

vsbd_Acquire_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD FAR PASCAL vsbdReleaseSoundBlaster(void)
;
;   EXIT:
;       IF success
;           AX = 0, go ahead and close
;       ELSE
;           AX = non-zero error code:
;               SB_API_RSB_Err_Bad_SB       equ 01h
;                   The SB base specified is not being virtualized by VSBD.
;
;               SB_API_RSB_Err_Not_Yours    equ 02h
;                   The SB is NOT owned by callers VM.
;
;   USES:
;       Flags, AX, BX, DX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, Data
        assumes es, nothing

cProc vsbdReleaseSoundBlaster <FAR, PASCAL, PUBLIC> <>
cBegin

        AssertF byte ptr gwAcquireCount

        xor     ax, ax                      ; assume success
        dec     [gwAcquireCount]            ; Q: final release?
        jnz     vsbd_Release_Exit           ;   N: succeed...

        mov     ax, [glpVSBDEntry].off      ; Q: is VSBD installed?
        or      ax, [glpVSBDEntry].sel
        jz      vsbd_Release_Exit           ;   N: then leave (return success)


;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   AX = Base of SB to release (for example, 0240h)
;   BX = Flags: fSB_ASB_Acquire_DSP, fSB_ASB_Acquire_AdLib_Synth
;   DX = SB_API_Release_Sound_Blaster (2)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ax, [gwPort]                ; base port to release
        mov     bx, fSB_ASB_Acquire_DSP     ; just the DSP
        mov     dx, SB_API_Release_Sound_Blaster
        call    [glpVSBDEntry]

vsbd_Release_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm dspReset | Reset the DSP.
;
; @rdesc If the reset is completed successfully then the carry
;     flag is cleared and 0 is returned in AX.  If an error
;     occurs, carry is set and an error code is returned in AX.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, Data
        assumes es, nothing

        public dspReset
dspReset proc far

        D2 <dspReset>

        AssertF byte ptr [gwAcquireCount]   ; better have acquired it already!

        mov     dx, [gwPort]                ; get the base port address
        add     dx, DSP_PORT_RESET          ; point to reset port
        mov     al, 1
        out     dx, al                      ; reset active

        ; wait 3 microseconds for DSP reset to take a good hold

        mov     cx, 100
        loop    $

        xor     al, al
        out     dx, al                      ; clear the reset

        ; read the data port to confirm 0AAH is there

        mov     cx, 25                      ; lots of tries

reset1:
        call    dspRead
        jc      reset2                      ; jump if it timed out

        cmp     al, 0AAH                    ; correct return value ?
        jz      reset3                      ; jump if it is

reset2:
        loop    reset1 
        mov     ax, -1                      ; bad reset!
        jmp     reset4

reset3:

        xor     ax, ax

reset4:
        ret

dspReset endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; @doc INTERNAL 
;
; @asm  void | MemCopy | memory copy
;
; @parm LPVOID | lpDst | destiation
; @parm LPVOID | lpSrc | source
; @parm WORD   | wCount | number of bytes to copy
;
; @rdesc nothing
;
; @comm This function does not handle segment crossings
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, nothing
        assumes es, nothing

cProc MemCopy <FAR, PASCAL, PUBLIC> <ds, si, di>
        ParmD   lpDst
        ParmD   lpSrc
        ParmW   wCount
cBegin

        cld                             ; let's not assume this

        lds     si, lpSrc               ; get source pointer
        les     di, lpDst               ; get dest pointer
        mov     cx, wCount              ; cx is count of bytes

        shr     cx, 1                   ; copy the memory
        rep     movsw
        adc     cl, cl
        rep     movsb

cEnd

sEnd CodeSeg

        end
