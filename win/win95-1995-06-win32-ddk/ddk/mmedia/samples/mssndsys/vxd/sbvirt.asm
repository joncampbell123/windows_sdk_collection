        page    60, 132

;******************************************************************************
        title   SBVIRT.ASM - Sound Blaster virtualization routines
;******************************************************************************
;---------------------------------------------------------------------------;
;
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1992- 1995     Microsoft Corporation.  All Rights Reserved.
;
;---------------------------------------------------------------------------;
;
;   Title:    MSSNDSYS.VXD - MICROSOFT Windows Sound System VxD
;
;   Module:   SBVIRT.ASM - Sound Blaster virtualization routines
;
;   Version:  1.00
;
;   Date:     November 24, 1992
;
;******************************************************************************
;
;   Functional Description:
;
;      Traps given Sound Blaster I/O range and maps to WSS I/O.
;
;******************************************************************************

        .386p

;==============================================================================
;                             I N C L U D E S
;==============================================================================

        .xlist
        include vmm.inc
        include equates.inc
        include debug.inc
        include shell.inc
        include vdmad.inc
        include vkd.inc
        include vpicd.inc
        include configmg.inc
        include mssndsys.inc
        include sbvirt.inc
        .list

;------------------------------------------------------------------------------
;                 E X T E R N A L    R E F E R E N C E S
;------------------------------------------------------------------------------

; 
; External CODEC manipulation functions
;

EXTRN   CODEC_WaitForReady:NEAR
EXTRN   CODEC_RegRead:NEAR
EXTRN   CODEC_RegWrite:NEAR
EXTRN   CODEC_EnterMCE:NEAR
EXTRN   CODEC_LeaveMCE:NEAR
EXTRN   CODEC_EnterTRD:NEAR
EXTRN   CODEC_LeaveTRD:NEAR
EXTRN   CODEC_ExtMute:NEAR
EXTRN   CODEC_SetFormat:NEAR

;
; External acquire/release functions
;

EXTRN   MSSNDSYS_Acquire_SndSys:NEAR
EXTRN   MSSNDSYS_Release_SndSys:NEAR
EXTRN   MSSNDSYS_Force_OPL3_Into_OPL2_Mode:NEAR

;
; External data
;

EXTRN   gbMute:BYTE
EXTRN   gdwVolDnHKHandle:DWORD
EXTRN   gdwVolUpHKHandle:DWORD

;==============================================================================
;                             P n P    D A T A
;==============================================================================

VxD_PAGEABLE_DATA_SEG

;------------------------------------------------------------------------------
; 
; This is the list of DSP offsets for the I/O ports trapped in the SB.
;
; The associated I/O trap dispatch routines will map these to the
; WSS equivalents.
;
; NOTE!  These are only offsets from the DSP Base I/O address. The
;        actual _virtual_ address of the base I/O will be added during
;        the actual mapping into the I/O permissions bitmap.
;
;------------------------------------------------------------------------------

Begin_VxD_IO_Table MSSNDSYS_SBVirt_Port_Table

        VxD_IO  DSP_PORT_CMSD0,         SBVirt_dspNotUsed
        VxD_IO  DSP_PORT_CMSR0,         SBVirt_dspNotUsed
        VxD_IO  DSP_PORT_CMSD1,         SBVirt_dspNotUsed
        VxD_IO  DSP_PORT_CMSR1,         SBVirt_dspNotUsed
        VxD_IO  DSP_PORT_MIXREG,        SBVirt_dspNotUsed
        VxD_IO  DSP_PORT_MIXDATA        SBVirt_dspNotUsed
        VxD_IO  DSP_PORT_RESET,         SBVirt_dspReset
        VxD_IO  DSP_PORT_07h,           SBVirt_dspNotUsed
        VxD_IO  DSP_PORT_FMD0,          SBVirt_dspFMD0
        VxD_IO  DSP_PORT_FMR0,          SBVirt_dspFMR0
        VxD_IO  DSP_PORT_READ,          SBVirt_dspRead
        VxD_IO  DSP_PORT_0Bh,           SBVirt_dspNotUsed
        VxD_IO  DSP_PORT_WRITE,         SBVirt_dspWrite
        VxD_IO  DSP_PORT_0Dh,           SBVirt_dspNotUsed
        VxD_IO  DSP_PORT_DATAAVAIL,     SBVirt_dspDataAvail
        VxD_IO  DSP_PORT_0Fh,           SBVirt_dspNotUsed

End_VxD_IO_Table MSSNDSYS_SBVirt_Port_Table

VxD_PAGEABLE_DATA_ENDS

VxD_LOCKED_DATA_SEG

;------------------------------------------------------------------------------
;
; This is a list of the DSP commands and the associated dispatch routines.
; When a DSP write occurs and we are not in a specialized write, this
; table is used.
;
;------------------------------------------------------------------------------

DSPCmdTable     label   DWORD

        dd      WAVEWR,                 OFFSET32 dspWaveWrite
        dd      WAVEWRA,                OFFSET32 dspWaveWriteAuto


        dd      SETSAMPRATE,            OFFSET32 dspSetSampleRate
        dd      SETBLCKSIZE,            OFFSET32 dspSetBlockSize
        dd      SPKRON,                 OFFSET32 dspSpeakerOn
        dd      SPKROFF,                OFFSET32 dspSpeakerOff
        dd      SPKRSTATUS,             OFFSET32 dspSpeakerStatus
        dd      HALTDMA,                OFFSET32 dspHaltDMA
        dd      CONTDMA,                OFFSET32 dspContinueDMA
        dd      STOPAUTO,               OFFSET32 dspStopAuto
        dd      RESVD_0,                OFFSET32 dspTableMunge
        dd      RESVD_1,                OFFSET32 dspLoadReg
        dd      RESVD_2,                OFFSET32 dspApplyReg
        dd      INVERTER,               OFFSET32 dspInverter
        dd      GETDSPVER,              OFFSET32 dspGetDSPVersion
        dd      GENERATEINT,            OFFSET32 dspGenerateInt

TOTAL_DSP_CMDS equ ($-DSPCmdTable)/8

VxD_LOCKED_DATA_ENDS

;==============================================================================
;                             P n P   C O D E
;==============================================================================

VxD_PNP_CODE_SEG

;----------------------------------------------------------------------------
;
;   _SBVIRT_PnP_ConfigHandler
;
;   DESCRIPTION:
;      The procedure which we register with CONFIGMG to receive our
;      CONFIG_ messages. This simply calls the SBVIRT_Config_Handler
;      function in cnfgmgr.c
;
;   ENTRY:
;      standard config handler parameters
;
;   EXIT: CONFIGRET code
;
;   USES:
;       FLAGS
;----------------------------------------------------------------------------

BeginProc _SBVIRT_PnP_ConfigHandler, PUBLIC
        
        cf      equ     [ebp + 8]
        scf     equ     [ebp + 12]
        dn      equ     [ebp + 16]
        refdata equ     [ebp + 20]
        flags   equ     [ebp + 24]

        push    ebp
        mov     ebp, esp

        cCall   _SBVIRT_Config_Handler, <cf, scf, dn, refdata, flags>

        pop     ebp
        clc

        ret

EndProc _SBVIRT_PnP_ConfigHandler

;------------------------------------------------------------------------------
;
;   SBVirt_Get_pSSI_From_Child
;
;   DESCRIPTION:
;       Gets the pointer to the Sound System Info (SSI) structure
;       of the parent of the given child devnode.
;
;   PARAMETERS:
;       DEVNODE dn
;          child DevNode from CONFIGMG
;
;   EXIT:
;       pSSI or NULL
;
;------------------------------------------------------------------------------

BeginProc SBVirt_Get_pSSI_From_Child, CCALL, PUBLIC

        ArgVar dn, DWORD

        EnterProc

        sub     esp, 4
        mov     eax, esp

        VxDCall _CONFIGMG_Get_Parent, <eax, dn, 0>
        cmp     eax, CR_SUCCESS
        jnz     SHORT GFC_Exit_Failure

        mov     eax, [esp]
        push    edi
        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <eax, pSSI_FromDevNode>
        mov     eax, edi
        pop     edi
        jmp     SHORT GFC_Exit

GFC_Exit_Failure:
        xor     eax, eax

GFC_Exit:
        add     esp, 4

        LeaveProc

        Return
    
EndProc SBVirt_Get_pSSI_From_Child

;------------------------------------------------------------------------------
;
;   _SBVirt_Set_Config
;
;   DESCRIPTION:
;       Applies the CONFIGMG settings to the emulation device.
;
;   PARAMETERS:
;       DEVNODE dn
;          child DevNode from CONFIGMG
;
;   EXIT:
;       CR_SUCCESS if successful, otherwise CR_FAILURE
;
;------------------------------------------------------------------------------

BeginProc _SBVirt_Set_Config, PUBLIC

        dn      equ     [ebp + 8]
        wBaseSB equ     [ebp + 12]
        pwDMA   equ     [ebp + 16]
        pwIRQ   equ     [ebp + 20]

        push    ebp
        mov     ebp, esp
        push    edi
        push    esi

        cCall   SBVirt_Get_pSSI_From_Child, <dn>
        or      eax, eax
        jz      SHORT SC_Exit_Failure
        mov     edi, eax

        mov     esi, pwDMA
        movsx   eax, word ptr [esi]
        cmp     eax, -1
        je      SHORT SC_No_DMA

        mov     esi, OFFSET32 SBVIRT_Virtual_DMA_Trap
        VxDCall VDMAD_Virtualize_Channel
        mov     [edi.ssi_dwSBDMAHandle], eax

ifdef DEBUG
        jnc     SHORT @F
        Debug_Out "SBVIRT: Unable to virtualize DMA channel!"
@@:
endif
        jc      SHORT SC_Exit_Failure


        jmp     SHORT SC_Get_IRQ

SC_No_DMA:
        movzx   eax, [edi.ssi_bDMADAC]
        mov     word ptr [esi], ax

SC_Get_IRQ:
        movzx   eax, [edi.ssi_bIRQ]
        mov     esi, pwIRQ
        mov     word ptr [esi], ax

        ;
        ; MSSNDSYS_Install_SBVirt_Traps sets EAX to return code
        ;

        cCall   MSSNDSYS_Install_SBVirt_Traps, <edi, wBaseSB>
        jmp     SHORT SC_Exit

SC_Exit_Failure:
        mov     eax, CR_FAILURE

SC_Exit:
        pop     esi
        pop     edi
        pop     ebp
        ret
    
EndProc _SBVirt_Set_Config

;------------------------------------------------------------------------------
;
;   MSSNDSYS_Install_SBVirt_Traps
;
;   DESCRIPTION:
;       Install I/O handlers for given range of Sound Blaster I/O addresses.
;
;   PARAMETERS:
;       PSNDSYSINFO pSSI
;          pointer to SSI
;
;       WORD wBaseSB
;          sound blaster emulation base
;
;   EXIT:
;       CR_SUCCESS if successful, otherwise CR_FAILURE
;
;------------------------------------------------------------------------------

BeginProc MSSNDSYS_Install_SBVirt_Traps, CCALL

        ArgVar  pSSI, DWORD
        ArgVar  wBaseSB, DWORD

        EnterProc

        ;
        ; Some games attempt to play with the Sound Blaster DSP 
        ; before performing a DSP_RESET... assure that the virtual
        ; registers are cleared before allowing access.
        ;

        pushad

        mov     edi, pSSI
        mov     edx, wBaseSB

        mov     esi, edi
        call    SBVirt_dspInitVirtualRegs

        ;
        ; Build table for Install_Mult_IO_Handlers
        ;
        ;
        ; NOTE! This assumes a contiguous block of I/O address space.
        ;

        mov     [edi.ssi_wIOAddressSB], dx
        push    edi

        mov     esi, OFFSET32 MSSNDSYS_SBVirt_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]

        Trace_Out "SBVIRT: Install_SBVirt_Traps: base I/O =#DX"

        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

MIST_AutoSelect_IO_Loop:
        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    MIST_AutoSelect_IO_Loop

        ;
        ; Tell VMM to trap Sound Blaster I/O ports.
        ;

        mov     edx, [esp]                      ; reference data
        cCall   Install_Mult_IO_Handlers_Ex
        pop     edi
        mov     [edi.ssi_hSBStubs], eax
        or      eax, eax

ifdef DEBUG
        jnz     SHORT @F
        Debug_Out "SBVIRT: can't trap Sound Blaster ports!"
@@:
endif
        jz      MIST_Exit_Failure

        mov     [esp.PushAD_EAX], CR_SUCCESS
        jmp     SHORT MIST_Exit

MIST_Exit_Failure:
        mov     [edi.ssi_wIOAddressSB], 0
        mov     [esp.PushAD_EAX], CR_FAILURE

MIST_Exit:
        popad

        LeaveProc

        Return

EndProc MSSNDSYS_Install_SBVirt_Traps

;------------------------------------------------------------------------------
;
;   _SBVirt_Remove_Config
;
;   DESCRIPTION:
;       Removes the emulation device.
;
;   PARAMETERS:
;       DEVNODE dn
;          child DevNode from CONFIGMG
;
;   EXIT:
;       CR_SUCCESS if successful, otherwise CR_FAILURE
;
;------------------------------------------------------------------------------

BeginProc _SBVirt_Remove_Config, PUBLIC

        dn      equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        cCall   SBVirt_Get_pSSI_From_Child, <dn>
        or      eax, eax
        jz      SHORT RC_Exit_Failure

        push    eax
        mov     eax, [eax.ssi_dwSBDMAHandle]
        or      eax, eax
        jz      SHORT RC_No_DMA
        VxDCall VDMAD_Unvirtualize_Channel

RC_No_DMA:
        pop     eax
        cCall   MSSNDSYS_Remove_SBVirt_Traps, <eax>
        mov     eax, CR_SUCCESS
        jmp     SHORT RC_Exit

RC_Exit_Failure:
        mov     eax, CR_FAILURE

RC_Exit:
        pop     ebp
        ret
    
EndProc _SBVirt_Remove_Config

;------------------------------------------------------------------------------
;
;   MSSNDSYS_Remove_SBVirt_Traps
;
;   DESCRIPTION:
;       Removes I/O handlers for given range.
;
;   ENTRY:
;       EDI = pSSI
;
;   EXIT:
;       Nothing.
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc MSSNDSYS_Remove_SBVirt_Traps, CCALL

        ArgVar  pSSI, DWORD

        EnterProc

        pushad

        mov     edi, pSSI
        mov     eax, [edi.ssi_hSBStubs]
        or      eax, eax
        jz      SHORT MRST_No_SBVirtTraps

        ;
        ; Build table for Remove_Mult_IO_Handlers
        ;

        mov     esi, OFFSET32 MSSNDSYS_SBVirt_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]
        movzx   edx, [edi.ssi_wIOAddressSB]

        Trace_Out "Remove_SBVirt_IO_Handlers: base I/O =#DX"

        push    edi                             ; save list node
        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

MRST_SBVirt_IO_Loop:

        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    MRST_SBVirt_IO_Loop

        mov     eax, [esp]
        mov     eax, [eax.ssi_hSBStubs]
        cCall   Remove_Mult_IO_Handlers_Ex
        pop     edi

        mov     [edi.ssi_hSBStubs], 0


MRST_No_SBVirtTraps:

        popad

        LeaveProc

        Return

EndProc MSSNDSYS_Remove_SBVirt_Traps 

;------------------------------------------------------------------------------
;
;   _SBVirt_IsOwned
;
;   DESCRIPTION:
;       Checks the ownership status of the Sound Blaster
;       emulation device.
;
;   PARAMETERS:
;       DEVNODE dn
;          DevNode from CONFIGMG
;
;   EXIT:
;       TRUE if owned, FALSE otherwise
;
;------------------------------------------------------------------------------

BeginProc _SBVirt_IsOwned, PUBLIC

        dn      equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        cCall   _SBVirt_Get_pSSI_From_Child, <dn>
        or      eax, eax
        jz      SHORT SIO_Exit
        test    [eax.ssi_wFlags], SSI_FLAG_SBACQUIRED
        jz      SHORT SIO_Not_Owned

        mov     eax, NOT False
        jmp     SHORT SIO_Exit

SIO_Not_Owned:
        mov     eax, False

SIO_Exit:
        pop     ebp
        ret
    
EndProc _SBVirt_IsOwned

VxD_PNP_CODE_ENDS

;==============================================================================
;                    N O N P A G E A B L E   C O D E
;==============================================================================

VxD_LOCKED_CODE_SEG


;------------------------------------------------------------------------------
;   SBVirt_Virtual_DMA_Trap
;
;   DESCRIPTION:
;      Sound Blaster DMA virtualization for PCMCIA implementations.
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;------------------------------------------------------------------------------

BeginProc SBVirt_Virtual_DMA_Trap, High_Freq

        pushad

        ;
        ; Translate to rep outs for PCMCIA...
        ;

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <eax, pSSI_FromSBDMA>
        or      edi, edi
        jz      SVDT_Exit

        VxDCall VDMAD_Get_Virt_State

        test    dl, DMA_masked
        jz      SHORT SVDT_CheckNotify

        ;
        ; DMA masked
        ;

        test    [edi.ssi_SBVirtRegs.sbvr_fwDMASim], SBVIRT_DMASIMF_MASKED
        jnz     SVDT_Exit


        Trace_Out "SBVIRT: PCMCIA DMA translation: masked"
        or      [edi.ssi_SBVirtRegs.sbvr_fwDMASim], SBVIRT_DMASIMF_MASKED

        jmp     SVDT_JmpDefault

SVDT_CheckNotify:

        ;
        ; Just unmasked it, so determine mode and act...
        ;

        Trace_Out "SBVIRT: PCMCIA DMA translation: unmasked"

        mov     [edi.ssi_SBVirtRegs.sbvr_dwDMASimAddr], esi
        and     [edi.ssi_SBVirtRegs.sbvr_fwDMASim], \
                    not ( SBVIRT_DMASIMF_MASKED + \
                          SBVIRT_DMASIMF_AUTOINIT )


        test    dl, DMA_AutoInit
        jz      SHORT SVDT_JmpDefault

ifdef DEBUG
        Trace_Out "SBVIRT: auto-init DMA requested @#ESI, cb:#CX"
endif
        or      [edi.ssi_SBVirtRegs.sbvr_fwDMASim], SBVIRT_DMASIMF_AUTOINIT

SVDT_JmpDefault:
        popad   
        VxDJmp  VDMAD_Default_Handler

SVDT_Exit:
        popad

        ret
        
EndProc SBVirt_Virtual_DMA_Trap

;------------------------------------------------------------------------------
;
;   SBVirt_Check_Ownership
;
;   DESCRIPTION:
;       Checks the ownership of the OPL3 and CODEC.  Acquires both
;       if unowned, otherwise returns failure.
;
;   ENTRY:
;       EBX = VM to acquire hardware
;       ESI = pSSI
;
;   EXIT:
;       if Carry clear
;           Everything A-OK
;       else Carry set
;           Failure
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc SBVirt_Check_Ownership, PUBLIC, High_Freq

        push    eax
        mov     eax, [esi.ssi_dwCODECOwnerCur]
        or      eax, eax
        jz      SHORT SBCO_NoCODECOwner
        cmp     eax, ebx
        jnz     SBCO_Failed

SBCO_NoCODECOwner:

        test    [esi.ssi_wFlags], SSI_FLAG_SBACQUIRED
        jnz     SBCO_Success

        cmp     [esi.ssi_dwCODECOwnerCur], ebx
        je      SHORT SBCO_Reset_Virtual_SB

	;
	; force a hardware state reset when last handle != sys vm
	;

        push    ebx
	mov	ebx, [esi.ssi_dwCODECOwnerLast]
        VMMCall Test_Sys_VM_Handle
        je      SHORT SBCO_Sys_VM_Last
        mov     [esi.ssi_dwCODECOwnerLast], -1

SBCO_Sys_VM_Last:
        pop     ebx

        Trace_Out "SBVirt: Assigning ownership of CODEC/OPL3 to #EBX"

        mov     eax, fSS_ASS_Acquire_CODEC 
        test    [esi.ssi_wHardwareOptions], DAK_FMSYNTH
        jz      SHORT SBCO_No_OPL3
        or      eax, fSS_ASS_Acquire_OPL3
        
SBCO_No_OPL3:
        mov     edi, esi
        call    MSSNDSYS_Acquire_SndSys
        jc      SBCO_Failed

        test    [esi.ssi_wHardwareOptions], DAK_FMSYNTH
        jz      SHORT SBCO_Reset_Virtual_SB

        call    MSSNDSYS_Force_OPL3_Into_OPL2_Mode

SBCO_Reset_Virtual_SB:

        cmp     [esi.ssi_wCODECClass], CODEC_J_CLASS
        je      SHORT SBCO_Not_KClass

        ;
        ; Use TRD on 'K' or better class CODECs.
        ;

        push    edx
        movzx   edx, [esi.ssi_wCODECBase]
        call    CODEC_EnterTRD
        pop     edx

SBCO_Not_KClass:

        ;
        ; Go ahead and enable the hot keys for this VM...
        ;

        mov     eax, gdwVolUpHKHandle
        VxDCall VKD_Local_Enable_Hot_Key
        mov     eax, gdwVolDnHKHandle
        VxDCall VKD_Local_Enable_Hot_Key

        ;
        ; dspReset shouldn't clear attenuation or current sample
        ; rate (causing clicks.)  Only reset when virtual SB is
        ; acquired.
        ;
        ; Use an invalid current rate since 0 will cause the
        ; CODEC_SetFormat routine to skip programming the CODEC when
        ; a DSP rate between 8 & 9.6 kHz is selected.  Use 0xFF
        ; as the "format not programmed" rate value.
        ;

        mov     [esi.ssi_SBVirtRegs.sbvr_bCurrentRate], -1
        mov     [esi.ssi_SBVirtRegs.sbvr_bAttenuation], 05h

        ;
        ; Flag that the virtual SB was acquired...
        ;

        or      [esi.ssi_wFlags], SSI_FLAG_SBACQUIRED
        or      [edi.ssi_SBVirtRegs.sbvr_fwDMASim], SBVIRT_DMASIMF_MASKED
        and     [edi.ssi_SBVirtRegs.sbvr_fwDMASim], not SBVIRT_DMASIMF_AUTOINIT

        jmp     SHORT SBCO_Success

SBCO_Failed:

        ;
        ; Make sure it's released...
        ;

        mov     edi, esi
        mov     eax, fSS_ASS_Acquire_CODEC or fSS_ASS_Acquire_OPL3
        call    MSSNDSYS_Release_SndSys
        stc
        jmp     SHORT SBCO_Exit

SBCO_Success:
        clc
        
SBCO_Exit:
        pop     eax
        ret

EndProc SBVirt_Check_Ownership

;------------------------------------------------------------------------------
;   I/O trap handler:
;
;      SBVirt_dspNotUsed
;         This is a trap handler for those ports not implemented.
;
;   ENTRY:
;       EBX = VM Handle.
;       ECX = Type of I/O
;       EDX = Port number
;       ESI = reference data (SoundSysInfo structure)
;       EBP = Pointer to client register structure
;
;   EXIT:
;       EAX = data input or output depending on type of I/O
;
;   USES:
;       FLAGS
;
;------------------------------------------------------------------------------

BeginProc SBVirt_dspNotUsed, High_Freq

        xor     eax, eax
        dec     eax
        ret

EndProc SBVirt_dspNotUsed

;------------------------------------------------------------------------------
;
;   SBVirt_dspInitVirtualRegs - Virtual DSP Reset
;
;   ENTRY:
;       ESI = pSSI
;
;   EXIT:
;       NOTHING.
;
;   USES:
;       FLAGS
;
;------------------------------------------------------------------------------

BeginProc SBVirt_dspInitVirtualRegs

        push    eax

        ;
        ;  Reset internal virtual registers
        ;

        xor     eax, eax
        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], eax
        mov     [esi.ssi_SBVirtRegs.sbvr_wSampleRate], ax
        mov     [esi.ssi_SBVirtRegs.sbvr_wBlockSize], ax
        and     [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_DMAREQ
        mov     [esi.ssi_SBVirtRegs.sbvr_dwDataReadPtr], eax
        mov     [esi.ssi_SBVirtRegs.sbvr_dwDataQueuePtr], eax
        mov     [esi.ssi_SBVirtRegs.sbvr_bDataWrite], SB_REG_WRITE_INIT
        mov     [esi.ssi_SBVirtRegs.sbvr_bDataAvail], SB_REG_DATAAVAIL_INIT
        mov     [esi.ssi_SBVirtRegs.sbvr_bSpeakerStatus], STATUS_SPEAKER_OFF
        mov     eax, SB_REG_READ_INIT
        cCall   SBVirt_AddDataToQueue

        pop     eax

        ret

EndProc SBVirt_dspInitVirtualRegs

;------------------------------------------------------------------------------
;   I/O trap handler:
;
;      SBVirt_dspReset - DSP Reset, W/O
;
;   ENTRY:
;       EBX = VM Handle.
;       ECX = Type of I/O
;       EDX = Port number
;       ESI = reference data (SoundSysInfo structure)
;       EBP = Pointer to client register structure
;
;   EXIT:
;       EAX = data input or output depending on type of I/O
;
;   USES:
;       FLAGS
;
;------------------------------------------------------------------------------

BeginProc SBVirt_dspReset, High_Freq

        Trace_Out "SBVirt_dspReset"

        call    SBVirt_Check_Ownership
        jc      SHORT dspReset_Exit

        Dispatch_Byte_IO <SHORT dspReset_Exit>, Fall_Through

        test    al, 1                           ; Q: reset active?
        jz      SHORT dspResetO_NoActive        ;    N: just clear status

        Trace_Out "dspReset active"

        test    [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_DMAREQ 
        jz      SHORT dspReset_NoDMA

        call    dspHaltDMA

dspReset_NoDMA:
        call    SBVirt_dspInitVirtualRegs
        or      [esi.ssi_SBVirtRegs.sbvr_bDataWrite], FLAG_WRITEBUSY

        jmp     SHORT dspReset_Exit

dspResetO_NoActive:

        Trace_Out "dspReset not-active"

        and     [esi.ssi_SBVirtRegs.sbvr_bDataWrite], not FLAG_WRITEBUSY

dspReset_Exit:
        ret

EndProc SBVirt_dspReset

;------------------------------------------------------------------------------
;   I/O trap handler:
;
;      SBVirt_dspFMD0 - FM music data/status port, R/W
;
;
;   ENTRY:
;       EBX = VM Handle.
;       ECX = Type of I/O
;       EDX = Port number
;       ESI = reference data (SoundSysInfo structure)
;       EBP = Pointer to client register structure
;
;   EXIT:
;       EAX = data input or output depending on type of I/O
;
;   USES:
;       EDX, FLAGS
;
;------------------------------------------------------------------------------

BeginProc SBVirt_dspFMD0, High_Freq

        call    SBVirt_Check_Ownership
        jc      SHORT dspFMD0_Exit

        movzx   edx, [esi.ssi_wIOAddressOPL3]

        Dispatch_Byte_IO <SHORT dspFMD0_IN>, Fall_Through

        cmp     ah, 2
        jne     SHORT dspFMD0_OUT_Not_2nd
        add     edx, 2

dspFMD0_OUT_Not_2nd:
        out     dx, al
        jmp     SHORT dspFMD0_Exit

dspFMD0_IN:
        in      al, dx

dspFMD0_Exit:
        clc
        ret

EndProc SBVirt_dspFMD0

;------------------------------------------------------------------------------
;   I/O trap handler:
;
;      SBVirt_dspFMR0 - FM music data/status port, W/O
;
;
;   ENTRY:
;       EBX = VM Handle.
;       ECX = Type of I/O
;       EDX = Port number
;       ESI = reference data (SoundSysInfo structure)
;       EBP = Pointer to client register structure
;
;   EXIT:
;       EAX = data input or output depending on type of I/O
;
;   USES:
;       EDX, FLAGS
;
;------------------------------------------------------------------------------

BeginProc SBVirt_dspFMR0, High_Freq

        call    SBVirt_Check_Ownership
        jc      SHORT dspFMR0_Exit

        Dispatch_Byte_IO <SHORT dspFMR0_IN>, Fall_Through

        movzx   edx, [esi.ssi_wIOAddressOPL3]
        inc     edx
        out     dx, al
        clc
        ret

dspFMR0_IN:
        xor     eax, eax
        dec     eax

dspFMR0_Exit:
        ret

EndProc SBVirt_dspFMR0

;------------------------------------------------------------------------------
;   I/O trap handler:
;
;      SBVirt_dspRead - DSP Read data, R/O
;
;
;   ENTRY:
;       EBX = VM Handle.
;       ECX = Type of I/O
;       EDX = Port number
;       ESI = reference data (SoundSysInfo structure)
;       EBP = Pointer to client register structure
;
;   EXIT:
;       EAX = data input or output depending on type of I/O
;
;   USES:
;       FLAGS
;
;------------------------------------------------------------------------------

BeginProc SBVirt_dspRead, High_Freq

        call    SBVirt_Check_Ownership
        jc      SHORT dspReadO_Exit

        Dispatch_Byte_IO Fall_Through, <SHORT dspReadO_Exit>

        push    ecx

        mov     ecx, [esi.ssi_SBVirtRegs.sbvr_dwDataReadPtr]

        cmp     ecx, [esi.ssi_SBVirtRegs.sbvr_dwDataQueuePtr]
        jz      SHORT dspReadI_NoData

        inc     ecx
        cmp     ecx, MAXLEN_DATAQUEUE
        jb      SHORT dspReadI_NoWrap
        xor     ecx, ecx

dspReadI_NoWrap:
        cmp     ecx, [esi.ssi_SBVirtRegs.sbvr_dwDataQueuePtr]
        jnz     SHORT dspReadI_MoreData

dspReadI_NoData:
        and     [esi.ssi_SBVirtRegs.sbvr_bDataAvail], not FLAG_DATAREADY 

dspReadI_MoreData:
        mov     al, [esi.ssi_SBVirtRegs.sbvr_abDataRead + ecx]
        mov     [esi.ssi_SBVirtRegs.sbvr_dwDataReadPtr], ecx
        pop     ecx

dspReadO_Exit:
        ret

EndProc SBVirt_dspRead

;------------------------------------------------------------------------------
;
;   SBVirt_AddDataToQueue
;
;   ENTRY:
;       ESI = pSSI
;       AL = Data byte to add to queue
;
;   EXIT:
;       NOTHING
;
;   USES:
;       FLAGS
;
;------------------------------------------------------------------------------

BeginProc SBVirt_AddDataToQueue

        push    ecx
        mov     ecx, [esi.ssi_SBVirtRegs.sbvr_dwDataQueuePtr]
        inc     ecx
        cmp     ecx, MAXLEN_DATAQUEUE
        jb      SHORT AD_NoWrap
        xor     ecx, ecx

AD_NoWrap:
ifdef DEBUG
        cmp     ecx, [esi.ssi_SBVirtRegs.sbvr_dwDataReadPtr]
        jne     SHORT @F
        Trace_Out "SBVIRT: Overwriting data in queue..."
@@:       
endif
        mov     [esi.ssi_SBVirtRegs.sbvr_abDataRead + ecx], al
        mov     [esi.ssi_SBVirtRegs.sbvr_dwDataQueuePtr], ecx
        or      [esi.ssi_SBVirtRegs.sbvr_bDataAvail], FLAG_DATAREADY
        pop     ecx
        ret
       
EndProc SBVirt_AddDataToQueue

;------------------------------------------------------------------------------
;   I/O trap handler:
;
;      SBVirt_dspWrite - DSP Write data or command, R/W
;
;
;   ENTRY:
;       EBX = VM Handle.
;       ECX = Type of I/O
;       EDX = Port number
;       ESI = reference data (SoundSysInfo structure)
;       EBP = Pointer to client register structure
;
;   EXIT:
;       EAX = data input or output depending on type of I/O
;
;   USES:
;       FLAGS
;
;------------------------------------------------------------------------------

BeginProc SBVirt_dspWrite, High_Freq

        call    SBVirt_Check_Ownership
        jc      DEBFAR dspWrite_Exit

        Dispatch_Byte_IO <SHORT dspWrite_In>, Fall_Through

        push    eax
        push    ecx

        mov     ecx, [esi.ssi_SBVirtRegs.sbvr_pWriteFunc]
        jecxz   SHORT WO_CheckDSPCmd
        call    ecx

        clc
        jmp     SHORT WO_Exit

WO_CheckDSPCmd:

ifdef DEBUG
        xor     ah, ah
        Trace_Out "WR = #AX"
endif

        movzx   eax, al
        mov     ecx, TOTAL_DSP_CMDS

WO_DSPCmd_Loop:
        cmp     eax, DSPCmdTable[ ecx * 8 - 8 ] ; Q: Is this supported?
        je      short WO_DSPCmd_Virtualize      ;    Y: virtualize it
        loop    WO_DSPCmd_Loop                  ;    N: try next command
        stc                                     ; Here if not valid...
        jmp     SHORT WO_Exit

WO_DSPCmd_Virtualize:
        call    cs:DSPCmdTable[ ecx * 8 - 4 ]   ; call handler
        clc

WO_Exit:
        pop     ecx
        pop     eax
        ret

dspWrite_In:
        mov     al, [esi.ssi_SBVirtRegs.sbvr_bDataWrite]
        and     [esi.ssi_SBVirtRegs.sbvr_bDataWrite], not FLAG_WRITEBUSY
        clc
        ret

dspWrite_Exit:
        stc
        ret

EndProc SBVirt_dspWrite

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspWaveWritePIO - start wave output (programmed I/O)
;
;   ENTRY:
;       ESI = pSSI
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;   STACK:
;------------------------------------------------------------------------------

BeginProc dspWaveWritePIO, High_Freq

        test    [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_PPIO
        jnz     SHORT WWP_AlreadyConfig

        push    eax                             ; save regs
        push    edx

        movzx   edx, [esi.ssi_wCODECBase]       ; get base address

        call    CODEC_EnterMCE

        mov     ah, CODEC_REG_LOWERBASE         ; set PPIO counter
        xor     al, al
        call    CODEC_RegWrite
        mov     ah, CODEC_REG_UPPERBASE
        call    CODEC_RegWrite

        mov     ah, CODEC_REG_INTERFACE
        mov     al, (AD1848_CONFIG_PEN or AD1848_CONFIG_PPIO)
        call    CODEC_RegWrite

        call    CODEC_LeaveMCE

        mov     ah, CODEC_REG_LEFTOUTPUT
        mov     al, [esi.ssi_SBVirtRegs.sbvr_bAttenuation]
        call    CODEC_RegWrite
        mov     ah, CODEC_REG_RIGHTOUTPUT
        call    CODEC_RegWrite

        mov     ah, CODEC_REG_DSP                ; no interrupts
        mov     al, gbMute
        call    CODEC_RegWrite

        or      [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_PPIO

        pop     edx
        pop     eax

WWP_AlreadyConfig:
        mov     [esi.ssi_SBVirtRegs.sbvr_bDataWritePtr], 0
        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], OFFSET32 dspWritePIOData

        ret

EndProc dspWaveWritePIO

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspWritePIOData - programmed I/O wave output
;
;   ENTRY:
;       ESI = pSSI
;
;   CALLED BY:
;       SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;   STACK:
;------------------------------------------------------------------------------

BeginProc dspWritePIOData, High_Freq

        push    edx

        movzx   edx, [esi.ssi_wCODECBase]       ; write direct data
        add     dx, SS_CODEC_DIRECT
        out     dx, al

        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], 0

        pop     edx
        ret

EndProc dspWritePIOData

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspWaveWrite - start wave output (single mode)
;
;   ENTRY:
;       ESI = pSSI
;
;   CALLED BY: 
;       SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;   STACK:
;------------------------------------------------------------------------------

BeginProc dspWaveWrite

        Debug_Out "In dspWaveWrite"

        push    edx

        movzx   edx, [esi.ssi_wCODECBase]       ; get base address

        test    [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_PPIO
        jz      SHORT WW_ModeOK

        cmp     [esi.ssi_wCODECClass], CODEC_J_CLASS
        jne     SHORT @F

        mov     eax, 1                          ; mute it
        call    CODEC_ExtMute

@@:
        call    CODEC_EnterMCE

        mov     ah, CODEC_REG_INTERFACE         ; turn off PEN and PPIO
        mov     al, AD1848_CONFIG_SDC
        call    CODEC_RegWrite

        call    CODEC_LeaveMCE

        cmp     [esi.ssi_wCODECClass], CODEC_J_CLASS
        jne     SHORT @F

        xor     eax, eax                        ; ok to un-mute
        call    CODEC_ExtMute

@@:
        and     [esi.ssi_SBVirtRegs.sbvr_wModeFlags], not MODE_PPIO

WW_ModeOK:

        ;
        ; Some games are ACK'ing the IRQ before EOI'ing. Watch this!
        ;

        push    ecx
        mov     eax, [esi.ssi_dwIRQHandle]
        VxDCall VPICD_Get_Status
        test    ecx, VPICD_Stat_In_Service
        pop     ecx
        jz      SHORT WW_Not_In_Service

        Trace_Out "Write <A>"

        or      [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_IRQACK

WW_Not_In_Service:

        ;
        ; Too many interrupts are being generated by the CODEC
        ; make sure things get shut off properly before the next
        ; DMA transfer.
        ;

        pushfd                                  ; If we get interrupts here,
        cli                                     ; we're toast!

        ;
        ; turn off IRQs
        ;

        mov     ah, CODEC_REG_DSP
        mov     al, gbMute
        call    CODEC_RegWrite

        ;
        ; turn off PEN
        ;

        mov     ah, CODEC_REG_INTERFACE
        mov     al, AD1848_CONFIG_SDC
        call    CODEC_RegWrite

        ;
        ; clear any pending IRQs
        ;

        xor     al, al
        mov     dx, [esi.ssi_wCODECBase]
        add     dx, SS_CODEC_STATUS
        out     dx, al
        sub     dx, SS_CODEC_STATUS

        mov     [esi.ssi_SBVirtRegs.sbvr_wBlockSize], 0
        mov     [esi.ssi_SBVirtRegs.sbvr_bDataWritePtr], 0
        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], OFFSET32 dspWriteWaveSize
        and     [esi.ssi_SBVirtRegs.sbvr_wModeFlags], not( MODE_AUTODMA )

        popfd
        pop     edx
        ret

EndProc dspWaveWrite

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspContinueDMA - restart wave output (single mode)
;
;   ENTRY:
;      Assumes the base count register on the CODEC was already
;      programmed... 
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;------------------------------------------------------------------------------

BeginProc dspContinueDMA, High_Freq

        pushfd
        push    edx
        push    ecx
        push    eax

        Debug_Out "ContinueDMA called"

        cli

        or      [esi.ssi_SBVirtRegs.sbvr_bDataWrite], FLAG_WRITEBUSY
        or      [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_DMAREQ
        movzx   ecx, [esi.ssi_SBVirtRegs.sbvr_wBlockSize]
        movzx   edx, [esi.ssi_wCODECBase]      ; get base address

        jmp     SHORT WWS_2ndEntry

EndProc dspContinueDMA

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspWriteWaveSize - start wave output (single mode)
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;   STACK:
;------------------------------------------------------------------------------

BeginProc dspWriteWaveSize, High_Freq

        pushfd
        push    edx
        push    ecx
        push    eax

        mov     cx, [esi.ssi_SBVirtRegs.sbvr_wBlockSize]
        mov     cl, ch
        mov     ch, al
        mov     [esi.ssi_SBVirtRegs.sbvr_wBlockSize], cx
        mov     cl, [esi.ssi_SBVirtRegs.sbvr_bDataWritePtr]
        inc     cl
        cmp     cl, 2
        jb      WWS_Exit

        ;
        ; save internal virtual reg information
        ;

        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], 0
        mov     [esi.ssi_SBVirtRegs.sbvr_bDataWritePtr], 0

        ;
        ; This is byte count.  When using multibyte formats,
        ; this should be computed as the sample count.
        ;

        cli

        movzx   edx, [esi.ssi_wCODECBase]       ; get base address
        movzx   ecx, [esi.ssi_SBVirtRegs.sbvr_wBlockSize]

        Trace_Out "S = #cx"


WWS_2ndEntry:
        ;
        ; PCMCIA DMA simulation...
        ;

        test    [esi.ssi_wFlags], SSI_FLAG_BUSTYPE_PCMCIA
        jz      SHORT WWS_NotPCMCIA


WWS_SingleBuffer:
        Debug_Out "SBVIRT: PCMCIA single buffer, #CX"

        ;
        ; Copy full buffer to 1 SRAM page...
        ;

        cld
        push    ecx
        inc     ecx
        add     edx, NMC_REG_CTRL_WAVJAMMER
        push    edx
        mov     al, 06h                         ; clear CODEC & 
        out     dx, al                          ;    Host SRAM pointers
        push    esi
        shr     ecx, 1
        movzx   edx, [esi.ssi_wPCMCIA_SRAMBase]
        mov     esi, [esi.ssi_SBVirtRegs.sbvr_dwDMASimAddr]
        rep     outsw
        pop     esi
        pop     edx
        mov     al, 07h                         ; flip host & CODEC SRAM page
        out     dx, al
        sub     edx, NMC_REG_CTRL_WAVJAMMER
        pop     ecx

WWS_NotPCMCIA:
        mov     ah, CODEC_REG_LOWERBASE
        mov     al, cl
        call    CODEC_RegWrite
        mov     ah, CODEC_REG_UPPERBASE
        mov     al, ch
        call    CODEC_RegWrite
        mov     ah, CODEC_REG_LEFTOUTPUT
        mov     al, [esi.ssi_SBVirtRegs.sbvr_bAttenuation]

        ;
        ; Mute DAC when speaker is turned off.
        ;

        cmp     [esi.ssi_SBVirtRegs.sbvr_bSpeakerStatus], STATUS_SPEAKER_ON
        je      SHORT WWS_Speaker_On
        or      al, 80h                         ; mute DAC

WWS_Speaker_On:
        call    CODEC_RegWrite
        mov     ah, CODEC_REG_RIGHTOUTPUT
        call    CODEC_RegWrite

        mov     ah, CODEC_REG_INTERFACE
        mov     al, AD1848_CONFIG_PEN
        call    CODEC_RegWrite

        mov     ah, CODEC_REG_DSP
        mov     al, gbMute
        or      al, 02h                         ; start interrupts
        call    CODEC_RegWrite

        ;
        ; Simulate "busy" DSP.  Some games look for this
        ; transition.
        ;



        or      [esi.ssi_SBVirtRegs.sbvr_bDataWrite], FLAG_WRITEBUSY
        or      [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_DMAREQ
        
        xor     cx, cx

WWS_Exit:
        mov     [esi.ssi_SBVirtRegs.sbvr_bDataWritePtr], cl
        pop     eax
        pop     ecx
        pop     edx
        popfd
        ret

EndProc dspWriteWaveSize

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspWaveWriteAuto - start wave output (auto-init mode)
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;   STACK:
;------------------------------------------------------------------------------

BeginProc dspWaveWriteAuto, High_Freq

        pushfd
        push    eax
        push    ecx
        push    edx

        cli

        movzx   edx, [esi.ssi_wCODECBase]       ; get base address

        cmp     [esi.ssi_wCODECClass], CODEC_J_CLASS
        je      SHORT WWA_Not_KClass

        call    CODEC_LeaveTRD

WWA_Not_KClass:
        test    [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_PPIO
        jz      SHORT WWA_ModeOK

        cmp     [esi.ssi_wCODECClass], CODEC_J_CLASS
        jne     SHORT @F

        mov     eax, 1                          ; mute it
        call    CODEC_ExtMute

@@:
        call    CODEC_EnterMCE

        mov     ah, CODEC_REG_INTERFACE         ; turn off PEN and PPIO
        mov     al, AD1848_CONFIG_SDC
        call    CODEC_RegWrite

        call    CODEC_LeaveMCE

        cmp     [esi.ssi_wCODECClass], CODEC_J_CLASS
        jne     SHORT @F

        xor     eax, eax                        ; ok to un-mute
        call    CODEC_ExtMute

@@:

        and     [esi.ssi_SBVirtRegs.sbvr_wModeFlags], not MODE_PPIO 

WWA_ModeOK:
        movzx   ecx, [esi.ssi_SBVirtRegs.sbvr_wBlockSize]

        or      [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_DMAREQ + MODE_AUTODMA


        movzx   edx, [esi.ssi_wCODECBase]       ; get base address
        mov     ah, CODEC_REG_LOWERBASE
        mov     al, cl
        call    CODEC_RegWrite
        mov     ah, CODEC_REG_UPPERBASE
        mov     al, ch
        call    CODEC_RegWrite

        mov     ah, CODEC_REG_LEFTOUTPUT
        mov     al, [esi.ssi_SBVirtRegs.sbvr_bAttenuation]
        call    CODEC_RegWrite
        mov     ah, CODEC_REG_RIGHTOUTPUT
        call    CODEC_RegWrite

        mov     ah, CODEC_REG_DSP
        mov     al, gbMute
        or      al, 02h                         ; start interrupts
        call    CODEC_RegWrite

        mov     ah, CODEC_REG_INTERFACE
        mov     al, AD1848_CONFIG_PEN
        call    CODEC_RegWrite

        pop     edx
        pop     ecx
        pop     eax
        popfd
        ret
        
EndProc dspWaveWriteAuto

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspSetSampleRate - sets the wave I/O sample rate
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;   STACK:
;------------------------------------------------------------------------------

BeginProc dspSetSampleRate, High_Freq

        mov     [esi.ssi_SBVirtRegs.sbvr_wSampleRate], 0
        mov     [esi.ssi_SBVirtRegs.sbvr_bDataWritePtr], 0
        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], OFFSET32 dspWriteSampleRate
        ret
        
EndProc dspSetSampleRate
        
;------------------------------------------------------------------------------
;   DSP write handler:
;
;      dspWriteSampleRate - sets wave I/O sample rate
;                           (one byte format)  
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;   STACK:
;------------------------------------------------------------------------------

BeginProc dspWriteSampleRate, High_Freq

        push    eax
        push    ebx
        push    edx

        neg     al                      
        movzx   ebx, al                 ; EBX = 256 - AL
        or      ebx, ebx
        jnz     SHORT WSR_NonZero

        mov     ebx, 100h

WSR_NonZero:
        mov     edx, 000Fh
        mov     eax, 4240h              ; DX:AX = 1,000,000
        div     bx                      ; 1,000,000 / (256 - Rate)

        mov     [esi.ssi_SBVirtRegs.sbvr_wSampleRate], ax
        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], 0

        call    CODEC_SetFormat

        pop     edx
        pop     ebx
        pop     eax
        ret

EndProc dspWriteSampleRate

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspSetBlockSize - sets the DMA transfer block size
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;   STACK:
;------------------------------------------------------------------------------

BeginProc dspSetBlockSize, High_Freq

        mov     [esi.ssi_SBVirtRegs.sbvr_wBlockSize], 0
        mov     [esi.ssi_SBVirtRegs.sbvr_bDataWritePtr], 0
        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], OFFSET32 dspWriteBlockSize
        ret
        
EndProc dspSetBlockSize 

;------------------------------------------------------------------------------
;   DSP write handler:
;
;      dspWriteBlockSize - sets the DMA transfer block size
;                          (two byte format)
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;   STACK:
;------------------------------------------------------------------------------

BeginProc dspWriteBlockSize, High_Freq

        push    edx
        movzx   edx, [esi.ssi_SBVirtRegs.sbvr_wBlockSize]
        mov     dl, dh
        mov     dh, al
        mov     [esi.ssi_SBVirtRegs.sbvr_wBlockSize], dx
        mov     dl, [esi.ssi_SBVirtRegs.sbvr_bDataWritePtr]
        inc     dl
        cmp     dl, 2
        jb      SHORT dspWBS_Exit

        cmp     edx, 16
        jae     SHORT WBS_Size_OK
        mov     edx, 16
        mov     [esi.ssi_SBVirtRegs.sbvr_wBlockSize], dx

WBS_Size_OK:

        xor     edx, edx
        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], edx

dspWBS_Exit:
        mov     [esi.ssi_SBVirtRegs.sbvr_bDataWritePtr], dl
        pop     edx
        ret

EndProc dspWriteBlockSize

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspSpeakerOn - turns on the speaker output
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;   STACK:
;------------------------------------------------------------------------------

BeginProc dspSpeakerOn, High_Freq

        mov     [esi.ssi_SBVirtRegs.sbvr_bSpeakerStatus], STATUS_SPEAKER_ON

        ret
        
EndProc dspSpeakerOn

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspSpeakerOff - turns off the speaker output
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;   STACK:
;------------------------------------------------------------------------------

BeginProc dspSpeakerOff, High_Freq

        mov     [esi.ssi_SBVirtRegs.sbvr_bSpeakerStatus], STATUS_SPEAKER_OFF

        ret
        
EndProc dspSpeakerOff

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspSpeakerStatus - sets up the speaker status for next DSP read
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;   STACK:
;------------------------------------------------------------------------------

BeginProc dspSpeakerStatus, High_Freq

        push    eax

        ;
        ;  Set up status for speaker (next DSP read)
        ;

        mov     al, [esi.ssi_SBVirtRegs.sbvr_bSpeakerStatus]
        cCall   SBVirt_AddDataToQueue

        pop     eax
        ret

EndProc dspSpeakerStatus

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspStopAuto - stops the DMA transmission (WSS side)
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;   STACK:
;------------------------------------------------------------------------------

BeginProc dspStopAuto

        Trace_Out "dspStopAuto"

        cmp     [esi.ssi_wCODECClass], CODEC_J_CLASS
        je      SHORT SA_Not_KClass

        push    edx
        movzx   edx, [esi.ssi_wCODECBase]
        call    CODEC_EnterTRD
        pop     edx

SA_Not_KClass:
        and     [esi.ssi_SBVirtRegs.sbvr_wModeFlags], not MODE_AUTODMA
        ret

EndProc dspStopAuto

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspHaltDMA - stops the DMA transmission (WSS side)
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;   STACK:
;------------------------------------------------------------------------------

BeginProc dspHaltDMA, High_Freq

        push    eax
        push    edx

        ;
        ; turn off DAC outputs and minimize ADC gain
        ;

        movzx   edx, [esi.ssi_wCODECBase]       ; get base address

        mov     ah, CODEC_REG_LEFTOUTPUT
        mov     al, 03Fh
        call    CODEC_RegWrite
        inc     ah
        call    CODEC_RegWrite

        ;
        ; kill DMA
        ;

        mov     al, AD1848_CONFIG_SDC
        mov     ah, CODEC_REG_INTERFACE
        call    CODEC_RegWrite

        ;
        ; tell CODEC to shut-off
        ;

        mov     ah, CODEC_REG_DSP
        mov     al, gbMute
        call    CODEC_RegWrite

        ;
        ; clear any pending IRQs
        ;

        xor     al, al
        add     dx, SS_CODEC_STATUS
        out     dx, al
        sub     dx, SS_CODEC_STATUS

        and     [esi.ssi_SBVirtRegs.sbvr_wModeFlags], not( MODE_DMAREQ )

        pop     edx
        pop     eax
        ret

EndProc dspHaltDMA

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspInverter - sets up for the next byte to be "processed".
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;
;------------------------------------------------------------------------------

BeginProc dspInverter

        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], OFFSET32 dspWriteInverterData
        ret

EndProc dspInverter

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspWriteInverterData - not's the value and returns it.
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;
;------------------------------------------------------------------------------

BeginProc dspWriteInverterData

        push    eax

        not     al
        cCall   SBVirt_AddDataToQueue
        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], 0

        pop     eax
        ret

EndProc dspWriteInverterData

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspTableMunge - sets up for the next byte to be "processed".
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;
;------------------------------------------------------------------------------

BeginProc dspTableMunge

        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], OFFSET32 dspWriteTableMunge
        ret

EndProc dspTableMunge

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspWriteTableMunge - stores the value, munges with DMA and returns.
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;
;------------------------------------------------------------------------------

BeginProc dspWriteTableMunge

        push    esi
        push    edx
        push    ecx
        push    eax

        ;
        ; NOTE!!!  The below code assumes eax is at [esp].  BEWARE
        ;          of this assumption if you change the code!
        ;

        mov     eax, [esi.ssi_dwSBDMAHandle]
        or      eax, eax
        jnz     SHORT WTM_GetVirtState
        mov     eax, [esi.ssi_dwDMADACHandle]

WTM_GetVirtState:
        VxDCall VDMAD_Get_Virt_State

        Trace_Out "TableMunge: retrieved virtual state for #EAX"

        test    dl, DMA_masked
        jnz     SHORT WTM_Cant_WriteMem

        mov     al, byte ptr [esp]
        mov     ah, [esi.ssi_SBVirtRegs.sbvr_bTableMunge]

        push    ebx                             ; save VM handle

        test    [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_FLOPPED
        jnz     SHORT WTM_2ndByte

WTM_1stByte:
        mov     ah, al
        and     ah, 06h
        shl     ah, 1
        mov     bl, 40h
        test    al, 10h
        jz      SHORT WTM1B_NotSet
        mov     bl, 20h

WTM1B_NotSet:
        sub     bl, ah
        add     al, bl
        xor     ah, ah
        mov     [esi.ssi_SBVirtRegs.sbvr_bTableMunge], al
        or      [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_FLOPPED
        jmp     SHORT WTM_WriteDMA

WTM_2ndByte:
        xor     al, 0a5h
        add     al, ah
        mov     [esi.ssi_SBVirtRegs.sbvr_bTableMunge], al

WTM_WriteDMA:
        mov     byte ptr [esi], al
        inc     esi
        dec     ecx
        cmp     ecx, -1
        jne     SHORT @F
        or      dl, DMA_masked
@@:
        pop     ebx                             ; restore VM handle

        mov     eax, [esi.ssi_dwSBDMAHandle]
        or      eax, eax
        jnz     SHORT WTM_SetVirtState
        mov     eax, [esi.ssi_dwDMADACHandle]

WTM_SetVirtState:
        VxDCall VDMAD_Set_Virt_State

WTM_Cant_WriteMem:
        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], 0
        pop     eax
        pop     ecx
        pop     edx
        pop     esi
        ret

EndProc dspWriteTableMunge

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspLoadReg - sets up for the next byte to be "processed".
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;
;------------------------------------------------------------------------------

BeginProc dspLoadReg

        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], OFFSET32 dspWriteLoadReg
        ret

EndProc dspLoadReg

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspWriteLoadReg - stores the value and returns.
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;
;------------------------------------------------------------------------------

BeginProc dspWriteLoadReg

        mov     [esi.ssi_SBVirtRegs.sbvr_bApplyReg], al
        mov     [esi.ssi_SBVirtRegs.sbvr_pWriteFunc], 0
        ret

EndProc dspWriteLoadReg

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspApplyReg - returns the byte written in the reserved
;                    register.
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;
;------------------------------------------------------------------------------

BeginProc dspApplyReg

        push    eax
        mov     al, [esi.ssi_SBVirtRegs.sbvr_bApplyReg]
        cCall   SBVirt_AddDataToQueue
        pop     eax
        ret

EndProc dspApplyReg

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspGetDSPVersion - stuffs the DSP version into the virtual
;                         read register.
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;
;------------------------------------------------------------------------------

BeginProc dspGetDSPVersion, High_Freq

        push    eax

        mov     eax, DSP_RETURN_DSP20
        cCall   SBVirt_AddDataToQueue
        shr     eax, 8
        cCall   SBVirt_AddDataToQueue
        mov     eax, DSP_RETURN_DSPVERPLUS
        cCall   SBVirt_AddDataToQueue
        shr     eax, 8
        cCall   SBVirt_AddDataToQueue

        pop     eax
        ret
        
EndProc dspGetDSPVersion

;------------------------------------------------------------------------------
;   DSP command handler:
;
;      dspGenerateInt - generates a test interrupt
;
;   ENTRY:
;
;   CALLED BY: SBVirt_dspWrite        
;
;   EXIT:
;
;   USED:  Flags
;
;------------------------------------------------------------------------------

BeginProc dspGenerateInt, High_Freq

        Trace_Out "dspGenerateInt"

        ;
        ; VPICD provides services, use em!
        ;

        push    eax
        mov     eax, [esi.ssi_dwIRQHandle]
        VxDCall VPICD_Set_Int_Request
        pop     eax

        ret

EndProc dspGenerateInt

;------------------------------------------------------------------------------
;   I/O trap handler:
;
;      sb_dspDataAvail - DSP Data available status, R/O
;
;   ENTRY:
;
;
;   EXIT:
;
;   USES:
;      BX,Flags
;
;------------------------------------------------------------------------------

BeginProc SBVirt_dspDataAvail, High_Freq

        call    SBVirt_Check_Ownership
        jc      dspDataAvailO_Exit

        Dispatch_Byte_IO Fall_Through, <dspDataAvailO_Exit>

        push    eax
        push    ecx
        push    edx

        test    [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_AUTODMA
        jz      SHORT dspDAI_NotAuto

        pushfd
        cli

        movzx   edx, [esi.ssi_wCODECBase]       ; get base address
        add     edx, SS_CODEC_STATUS            ; ACK interrupt
        out     dx, al
        sub     edx, SS_CODEC_STATUS

        popfd
        jmp     SHORT dspDAI_Exit

dspDAI_NotAuto:
        test    [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_IRQACK
        jz      SHORT @F
        and     [esi.ssi_SBVirtRegs.sbvr_wModeFlags], not MODE_IRQACK
        jmp     SHORT dspDAI_Exit

@@:
        mov     eax, [esi.ssi_dwIRQHandle]
        VxDCall VPICD_Get_Status
        test    ecx, VPICD_Stat_In_Service
        jz      SHORT dspDAI_Exit

        Trace_Out "<A>"

        pushfd                                  ; If we get interrupts here,
        cli                                     ; we're toast!

        movzx   edx, [esi.ssi_wCODECBase]       ; get base address

        mov     ah, CODEC_REG_INTERFACE         ; turn off PEN and PPIO
        mov     al, AD1848_CONFIG_SDC
        call    CODEC_RegWrite

        mov     ah, CODEC_REG_DSP               ; turn off interrupts
        mov     al, gbMute
        call    CODEC_RegWrite

        add     edx, SS_CODEC_STATUS            ; ACK interrupt
        out     dx, al
        sub     edx, SS_CODEC_STATUS

        popfd

        test    [esi.ssi_SBVirtRegs.sbvr_wModeFlags], MODE_DMAREQ
        jz      SHORT dspDAI_Exit

        movzx   ecx, [esi.ssi_SBVirtRegs.sbvr_wBlockSize]
        or      ecx, ecx
        jnz     SHORT dspDAI_Exit

        Trace_Out "<H>"

        call    dspHaltDMA

dspDAI_Exit:
        pop     edx
        pop     ecx
        pop     eax

        mov     al, [esi.ssi_SBVirtRegs.sbvr_bDataAvail]

        clc
        ret

dspDataAvailO_Exit:
        stc
        ret

EndProc SBVirt_dspDataAvail

;------------------------------------------------------------------------------
;
;   SBVirt_dspCleanUp
;
;   DESCRIPTION:
;      Some games leave outstanding DMA requests when shutting
;      down.  This results in a repeating DMA buffer, SO,
;      we'll make sure that things get cleaned up before
;      exiting.
;
;   ENTRY:
;      EBX = VM handle
;      EDI = pSSI
;
;   EXIT:
;      NOTHING.
;
;   USES:
;      Flags
;
;------------------------------------------------------------------------------

BeginProc SBVirt_dspCleanUp, PUBLIC

        push    eax

        cmp     [edi.ssi_wCODECClass], CODEC_J_CLASS
        je      SHORT @F

        ;
        ; Clear TRD if we set it... ('K' or better class CODECs).
        ;

        push    edx
        movzx   edx, [edi.ssi_wCODECBase]
        call    CODEC_LeaveTRD
        pop     edx

@@:
        ;
        ; Disable hot keys when VM releases the virtual SB.
        ;

        mov     eax, gdwVolUpHKHandle
        VxDCall VKD_Local_Disable_Hot_Key

        mov     eax, gdwVolDnHKHandle
        VxDCall VKD_Local_Disable_Hot_Key

        ;
        ; Clean up outstanding DMA requests
        ;

        mov     ax, [edi.ssi_SBVirtRegs.sbvr_wModeFlags]
        Trace_Out "dspCleanUp: wModeFlags = #AX"

        test    ax, MODE_DMAREQ
        jz      SHORT dspCleanUp_Exit

        Trace_Out "VM #EBX left outstanding DMA request with virtual SB!!!"

        mov     esi, edi
        call    dspHaltDMA

dspCleanUp_Exit:

        ;
        ;  No owner for the virtual SB.
        ;

        and     [edi.ssi_wFlags], NOT( SSI_FLAG_SBACQUIRED )
        pop     eax

        ret

EndProc SBVirt_dspCleanUp

;------------------------------------------------------------------------------
;
;   MSSNDSYS_Hot_Key_Handler
;
;   DESCRIPTION:
;       This is the hot key handler... enabled only when
;       virtual SB has been enabled.
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc MSSNDSYS_Hot_Key_Handler

        push    eax

        Trace_Out "MSSNDSYS_Hot_Key_Handler: hot key pressed"

        cmp     al, 49h
        jne     SHORT VHKH_VolDown

        ;
        ; Decrease attenuation (volume++)
        ;

        mov     al, [esi.ssi_SBVirtRegs.sbvr_bAttenuation]
        or      al, al
        jz      SHORT VHKH_Exit
        dec     al
        mov     [esi.ssi_SBVirtRegs.sbvr_bAttenuation], al
        jmp     SHORT VHKH_Exit

VHKH_VolDown:

        ;
        ; Increase attentuation (volume--)
        ;

        mov     al, [esi.ssi_SBVirtRegs.sbvr_bAttenuation]
        inc     al
        and     al, 3Fh
        mov     [esi.ssi_SBVirtRegs.sbvr_bAttenuation], al

VHKH_Exit:
        pop     eax
        ret

EndProc MSSNDSYS_Hot_Key_Handler

VxD_LOCKED_CODE_ENDS

end
