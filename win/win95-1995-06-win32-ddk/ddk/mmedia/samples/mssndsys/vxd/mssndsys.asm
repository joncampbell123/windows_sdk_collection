        page    60, 132

;******************************************************************************
        title   MSSNDSYS.ASM - Main module
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
;   Title:    MSSNDSYS.386 - MICROSOFT Windows Sound System 386 Driver
;
;   Module:   MSSNDSYS.ASM - Main module
;
;   Version:  1.00
;
;   Date:     March 17, 1992
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
        include vdmad.inc
        include vkd.inc
        include mssndsys.inc

        include msgmacro.inc
        include messages.inc

        include equates.inc
        .list

;==============================================================================
;             V I R T U A L   D E V I C E   D E C L A R A T I O N
;==============================================================================

ifdef MSSNDSYS
Declare_Virtual_Device mssndsys, MSSNDSYS_Ver_Major, MSSNDSYS_Ver_Minor,\
                       MSSNDSYS_Control, MSSNDSYS_Device_ID,\
                       Undefined_Init_Order, MSSNDSYS_API_Handler,\
                       MSSNDSYS_API_Handler
endif
ifdef AZTECH
Declare_Virtual_Device azt16, MSSNDSYS_Ver_Major, MSSNDSYS_Ver_Minor,\
                       MSSNDSYS_Control, MSSNDSYS_Device_ID,\
                       Undefined_Init_Order, MSSNDSYS_API_Handler,\
                       MSSNDSYS_API_Handler
endif

;------------------------------------------------------------------------------
;                 E X T E R N A L    R E F E R E N C E S
;------------------------------------------------------------------------------

EXTRN CODEC_Reset:NEAR
EXTRN CODEC_Save:NEAR

EXTRN MSSNDSYS_Dyn_Device_Init:NEAR
EXTRN MSSNDSYS_Dyn_Device_Exit:NEAR
EXTRN OPL3_RegWrite:NEAR
EXTRN OPL3_IODelay:NEAR
EXTRN MSSNDSYS_API_Handler:NEAR
EXTRN MSSNDSYS_Add_Instance_To_VM_List:NEAR
EXTRN MSSNDSYS_PnP_New_DevNode:NEAR

ifdef MSSNDSYS
EXTRN SBVirt_dspCleanUp:NEAR
endif

EXTRN gdwVolDnHKHandle:DWORD
EXTRN gdwVolUpHKHandle:DWORD
EXTRN CODEC_InitRegs:BYTE

ifdef AZTECH
;
; External Aztech Washington16 mode change
;
EXTRN   SetMode_To_SBPro:NEAR
EXTRN   SetMode_To_WSS:NEAR
endif

;==============================================================================
;                   N O N P A G E A B L E   D A T A
;==============================================================================

VxD_LOCKED_DATA_SEG

;------------------------------------------------------------------------------
;             G L O B A L   D A T A   D E C L A R A T I O N S
;------------------------------------------------------------------------------

        public  gdwCBOffset
gdwCBOffset     dd      0       ; VM control block offset

        public  ghlSSI
ghlSSI          dd      0       ; Handle to SNDSYSINFO list

        public  gpEndV86App
gpEndV86App     dd      0       ; old service proc      

gdwSSIOffsets   label   dword
        dd      ssi_dn,                 fpSSI_FromDWord
        dd      ssi_dwIRQHandle,        fpSSI_FromDWord
        dd      ssi_dwDMADACHandle,     fpSSI_FromDWord
        dd      ssi_dwDMAADCHandle,     fpSSI_FromDWord
        dd      ssi_wCODECBase,         fpSSI_FromWord
        dd      ssi_wIOAddressOPL3,     fpSSI_FromWord
        dd      ssi_dwSBDMAHandle,      fpSSI_FromDWord

VxD_LOCKED_DATA_ENDS

;===========================================================================;
;                   N O N P A G E A B L E   C O D E
;===========================================================================;

VxD_LOCKED_CODE_SEG

;----------------------------------------------------------------------------
;
;   MSSNDSYS_Control
;
;   DESCRIPTION:
;       Dispatch control messages to the correct handlers. Must be in locked
;       code segment. (All VxD segments are locked in 3.0 and 3.1)
;
;   ENTRY:
;       EAX = Message
;       EBX = VM that the message is for
;       All other registers depend on the message.
;
;   EXIT:
;       Carry clear if no error (or we don't handle the message), set
;       if something bad happened and the message can be failed.
;
;   USES:
;       All registers.
;
;----------------------------------------------------------------------------

BeginProc MSSNDSYS_Control

        Control_Dispatch VM_Critical_Init,        MSSNDSYS_Init_VM_Lists
        Control_Dispatch VM_Not_Executeable,      MSSNDSYS_VM_Not_Executeable
        Control_Dispatch Sys_VM_Init,             MSSNDSYS_Init_VM_Lists

        Control_Dispatch Sys_Dynamic_Device_Init, MSSNDSYS_Dyn_Device_Init
        Control_Dispatch Sys_Dynamic_Device_Exit, MSSNDSYS_Dyn_Device_Exit
        Control_Dispatch PnP_New_DevNode,         MSSNDSYS_PnP_New_DevNode

ifdef DEBUG
        Control_Dispatch Debug_Query,             MSSNDSYS_Debug_Dump
endif

        clc
        ret

EndProc MSSNDSYS_Control

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Get_pSSI_From_XXX
;
;   DESCRIPTION:
;       Retrieves the list node associated with the dwCompare value.
;       If list is empty, non-existant or there is no associated node
;       with the dwCompare value, the function returns with carry set.
;
;   ENTRY:
;       DWORD dwCompare
;          value to compare
;
;       DWORD fdwOptions
;          search options
;
;   EXIT:
;       CLC && EDI contain SNDSYSINFO node, if successful
;       otherwise STC
;
;   USES:
;       EDI, FLAGS
;
;---------------------------------------------------------------------------;

BeginProc _MSSNDSYS_Get_pSSI_From_XXX, PUBLIC

dwCompare       equ     [ebp + 8]
dwSearchFor     equ     [ebp + 12]

        push    ebp
        mov     ebp, esp

        push    eax
        push    ebx
        push    edx
        push    esi
        pushfd

        mov     eax, dwSearchFor
        shl     eax, 3
        mov     edx, gdwSSIOffsets[ eax ]
        mov     eax, gdwSSIOffsets[ eax + 4 ]
        mov     dwSearchFor, eax
        mov     ebx, dwCompare

        mov     esi, ghlSSI 
        or      esi, esi
        jz      SHORT MGSFX_Exit_Failure

        cli
        VMMCall List_Get_First
        jz      SHORT MGSFX_Exit_Failure

MGSFX_Compare:
        mov     edi, [eax.hwl_pSSI]
        test    dword ptr dwSearchFor, fpSSI_FromWord
        jnz     SHORT MGSFX_WordCompare
        cmp     dword ptr [edi + edx], ebx
        jne     SHORT MGSFX_Next

MGSFX_Success:
        popfd
        clc
        jmp     SHORT MGSFX_Exit

MGSFX_WordCompare:
        cmp     word ptr [edi + edx], bx
        je      SHORT MGSFX_Success
        
MGSFX_Next:
        VMMCall List_Get_Next
        jz      SHORT MGSFX_Exit_Failure
        jmp     SHORT MGSFX_Compare

MGSFX_Exit_Failure:
        popfd
        xor     edi, edi
        stc

MGSFX_Exit:
        pop     esi
        pop     edx
        pop     ebx
        pop     eax
        pop     ebp
        ret
        
EndProc _MSSNDSYS_Get_pSSI_From_XXX

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Get_VM_HW_State_From_pSSI
;
;   DESCRIPTION:
;       Retrieves the list node associated with a pSSI.
;       If list is empty, non-existant or the pSSI does not
;       have an associated node, this function returns with
;       carry set.
;
;   ENTRY:
;       EBX = VM handle
;       EDI = pSSI
;
;   EXIT:
;       CLC && ESI contains pointer to VM's hardware state node,
;       otherwise STC
;
;   USES:
;       ESI, FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Get_VM_HW_State_From_pSSI, PUBLIC

        push    eax
        push    ebx
        pushfd

        Assert_VM_Handle ebx

        add     ebx, gdwCBOffset
        mov     esi, [ebx.mscb_hlhws]           ; get hws list handle
        or      esi, esi
        jz      SHORT MGVHS_Exit_Failure

        cli                                     ; playing with the list...

        VMMCall List_Get_First
        jz      SHORT MGVHS_Exit_Failure

MGVHS_Compare:
        cmp     [eax.hws_pSSI], edi
        jne     SHORT MGVHS_Next
        mov     esi, eax                        ; ESI gets HWI structure
        popfd
        clc
        jmp     SHORT MGVHS_Exit
        
MGVHS_Next:
        VMMCall List_Get_Next
        jz      SHORT MGVHS_Exit_Failure
        jmp     SHORT MGVHS_Compare

MGVHS_Exit_Failure:
        Debug_Out "MSSNDSYS: Get_VM_HW_State_From_pSSI failing!!"

        popfd
        xor     esi, esi
        stc

MGVHS_Exit:
        pop     ebx
        pop     eax
        ret

EndProc MSSNDSYS_Get_VM_HW_State_From_pSSI

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_End_V86_App
;
;   DESCRIPTION:
;       Watches the V86 app terminations...
;
;   ENTRY:
;       EBX = VM that performed the int 21, AX = 4B00h (Current VM)
;       ESI = high linear address of PSP which is terminating
;        DX = PSP (segment value) of application which about to be run
;
;       All other registers are reserved for future use.  Do not assume
;       that the high word of the EDX register will be zero.
;
;   EXIT:
;       Nothing.
;
;   USES:
;       Nothing.
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_End_V86_App, High_Freq, Hook_Proc, gpEndV86App

        pushad
        pushfd

        VMMCall Test_Sys_VM_Handle
        je      SHORT EVA_Exit

        mov     esi, ghlSSI
        or      esi, esi
        jz      SHORT EVA_Exit

        cli
        VMMCall List_Get_First
        popfd
        pushfd

        or      eax, eax
        jz      SHORT EVA_Exit

EVA_Compare_CODEC:
        xor     ecx, ecx
        mov     edi, [eax.hwl_pSSI]
        cmp     [edi.ssi_dwCODECOwnerCur], ebx
        jne     SHORT EVA_Compare_OPL3
        or      ecx, fSS_ASS_Acquire_CODEC

EVA_Compare_OPL3:
        cmp     [edi.ssi_dwOPL3OwnerCur], ebx
        jne     SHORT EVA_Release
        or      ecx, fSS_ASS_Acquire_OPL3

EVA_Release:
        jecxz   SHORT EVA_Next

        push    eax
        mov     eax, ecx
        cCall   MSSNDSYS_Release_SndSys
        pop     eax

EVA_Next:
        cli
        VMMCall List_Get_Next
        popfd
        pushfd
        or      eax, eax
        jz      SHORT EVA_Exit
        jmp     SHORT EVA_Compare_CODEC

EVA_Exit:
        popfd
        popad
        jmp     [gpEndV86App]

EndProc MSSNDSYS_End_V86_App

;------------------------------------------------------------------------------
;   MSSNDSYS_Virtual_DMA_Trap
;
;   DESCRIPTION:
;      Assures that DMA_demand_mode is used in DOS VMs or
;      if SingleModeDMA flag is set, forces single mode DMA.
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;------------------------------------------------------------------------------

BeginProc MSSNDSYS_Virtual_DMA_Trap, High_Freq

        VMMCall Test_Sys_VM_Handle              ; no need to check sys VM
        je      SHORT VVDT_Exit

        VxDCall VDMAD_Get_Virt_State

        and     dl, NOT (DMA_mode_mask)         ; assure DMA_demand_mode

        ;
        ; Check for SingleModeDMA for Symphony chipset and
        ; if specified, force it.
        ;

        push    edi
        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <eax, pSSI_FromDMADAC>
        or      edi, edi
        jnz     SHORT VVDT_Check
        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <eax, pSSI_FromDMAADC>

VVDT_Check:
        or      edi, edi
        jz      SHORT VVDT_NotOwned

        test    [edi.ssi_wFlags], SSI_FLAG_SINGLEMODEDMA
        jz      SHORT VVDT_NotOwned
        or      dl, DMA_single_mode

VVDT_NotOwned:
        pop     edi

        xor     dh, dh
        VxDCall VDMAD_Set_Virt_State

VVDT_Exit:
        VxDCall VDMAD_Default_Handler
        ret
        
EndProc MSSNDSYS_Virtual_DMA_Trap

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_IRQ_Hw_Int_Proc
;
;   DESCRIPTION:
;       Reflects interrupt to current owner or handles it if no owner.
;
;   ENTRY:
;       EAX = IRQ Handle
;       EBX = Current VM Handle
;       EDX = reference data (pSSI)
;
;   EXIT:
;
;   USES:
;       Flags, EBX
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_IRQ_Hw_Int_Proc, High_Freq

        push    eax
        push    edi
        push    edx

        mov     edi, edx                        ; edi -> pSSI

        mov     eax, [edi.ssi_dwCODECOwnerCur]  ; all interrupts go to owner
        or      eax, eax                        ; Q: is there an owner?
        jz      MI_Unowned                      ;   N: eat interrupt
        mov     ebx, eax                        ;   Y: set int request to owner

        test    [edi.ssi_wHardwareOptions], DAK_FMSYNTH
        jz      SHORT MI_NoOPL3
        
        ;
        ; If this came from the OPL3, then clear it and EOI.
        ;

        movzx   edx, [edi.ssi_wIOAddressOPL3]
        in      al, dx
        test    al, 80h                        ; these bits are important...
        jz      SHORT MI_NoOPL3

        ;
        ; This silly hacking around is to assure that we reflect
        ; the proper timer status to the VM when we eat the
        ; interrupt... otherwise games may not see the FM device.
        ;

        push    ebx
        mov     ebx, [edi.ssi_dwOPL3OwnerCur]
        or      ebx, ebx
        jz      MI_NoOPL3Stat
        test    [edi.ssi_wFlags], SSI_FLAG_OPL3STATUS
        jnz     MI_NoOPL3Stat

        mov     [edi.ssi_bOPL3Status], al
        or      [edi.ssi_wFlags], SSI_FLAG_OPL3STATUS

        push    edx
        push    esi
        mov     esi, OFFSET32 OPL3_StatusEvent
        mov     edx, edi
        VMMCall Schedule_Global_Event
        pop     esi
        pop     edx

MI_NoOPL3Stat:

        pop     ebx
        VMMCall Test_Sys_VM_Handle              ; Q: Is CODEC owner the SysVM?
        jne     SHORT MI_JustEOI                ;    N: just EOI

        Trace_Out "MSSNDSYS: acking IRQ from OPL3."

        ;
        ; ACK IRQ to OPL3 and get out...
        ;

        push    ebx
        mov     ebx, AD_MASK
        mov     al, 60h
        call    OPL3_RegWrite
        pop     ebx

MI_NoOPL3:



        pop     edx
        pop     edi
        pop     eax

        Assert_VM_Handle ebx

        VxDcall VPICD_Set_Int_Request           ; set int request and return

        clc
        ret

MI_Unowned:

        ;
        ; Spurious interrupt, eat it if it's from the OPL3.
        ;

        test    [edi.ssi_wHardwareOptions], DAK_FMSYNTH
        jz      SHORT MI_AckCODEC

        movzx   edx, [edi.ssi_wIOAddressOPL3]
        in      al, dx
        test    al, 80h
        jz      SHORT MI_AckCODEC

        ;
        ; ACK IRQ to OPL3...
        ;

        push    ebx
        mov     ebx, AD_MASK
        mov     al, 60h
        call    OPL3_RegWrite
        pop     ebx

MI_AckCODEC:

        movzx   edx, [edi.ssi_wCODECBase]

        ;
        ; Acknowledge interrupt from CODEC
        ;

        xor     eax, eax
        add     edx, SS_CODEC_STATUS            ; outp( CODEC_STATUS, 0 )
        out     dx, al
        sub     edx, SS_CODEC_STATUS            ; back to base

MI_JustEOI:

        pop     edx
        pop     edi
        pop     eax

        VxDCall VPICD_Phys_EOI                  ; EOI the PIC
        clc
        ret

EndProc MSSNDSYS_IRQ_Hw_Int_Proc

ifdef MSSNDSYS

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Shared_Hw_Int_Proc
;
;   DESCRIPTION:
;       Reflects interrupt to current owner or handles it if no owner.
;
;   ENTRY:
;       EAX = IRQ Handle
;       EBX = Current VM Handle
;       EDX = reference data (pSSI)
;
;   EXIT:
;
;   USES:
;       Flags, EBX
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Shared_Hw_Int_Proc, High_Freq

        push    eax
        push    edi
        push    edx

        mov     edi, edx                        ; edi -> pSSI

        test    [edi.ssi_wFlags], SSI_FLAG_SHAREDIRQEVENT
        jnz     SHORT SI_DontSchedule

        push    esi
        or      [edi.ssi_wFlags], SSI_FLAG_SHAREDIRQEVENT
        mov     esi, OFFSET32 MSSNDSYS_Shared_IRQ_Event
        mov     edx, edi
        VMMCall Schedule_Global_Event
        pop     esi

SI_DontSchedule:

        movzx   edx, [edi.ssi_wCODECBase]
        add     edx, NMC_REG_CTRL_WAVJAMMER
        in      al, dx
        test    al, NMCSS16_CTRL_CODECINTSTATUS         ; bit 6
        jz      SHORT SI_CheckSCSI

        Trace_Out "MSSNDSYS: IRQ to CODEC"

        mov     eax, [edi.ssi_dwCODECOwnerCur]  ; all interrupts go to owner
        or      eax, eax                        ; Q: is there an owner?
        jz      SI_AckCODEC                     ;   N: eat interrupt
        mov     ebx, eax                        ;   Y: set int request to owner


        mov     eax, [edi.ssi_dwIRQHandle]
        VxDCall VPICD_Set_Int_Request

SI_AckCODEC:

        ;
        ; In the IRQ sharing scenario, we've gotta clean
        ; up our CODEC interrupt status here so SCSI interrupts
        ; can fly.  This prevents dead-lock with the SCSI device.
        ;

        movzx   edx, [edi.ssi_wCODECBase]

        ;
        ; Acknowledge interrupt from CODEC
        ;

        xor     eax, eax
        add     edx, SS_CODEC_STATUS            ; outp( CODEC_STATUS, 0 )
        out     dx, al
        sub     edx, SS_CODEC_STATUS            ; back to base

SI_CheckSCSI:

        movzx   edx, [edi.ssi_wSCSIStatus]
        
        in      al, dx
        test    al, AIC6x60_INTSTATUS
        jz      SI_JustEOI

        Trace_Out "MSSNDSYS: IRQ to SCSI"

        ;
        ; let SCSI take care of this one...
        ;

        pop     edx
        pop     edi
        pop     eax

        stc
        ret

SI_JustEOI:
        pop     edx
        pop     edi
        pop     eax

        VxDCall VPICD_Phys_EOI                  ; EOI the PIC
        clc
        ret

EndProc MSSNDSYS_Shared_Hw_Int_Proc

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Shared_IRQ_Event
;
;   DESCRIPTION:
;       Event handler to pulse IRQ line for shared IRQ strategy.
;
;   ENTRY:
;       EDX = SSI
;
;   EXIT:
;       Next IRQ may fire after exit
;
;   USES:
;       EDI, EDX, EBX
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Shared_IRQ_Event, High_Freq

        pushfd
        cli
        mov     edi, edx

        and     [edi.ssi_wFlags], not SSI_FLAG_SHAREDIRQEVENT

        ;
        ; strobe CODEC IRQ if necessary...
        ;

        mov     eax, [edi.ssi_dwIRQHandle]      ; EAX = IRQ handle
        VxDCall VPICD_Get_Complete_Status
        test    ecx, VPICD_Stat_Virt_Dev_Req + VPICD_Stat_Virt_Req + VPICD_Stat_IRET_Pending
        jnz     SHORT SIE_NotCODEC
        movzx   edx, [edi.ssi_wCODECBase]
        add     edx, NMC_REG_CTRL_WAVJAMMER
        in      al, dx
        test    al, NMCSS16_CTRL_CODECINTSTATUS         ; bit 6
ifdef DEBUG
        jz      SHORT @F
        Trace_Out "MSSNDSYS: CODEC strobe"
@@:
endif
        jnz     SIE_Strobe

SIE_NotCODEC:

        ;
        ; strobe SCSI IRQ if necessary...
        ;

        movzx   edx, [edi.ssi_wSCSIStatus]
        in      al, dx
        test    al, AIC6x60_INTSTATUS
ifdef DEBUG
        jz      SHORT @F
        Trace_Out "MSSNDSYS: SCSI strobe"
@@:
endif
        jz      SHORT SIE_Exit

SIE_Strobe:

        ;
        ; strobe it...
        ;

        movzx   edx, [edi.ssi_wCODECBase]
        add     edx, NMC_REG_CTRL_WAVJAMMER
        mov     al, NMCSS16_CTRL_INTDISABLE             ; bit 3
        out     dx, al
        xor     al, al
        out     dx, al

SIE_Exit:
        popfd
        ret

EndProc MSSNDSYS_Shared_IRQ_Event

endif

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_IRQ_EOI_Proc
;
;   DESCRIPTION:
;       Clears the interrupt request and does the physical EOI
;
;   ENTRY:
;       EAX = IRQ Handle
;       EBX = Current VM Handle
;
;   EXIT:
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_IRQ_EOI_Proc, High_Freq

        ;
        ; Clear int request but make sure that IRQ was physically
        ; in service before physically EOI'ing the PIC.
        ;

        push    ecx

        VxDCall VPICD_Clear_Int_Request     ; clear virtual IRQ request
        VxDCall VPICD_Get_Complete_Status
        test    ecx, VPICD_Stat_Phys_In_Serv
        jz      SHORT EP_Exit
        VxDCall VPICD_Phys_EOI

EP_Exit:
        pop     ecx
        clc
        ret

EndProc MSSNDSYS_IRQ_EOI_Proc

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_IRQ_Mask_Changed_Proc
;
;   DESCRIPTION:
;       If the _owning_ VM is masking or unmasking the IRQ, then we need
;       to set the physical state accordingly.  It is perfectly OK for
;       a non-owning VM to mask/unmask the IRQ and not disturb the owning
;       VM.  We can *NOT* assign ownership when the IRQ is [un]masked
;       because some apps will mask all IRQs so they can perform some
;       operations and then reset the PIC to the previous state.  This
;       operation has nothing to do with the SndSys.
;
;       Ownership will only be assigned if someone _asks_ for it through
;       the provided API, or if someone touches ports on the card.
;
;   ENTRY:
;       EAX = IRQ Handle
;       EBX = Current VM Handle
;       ECX = 0 if VM is unmasking
;
;   EXIT:
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_IRQ_Mask_Changed_Proc, High_Freq
               
        push    edi

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <eax, pSSI_FromIRQ>
        or      edi, edi
        jz      SHORT MSSNDSYS_Auto_Mask

        ;
        ; No mask changing when the IRQ is shared...
        ;

        test    [edi.ssi_wFlags], SSI_FLAG_IRQSHARING
        jnz     SHORT MSSNDSYS_Mask_Exit

        cmp     [edi.ssi_dwCODECOwnerCur], ebx  ; Q: is this the owner?
        jne     SHORT MSSNDSYS_Auto_Mask        ;   N: hmm...

        jecxz   MSSNDSYS_Mask_Unmasking

MSSNDSYS_Mask_Masking:

        Trace_Out "MSSNDSYS_IRQ_Mask_Changed_Proc: MASKING!"

        VxDcall VPICD_Physically_Mask
        jmp     SHORT MSSNDSYS_Mask_Exit

MSSNDSYS_Mask_Unmasking:

        Trace_Out "MSSNDSYS_IRQ_Mask_Changed_Proc: *UN*MASKING!"

        VxDcall VPICD_Physically_Unmask
        jmp     SHORT MSSNDSYS_Mask_Exit

MSSNDSYS_Auto_Mask:

        Trace_Out "MSSNDSYS_IRQ_Mask_Changed_Proc: Auto-Masking(#ECX)"

        ;
        ; No 'owner' of the SndSys hardware -- so we will FORCE the
        ; mask to the default state (ignoring the caller's request).
        ; This is only for the PHYSICAL state -- the virtual state
        ; remains as the VM expects.
        ;

        test    [edi.ssi_wFlags], SSI_FLAG_IRQWASUNMASKED
        jnz     SHORT MSSNDSYS_Mask_Unmasking
        jz      SHORT MSSNDSYS_Mask_Masking

MSSNDSYS_Mask_Exit:
        pop     edi
        clc
        ret

EndProc MSSNDSYS_IRQ_Mask_Changed_Proc

VxD_LOCKED_CODE_ENDS

;===========================================================================;
;                          P A G E A B L E   C O D E
;===========================================================================;

VxD_PAGEABLE_CODE_SEG

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_IO_Default_AutoSelect
;
;   DESCRIPTION:
;       Handle IO trapping of the AutoSelect ports.
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
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_IO_Default_AutoSelect

        ;
        ; Only the System VM can see the AutoSelect all other VM's
        ; will not be able to touch it...
        ;

        VMMCall Test_Sys_VM_Handle
        jne     SHORT VIDP_Fail_IO

        Debug_Out "Fatal ERROR! Sys_VM touching AutoSelect without owning it!"

        jmp     SHORT MSSNDSYS_IO_Default_CODEC

VIDP_Fail_IO:
        Dispatch_Byte_IO Fall_Through, <SHORT VIDP_Exit>

        ;
        ; This will let certain DOS apps see the AutoSelect hardware,
        ; but it's read only.  Reconfiguration is not allowed and some
        ; games will always try to reconfigure to IRQ 7/DMA 1 no matter
        ; what -- they lose if the hardware is not already in that config.
        ;

        in      al, dx

VIDP_Exit:
        Trace_Out "VM other than system VM touching the AutoSelect!!!"

        ret

EndProc MSSNDSYS_IO_Default_AutoSelect

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_IO_Default_CODEC
;
;   DESCRIPTION:
;       Handle IO trapping of the SndSys ports.
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
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_IO_Default_CODEC, High_Freq

        Trace_Out "MSSNDSYS_IO_Default_CODEC, pSSI = #ESI"

        push    eax                             ; save
        mov     eax, [esi.ssi_dwCODECOwnerCur]  ; get current owner...

        cmp     eax, ebx                        ; Q: does this VM own it?

ifdef DEBUG
        jne     SHORT MSSNDSYS_IODef_New_Owner  ;   N: then try to assign owner
        Debug_Out "MSSNDSYS: #EAX OWNS CODEC AND TRAPPING IS ENABLED!?!"
endif

        ;
        ; Trapping should have been disabled for the owning VM, but if
        ; somehow (how?) we have lost track of ownership, allow access
        ; by the owner.
        ;

        je      SHORT MSSNDSYS_IODef_Allow_Access

        ;
        ; This is where the 'auto-acquire' comes into action. If the
        ; auto-acquire is not enabled, we will fail the IO, but we will
        ; be quiet about it.
        ;

MSSNDSYS_IODef_New_Owner:

        or      eax, eax                        ; Q: is there already an owner?
        jnz     SHORT MSSNDSYS_IODef_Not_Owner  ;   Y: yes, fail call!

        ;
        ; We are going to auto-assign the hardware to this VM.  However,
        ; because this is an auto-assign, we'll acquire all of the hardware
        ; so include the OPL3 in the acquire...
        ;
        ; EAX = 0
        ; EBX = VM handle
        ; ECX = type of I/O
        ; EDX = port touched
        ;

        push    ebx
        push    ecx
        push    edx
        mov     edi, esi
        mov     eax, fSS_ASS_Acquire_CODEC
        call    MSSNDSYS_Acquire_SndSys          ; aquire SS
        pop     edx
        pop     ecx
        pop     ebx
        jc      SHORT MSSNDSYS_IODef_Not_Owner   ; fail if cannot acquire!

        Trace_Out "MSSNDSYS_IO_Default_CODEC: autoaquired by VM #EBX"

MSSNDSYS_IODef_Allow_Access:
        pop     eax                             ; restore

        Dispatch_Byte_IO Fall_Through, <SHORT MSSNDSYS_IODef_Real_Out>
        in      al, dx                          ; input from physical port
        jmp     SHORT MSSNDSYS_IODef_Exit

MSSNDSYS_IODef_Real_Out:
        out     dx, al                          ; output to physical port
        Assumes_Fall_Through MSSNDSYS_IODef_Exit

MSSNDSYS_IODef_Exit:
        ret

MSSNDSYS_IODef_Not_Owner:
        mov     eax, fSS_ASS_Acquire_CODEC
        mov     edi, esi
        call    MSSNDSYS_Warning

MSSNDSYS_IODef_Fail_IO:
        pop     eax
        xor     eax, eax                    ; fail input with -1 value
        dec     eax
        ret

EndProc MSSNDSYS_IO_Default_CODEC

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_IO_Default_SB
;
;   DESCRIPTION:
;       Handle IO trapping of the SndSys ports.
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
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_IO_Default_SB, High_Freq

        Trace_Out "MSSNDSYS_IO_Default_SB, pSSI = #ESI"

        push    eax                             ; save
        mov     eax, [esi.ssi_dwCODECOwnerCur]  ; get current owner...

        cmp     eax, ebx                        ; Q: does this VM own it?

ifdef DEBUG
        jne     SHORT MSSNDSYS_IODefSB_New_Owner  ;   N: then try to assign owner
        Debug_Out "MSSNDSYS: #EAX OWNS CODEC AND TRAPPING IS ENABLED!?!"
endif

        ;
        ; Trapping should have been disabled for the owning VM, but if
        ; somehow (how?) we have lost track of ownership, allow access
        ; by the owner.
        ;

        je      SHORT MSSNDSYS_IODefSB_Allow_Access

        ;
        ; This is where the 'auto-acquire' comes into action. If the
        ; auto-acquire is not enabled, we will fail the IO, but we will
        ; be quiet about it.
        ;

MSSNDSYS_IODefSB_New_Owner:

        or      eax, eax                    ; Q: is there already an owner?
        jnz     SHORT MSSNDSYS_IODefSB_Not_Owner  ;   Y: yes, fail call!

        ;
        ; We are going to auto-assign the hardware to this VM.  However,
        ; because this is an auto-assign, we'll acquire all of the hardware
        ; so include the OPL3 in the acquire...
        ;
        ; EAX = 0
        ; EBX = VM handle
        ; ECX = type of I/O
        ; EDX = port touched
        ;

        push    ebx
        push    ecx
        push    edx
        mov     edi, esi
        mov     eax, fSS_ASS_Acquire_CODEC
        call    MSSNDSYS_Acquire_SndSys     ; acquire SS
        pop     edx
        pop     ecx
        pop     ebx
        jc      SHORT MSSNDSYS_IODefSB_Not_Owner   ; fail if cannot acquire!

        Trace_Out "MSSNDSYS_IO_Default_SB: autoaquired by VM #EBX"

MSSNDSYS_IODefSB_Allow_Access:
        pop     eax                         ; restore

ifdef AZTECH
        mov     edi, esi
        call SetMode_To_SBPro
endif

        Dispatch_Byte_IO Fall_Through, <SHORT MSSNDSYS_IODefSB_Real_Out>
        in      al, dx                      ; input from physical port
        jmp     SHORT MSSNDSYS_IODefSB_Exit

MSSNDSYS_IODefSB_Real_Out:
        out     dx, al                      ; output to physical port
        Assumes_Fall_Through MSSNDSYS_IODefSB_Exit

MSSNDSYS_IODefSB_Exit:
        ret

MSSNDSYS_IODefSB_Not_Owner:
        mov     eax, fSS_ASS_Acquire_CODEC
        mov     edi, esi
        call    MSSNDSYS_Warning

MSSNDSYS_IODefSB_Fail_IO:
        pop     eax
        xor     eax, eax                    ; fail input with -1 value
        dec     eax
        ret

EndProc MSSNDSYS_IO_Default_SB

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Force_OPL3_Into_OPL2_Mode
;
;   DESCRIPTION:
;       This function will place the OPL3 into OPL2 mode so that applications
;       in DOS VM's can see the OPL3 in its power on state when it is auto-
;       acquired. If an application looks for an OPL2 (AdLib compat.) and
;       the OPL3 is not in OPL2 compatibility mode, then the application
;       will not be able to 'see' the OPL2.
;
;   ENTRY:
;       EDI = pSSI
;
;   EXIT:
;       The OPL3 will be in OPL2 mode
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Force_OPL3_Into_OPL2_Mode


        push    eax
        push    edx

        Trace_Out "MSSNDSYS: forcing OPL3 into OPL2 mode!"

        ;
        ; "Quiet" the OPL3
        ;

        push    ebx
        push    ecx

        ; Tell the FM chip to use 4-operator mode, and then 
        ; fill in any other random variables.

        movzx   edx, [edi.ssi_wIOAddressOPL3]

        mov     ebx, AD_NEW
        mov     al, 1
        call    OPL3_RegWrite

        mov     ebx, AD_MASK
        mov     al, 60h
        call    OPL3_RegWrite

        mov     ebx, AD_CONNECTION
        mov     al, 3fh
        call    OPL3_RegWrite

        mov     ebx, AD_NTS
        xor     al, al
        call    OPL3_RegWrite

        ;
        ; Turn off the drums, and use high vibrato/modulation
        ;

        mov     ebx, AD_DRUM
        mov     al, 0c0h
        call    OPL3_RegWrite

        ;
        ; Turn off all oscillators
        ;

        mov     ecx, 15h
        mov     ebx, AD_LEVEL
        mov     al, 3fh

VFO_Oscillator_Loop:
        xor     bh, bh
        call    OPL3_RegWrite           ; out( AD_LEVEL + i, 3fh )
        mov     bh, 1                   
        call    OPL3_RegWrite           ; out( AD_LEVEL2 + i, 3fh )
        inc     bl
        loop    VFO_Oscillator_Loop

        ;
        ; Turn off all voices
        ;

        mov     ecx, 8
        mov     ebx, AD_BLOCK
        xor     eax, eax

VFO_Voices_Loop:
        xor     bh, bh
        call    OPL3_RegWrite           ; out( AD_BLOCK + i, 00h )
        mov     bh, 1
        call    OPL3_RegWrite           ; out( AD_BLOCK2 + i, 00h )
        inc     bl
        loop    VFO_Voices_Loop

        ;
        ; Kick into OPL2 mode
        ;

        mov     ebx, AD_CONNECTION      ; out( AD_CONNECTION, 00h )
        xor     eax, eax
        call    OPL3_RegWrite
        mov     ebx, AD_NEW             ; out( AD_NEW, 00h )
        call    OPL3_RegWrite

        ;
        ; Delay cause OPL3 doesn't like instructions immediately
        ; after kicking it into OPL2 mode.
        ; 

        call    OPL3_IODelay
        call    OPL3_IODelay
        call    OPL3_IODelay

        pop     ecx
        pop     ebx

        pop     edx
        pop     eax
        ret

EndProc MSSNDSYS_Force_OPL3_Into_OPL2_Mode

;---------------------------------------------------------------------------;
;
;   OPL3_StatusEvent
;
;   DESCRIPTION:
;       Event handler to enable trapping for the OPL3 status byte
;       after interrupt.
;
;   ENTRY:
;       EDX = SSI
;
;   EXIT:
;       Trapping enabled for OPL3 status port
;
;   USES:
;       EDI, EDX, EBX
;
;---------------------------------------------------------------------------;

BeginProc OPL3_StatusEvent

        mov     edi, edx
        mov     ebx, [edi.ssi_dwOPL3OwnerCur]
        or      ebx, ebx
        jz      SHORT OTE_NoOwner

        Trace_Out "MSSNDSYS: OPL3 status event for VM #EBX, SSI #EDI"

        movzx   edx, [edi.ssi_wIOAddressOPL3]
        VMMCall Enable_Local_Trapping
        ret

OTE_NoOwner:
        Trace_Out "MSSNDSYS: OPL3 status event, but owner has been lost?"
        ret

EndProc OPL3_StatusEvent

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_IO_Default_OPL3
;
;   DESCRIPTION:
;       Handle IO trapping of the OPL3 ports.
;
;   ENTRY:
;       EBX = VM Handle.
;       ECX = Type of I/O
;       EDX = Port number
;       EBP = Pointer to client register structure
;       ESI = reference data (SoundSysInfo structure)
;
;   EXIT:
;       EAX = data input or output depending on type of I/O
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_IO_Default_OPL3, PUBLIC, High_Freq

        Trace_Out "MSSNDSYS_IO_Default_OPL3, pSSI = #ESI"

        test    [esi.ssi_wFlags], SSI_FLAG_OPL3STATUS
        jz      SHORT MSSNDSYS_IODef_OPL3_Normal

        ;
        ; We're here because we ate the status byte at interrupt time.
        ;

        Dispatch_Byte_IO Fall_Through, <SHORT MSSNDSYS_IODef_OPL3_Real_Out>

        and     [esi.ssi_wFlags], NOT SSI_FLAG_OPL3STATUS
        VMMCall Disable_Local_Trapping
        mov     al, [esi.ssi_bOPL3Status]
        ret

MSSNDSYS_IODef_OPL3_Normal:
        push    eax                             ; save
        mov     eax, [esi.ssi_dwOPL3OwnerCur]   ; get OPL3 owner

        cmp     eax, ebx                        ; Q: does this VM own it?

ifdef DEBUG
        jne     SHORT MSSNDSYS_IODef_OPL3_New_Owner
        Debug_Out "MSSNDSYS: #EAX OWNS OPL3 AND TRAPPING IS ENABLED!?!"
endif

        ;
        ; Trapping should have been disabled for the owning VM, but if
        ; somehow (how?) we have lost track of ownership, allow access
        ; by the owner.
        ;

        je      DEBFAR MSSNDSYS_IODef_OPL3_Allow_Access


        ;
        ; This is where the 'auto-acquire' comes into action. If the
        ; auto-acquire is not enabled, we will fail the IO, but we will
        ; be quiet about it.
        ;

MSSNDSYS_IODef_OPL3_New_Owner:

        or      eax, eax                            ; Q: is there already an owner?
        jnz     SHORT MSSNDSYS_IODef_OPL3_Not_Owner ;   Y: yes, fail call!

        ;
        ; We are going to auto-assign the hardware to this VM.  However,
        ; because this is an auto-assign, we'll acquire all of the hardware
        ; so include the OPL3 in the acquire...
        ;
        ; EAX = 0
        ; EBX = VM handle
        ; ECX = type of I/O
        ; EDX = port touched
        ;

        push    ebx
        push    ecx
        push    edx
        mov     edi, esi
        mov     eax, fSS_ASS_Acquire_OPL3
        call    MSSNDSYS_Acquire_SndSys         ; aquire OPL3
        pop     edx
        pop     ecx
        pop     ebx
        jc      SHORT MSSNDSYS_IODef_OPL3_Not_Owner

        ;
        ; The OPL3 _MUST_ be put into OPL2 mode so games looking for
        ; Ad Lib compatibility can see the OPL3... we do this _BEFORE_
        ; allowing the port I/O to go to the physical device.
        ;

        ; ESI = pSSI

        call    MSSNDSYS_Force_OPL3_Into_OPL2_Mode

        Trace_Out "MSSNDSYS_IO_Default_OPL3: autoaquired by VM #EBX"

MSSNDSYS_IODef_OPL3_Allow_Access:
        pop     eax                             ; restore

        Dispatch_Byte_IO Fall_Through, <SHORT MSSNDSYS_IODef_OPL3_Real_Out>
        in      al, dx                          ; input from physical port
        jmp     SHORT MSSNDSYS_IODef_OPL3_Exit

MSSNDSYS_IODef_OPL3_Real_Out:
        out     dx, al                          ; output to physical port

MSSNDSYS_IODef_OPL3_Exit:
        ret

MSSNDSYS_IODef_OPL3_Not_Owner:
        mov     eax, fSS_ASS_Acquire_OPL3
        mov     edi, esi
        call    MSSNDSYS_Warning

MSSNDSYS_IODef_OPL3_Fail_IO:
        pop     eax
        xor     eax, eax                    ; fail input with -1 value
        dec     eax
        ret

EndProc MSSNDSYS_IO_Default_OPL3

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Trapping_Enable_CODEC
;
;   DESCRIPTION:
;       Enables trapping of board's ports in owning VM
;
;   ENTRY:
;       EBX = VM handle to enable trapping in
;       EDI = pSSI
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Trapping_Enable_CODEC

        push    esi
        push    eax
        push    ecx
        push    edx

        Assert_VM_Handle ebx

        ;
        ; step through all ports to re-enable trapping for VM
        ;

        test    [edi.ssi_wHardwareOptions], DAK_AUTOSELECT
        jz      SHORT MSSNDSYS_Enable_Trap_CODEC

MSSNDSYS_Enable_Trap_AutoSelect:
        mov     si, SS_LAST_PORT_PAL
        xor     ecx, ecx
        movzx   edx, [edi.ssi_wIOAddress]               ; always AutoSelect
                                                        ; base if present

MSSNDSYS_Enable_Trap_AutoSelect_IO:
        VMMCall Enable_Local_Trapping
        inc     dx
        cmp     cx, si
        je      SHORT MSSNDSYS_Enable_Trap_CODEC
        inc     cx
        jmp     SHORT MSSNDSYS_Enable_Trap_AutoSelect_IO

MSSNDSYS_Enable_Trap_CODEC:
        mov     si, SS_LAST_PORT_CODEC
        xor     ecx, ecx
        movzx   edx, [edi.ssi_wCODECBase]               ; always CODEC base

MSSNDSYS_Enable_Trap_CODEC_IO:
        VMMCall Enable_Local_Trapping
        inc     dx
        cmp     cx, si
        je      SHORT MSSNDSYS_Enable_Trap_SB
        inc     cx
        jmp     SHORT MSSNDSYS_Enable_Trap_CODEC_IO

MSSNDSYS_Enable_Trap_SB:
        test    [edi.ssi_wFlags], SSI_FLAG_HWSB
        jz      SHORT MSSNDSYS_Trapping_Enable_CODEC_Exit

        mov     si, SS_LAST_PORT_SB
        xor     ecx, ecx
        movzx   edx, [edi.ssi_wIOAddressSB]

MSSNDSYS_Enable_Trap_SB_IO:
        VMMCall Enable_Local_Trapping
        inc     dx
        cmp     cx, si
        je      SHORT MSSNDSYS_Trapping_Enable_CODEC_Exit
        inc     cx
        jmp     SHORT MSSNDSYS_Enable_Trap_SB_IO
        
MSSNDSYS_Trapping_Enable_CODEC_Exit:
        pop     edx
        pop     ecx
        pop     eax
        pop     esi
        clc
        ret

EndProc MSSNDSYS_Trapping_Enable_CODEC

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Trapping_Disable_CODEC
;
;   DESCRIPTION:
;       Disables trapping of board's ports in an owning VM
;
;   ENTRY:
;       EBX = VM handle to disable trapping in
;       EDI = SSI
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Trapping_Disable_CODEC

        push    esi
        push    eax
        push    ecx
        push    edx

        Assert_VM_Handle ebx

        ;
        ; step through all CODEC ports to disable trapping
        ;

        test    [edi.ssi_wHardwareOptions], DAK_AUTOSELECT
        jz      SHORT MSSNDSYS_Disable_Trap_CODEC

MSSNDSYS_Disable_Trap_PAL:

        ;
        ; Remove the PAL from view of the DOS VM.
        ; Only the System VM will be able to see/use the PAL.
        ;

        VMMCall Test_Sys_VM_Handle
        jne     SHORT MSSNDSYS_Disable_Trap_CODEC

        mov     si, SS_LAST_PORT_PAL
        xor     ecx, ecx
        movzx   edx, [edi.ssi_wIOAddress]               ; always AutoSelect
                                                        ; base if present

MSSNDSYS_Disable_Trap_AutoSelect_IO:
        VMMCall Disable_Local_Trapping
        inc     dx
        cmp     cx, si
        je      SHORT MSSNDSYS_Disable_Trap_CODEC
        inc     cx
        jmp     SHORT MSSNDSYS_Disable_Trap_AutoSelect_IO

MSSNDSYS_Disable_Trap_CODEC:
        mov     si, SS_LAST_PORT_CODEC
        xor     ecx, ecx
        movzx   edx, [edi.ssi_wCODECBase]               ; always CODEC base
        add     edx, SS_CODEC_ADDRESS                   ; first CODEC I/O port

MSSNDSYS_Disable_Trap_CODEC_IO:
        VMMCall Disable_Local_Trapping
        inc     dx
        cmp     cx, si
        je      SHORT MSSNDSYS_Disable_Trap_SB
        inc     cx
        jmp     SHORT MSSNDSYS_Disable_Trap_CODEC_IO

MSSNDSYS_Disable_Trap_SB:
        test    [edi.ssi_wFlags], SSI_FLAG_HWSB
        jz      SHORT MSSNDSYS_Trapping_Disable_CODEC_Exit

        mov     si, SS_LAST_PORT_SB
        xor     ecx, ecx
        movzx   edx, [edi.ssi_wIOAddressSB]

MSSNDSYS_Disable_Trap_SB_IO:
        VMMCall Disable_Local_Trapping
        inc     dx
        cmp     cx, si
        je      SHORT MSSNDSYS_Trapping_Disable_CODEC_Exit
        inc     cx
        jmp     SHORT MSSNDSYS_Disable_Trap_SB_IO

MSSNDSYS_Trapping_Disable_CODEC_Exit:
        pop     edx
        pop     ecx
        pop     eax
        pop     esi
        clc
        ret

EndProc MSSNDSYS_Trapping_Disable_CODEC

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_OPL3_Clean_Up
;
;   DESCRIPTION:
;       Clean up the OPL3 when released.  This is so we can assure that
;       timer interrupts have been masked and reset.
;
;
;   ENTRY:
;       EDI = pSSI
;
;   EXIT:
;       IF carry clear
;           success
;       ELSE
;           OPL3 not cleaned up (hardware not present?)
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_OPL3_Clean_Up
        
        push    eax
        push    ebx
        push    edx

        Trace_Out "OPL3 Clean up."

        movzx   edx, [edi.ssi_wIOAddressOPL3]
        mov     ebx, AD_MASK
        mov     eax, 060h                       ; reset timers / disable IRQs
        call    OPL3_RegWrite                   ; Write it to OPL3
        call    OPL3_IODelay

        pop     edx
        pop     ebx
        pop     eax

        ret

EndProc MSSNDSYS_OPL3_Clean_Up

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Trapping_Enable_OPL3
;
;   DESCRIPTION:
;       Enables trapping of OPL3's ports in owning VM
;
;   ENTRY:
;       EBX = VM handle to enable trapping in
;       EDI = pSSI
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Trapping_Enable_OPL3

        push    esi
        push    eax
        push    ecx

        Assert_VM_Handle ebx

        ;
        ; step through all OPL3 ports to re-enable trapping for VM
        ;

        mov     si, SS_LAST_PORT_OPL3
        xor     ecx, ecx
        movzx   edx, [edi.ssi_wIOAddressOPL3]

TEO_TrapEm:
        VMMcall Enable_Local_Trapping
        inc     dx
        cmp     cx, si
        je      SHORT TEO_Exit
        inc     cx
        jmp     SHORT TEO_TrapEm

TEO_Exit:
        pop     ecx
        pop     eax
        pop     esi
        clc
        ret

EndProc MSSNDSYS_Trapping_Enable_OPL3

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Trapping_Disable_OPL3
;
;   DESCRIPTION:
;       Disables trapping of OPL3's ports in an owning VM
;
;   ENTRY:
;       EBX = VM handle to disable trapping in
;       EDI = pSSI
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Trapping_Disable_OPL3

        push    esi
        push    eax
        push    ecx
        push    edx

        Assert_VM_Handle ebx

        ;
        ; step through all OPL3 related ports to disable trapping for VM
        ;

        mov     si, SS_LAST_PORT_OPL3
        xor     ecx, ecx
        movzx   edx, [edi.ssi_wIOAddressOPL3]

TDO_UntrapEm:
        VMMcall Disable_Local_Trapping
        inc     dx
        cmp     cx, si
        je      SHORT TDO_Exit
        inc     cx
        jmp     SHORT TDO_UntrapEm

TDO_Exit:
        pop     edx
        pop     ecx
        pop     eax
        pop     esi
        clc
        ret

EndProc MSSNDSYS_Trapping_Disable_OPL3

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Warning
;
;   DESCRIPTION:
;
;   ENTRY:
;       EAX = warning for: fSS_ASS_Acquire_CODEC, fSS_ASS_Acquire_OPL3
;       EDX = port being touched
;       EBX = VM to bring up warning dlg for
;       EDI = pSSI
;
;   EXIT:
;
;   USES:
;       Flags, ESI, EDI, EAX, ECX
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Warning

        ;
        ; Check to see if warnings are enabled (default).  If they are
        ; then see if we are currently sitting in a warning waiting for
        ; the user's response; if all is clear then put up the warning.
        ;

        test    [edi.ssi_wFlags], SSI_FLAG_DISABLEWARNING
        jnz     SHORT MSSNDSYS_IO_Skip_Warning

        cCall   MSSNDSYS_Get_VM_HW_State_From_pSSI

ifdef DEBUG
        jnc     SHORT @F

        Debug_Out "MSSNDSYS_Warning: failed to get hardware state for VM!"
@@:
endif
        jc      SHORT MSSNDSYS_IO_Skip_Warning

        cmp     eax, fSS_ASS_Acquire_OPL3
        je      SHORT MSSNDSYS_IO_Display_Warning_OPL3

        ;
        ; Display warning for CODEC
        ;

        test    [esi.hws_dwFlags], HWCB_FLAG_ALREADYWARNEDCODEC
        jnz     SHORT MSSNDSYS_IO_Skip_Warning

ifdef DEBUG
        mov     eax, [edi.ssi_dwCODECOwnerCur]         ; get SS owner
        Trace_Out "MSSNDSYS: #EBX is touching CODEC Port #DX--#EAX owns it!!"
endif

        or      [esi.hws_dwFlags], HWCB_FLAG_ALREADYWARNEDCODEC

        GET_MESSAGE_PTR <gszNoAccessMessageCODEC>, ecx
        jmp     SHORT MSSNDSYS_IO_Display_Warning

        ;
        ; Display warning for OPL3
        ;

MSSNDSYS_IO_Display_Warning_OPL3:

        test    [esi.hws_dwFlags], HWCB_FLAG_ALREADYWARNEDOPL3
        jnz     SHORT MSSNDSYS_IO_Skip_Warning

ifdef DEBUG
        mov     eax, [edi.ssi_dwOPL3OwnerCur]           ; get OPL3 owner
        Trace_Out "MSSNDSYS: #EBX is touching OPL3 Port #DX--#EAX owns it!!"
endif

        or      [esi.hws_dwFlags], HWCB_FLAG_ALREADYWARNEDOPL3
        GET_MESSAGE_PTR <gszNoAccessMessageOPL3>, ecx
        Assumes_Fall_Through MSSNDSYS_IO_Display_Warning

MSSNDSYS_IO_Display_Warning:

        ;
        ; Have the SHELL put up an appropriate warning...
        ; ECX -> message to display
        ;  

        mov     eax, MB_OK or MB_ICONEXCLAMATION        ; message box flags
        xor     esi, esi                                ; no callback
        xor     edi, edi                                ; default caption
        VxDcall SHELL_Message

MSSNDSYS_IO_Skip_Warning:

        ret

EndProc MSSNDSYS_Warning

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_VM_Not_Executeable
;
;   DESCRIPTION:
;       This procedure checks whether the VM being destroyed owns the
;       Windows Sound System.  If it does, then dwxxxOwnerCur is cleared
;       and the CODEC/OPL3 is reset.  Note that we reset because the VM
;       could have crashed and left the hardware in an annoying audible
;       state.
;
;   ENTRY:
;       EBX = handle of VM being destroyed
;       EDX = Flags: VNE_Crashed, VNE_Nuked, VNE_CreateFail,
;                    VNE_CrInitFail, VNE_InitFail
;
;   EXIT:
;       Carry clear
;
;   USES:
;       FLAGS, EBX, EAX
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_VM_Not_Executeable

        pushfd

        mov     esi, ghlSSI
        or      esi, esi
        jz      VNE_No_Hardware

        cli

        VMMCall List_Get_First
        jz      SHORT VNE_No_Hardware

VNE_Compare_CODEC:
        popfd                                   ; STI if necessary
        pushfd

        push    eax                             ; save list node
        mov     edi, [eax.hwl_pSSI]
        xor     eax, eax
        cmp     [edi.ssi_dwCODECOwnerCur], ebx
        jne     SHORT VNE_Compare_OPL3

        or      eax, fSS_ASS_Acquire_CODEC
        mov     [edi.ssi_dwCODECOwnerLast], -1  ; not owner anymore/not reset

VNE_Compare_OPL3:
        cmp     [edi.ssi_dwOPL3OwnerCur], ebx
        jne     VNE_Check_For_Release

        or      eax, fSS_ASS_Acquire_OPL3
        mov     [edi.ssi_dwOPL3OwnerLast], -1

VNE_Check_For_Release:
        or      eax, eax
        jz      SHORT VNE_No_Ownership
        call    MSSNDSYS_Release_SndSys

VNE_No_Ownership:
        pop     eax

        cli
        VMMCall List_Get_Next
        jz      SHORT VNE_No_Hardware
        jmp     SHORT VNE_Compare_CODEC

VNE_No_Hardware:

        mov     edi, gdwCBOffset
        xor     esi, esi
        xchg    esi, [ebx + edi].mscb_hlhws
        or      esi, esi
        jz      SHORT VNE_No_List
        VMMCall List_Destroy

VNE_No_List:

        popfd
        clc
        ret

EndProc MSSNDSYS_VM_Not_Executeable

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Init_VM_Lists
;
;   DESCRIPTION:
;       Initializes VM lists and disables hot keys.
;
;   ENTRY:
;       EBX = VM handle
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Init_VM_Lists

BUG <Need to copy Sys VM settings for line-in into VM hardware state>

        Assert_Ints_Enabled

        pushad
        pushfd

        mov     edi, gdwCBOffset

        mov     eax, LF_Alloc_Error + LF_Async
        mov     ecx, size VM_HWSTATE_NODE

        VMMCall List_Create
        jc      IVL_Exit_Failure_NoList

        mov     [ebx + edi].mscb_hlhws, esi

        ;
        ; If no hardware instances, just exit with success but
        ; we're loaded, so it shouldn't happen.
        ;

        mov     esi, ghlSSI 
        or      esi, esi

ifdef DEBUG
        jnz     SHORT @F
        Debug_Out "MSSNDSYS: initialize VM lists with no hardware registered???"
@@:
endif
        jz      SHORT IVL_Disable_HotKeys

        cli
        VMMCall List_Get_First
        sti
        jz      SHORT IVL_Disable_HotKeys

IVL_Instance_Loop:
        mov     edi, [eax.hwl_pSSI]
        call    MSSNDSYS_Add_Instance_To_VM_List
        jc      SHORT IVL_Exit_Failure
        cli
        VMMCall List_Get_Next
        sti
        jz      SHORT IVL_Disable_HotKeys
        jmp     SHORT IVL_Instance_Loop

IVL_Disable_HotKeys:

ifdef MSSNDSYS
        ;
        ; Disable hot keys until VM acquires the virtual SB.
        ;

        mov     eax, gdwVolUpHKHandle               
        VxDCall VKD_Local_Disable_Hot_Key

        mov     eax, gdwVolDnHKHandle
        VxDCall VKD_Local_Disable_Hot_Key
endif

        jmp     SHORT IVL_Exit_Success

IVL_Exit_Failure:
        mov     edi, gdwCBOffset
        mov     esi, [ebx + edi].mscb_hlhws
        or      esi, esi
        jz      SHORT IVL_Exit_Failure_NoList
        VMMCall List_Destroy

IVL_Exit_Failure_NoList:
        popfd
        stc
        jmp     SHORT IVL_Exit

IVL_Exit_Success:
        popfd
        clc

IVL_Exit:
        popad
        ret

EndProc MSSNDSYS_Init_VM_Lists

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Release_SndSys
;
;   DESCRIPTION:
;       This function will releases the SS from ownership by a VM.
;
;   ENTRY:
;       EAX = Flags
;           fSS_ASS_Acquire_CODEC       equ 00000001b
;           fSS_ASS_Acquire_OPL3        equ 00000010b
;       EBX = VM handle wanting to release
;       EDI = SSI
;
;   EXIT:
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Release_SndSys, PUBLIC


        push    eax
        push    edx
        push    esi

        Assert_VM_Handle ebx

        call    MSSNDSYS_Get_VM_HW_State_From_pSSI
ifdef DEBUG
        jnc     SHORT @F

        Debug_Out "MSSNDSYS: Release_SndSys can't get HW state???"
@@:
endif
        jc      RS_Exit

        ;
        ; release OPL3 (release woger, release bwian)
        ;
        ; EAX = flags
        ; EBX = VM handle
        ;

        test    eax, fSS_ASS_Acquire_OPL3
        jz      SHORT RS_Try_CODEC

        cmp     [edi.ssi_dwOPL3OwnerCur], ebx
        jne     SHORT RS_Try_CODEC

        call    MSSNDSYS_OPL3_Clean_Up

        call    MSSNDSYS_Trapping_Enable_OPL3
        mov     [edi.ssi_dwOPL3OwnerCur], 0    ; zero out owner VM handle

        and     [esi.hws_dwFlags], not HWCB_FLAG_ALREADYWARNEDOPL3

        ;
        ; Fall-through and try to release CODEC if specified
        ;

        ;
        ; release CODEC
        ;
        ; EAX = flags
        ; EBX = VM handle
        ; EDI = SSI
        ;

RS_Try_CODEC:
        test    eax, fSS_ASS_Acquire_CODEC
        jz      DEBFAR RS_Exit

        cmp     [edi.ssi_dwCODECOwnerCur], ebx
        jne     DEBFAR RS_Exit

        VMMCall Test_Sys_VM_Handle
        jne     SHORT RS_NotSysVM

        test    [edi.ssi_wFlags], SSI_FLAG_BUSTYPE_PCMCIA
        jnz     SHORT RS_NotSysVM

        Trace_Out "Enabling DMA translation for #EBX"

        mov     eax, [edi.ssi_dwDMADACHandle]
        VxDCall VDMAD_Enable_Translation
        mov     eax, [edi.ssi_dwDMAADCHandle]
        or      eax, eax
        jz      SHORT RS_NotSysVM
        VxDCall VDMAD_Enable_Translation

RS_NotSysVM:

        ;
        ; Make sure we clean-up anything left over from the virtual SB.
        ;
ifdef AZTECH
        call SetMode_To_WSS
endif

ifdef MSSNDSYS
        test    [edi.ssi_wFlags], SSI_FLAG_SBACQUIRED
        jz      SHORT RS_NoSBAcquired
        call    SBVirt_dspCleanUp

RS_NoSBAcquired:
endif
        call    MSSNDSYS_Trapping_Enable_CODEC
        mov     [edi.ssi_dwCODECOwnerCur], 0    ; zero out owner VM handle

        mov     eax, [edi.ssi_dwIRQHandle]      ; EAX = IRQ handle

ifdef DEBUG
        push    ecx
        VxDcall VPICD_Get_Complete_Status
        test    ecx, VPICD_Stat_IRET_Pending+VPICD_Stat_In_Service+VPICD_Stat_Phys_In_Serv+VPICD_Stat_Virt_Req+VPICD_Stat_Phys_Req+VPICD_Stat_Virt_Dev_Req
        jz      SHORT @F
        Debug_Out "MSSNDSYS: Release_SndSys: Releasing with IRQ in service!!! #ECX"
@@:
        pop     ecx
endif

        ;
        ; Mask the IRQ _first_ so no more interrupts will be generated
        ; after we EOI--then clear any pending interrupts that have been
        ; 'set' into the VM by us.
        ;

        ;
        ; EAX = IRQ handle
        ;

        test    [edi.ssi_wFlags], SSI_FLAG_IRQSHARING
        jnz     SHORT RS_SharedIRQ

        VxDcall VPICD_Physically_Mask
        VxDcall VPICD_Phys_EOI

RS_SharedIRQ:
        VxDcall VPICD_Clear_Int_Request         ; clear any pending request

        and     [esi.hws_dwFlags], not HWCB_FLAG_ALREADYWARNEDCODEC

RS_Exit:
        pop     esi
        pop     edx
        pop     eax

        clc
        ret

EndProc MSSNDSYS_Release_SndSys


;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Acquire_SndSys
;
;   DESCRIPTION:
;       This function acquires SndSys for a VM.
;
;   ENTRY:
;       EAX = Flags
;           fSS_ASS_Acquire_CODEC          equ 00000001b
;           fSS_ASS_Acquire_OPL3           equ 00000010b
;       EBX = VM handle to own SS
;       EDI = SSI
;
;   EXIT:
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Acquire_SndSys, PUBLIC

        push    eax
        push    ecx
        push    edx
        push    esi

        Assert_VM_Handle ebx

        ;
        ; acquire CODEC
        ;
        ; EAX = flags
        ; EBX = VM handle
        ; EDI = SSI associated with CODEC base port
        ;

        test    eax, fSS_ASS_Acquire_OPL3
        jz      SHORT AS_Try_For_CODEC

        cmp     [edi.ssi_dwOPL3OwnerCur], 0
        je      SHORT AS_Do_It_OPL3

        cmp     [edi.ssi_dwOPL3OwnerCur], ebx
        je      SHORT AS_Try_For_CODEC
        jmp     SHORT AS_Fail

        ;
        ; EAX = flags
        ; EBX = VM handle
        ;

AS_Do_It_OPL3:

        mov     [edi.ssi_dwOPL3OwnerCur], ebx   ; assign ownership
        call    MSSNDSYS_Trapping_Disable_OPL3

        mov     [edi.ssi_dwOPL3OwnerLast], ebx  ; set last owner appropriately

        Assumes_Fall_Through AS_Try_For_CODEC

        ;
        ; acquire CODEC
        ;
        ; EAX = flags
        ; EBX = VM handle
        ; EDI = SSI associated with CODEC base port
        ;

AS_Try_For_CODEC:

        test    eax, fSS_ASS_Acquire_CODEC
        jz      AS_Exit                                 ; test clears carry

        cmp     [edi.ssi_dwCODECOwnerCur], 0
        je      SHORT AS_Do_It

        cmp     [edi.ssi_dwCODECOwnerCur], ebx
        je      AS_Success

        ;
        ; Failed to acquire SS because it is currently owned
        ;

AS_Fail:
        stc
        jc      AS_Exit

        ;
        ; EAX = flags
        ; EBX = VM handles
        ; EDI = SSI
        ;

AS_Do_It:
        mov     [edi.ssi_dwCODECOwnerCur], ebx          ; assign ownership

        cmp     [edi.ssi_dwCODECOwnerLast], ebx
        je      SHORT AS_Dont_Reset

        mov     ebx, [edi.ssi_dwCODECOwnerLast]
        VMMCall Validate_VM_Handle
        jc      AS_Dont_Save

        call    MSSNDSYS_Get_VM_HW_State_From_pSSI

ifdef DEBUG
        jnc     SHORT @F
        Debug_Out "MSSNDSYS: Acquire_SndSys can't save, pSSI invalid?"
@@:
endif

        jc      SHORT AS_Dont_Save

        ;
        ; ESI = VM's hardware state node (phws)
        ;

        call    CODEC_Save

AS_Dont_Save:
        mov     ebx, [edi.ssi_dwCODECOwnerCur]

        call    MSSNDSYS_Get_VM_HW_State_From_pSSI

ifdef DEBUG
        jnc     SHORT @F
        Debug_Out "MSSNDSYS: Acquire_SndSys can't reset, pSSI invalid?"
@@:
endif

        jc      SHORT AS_Dont_Reset

        ;
        ; ESI = VM's hardware state node (phws)
        ;

        call    CODEC_Reset

AS_Dont_Reset:
        mov     [edi.ssi_dwCODECOwnerLast], ebx

        call    MSSNDSYS_Trapping_Disable_CODEC

        VMMCall Test_Sys_VM_Handle
        jne     SHORT AS_NotSysVM

        test    [edi.ssi_wFlags], SSI_FLAG_BUSTYPE_PCMCIA
        jnz     SHORT AS_NotSysVM

        Trace_Out "Disabling DMA translation for #EBX"

        mov     eax, [edi.ssi_dwDMADACHandle]
        VxDCall VDMAD_Disable_Translation
        mov     eax, [edi.ssi_dwDMAADCHandle]
        or      eax, eax
        jz      SHORT AS_NotSysVM
        VxDCall VDMAD_Disable_Translation

AS_NotSysVM:

        test    [edi.ssi_wFlags], SSI_FLAG_IRQSHARING
        jnz     SHORT AS_Success

        ;
        ; Set the physical IRQ mask state to the current
        ; owner's virtual state
        ;

        mov     eax, [edi.ssi_dwIRQHandle]
        VxDcall VPICD_Get_Complete_Status
        test    ecx, VPICD_Stat_Virt_Mask
        jz      SHORT AS_IRQ_Unmask

        VxDcall VPICD_Physically_Mask
        jmp     SHORT AS_Success

AS_IRQ_Unmask:
        VxDcall VPICD_Physically_Unmask
        Assumes_Fall_Through AS_Success

AS_Success:
        clc

AS_Exit:
        pop     esi
        pop     edx
        pop     ecx
        pop     eax
        ret

EndProc MSSNDSYS_Acquire_SndSys

BeginDoc
;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Get_Version
;
;   DESCRIPTION:
;       Get MSSNDSYS device version.
;
;   ENTRY:
;
;   EXIT:
;       IF Carry clear
;           EAX is version; AH = Major, AL = Minor
;       ELSE
;           MSSNDSYS device not installed
;
;   USES:
;       Flags, EAX
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc MSSNDSYS_Get_Version

        mov     eax, (MSSNDSYS_Ver_Major shl 8) or MSSNDSYS_Ver_Minor
        clc
        ret

EndProc MSSNDSYS_Get_Version

VxD_PAGEABLE_CODE_ENDS

ifdef DEBUG

;===========================================================================;
;              B E G I N:  D E B U G G I N G   A N N E X
;===========================================================================;

VxD_DEBUG_ONLY_CODE_SEG

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Debug_Dump
;
;   DESCRIPTION:
;       This function is invoked from WDEB386 by typing '.MSSNDSYS' at
;       the command prompt. Its purpose is to dump information about
;       the current state of this device.
;
;       This function is only available under the DEBUG build of this VxD.
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Debug_Dump

        pushad

        Trace_Out "MSSNDSYS Debug Dump-O-Matic:"

        mov     esi, ghlSSI
        or      esi, esi
        jz      MDD_NoSSI

        VMMCall List_Get_First
        jz      MDD_NoSSI

MDD_DisplaySSI:
        mov     edi, [eax.hwl_pSSI]
        Trace_Out "pSSI = #EDI"

        Dump_Struc_Head

        Dump_Struc edi, ssi_dwSize              
        Dump_Struc edi, ssi_wFlags              
        Dump_Struc edi, ssi_wIOAddress          
        Dump_Struc edi, ssi_bIRQ                
        Dump_Struc edi, ssi_bDMADAC             
        Dump_Struc edi, ssi_bDMAADC             
        Dump_Struc edi, ssi_bVersionCODEC       
        Dump_Struc edi, ssi_wVersionVxD         
        Dump_Struc edi, ssi_wVersionPAL         
        Dump_Struc edi, ssi_dwDMABufferHandle   
        Dump_Struc edi, ssi_lpDMABufferPhys     
        Dump_Struc edi, ssi_lpDMABufferLinear   
        Dump_Struc edi, ssi_dwDMABufferLen      
        Dump_Struc edi, ssi_wDMABufferSelector  
        Dump_Struc edi, ssi_wIOAddressOPL3      
        Dump_Struc edi, ssi_dwCODECOwnerCur     
        Dump_Struc edi, ssi_dwCODECOwnerLast    
        Dump_Struc edi, ssi_dwIRQHandle         
        Dump_Struc edi, ssi_dwOPL3OwnerCur      
        Dump_Struc edi, ssi_dwOPL3OwnerLast     
        Dump_Struc edi, ssi_dwDMADACHandle      
        Dump_Struc edi, ssi_dwDMAADCHandle      
        Dump_Struc edi, ssi_wIOAddressSB
        Dump_Struc edi, ssi_wCODECBase     
        Dump_Struc edi, ssi_wCODECClass    
        Dump_Struc edi, ssi_wAGABase       
        Dump_Struc edi, ssi_wOEM_ID             
        Dump_Struc edi, ssi_wHardwareOptions    
        Dump_Struc edi, ssi_dn                  
        Dump_Struc edi, ssi_hAutoSelectStubs    
        Dump_Struc edi, ssi_hCODECStubs         
        Dump_Struc edi, ssi_hSBStubs        
        Dump_Struc edi, ssi_SBVirtRegs          

        Trace_Out " "

        VMMcall Get_Cur_VM_Handle
        Trace_Out "       Current VM: #EBX"
        VMMcall Get_Sys_VM_Handle
        Trace_Out "        System VM: #EBX"

        VMMCall List_Get_Next
        jnz     MDD_DisplaySSI

MDD_Exit:
        popad
        ret

MDD_NoSSI:
        Trace_Out "No hardware instances registered."
        jmp     SHORT MDD_Exit

EndProc MSSNDSYS_Debug_Dump

VxD_DEBUG_ONLY_CODE_ENDS

;===========================================================================;
;              E N D:  D E B U G G I N G   A N N E X
;===========================================================================;

endif

end
