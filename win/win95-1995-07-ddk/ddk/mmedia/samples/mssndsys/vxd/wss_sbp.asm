        page    60, 132

;******************************************************************************
        title   WSS_SBP.ASM - WASHINGTON 16 mode(WSS&SBP) switch functions
;
;   Functional Description:
;
;       NOVA 16 / WASHINGTON 16 PNP Initial.
;       WASHINGTON 16 mode(WSS&SBP) switch functions.
;       Store the Configuration Information to EEPROM.
;
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1995 Microsoft Corporation.  All Rights Reserved.
;
;******************************************************************************

        .386p

;==============================================================================
;                             I N C L U D E S
;==============================================================================

        .xlist
        include vmm.inc
        include debug.inc
        include sbvirt.inc
        include shell.inc
        include vpicd.inc
        include configmg.inc

        include mssndsys.inc
        include equates.inc
        .list

;------------------------------------------------------------------------------
;                 E X T E R N A L    R E F E R E N C E S
;------------------------------------------------------------------------------

; 
; External CODEC manipulation functions
;

EXTRN   CODEC_RegRead:NEAR
EXTRN   CODEC_RegWrite:NEAR
EXTRN   CODEC_EnterMCE:NEAR
EXTRN   CODEC_LeaveMCE:NEAR

;==============================================================================
;                    N O N P A G E A B L E   C O D E
;==============================================================================

VxD_LOCKED_CODE_SEG

;---------------------------------------------------------------------------;
;
;   Wait_A_While
;
;   DESCRIPTION:
;       Wait for a while by reading from 0388h.
;   ENTRY:
;
;   EXIT:
;
;   USED:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc  Wait_A_While, PUBLIC

        pushad

        mov     ecx, 8000h
        mov     dx, 0388h

Wait_Loop:
        in      al, dx
        loop    SHORT Wait_Loop

        popad
        ret

EndProc Wait_A_While

;------------------------------------------------------------------------------
;
;   dspRead
;
;   DESCRIPTION:
;       Reads the register specified in AH and returns in AL.
;
;   ENTRY:
;       EDX = base address of DSP
;
;   EXIT:
;       CLC => DSP ready (transfer OK), AL contains value
;       STC => DSP not ready (error)
;
;   USES:
;       AL, Flags
;
;------------------------------------------------------------------------------

BeginProc dspRead, PUBLIC

        push    ecx
        push    edx

        add     edx, DSP_PORT_DATAAVAIL
        mov     ecx, 1500

dr_DataLoop:
        in      al, dx
        or      al, al
        js      SHORT dr_DataAvail
        loop    dr_DataLoop
        stc
        jmp     SHORT dr_Exit

dr_DataAvail:
        sub     edx, (DSP_PORT_DATAAVAIL - DSP_PORT_READ)
        in      al, dx
        clc

dr_Exit:
        pop     edx
        pop     ecx
        ret

EndProc dspRead

;------------------------------------------------------------------------------
;
;   dspWrite
;
;   DESCRIPTION:
;       Writes AL to the DSP.
;
;   ENTRY:
;       EDX = base address of DSP
;       AL = value to write to register
;
;   EXIT:
;       CLC => DSP ready (transfer OK)
;       STC => DSP not ready (error)
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc dspWrite, PUBLIC

        push    eax
        push    ecx
        push    edx

        add     edx, DSP_PORT_WRITE

        push    eax                             ; save for later

        xor     ecx, ecx                        ; !!!!!!!!!!!!!!!
        mov     ah, 10
 
dw_BusyLoop:
        in      al, dx
        or      al, al                          ; Q: high bit low? (clc)
        jns     SHORT dw_Ready
        loop    SHORT dw_BusyLoop
        dec     ah
        jnz     SHORT dw_BusyLoop

        Debug_Out "dwBusyLoop: TIMEOUT ON DSP."
        pop     eax

        stc
        jmp     SHORT dw_Exit

dw_Ready:
        pop     eax
        out     dx, al                          ; write the data
        clc                                     ; successful write

dw_Exit:
        pop     edx
        pop     ecx
        pop     eax
        ret

EndProc dspWrite

;---------------------------------------------------------------------------;
;
;   dspReset
;
;   DESCRIPTION:
;       Resets the DSP on the SB
;
;   ENTRY:
;       EDX = base I/O address of DSP
;
;   EXIT:
;       IF carry clear
;           success
;       ELSE
;           DSP not reset (hardware not present?)
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc dspReset, PUBLIC

        Trace_Out "dspReset: Resetting DSP"

        push    eax
        push    edx
        push    ecx

        add     edx, DSP_PORT_RESET
        mov     al, 1
        out     dx, al
        sub     edx, DSP_PORT_RESET

        IO_Delay
        IO_Delay

        add     edx, DSP_PORT_RESET
        xor     al, al
        out     dx, al
        sub     edx, DSP_PORT_RESET
        
        call    Wait_A_While

        mov     ecx, 0600h
DspR_BusyLoop:
        cCall   dspRead
        jc      SHORT DspR_IsBusy
        cmp     al, 0AAh
        jz      SHORT DspR_Success
DspR_IsBusy:
        call    Wait_A_While
        loop    SHORT dspR_BusyLoop

        stc
        jmp     SHORT DspR_Exit

DspR_Success:
        clc

DspR_Exit:

        pop     ecx
        pop     edx
        pop     eax
        ret

EndProc dspReset

;---------------------------------------------------------------------------;
;
;   SetMode_To_SBPro
;
;   DESCRIPTION:
;       Sound Card Mode switch
;
;   ENTRY:
;       EDI contains SSI pointer.
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc SetMode_To_SBPro

        cmp     [edi.ssi_bAZT_CardType], AZTECH_WASHINGTON
        jnz     SMTS_Exit

        cmp     [edi.ssi_bWSTMode], SBPRO_MODE
        jz      SMTS_Exit

SMTS_Begin:

        pushad

        Trace_Out "<Aztech> : Switch to SBPRO mode"

        ; Set Mode from WSS to SBPro

        movzx   edx, [edi.ssi_wIOAddressSB]
        mov     al, 09h
        call    dspWrite            ; out 9 to 2xC
        jc      SHORT SMTS_Failure

        mov     al, 01h
        call    dspWrite            ; out 1 to 2xC
        jc      SHORT SMTS_Failure

        call    Wait_A_While

        movzx   edx, [edi.ssi_wIOAddressSB]
        add     dx, 404H                ; DX = 624 / 644
        mov     ecx, 10H

SMTS_Wait_Loop:
        in      al, dx
        test    al, 08H
        jnz     SMTS_Success
        call    Wait_A_While
        loop    SMTS_Wait_Loop

SMTS_Success:
        mov     [edi.ssi_bWSTMode], SBPRO_MODE
        movzx   edx, [edi.ssi_wIOAddressSB]
        call    dspReset

        ;
        ; set format to something so that the hardware
        ; syncs up correctly, otherwise the games 
        ; that program stereo mode intially won't work
        ; correctly.
        ;
        mov     al, SETSAMPRATE
        call    dspWrite
        mov     al, 01h
        call    dspWrite
        popad

SMTS_Exit:

        clc
        ret

SMTS_Failure:
        popad
        stc
        ret


EndProc SetMode_To_SBPro

;---------------------------------------------------------------------------;
;
;   SetMode_To_WSS
;
;   DESCRIPTION:
;       Sound Card Mode switch
;
;   ENTRY:
;       EDI contains SSI pointer.
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc SetMode_To_WSS

        cmp     [edi.ssi_bAZT_CardType], AZTECH_WASHINGTON
        jnz     SMTW_Exit

        cmp     [edi.ssi_bWSTMode], WSS_MODE
        jz      SMTW_Exit

        pushad

        Trace_Out "<Aztech> : Switch to WSS mode"

        movzx   edx, [edi.ssi_wIOAddressSB]

        call    dspReset
        jc      SMTW_Failure

        ; Set Mode from SBPro to WSS

        mov     al, 09h
        call    dspWrite            ; out 9 to 2xC
        jc      SMTW_Failure

        mov     al, 00h
        call    dspWrite            ; out 0 to 2xC
        jc      SMTW_Failure

        call    Wait_A_While

        movzx   edx, [edi.ssi_wIOAddressSB]
        add     dx, 404H
        mov     ecx, 10H

SMTW_Wait_Loop:
        in      al, dx
        test    al, 08H
        jz      SHORT SMTW_Success
        call    Wait_A_While
        loop    SMTW_Wait_Loop
        jmp     SHORT SMTW_Failure

SMTW_Success:
        mov     [edi.ssi_bWSTMode], WSS_MODE
        popad

SMTW_Exit:
        clc
        ret

SMTW_Failure:
        popad
        stc
        ret

EndProc SetMode_To_WSS

;---------------------------------------------------------------------------;
;
;   WriteEEPROM
;
;   DESCRIPTION:
;       Write a byte data to the EEPROM.
;
;   ENTRY:
;       EDI contains SSI pointer.
;       AH - EEPROM address
;       AL - Configure register value
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc WriteEEPROM

        pushad

        movzx   edx, [edi.ssi_wIOAddressSB]
        mov     ecx, 5                  ; try to write and verify 5 times
        mov     bx, ax                  ; save AX to BX

WriteEEPROM_Loop:
        mov     al, 8
        call    dspWrite            ; out 8 to 2xC
        jc      SHORT WriteEEPROM_Fail

        mov     al, 8
        call    dspWrite            ; out 8 to 2xC
        jc      SHORT WriteEEPROM_Fail

        mov     al, bh
        call    dspWrite            ; out EEPROM address to 2xC
        jc      SHORT WriteEEPROM_Fail

        mov     al, bl
        call    dspWrite            ; out EEPROM value to 2xC
        jc      SHORT WriteEEPROM_Fail

        call    Wait_A_While            ; Wait for EEPROM ready
        call    Wait_A_While            ; Wait for EEPROM ready


; Read EEPROM to verify the data

        mov     al, 8
        call    dspWrite            ; out 8 to 2xC
        jc      SHORT WriteEEPROM_Fail

        mov     al, 7
        call    dspWrite            ; out 7 to 2xC
        jc      SHORT WriteEEPROM_Fail

        mov     al, bh
        call    dspWrite            ; out EEPROM address to 2xC
        jc      SHORT WriteEEPROM_Fail

        call    Wait_A_While            ; Wait for EEPROM ready

        call    dspRead           ; read to verify
        jc      SHORT WriteEEPROM_Fail

        cmp     al, bl                  ; Is write value correct ?
        je      SHORT WriteEEPROM_Success

        loop    SHORT WriteEEPROM_Loop

WriteEEPROM_Fail:
        Trace_Out "<Aztech> : Write to EEPROM fail !!!"

        stc
        popad
        ret

WriteEEPROM_Success:
        Trace_Out "<Aztech> : Write to EEPROM successfully"

        clc
        popad
        ret

EndProc WriteEEPROM

;---------------------------------------------------------------------------;
;
;   StoreConfigRegs
;
;   DESCRIPTION:
;       Store configuration registers
;
;   ENTRY:
;       EDI contains SSI pointer.
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc StoreConfigRegs

        Trace_Out "<Aztech> : Backup configure reg. to EEPROM"

        pushad

        cmp     [edi.ssi_bAZT_CardType], AZTECH_WASHINGTON
        jz      SHORT SCR_WST_EEPROM
        cmp     [edi.ssi_bAZT_CardType], AZTECH_NOVA
        jz      SHORT SCR_NOVA_EEPROM

        Trace_Out "<Aztech> : It seems not NOVA16 or Washington16 !!!"

        jmp     SHORT SCR_Fail

SCR_WST_EEPROM:
        mov     ah, 0bh                 ; EEPROM address 0bh
        mov     ecx, 4                   ; count = 4, store 4 regs
        jmp     SHORT SCR_RegsPort

SCR_NOVA_EEPROM:
        mov     ah, 0ch                 ; EEPROM address 0ch
        mov     ecx, 3                   ; count = 3, store 3 regs

SCR_RegsPort:
        movzx   edx, [edi.ssi_wIOAddressSB]
        add     dx, 0400h               ; aztech config. reg. addr.

SCR_Loop:
        in      al, dx                  ; AL = value, AH = EEPROM add.
        call    WriteEEPROM
        inc     ah
        inc     dx
        loop    SHORT SCR_Loop

        Trace_Out "<Aztech> : Backup EEPROM successfully"

        clc
        popad
        ret

SCR_Fail:
        Trace_Out "<Aztech> : Backup EEPROM fail !!!"

        stc
        popad
        ret

EndProc StoreConfigRegs

VxD_LOCKED_CODE_ENDS

;==============================================================================
;                             P N P   C O D E
;==============================================================================

VxD_PNP_CODE_SEG

;---------------------------------------------------------------------------;
;
;   AZT_Init_Config
;
;   DESCRIPTION:
;       Initial Aztech configure register based on information retrieved
;       from CONFIGMG. Verify base port and set up sound card to WSS
;       compatible mode.
;
;   ENTRY:
;
;   EXIT:
;       EAX is CR_SUCCESS if successful, otherwise failure code
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc _AZT_Init_Config, PUBLIC

        wBaseSB         equ     [ebp + 8]
        wBaseAutoSel    equ     [ebp + 12]
        wSBIRQ          equ     [ebp + 16]
        wSBDMA          equ     [ebp + 20]


        Trace_Out "<Aztech> : Init_Config"

        push    ebp
        mov     ebp, esp
        pushad

        ;
        ; Assume failure...
        ;

        mov     [esp.PushAD_EAX], CR_FAILURE

        Trace_Out "<Aztech> : Set up Base IO to configure register"

        ; Set BASE0 bit first

        xor     ah, ah          ; Assume BASE0(AH bit 0) = 0 , 0220h
        mov     dx, wBaseSB
        cmp     dx, 0220h
        jz      SHORT AIC_Get_BASE0
        inc     ah              ; BASE0(AH bit 0) = 1 , 0240h

AIC_Get_BASE0:

        mov     dx, 0620h       ; Assume Current BASE=220h, DX=Config Reg
        in      al, dx
        test    al, 01h
        jz      SHORT AIC_Set_Base      ; Assume is Correct !!!

        mov     dx, 0640h       ; DX=Config Reg


AIC_Set_Base:
        add     dx, 04          ; DX=BASE+404
        in      al, dx
        or      al, 80h
        out     dx, al          ; Set MCE

        sub     dx, 04          ; DX=BASE+400
        in      al, dx
        and     al, 0FEh
        or      al, ah
        out     dx, al          ; Set BASE0 in Config Reg

        mov     dx, wBaseSB
        add     dx, 0404h       ; DX=BASE+404

        in      al, dx
        and     al, 7Fh
        out     dx, al          ; Clear MCE

        Trace_Out "<Aztech> : Test BASE Port"

        mov     dx, wBaseSB
        call    dspReset    ; Test BASE Port
        jc      AIC_Failure

        Trace_Out "<Aztech> : Test card type ( NOVA16 or Washington16 ??? )"

        mov     dx, wBaseSB
        mov     al, 8
        call    dspWrite            ; out 8 to 2xC
        jc      AIC_Failure

        mov     al, 5
        call    dspWrite            ; out 5 to 2xC
        jc      AIC_Failure

        call    dspRead
        jc      AIC_Failure

        cmp     al, AZTECH_WASHINGTON
        jnz     AIC_Not_Washington

        Trace_Out "<Aztech> : Washington16 write to configure register"

        ; Set MCE prepare to write to configure register

        mov     dx, wBaseSB
        add     dx, 404h        ; DX = BASE+404h
        in      al, dx
        or      al, 80h
        out     dx, al          ; Set MCE in BASE+404h

        ; Write to BASE+400h (SBDMA, SBIRQ)

        xor     ah, ah          ; AH = 0
        mov     bx, wSBIRQ
        cmp     bx, 0Ah
        jnz     SHORT AIC_W_Not_IRQ10
        or      ah, 20h
        jmp     AIC_W_Finish_IRQ

AIC_W_Not_IRQ10:
        cmp     bx, 07h
        jnz     SHORT AIC_W_Not_IRQ7
        or      ah, 10h
        jmp     AIC_W_Finish_IRQ

AIC_W_Not_IRQ7:
        cmp     bx, 05h
        jnz     SHORT AIC_W_Not_IRQ5
        or      ah, 08h
        jmp     AIC_W_Finish_IRQ

AIC_W_Not_IRQ5:
        cmp     bx, 09h
        jnz     AIC_W_Finish_IRQ
        or      ah, 04h

AIC_W_Finish_IRQ:
        mov     bx, wSBDMA
        cmp     bx, 03h
        jnz     SHORT AIC_W_Not_DMA3
        or      ah, 0C0h
        jmp     AIC_W_Finish_DMA

AIC_W_Not_DMA3:
        cmp     bx, 01h
        jnz     SHORT AIC_W_Not_DMA1
        or      ah, 080h
        jmp     AIC_W_Finish_DMA

AIC_W_Not_DMA1:
        cmp     bx, 00h
        jnz     SHORT AIC_W_Finish_DMA
        or      ah, 040h

AIC_W_Finish_DMA:
        mov     dx, wBaseSB
        add     dx, 400h        ; DX = BASE+400h
        in      al, dx
        and     al, 03h
        or      al, ah
        out     dx, al          ; Write SBDMA, SBIRQ to BASE+400h

        ; Write to BASE+401h (wBaseAutoSel, Enable MSS)

        xor     ah, ah          ; AH = 0
        mov     bx, wBaseAutoSel
        cmp     bx, 530h
        jnz     SHORT AIC_W_Not_MSBASE_530
        or      ah, 04h
        jmp     SHORT AIC_W_Finish_MSBASE

AIC_W_Not_MSBASE_530:
        cmp     bx, 604h
        jnz     SHORT AIC_W_Not_MSBASE_604
        or      ah, 05h
        jmp     SHORT AIC_W_Finish_MSBASE

AIC_W_Not_MSBASE_604:
        cmp     bx, 0E80h
        jnz     SHORT AIC_W_Not_MSBASE_E80
        or      ah, 06h
        jmp     SHORT AIC_W_Finish_MSBASE

AIC_W_Not_MSBASE_E80:
        cmp     bx, 0F40h
        jnz     SHORT AIC_W_Finish_MSBASE
        or      ah, 07h

AIC_W_Finish_MSBASE:
        mov     dx, wBaseSB
        add     dx, 401h        ; DX = BASE+401h
        in      al, dx
        and     al, 0F8h
        or      al, ah
        out     dx, al          ; Write wBaseAutoSel, Enable MSS to BASE+401h

        ; Clear MCE end of writting to configure register

        mov     dx, wBaseSB
        add     dx, 404h        ; DX = BASE+404h
        in      al, dx
        and     al, 7Fh
        out     dx, al          ; Clear MCE in BASE+404h


        ; Set Washington16 to WSS Mode

        mov     dx, wBaseSB

        mov     al, 09h
        call    dspWrite            ; out 9 to 2xC
        jc      AIC_Failure

        mov     al, 00h
        call    dspWrite            ; out 0 to 2xC
        jc      AIC_Failure

        call    Wait_A_While

        mov     dx, wBaseSB
        add     dx, 404H
        mov     ecx, 10H

AIC_Wait_Loop:
        in      al, dx
        test    al, 08H
        jz      AIC_Finish_ConfigRegs
        call    Wait_A_While
        loop    SHORT AIC_Wait_Loop
        jmp     AIC_Failure

AIC_Not_Washington:
        cmp     al, AZTECH_NOVA
        jnz     AIC_Not_NOVA

        Trace_Out "<Aztech> : NOVA16 write to configure register"

        ; Set MCE prepare to write to configure register

        mov     dx, wBaseSB
        add     dx, 404h        ; DX = BASE+404h
        in      al, dx
        or      al, 80h
        out     dx, al          ; Set MCE in BASE+404h

        ; Write to BASE+401h (SBIRQ)

        xor     ah, ah          ; AH = 0
        mov     bx, wSBIRQ
        cmp     bx, 07h
        jnz     SHORT AIC_N_Not_IRQ7
        or      ah, 08h
        jmp     AIC_N_Finish_IRQ

AIC_N_Not_IRQ7:
        cmp     bx, 05h
        jnz     SHORT AIC_N_Not_IRQ5
        or      ah, 04h
        jmp     AIC_N_Finish_IRQ

AIC_N_Not_IRQ5:
        cmp     bx, 03h
        jnz     SHORT AIC_N_Not_IRQ3
        or      ah, 02h
        jmp     AIC_N_Finish_IRQ

AIC_N_Not_IRQ3:
        cmp     bx, 09h
        jnz     AIC_N_Finish_IRQ
        or      ah, 01h

AIC_N_Finish_IRQ:
        mov     dx, wBaseSB
        add     dx, 401h        ; DX = BASE+401h
        in      al, dx
        and     al, 0F0h
        or      al, ah
        out     dx, al          ; Write SBIRQ to BASE+401h

        ; Write to BASE+402h (wBaseAutoSel, Enable MSS)

        xor     ah, ah          ; AH = 0
        mov     bx, wBaseAutoSel
        cmp     bx, 530h
        jnz     SHORT AIC_N_Not_MSBASE_530
        or      ah, 04h
        jmp     SHORT AIC_N_Finish_MSBASE

AIC_N_Not_MSBASE_530:
        cmp     bx, 604h
        jnz     SHORT AIC_N_Not_MSBASE_604
        or      ah, 05h
        jmp     SHORT AIC_N_Finish_MSBASE

AIC_N_Not_MSBASE_604:
        cmp     bx, 0E80h
        jnz     SHORT AIC_N_Not_MSBASE_E80
        or      ah, 06h
        jmp     SHORT AIC_N_Finish_MSBASE

AIC_N_Not_MSBASE_E80:
        cmp     bx, 0F40h
        jnz     SHORT AIC_N_Finish_MSBASE
        or      ah, 07h

AIC_N_Finish_MSBASE:
        mov     dx, wBaseSB
        add     dx, 402h        ; DX = BASE+402h
        in      al, dx
        and     al, 0F8h
        or      al, ah
        out     dx, al          ; Write wBaseAutoSel, Enable MSS to BASE+401h

        ; Clear MCE end of writting to configure register

        mov     dx, wBaseSB
        add     dx, 404h        ; DX = BASE+404h
        in      al, dx
        and     al, 7Fh
        out     dx, al          ; Clear MCE in BASE+404h

        jmp     SHORT AIC_Finish_ConfigRegs

AIC_Not_NOVA:

        Trace_Out "<Aztech> : This card is not NOVA16 or Washington16. No Configure set !!! "

        jmp     SHORT AIC_Failure

AIC_Finish_ConfigRegs:

        mov     [esp.PushAD_EAX], CR_SUCCESS

        Trace_Out "<Aztech> : _AZT_INI_Config successful"

        popad
        pop     ebp
        ret

AIC_Failure:

        Trace_Out "<Aztech> : Write to configure register failure !!!"

        mov     [esp.PushAD_EAX], CR_DEVICE_NOT_THERE
        popad
        pop     ebp
        ret
        
EndProc _AZT_Init_Config

;---------------------------------------------------------------------------;
;
;   AZT_SSI_Init
;
;   DESCRIPTION:
;       Initial SSI structure state of AZTECH Sound Card.
;       Sound Card Type & Compatible Mode.
;
;   ENTRY:
;       EDI contains SSI pointer.
;
;   EXIT:
;       STC if error, otherwise CLC
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc AZT_SSI_Init, PUBLIC


        Trace_Out "<Aztech> : AZT_SSI_Init"

        pushad

        movzx   edx, [edi.ssi_wIOAddressSB]
        mov     al, 8
        call    dspWrite            ; out 8 to 2xC
        jc      ASI_Exit_Failure

        mov     al, 5
        call    dspWrite            ; out 5 to 2xC
        jc      ASI_Exit_Failure

        call    dspRead
        jc      ASI_Exit_Failure

        cmp     al, AZTECH_WASHINGTON
        jnz     SHORT ASI_Not_Washington
        mov     [edi.ssi_bAZT_CardType], AZTECH_WASHINGTON
        mov     [edi.ssi_bWSTMode], WSS_MODE
        jmp     SHORT ASI_Exit_Success

ASI_Not_Washington:
        cmp     al, AZTECH_NOVA
        jnz     ASI_Exit_Failure
        mov     [edi.ssi_bAZT_CardType], AZTECH_NOVA

ASI_Exit_Success:
        clc
        popad
        ret

ASI_Exit_Failure:
        stc
        popad
        ret

EndProc AZT_SSI_Init

VxD_PNP_CODE_ENDS
end
