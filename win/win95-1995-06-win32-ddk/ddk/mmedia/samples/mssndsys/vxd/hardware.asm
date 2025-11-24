        page    60, 132

;******************************************************************************
        title   HARDWARE.ASM - Hardware control functions for WSS
;******************************************************************************
;---------------------------------------------------------------------------;
;
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1993 - 1995    Microsoft Corporation.  All Rights Reserved.
;
;---------------------------------------------------------------------------;
;
;   Title:    MSSNDSYS.386 - MICROSOFT Windows Sound System 386 Driver
;
;   Module:   HARDWARE.ASM - Hardware control functions for WSS
;
;   Version:  1.00
;
;   Date:     January 2, 1993
;
;******************************************************************************
;
;   Functional Description:
;
;      Provides functions to access WSS hardware.
;
;******************************************************************************

        .386p

;==============================================================================
;                             I N C L U D E S
;==============================================================================

        .xlist
        include vmm.inc
        include debug.inc
        include shell.inc
        include vpicd.inc

        include mssndsys.inc
        include equates.inc
        .list

;==============================================================================
;                   N O N P A G E A B L E   D A T A
;==============================================================================

VxD_LOCKED_DATA_SEG

EXTRN   gdwCBOffset:DWORD                       ; VM control block offset

        public  gbMode
gbMode  db              ?

        public  gbMute
gbMute  db              ?

        public  CODEC_InitRegs
CODEC_InitRegs  label   BYTE
        db      000h                    ; input control - left
        db      000h                    ; input control - right
ifdef MSSNDSYS
        db      09Fh                    ; AUX1 input control - left
        db      09Fh                    ; AUX1 input control - right
        db      09Fh                    ; AUX2 input control - left
        db      09Fh                    ; AUX2 input control - right
        db      03Fh                    ; DAC control - left
        db      03Fh                    ; DAC control - right
endif
ifdef AZTECH
        db      00Ch
        db      00Ch
        db      00Ch
        db      00Ch
        db      008h
        db      008h
endif
        db      04Bh                    ; clock/format reg - st,22kHz
        db      000h                    ; interface reg      
        db      000h                    ; pin control reg
        db      000h                    ; test/init reg
ifdef MSSNDSYS
        db      000h                    ; miscellaneous reg
endif
ifdef AZTECH
        db      040h
endif
        db      0FCh                    ; digitial mix disabled - full atten
        db      0FFh                    ; sample counter - upper base
        db      0FFh                    ; sample counter - lower base

CODEC_SavedRegs label   BYTE
        db      CODEC_NUM_IDXREGS dup (?)

CODEC_RateCnfg  label   BYTE
        db      01h, 0fh, 00h, 0eh, 03h, 02h, 05h
        db      07h, 04h, 06h, 0dh, 09h, 0bh, 0ch

CODEC_Rates     label   DWORD
        dd      (5510 + 6620) / 2, (6620 + 8000) / 2
        dd      (8000 + 9600) / 2, (9600 + 11025) / 2
        dd      (11025 + 16000) / 2, (16000 + 18900) / 2
        dd      (18900 + 22050) / 2, (22050 + 27420) / 2
        dd      (27420 + 32000) / 2, (32000 + 33075) / 2
        dd      (33075 + 37800) / 2, (37800 + 44100) / 2
        dd      (44100 + 48000) / 2, -1

CODEC_ActualRates       label  DWORD
        dd      5510, 6620, 8000, 9600, 11025, 16000, 18900
        dd      22050, 27420, 32000, 33075, 37800, 44100, 48000

VxD_LOCKED_DATA_ENDS

;==============================================================================
;                    N O N P A G E A B L E   C O D E
;==============================================================================

VxD_LOCKED_CODE_SEG

;------------------------------------------------------------------------------
;
;   CODEC_WaitForReady
;
;   DESCRIPTION:
;       Wait for CODEC to finish initializing (if it is).  This function
;       will timeout after XX milliseconds if the hardware is not
;       functioning properly (carry will be set on return).
;
;       If the CODEC is not initializing, then this function will return
;       immediately (which should be the normal case).
;
;       The timeout value must be at least 256 samples at 8kHz. We
;       compute this for 300 samples at 8kHz to be safe.
;
;   ENTRY:
;       EDX = base address of CODEC
;
;   EXIT:
;       CLC => CODEC ready
;       STC => CODEC not ready (error)
;
;   USED:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc CODEC_WaitForReady, PUBLIC

        push    eax
        push    ecx
        push    edx

        add     dx, SS_CODEC_ADDRESS

        mov     ecx, 20000h

        in      al, dx                      ; !!! HACK for CS4248 !!!

CODEC_WaitForReady_Loop:

        in      al, dx
        or      al, al                      ; Q: high bit low? (clear carry)
        jns     short CODEC_WaitForReady_Exit
        loopd   short CODEC_WaitForReady_Loop

        Debug_Out "Codec_WaitForReady: TIMEOUT ON CODEC!!! (#EDX)"

CODEC_WaitForRead_Fail:

        stc

        ; *** ASSUMES FALLTHROUGH ***
        ;    Don't modify carry before exit!

CODEC_WaitForReady_Exit:

        pop     edx
        pop     ecx
        pop     eax
        ret

EndProc CODEC_WaitForReady

;------------------------------------------------------------------------------
;
;   CODEC_RegRead
;
;   DESCRIPTION:
;       Reads the register specified in AH and returns in AL.
;
;   ENTRY:
;       EDX = base address of CODEC
;       AH = CODEC register to read
;
;   EXIT:
;       CLC => CODEC ready (transfer OK), AL contains value
;       STC => CODEC not ready (error)
;
;   USES:
;       AL, Flags
;
;------------------------------------------------------------------------------

BeginProc CODEC_RegRead, PUBLIC

        call    CODEC_WaitForReady
        jc      SHORT CODEC_RegRead_Exit        ; fail -- carry set

        ;
        ; Interrupts must be disabled here or else we're
        ; toast when we get hit.
        ;

        pushfd                                  ; If we get interrupts here,
        cli                                     ; we're toast!

        push    edx                             ; save reg
        add     dx, SS_CODEC_ADDRESS

        mov     al, ah                          ; CODEC register into al
        or      al, gbMode                      ; or in the current mode

        out     dx, al                          ; select the register
        inc     dx                              ; now goto SS_CODEC_DATA port
        .errnz ((SS_CODEC_DATA - 1) - SS_CODEC_ADDRESS)
        in      al, dx                          ; read register
        pop     edx                             ; restore reg

        popfd

        clc                                     ; successful write

CODEC_RegRead_Exit:
        ret

EndProc CODEC_RegRead

;------------------------------------------------------------------------------
;
;   CODEC_RegWrite
;
;   DESCRIPTION:
;       Writes AL to the register specified in AH.
;
;   ENTRY:
;       EDX = base address of CODEC
;       AH = CODEC register to write
;       AL = value to write to register
;
;   EXIT:
;       CLC => CODEC ready (transfer OK)
;       STC => CODEC not ready (error)
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc CODEC_RegWrite, PUBLIC

        call    CODEC_WaitForReady
        jc      SHORT CODEC_RegWrite_Exit       ; fail -- carry set

        ;
        ; Interrupts must be disabled here or else we're
        ; toast when we get hit.
        ;

        pushfd                                  ; If we get interrupts here,
        cli                                     ; we're toast!

        push    eax                             ; save regs
        push    edx
        add     dx, SS_CODEC_ADDRESS

        xchg    ah, al
        or      al, gbMode                      ; or in the current mode

        out     dx, al                          ; select the register
        inc     dx                              ; now goto SS_CODEC_DATA port
        .errnz ((SS_CODEC_DATA - 1) - SS_CODEC_ADDRESS)
        mov     al, ah
        out     dx, al                          ; write register

        pop     edx                             ; restore regs
        pop     eax      

        popfd

        clc                                     ; successful write

CODEC_RegWrite_Exit:
        ret

EndProc CODEC_RegWrite

;------------------------------------------------------------------------------
;
;   CODEC_EnterMCE
;
;   DESCRIPTION:
;       This mutes the CODEC, and then tells it to enter MCE.
;       CODEC_LeaveMCE must follow since this function mutes
;       the output.
;
;   ENTRY:
;       EDI = pSSI
;       EDX = base address of CODEC
;
;   EXIT:
;       CLC => CODEC ready
;       STC => CODEC not ready (error)
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc CODEC_EnterMCE, PUBLIC

        push    edx                             ; save regs
        push    ecx
        push    eax

        ;
        ; Interrupts must be disabled here or else we're
        ; toast when we get hit.
        ;

        pushfd                                  ; If we get interrupts here,
        cli                                     ; we're toast!

        cmp     [edi.ssi_wCODECClass], CODEC_J_CLASS
        jne     SHORT @F

        ;
        ; Save and mute I/O
        ;

        mov     ecx, 6
        push    esi
        xor     esi, esi

CODEC_EnterMCE_Save_Loop:
        mov     eax, esi
        mov     ah, al
        call    CODEC_RegRead
        mov     CODEC_SavedRegs[ esi ], al
        or      al, 80h
        call    CODEC_RegWrite
        inc     esi
        loop    CODEC_EnterMCE_Save_Loop

        mov     ecx, 2

CODEC_EnterMCE_Save2_Loop:
        mov     eax, esi
        mov     ah, al
        call    CODEC_RegRead
        mov     CODEC_SavedRegs[ esi ], al

        push    eax
        and     al, 0Fh
        cmp     al, 0Dh
        pop     eax
        jna     SHORT CODEC_EnterMCE_NoAdjust
        mov     al, 0Dh
        jmp     SHORT CODEC_EnterMCE_WriteIt

CODEC_EnterMCE_NoAdjust:
        and     al, 0F0h

CODEC_EnterMCE_WriteIt:
        call    CODEC_RegWrite
        loop    CODEC_EnterMCE_Save2_Loop

        pop     esi

@@:
        mov     al, gbMode                      ; get old mode
        or      al, CODEC_MODE_MCE              ; add Mode-Change-Enable
        mov     gbMode, al                      ; save new mode
        call    CODEC_WaitForReady              ; wait for CODEC
        jc      SHORT CODEC_EnterMCE_Exit_Fail  ; exit if not ready
        add     dx, SS_CODEC_ADDRESS            ; move to SS_CODEC_ADDRESS
        out     dx, al                          ; write new mode

CODEC_EnterMCE_Exit_Success:
        popfd
        clc
        jmp     SHORT CODEC_EnterMCE_Exit

CODEC_EnterMCE_Exit_Fail:
        popfd
        stc

CODEC_EnterMCE_Exit:
        pop     eax                             ; restore regs
        pop     ecx
        pop     edx

        ret

EndProc CODEC_EnterMCE

;------------------------------------------------------------------------------
;
;   CODEC_LeaveMCE
;
;   DESCRIPTION:
;       This tells the CODEC to leave MCE, waits for auto-cal
;       to stop (if specified) and then un-mutes.
;
;   ENTRY:
;       EDI = pSSI
;       EDX = base address of CODEC
;
;   EXIT:
;       CLC => CODEC ready
;       STC => CODEC not ready (error)
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc CODEC_LeaveMCE, PUBLIC

        push    edx
        push    ecx
        push    eax

        ;
        ; Interrupts must be disabled here or else we're
        ; toast when we get hit.
        ;

        pushfd                                  ; If we get interrupts here,
        cli                                     ; we're toast!

        mov     al, gbMode
        and     al, NOT( CODEC_MODE_MCE )
        mov     gbMode, al
        call    CODEC_WaitForReady
        jc      SHORT CODEC_LeaveMCE_Exit_Fail
        or      al, CODEC_REG_INTERFACE         ; mode | (register to read)
        add     dx, SS_CODEC_ADDRESS            ; offset by SS_CODEC_ADDRESS
        out     dx, al                          ; write out new mode
        inc     dx                              ; move to data
        .errnz ((SS_CODEC_DATA - 1) - SS_CODEC_ADDRESS)
        in      al, dx
        test    al, 08h                         ; Q: Auto-Calibrating?
        jz      SHORT CODEC_LeaveMCE_UnMute     ;    N: then just un-mute

        ;
        ; Wait for auto-calibration to start and then stop.
        ; The current register is the test register.
        ;

        mov     ecx, 5000                       ; NEED! CHECK THIS!

CODEC_LeaveMCE_WaitStart_Loop:
        in      al, dx
        test    al, 20h
        jnz     SHORT CODEC_LeaveMCE_WaitStop
        loop    CODEC_LeaveMCE_WaitStart_Loop

CODEC_LeaveMCE_WaitStop:
        mov     ecx, 5000

CODEC_LeaveMCE_WaitStop_Loop:
        in      al, dx
        test    al, 20h
        jz      SHORT CODEC_LeaveMCE_UnMute
        loop    CODEC_LeaveMCE_WaitStop_Loop

CODEC_LeaveMCE_UnMute:
        sub     dx, SS_CODEC_DATA

        cmp     [edi.ssi_wCODECClass], CODEC_J_CLASS
        jne     SHORT @F

        ;
        ; Un-mute the outputs!
        ;

        mov     ecx, 8
        push    esi
        xor     esi, esi

CODEC_LeaveMCE_Restore_Loop:
        mov     eax, esi
        mov     ah, al
        mov     al, CODEC_SavedRegs[ esi ]
        call    CODEC_RegWrite
        inc     esi
        loop    CODEC_LeaveMCE_Restore_Loop
        pop     esi

@@:
CODEC_LeaveMCE_Exit_Success:
        popfd
        clc
        jmp     SHORT CODEC_LeaveMCE_Exit

CODEC_LeaveMCE_Exit_Fail:
        popfd
        stc

CODEC_LeaveMCE_Exit:
        pop     eax
        pop     ecx
        pop     edx
        ret

EndProc CODEC_LeaveMCE

;------------------------------------------------------------------------------
;
;   CODEC_ExtMute
;
;   DESCRIPTION:
;       This mutes the CODEC (using XCtl1).
;
;   ENTRY:
;       EDX = base address of CODEC
;       AX = 0 for un-mute, otherwise mute
;
;   EXIT:
;       CLC => CODEC ready
;       STC => CODEC not ready (error)
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc CODEC_ExtMute, PUBLIC

        push    edx
        push    ecx
        push    eax

        mov     cx, ax
        mov     ah, CODEC_REG_DSP               ; read previous mute/filter
        call    CODEC_RegRead
        and     al, not( 0C0h )                 ; clear unnecessary bits

        mov     ah, gbMute                      ; get stored value
        or      cx, cx
        jz      SHORT EM_UnMute

        or      ah, 040h                        ; mute it
        jmp     SHORT EM_DoIt

EM_UnMute:
        and     ah, not( 040h )                 ; un-mute it

        push    eax
        mov     ecx, 5000h                      ; delay for clicks
        mov     ah, CODEC_REG_DSP
EM_WaitLoop:
        call    CODEC_RegRead
        loop    EM_WaitLoop
        pop     eax

EM_DoIt:
        mov     gbMute, ah                      ; store mute/filter
        or      al, ah
        mov     ah, CODEC_REG_DSP
        call    CODEC_RegWrite

        pop     eax
        pop     ecx
        pop     edx
        clc
        ret

EndProc CODEC_ExtMute

;------------------------------------------------------------------------------
;
;   CODEC_EnterTRD
;
;   DESCRIPTION:
;       This function enters the "Transfer Request Disable" mode of
;       the CODEC.  No further CDREQ's or PDREQ's will occur until
;       the interrupt has been cleared.
;
;       This is primarily being implemeted to simulate a single DMA
;       block transfer and then HALT.  If the chip is left with PEN
;       set we end up with a PDREQ outstanding and things get into
;       an undefined mode.
;
;   ENTRY:
;       EDX = base address of CODEC
;
;   EXIT:
;       CLC => CODEC ready
;       STC => CODEC not ready (error)
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc CODEC_EnterTRD, PUBLIC

        push    edx                             ; save regs
        push    ecx
        push    eax

        pushfd                                  ; If we get interrupts here,
        cli                                     ; we're toast!

        mov     al, gbMode                      ; get old mode
        or      al, CODEC_MODE_TRD              ; add Transfer-Request-Disable
        mov     gbMode, al                      ; save new mode
        call    CODEC_WaitForReady              ; wait for CODEC
        jc      SHORT CODEC_EnterTRD_Exit_Fail  ; exit if not ready
        add     dx, SS_CODEC_ADDRESS            ; move to SS_CODEC_ADDRESS
        out     dx, al                          ; write new mode

CODEC_EnterTRD_Exit_Success:
        popfd
        clc
        jmp     SHORT CODEC_EnterTRD_Exit

CODEC_EnterTRD_Exit_Fail:
        popfd
        stc

CODEC_EnterTRD_Exit:
        pop     eax                             ; restore regs
        pop     ecx
        pop     edx

        ret

EndProc CODEC_EnterTRD

;------------------------------------------------------------------------------
;
;   CODEC_LeaveTRD
;
;   DESCRIPTION:
;       This function leaves the "Transfer Request Disable" mode of the
;       CODEC by resetting the "mode".
;
;   ENTRY:
;       EDX = base address of CODEC
;
;   EXIT:
;       CLC => CODEC ready
;       STC => CODEC not ready (error)
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc CODEC_LeaveTRD, PUBLIC

        push    edx                             ; save regs
        push    ecx
        push    eax

        pushfd                                  ; If we get interrupts here,
        cli                                     ; we're toast!

        mov     al, gbMode                      ; get old mode
        and     al, NOT( CODEC_MODE_TRD )       ; remove TRD
        mov     gbMode, al                      ; save new mode
        call    CODEC_WaitForReady              ; wait for CODEC
        jc      SHORT CODEC_EnterTRD_Exit_Fail  ; exit if not ready
        add     dx, SS_CODEC_ADDRESS            ; move to SS_CODEC_ADDRESS
        out     dx, al                          ; write new mode

CODEC_LeaveTRD_Exit_Success:
        popfd
        clc
        jmp     SHORT CODEC_LeaveTRD_Exit

CODEC_LeaveTRD_Exit_Fail:
        popfd
        stc

CODEC_LeaveTRD_Exit:
        pop     eax                             ; restore regs
        pop     ecx
        pop     edx

        ret

EndProc CODEC_LeaveTRD

;---------------------------------------------------------------------------;
;
;   CODEC_Reset
;
;   DESCRIPTION:
;       Resets the CODEC on the SndSys from the VM's hardware state node.
;
;   ENTRY:
;       ESI = VM's hardware state node (phws)
;
;   EXIT:
;       IF carry clear
;           success
;       ELSE
;           CODEC not reset (invalid phws???)
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

EXTRN MSSNDSYS_Get_VM_HW_State_From_pSSI:NEAR

BeginProc CODEC_Reset, PUBLIC

        pushad

        mov     edi, [esi.hws_pSSI]
        movzx   edx, [edi.ssi_wCODECBase]

        Trace_Out "MSSNDSYS: CODEC_Reset, phws: #ESI, pSSI: #EDI"

        ;
        ; copy mixer settings from Sys VM state...
        ;

        push    edi
        push    esi

        VMMCall Get_Sys_VM_Handle
        call    MSSNDSYS_Get_VM_HW_State_From_pSSI
        jc      SHORT CR_No_SysVM_State
        mov     edi, [esp]

        ;
        ; left/right aux channels
        ;

        mov     eax, dword ptr [esi.hws_abCODECState][CODEC_REG_LEFTAUX1]
        mov     dword ptr [edi.hws_abCODECState][CODEC_REG_LEFTAUX1], eax

CR_No_SysVM_State:
        pop     esi
        pop     edi

        ;
        ; Interrupts must be disabled here or else we're
        ; toast when we get hit.
        ;

        pushfd                                  ; If we get interrupts here,
        cli                                     ; we're toast!

        ;
        ; Save the state to the VM's hardware state node
        ;

        ;
        ; Set CODEC to previous state (if one was saved) or
        ; to default state.
        ;

        cmp     [edi.ssi_wCODECClass], CODEC_J_CLASS
        jne     SHORT @F

        mov     eax, 1
        call    CODEC_ExtMute

@@:
        call    CODEC_EnterMCE

        mov     ecx, (CODEC_REG_LOWERBASE - CODEC_REG_DATAFORMAT) + 1
        mov     ah, CODEC_REG_LOWERBASE

CR_RestoreLoop:
        mov     al, [esi.hws_abCODECState + CODEC_REG_RIGHTOUTPUT][ecx]
        cmp     ah, CODEC_REG_DSP
        jne     SHORT CR_NotMuteReg
        or      al, gbMute
        jmp     SHORT CR_RestoreIt

        ;
        ; Don't enable ints or PEN, CEN, etc on reset.
        ;

CR_NotMuteReg:
        cmp     ah, CODEC_REG_INTERFACE
        jne     SHORT CR_NotInterfaceReg
        and     al, AD1848_CONFIG_SDC + AD1848_CONFIG_ACAL
        jmp     SHORT CR_RestoreIt

CR_NotInterfaceReg:
        cmp     ah, CODEC_REG_DSP
        jne     SHORT CR_RestoreIt
        and     al, NOT( 02h )

CR_RestoreIt:
        call    CODEC_RegWrite
        dec     ah
        loop    CR_RestoreLoop

ifdef AZTECH
        mov     ah, CODEC_REG_CAPDATAFMT
        mov     al, [esi.hws_abCODECState][CODEC_REG_CAPDATAFMT]
        call    CODEC_RegWrite
endif

        call    CODEC_LeaveMCE

        mov     ecx, CODEC_REG_RIGHTOUTPUT
        mov     ah, CODEC_REG_RIGHTOUTPUT

CR_VolRestoreLoop:
        mov     al, [esi.hws_abCODECState + ecx]
        call    CODEC_RegWrite
        dec     ah
        loop    CR_VolRestoreLoop

        cmp     [edi.ssi_wCODECClass], CODEC_J_CLASS
        jne     SHORT @F

        xor     eax, eax
        call    CODEC_ExtMute
@@:

CODEC_Reset_Success:

        ;
        ; prime the SRAM on the .WAVJammer
        ;

        test    [edi.ssi_wFlags], SSI_FLAG_BUSTYPE_PCMCIA
        jz      SHORT CODEC_Reset_Exit

        add     edx, NMC_REG_CTRL_WAVJAMMER
        
        push    edx
        mov     eax, 06h                        ; clear CODEC & 
        out     dx, al                          ;    Host SRAM pointers

        movzx   edx, [edi.ssi_wPCMCIA_SRAMBase]
        mov     ecx, 4000h
        mov     eax, 80h

@@:
        out     dx, al
        loop    @B
        xchg    edx, [esp]
        mov     al, 07h                         ; flip host & CODEC SRAM page
        out     dx, al
        pop     edx

        mov     ecx, 4000h
        mov     eax, 80h

@@:
        out     dx, al
        loop    @B

CODEC_Reset_Exit:
        popfd
        clc                                     ; flag success

        popad
        ret

EndProc CODEC_Reset

;---------------------------------------------------------------------------;
;
;   CODEC_Save
;
;   DESCRIPTION:
;       Saves the CODEC's state to the VM's hardware state node
;
;   ENTRY:
;       ESI = VM's hardware state node (phws)
;
;   EXIT:
;       IF carry clear
;           success
;       ELSE
;           CODEC not saved (invalid phws??)
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc CODEC_Save, PUBLIC

        pushad

        mov     edi, [esi.hws_pSSI]
        movzx   edx, [edi.ssi_wCODECBase]

        Trace_Out "MSSNDSYS: CODEC_Save, phws: #ESI, pSSI: #EDI"

        ;
        ; Interrupts must be disabled here or else we're
        ; toast when we get hit.
        ;

        pushfd                                  ; If we get interrupts here,
        cli                                     ; we're toast!

        ;
        ; Save the state to the VM's hardware state node
        ;

        mov     ecx, CODEC_REG_LOWERBASE
        mov     ah, CODEC_REG_LOWERBASE

CS_SaveLoop:
        call    CODEC_RegRead
        mov     [esi.hws_abCODECState][ecx], al
        dec     ah
        loop    CS_SaveLoop

ifdef AZTECH
        mov     ah, CODEC_REG_CAPDATAFMT
        call    CODEC_RegRead
        mov     [esi.hws_abCODECState][CODEC_REG_CAPDATAFMT], al
endif

        popfd
        clc                             ; flag success

        popad

        ret

EndProc CODEC_Save

;------------------------------------------------------------------------------
;
;   CODEC_SetFormat
;
;   DESCRIPTION:
;       Tells the CODEC the format of the wave I/O.
;
;   ENTRY:
;       ESI = pSSI
;
;   EXIT:
;       CLC => CODEC ready
;       STC => CODEC not ready (error)
;
;   USES:
;      Flags
;
;------------------------------------------------------------------------------

BeginProc CODEC_SetFormat

        push    eax
        push    ecx
        push    edx
        push    esi

        xor     ecx, ecx
        movzx   eax, ax

SF_FindRate_Loop:
        cmp     eax, CODEC_Rates[ ecx ]                 ; check against range
        jbe     SHORT SF_SetRate
        add     ecx, 4
        cmp     CODEC_Rates[ ecx ], -1
        jne     SHORT SF_FindRate_Loop
        sub     ecx, 4                                  ; use highest rate

SF_SetRate:
        mov     eax, CODEC_ActualRates[ ecx ]
        mov     [esi.ssi_SBVirtRegs.sbvr_dwCODECRate], eax
        shr     ecx, 2
        mov     al, CODEC_RateCnfg[ ecx ]
        cmp     al, [esi.ssi_SBVirtRegs.sbvr_bCurrentRate]
        je      SHORT SF_Exit

        mov     [esi.ssi_SBVirtRegs.sbvr_bCurrentRate], al

        movzx   edx, [esi.ssi_wCODECBase]               ; get base address

        cmp     [esi.ssi_wCODECClass], CODEC_J_CLASS
        jne     SHORT @F

        mov     eax, 1
        call    CODEC_ExtMute

@@:
        call    CODEC_EnterMCE
        jc      SHORT SF_Exit

        mov     ah, CODEC_REG_DATAFORMAT
        mov     al, [esi.ssi_SBVirtRegs.sbvr_bCurrentRate]

ifdef DEBUG
        push    eax
        xor     ah, ah
        Trace_Out "SetFormat = #AX"
        pop     eax
endif

        call    CODEC_RegWrite
        call    CODEC_LeaveMCE

        ;
        ; Wait for CS42XX parts to sync up with clock...
        ;

        cmp     [esi.ssi_bVersionCODEC], VER_CS4248
        je      SHORT CS_Delay
        cmp     [esi.ssi_bVersionCODEC], VER_CSPROTO
        jne     SHORT @F

CS_Delay:
        mov     ecx, 200

CS_Delay_Loop:
        in      al, dx
        loop    CS_Delay_Loop
        

@@:
        cmp     [esi.ssi_wCODECClass], CODEC_J_CLASS
        jne     SHORT @F

        xor     eax, eax
        call    CODEC_ExtMute
@@:

SF_Exit:
        pop     esi
        pop     edx
        pop     ecx
        pop     eax
        ret
        
EndProc CODEC_SetFormat

;------------------------------------------------------------------------------
;
;   OPL3_IODelay
;
;   DESCRIPTION:
;       Waits a bit to give the OPL3 a chance to catch up.
;
;   ENTRY:
;       EDX = base address of OPL3 
;
;   EXIT:
;       Nothing.
;
;   USES:
;       Nothing.
;
;------------------------------------------------------------------------------

BeginProc OPL3_IODelay, PUBLIC

        push    eax
        in      al, dx
        IO_Delay
        in      al, dx
        IO_Delay
        pop     eax

        ret

EndProc OPL3_IODelay

;------------------------------------------------------------------------------
;
;   OPL3_RegWrite
;
;   DESCRIPTION:
;       Writes AL to the register specified in AH.
;
;   ENTRY:
;       EDX = base address of OPL3 
;       EBX = OPL3 register to write (BH = 1 for second bank)
;       AL = value to write to register
;
;   EXIT:
;       Nothing.
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc OPL3_RegWrite, PUBLIC

        push    ebx
        push    edx
        push    eax

        mov     eax, ebx
        shl     ah, 1
        add     dl, ah
        out     dx, al                          ; Select the bank/register
        sub     dl, ah
        call    OPL3_IODelay
        pop     eax
        inc     dl
        out     dx, al
        call    OPL3_IODelay

        pop     edx
        pop     ebx
        ret

EndProc OPL3_RegWrite

VxD_LOCKED_CODE_ENDS

end
