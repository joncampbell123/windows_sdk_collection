        page    60, 132

;******************************************************************************
        title   STARTUP.ASM - Initialization routines
;******************************************************************************
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
;******************************************************************************
;
;   Title:    AVVXP500.VXD - AURAVISION VxP500 386 Driver
;
;   Module:   STARTUP.ASM - Initialization routines
;
;   Version:  1.00
;******************************************************************************
;
;   Functional Description:
;      Initialization procedures for AVVXP500.VXD.
;
;******************************************************************************

        .386p

;==============================================================================
;                             I N C L U D E S
;==============================================================================

        .xlist
        include vmm.inc
        include debug.inc
        include vdmad.inc
        include vpicd.inc
        include dosmgr.inc
        include configmg.inc
        include mmdevldr.inc

        include avvxp500.inc
        include equates.inc
        .list

;------------------------------------------------------------------------------
;                 E X T E R N A L    R E F E R E N C E S
;------------------------------------------------------------------------------

;EXTRN Is_VXP500_Valid:NEAR
;EXTRN _Validate_IRQ:NEAR

if 0
EXTRN AVVXP500_IO_Default_VXP500:NEAR
endif

EXTRN AVVXP500_IRQ_Hw_Int_Proc:NEAR
EXTRN AVVXP500_IRQ_EOI_Proc:NEAR
EXTRN AVVXP500_IRQ_Mask_Changed_Proc:NEAR

EXTRN AVVXP500_Release:NEAR

EXTRN gdwCBOffset:DWORD                 ; VM control block offset

if 0
; BUGBUG enable when contention management is in place
EXTRN AVVXP500_End_V86_App:NEAR
EXTRN gpEndV86App:DWORD                 ; old DOSMGR_End_V86_App service ptr
endif

EXTRN gAVDI:NEAR			; device information structure

ifdef DEBUG
EXTRN Bogus_DevNode:NEAR		; fake device node for debug
endif

;==============================================================================
;                       P A G E A B L E   D A T A
;==============================================================================

VxD_PAGEABLE_DATA_SEG

;
; Port map for VxP500 -- used when installing trap handlers.
;

if 0
Begin_VxD_IO_Table AVVXP500_Port_Table

        VxD_IO  VXP500_INDEX,   Some_Handler
        VxD_IO  VXP500_DATA,	Some_Handler

End_VxD_IO_Table AVVXP500_Port_Table
endif

AVVXP500_IRQ_Descriptor VPICD_IRQ_Descriptor <,,                    \
                            OFFSET32 AVVXP500_IRQ_Hw_Int_Proc,,     \
                            OFFSET32 AVVXP500_IRQ_EOI_Proc,         \
                            OFFSET32 AVVXP500_IRQ_Mask_Changed_Proc,,>

VxD_PAGEABLE_DATA_ENDS

;==============================================================================
;                                 I C O D E
;==============================================================================

VxD_INIT_CODE_SEG

;----------------------------------------------------------------------------
;
;   AVVXP500_Dyn_Device_Init
;
;   DESCRIPTION:
;       Device initialization entry point when dynaloaded.
;       Allocates the VM control block for AVVXP500.VXD.
;
;   ENTRY:
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       All Registers
;
;----------------------------------------------------------------------------

BeginProc AVVXP500_Dyn_Device_Init


        Trace_Out "AVVXP500: Dyn_Device_Init"

        ;
        ; Allocate our control block
        ;

        VMMCall _Allocate_Device_CB_Area, <<size AVVXP500_CB_STRUCT>, 0>

        or      eax, eax                                ; Q: Got it?
        jnz     SHORT DDI_GotCB                         ;    Y: Continue

DDI_Exit_Failure:
        Trace_Out "AVVXP500: Dyn_Device_Init failing"
        stc                                             ;    N: fail load
        ret

DDI_GotCB:
        mov     [gdwCBOffset], eax                      ;    Y: store offset

	;
	; BUGBUG!! enable when contention management is in place
	;
if 0
        ;
        ; watch when V86 apps terminate...
        ;

        mov     eax, @@DOSMGR_End_V86_App
        mov     esi, AVVXP500_End_V86_App
        VMMCall Hook_Device_Service
endif

DDI_Exit:
        clc
        ret

EndProc AVVXP500_Dyn_Device_Init

VxD_INIT_CODE_ENDS

;==============================================================================
;                             P N P   C O D E
;==============================================================================

VxD_PNP_CODE_SEG

;---------------------------------------------------------------------------;
;
;   _AVVXP500_IsOwned
;
;   DESCRIPTION:
;       Checks ownership of given devnode.
;
;   ENTRY:
;
;   EXIT:
;       EAX is True if owned, False otherwise
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc _AVVXP500_IsOwned, PUBLIC

        cmp     [gAVDI.avdi_dwVXP500OwnerCur], 0
        jne     SHORT MIO_Owned

MIO_NotOwned:
        mov     eax, False
        jmp     SHORT MIO_Exit

MIO_Owned:
        mov     eax, NOT False

MIO_Exit:
        ret

EndProc _AVVXP500_IsOwned

;---------------------------------------------------------------------------;
;
;   Allocate_VXP500
;
;   DESCRIPTION:
;       Installs IRQ handlers and installs I/O traps for all 
;       ports associated with the device.
;
;   ENTRY:
;
;   EXIT:
;       EAX is True if successful, otherwise False
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc _Allocate_VXP500

        dn              equ     [ebp + 8]
        wBaseVXP500     equ     [ebp + 12]
        wIRQ            equ     [ebp + 16]
        dwMemBase       equ     [ebp + 20]

        push    ebp
        mov     ebp, esp

        pushad

        Trace_Out "AVVXP500: Allocate_VXP500"

	;
        ; store devnode in AVDI
        ;

        mov     eax, dn
        mov     [gAVDI.avdi_dn], eax

        mov     ax, wBaseVXP500
        mov     [gAVDI.avdi_wIOAddressVXP500], ax

        mov     ax, wIRQ
        mov     [gAVDI.avdi_bIRQ], al

        mov     eax, dwMemBase
        mov     [gAVDI.avdi_dwMemBase], eax

        ;
        ; Allocate a GDT selector to access this memory
        ;

        VMMCall _MapPhysToLinear, <dwMemBase, 8000h, 0>
        cmp     eax, -1

ifdef DEBUG
         jne     SHORT @F
        Debug_Out "_Allocate_VXP500: failed to map physical address"
@@:
endif

        je      AV_Exit_Failure     

        VMMCall _BuildDescriptorDWORDS, <eax, 7fffh,\
                                         <D_PRES + D_DATA + D_W + D_SEG>,\
                                         0, 0>
        VMMCall _Allocate_GDT_Selector, <edx, eax, 0>

        mov     [gAVDI.avdi_wSelector], ax

        cCall   AVVXP500_Get_Version
        mov     [gAVDI.avdi_wVersionVxD], ax

ifdef DEBUG
        pushad

        movzx   edx, [gAVDI.avdi_wIOAddressVXP500]
        mov     bl, [gAVDI.avdi_bIRQ]

        Trace_Out "_Allocate_VXP500: base #DX  IRQ #BLh"

        popad
endif

        ;
        ; Virtualize IRQ...
        ;


        xor     eax, eax
        movsx   ecx, [gAVDI.avdi_bIRQ]
        cmp     ecx, -1
        je      SHORT AV_No_IRQ
        push    edi
        mov     [AVVXP500_IRQ_Descriptor.VID_IRQ_Number], cx
        mov     edi, OFFSET32 AVVXP500_IRQ_Descriptor
        VxDCall VPICD_Virtualize_IRQ
        pop     edi

ifdef DEBUG
        jnc     SHORT @F
        Debug_Out "AVVXP500: failed to virtualize IRQ!"
@@:
endif
        jc      AV_Exit_Failure

        ;
        ; store VPICD's IRQ handle and update progress...
        ;

        mov     [gAVDI.avdi_dwIRQHandle], eax

        ;
        ; Get auto-masking IRQ state...
        ;
        VMMCall Get_Sys_VM_Handle
        VxDCall VPICD_Get_Complete_Status
        Trace_Out "AVVXP500_Allocate_SB16: IRQ complete status=#ECX"

        test    ecx, VPICD_Stat_Phys_Mask
        jnz     SHORT AV_No_IRQ

        or      [gAVDI.avdi_wFlags], AVDI_FLAG_IRQWASUNMASKED

AV_No_IRQ:

if 0
        mov     esi, OFFSET32 AVVXP500_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]
        movzx   edx, word ptr wBaseVXP500

        Trace_Out "Install_VXP500_IO_Handlers: base I/O =#DX"

        ;
        ; Build table for Install_Mult_IO_Handlers
        ;
        ;
        ; NOTE! This assumes a contiguous block of I/O address space.
        ;

        push    edi                             ; save list node
        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

AV_IO_Loop:
        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    AV_IO_Loop

        mov     edx, [esp]                      ; reference data
        cCall   Install_Mult_IO_Handlers_Ex
        pop     edi
        mov     [gAVDI.avdi_hVXP500Stubs], eax
        or      eax, eax

;        jz      AV_Exit_Failure
endif

AV_Exit_Success:

        mov     [esp.PushAD_EAX], NOT False

AV_Exit:

        popad
        pop     ebp
        ret

AV_Exit_Failure:
        mov     [esp.PushAD_EAX], False

        Debug_Out "AVVXP500: Allocate_VXP500 FAILED!"
        jmp     SHORT AV_Exit

EndProc _Allocate_VXP500

;---------------------------------------------------------------------------;
;
;   DeAllocate_VXP500
;
;   DESCRIPTION:
;       Deeallocates SB16INFO structure and all associated traps,
;       virtualizations, etc.
;
;   ENTRY:
;       EDI = list node
;
;   EXIT:
;       Nothing.
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc DeAllocate_VXP500

        pListNode       equ     [ebp - 4]

        push    ebp
        mov     ebp, esp
        sub     esp, 4

        pushad

if 0
        mov     eax, [gAVDI.avdi_hVXP500Stubs]
        or      eax, eax
        jz      SHORT DM_No_VXP500Traps

        ;
        ; Build table for Remove_Mult_IO_Handlers
        ;

        mov     esi, OFFSET32 AVVXP500_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]
        movzx   edx, [gAVDI.avdi_wIOAddressVXP500]

        Trace_Out "Remove_VXP500_IO_Handlers: base I/O =#DX"

        push    edi                             ; save list node
        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

DM_IO_Loop:
        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    DM_IO_Loop

        mov     eax, [esp]
        mov     eax, [eax.avdi_hVXP500Stubs]
        cCall   Remove_Mult_IO_Handlers_Ex
        pop     edi

DM_No_VXP500Traps:
endif

        ;
        ; Unvirtualize IRQ
        ;

        mov     eax, [gAVDI.avdi_dwIRQHandle]
        or      eax, eax
        jz      SHORT DM_Exit
        VxDCall VPICD_Force_Default_Behavior

DM_Exit:
        popad
        add     esp, 4
        pop     ebp
        ret

EndProc DeAllocate_VXP500

;---------------------------------------------------------------------------;
;
;   AVVXP500_Set_Config
;
;   DESCRIPTION:
;       Sets up and validates the configuration for the device based
;       on information retrieved from CONFIGMG.
;
;   ENTRY:
;
;   EXIT:
;       EAX is CR_SUCCESS if successful, otherwise failure code
;
;   CHICAGO NOTES:
;       A DMA buffer is _always_ allocated.  If 0 is specified, the
;       default DMA buffer size is used.  Load failure occurs if
;       the DMA buffer can not be allocated.
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc _AVVXP500_Set_Config, PUBLIC

        dn              equ     [ebp + 8]
        wBaseVXP500     equ     [ebp + 12]
        wIRQ            equ     [ebp + 16]
        dwMemBase       equ     [ebp + 20]

        push    ebp
        mov     ebp, esp

        pushad

        Trace_Out "AVVXP500: Set_Config"

        ;
        ; Assume failure...
        ;

        mov     [esp.PushAD_EAX], CR_FAILURE

if 0
	;
	; BUGBUG!! Validate I/O and IRQ here...
	;

        ;
        ; verify I/O address(es)
        ;

        movsx   edx, word ptr wBaseVXP500               ; get VXP500 base
        call    Is_VXP500_Valid
        jc      MSC_Exit_NotThere

        movsx   eax, word ptr wIRQ
        or      eax, eax
        js      SHORT MSC_Allocate_Hardware

        cCall   _Validate_IRQ, <wBaseVXP500, wIRQ>      ; Q: IRQ valid?
        or      eax, eax
        jz      SHORT MSC_Exit                          ;    N: failure
endif

MSC_Allocate_Hardware:

        ;
        ; Now, put hooks into hardware accesses...
        ;

        cCall   _Allocate_VXP500, <dn, wBaseVXP500, wIRQ, dwMemBase>

        or      eax, eax
        jz      SHORT MSC_Exit

MSC_Exit_Success:
        mov     [esp.PushAD_EAX], CR_SUCCESS
        Trace_Out "AVVXP500: Set_Config successful"

MSC_Exit:
        popad
        pop     ebp
        ret

MSC_Exit_NotThere:
        Debug_Out "AVVXP500: Set_Config is returning CR_DEVICE_NOT_THERE"

        mov     [esp.PushAD_EAX], CR_DEVICE_NOT_THERE
        jmp     SHORT MSC_Exit

EndProc _AVVXP500_Set_Config

;---------------------------------------------------------------------------;
;
;   _AVVXP500_Remove_Config
;
;   DESCRIPTION:
;       Deallocates the DevNode resources and cleans up.
;
;   ENTRY:
;
;   EXIT:
;       Nothing.
;
;   USES:
;       EAX, EBX, Flags
;
;---------------------------------------------------------------------------;

BeginProc _AVVXP500_Remove_Config, PUBLIC

        push    edi
        push    ebx

        cCall   _AVVXP500_IsOwned
        or      eax, eax

ifdef DEBUG
        jz      SHORT @F

        Debug_Out "AVVXP500: devnode is owned and we're trying to remove?"
@@:
endif
        jz      SHORT MRC_NotOwned

        mov     ebx, [gAVDI.avdi_dwVXP500OwnerCur]
        or      ebx, ebx
        jz      SHORT MRC_NotOwned
        call    AVVXP500_Release

MRC_NotOwned:

        ;
        ; Search for the devnode's list node in the hardware instance
        ; list... if it's not found, we've got a CONFIG_REMOVE from
        ; CONFIGMG when we haven't allocated any instance information,
        ; return success anyway.
        ;

        call    DeAllocate_VXP500
        mov     eax, NOT False

MRC_Exit:
        pop     ebx
        pop     edi

        ret

EndProc _AVVXP500_Remove_Config

;----------------------------------------------------------------------------
;
;   AVVXP500_PnP_ConfigHandler
;
;   DESCRIPTION:
;      The procedure which we register with mmdevldr to receive our
;      CONFIG_ messages. This simply calls the AVVXP500_Config_Handler
;      function in configh.c
;
;   ENTRY:
;      standard config handler parameters
;
;   EXIT: CONFIGRET code
;
;   USES:
;       FLAGS
;----------------------------------------------------------------------------

BeginProc AVVXP500_PnP_ConfigHandler, CCALL, PUBLIC

        ArgVar  Function,DWORD
        ArgVar  SubFunction,DWORD
        ArgVar  MyDevNode,DWORD
        ArgVar  RefData,DWORD
        ArgVar  Flags,DWORD

        EnterProc

        cCall   _AVVXP500_Config_Handler, <[Function],[SubFunction],\
                                           [MyDevNode],[RefData],[Flags]>
        LeaveProc

        clc

        Return

EndProc AVVXP500_PnP_ConfigHandler

;----------------------------------------------------------------------------
;
;   AVVXP500_PnP_New_DevNode
;
;   DESCRIPTION:
;      This function handles the PNP_NEW_DEVNODE message sent by mmdevldr.
;      In response a Config_Handler function is registered with mmdevldr
;      as the config message handler for this devnode.
;
;   ENTRY:
;      ebx = DEVNODE, edx = DLVXD_LOAD_DRIVER (can be ignored)
;
;   EXIT:
;      ecx = -> PnP_ConfigHandler for MMDEVLDR
;
;   USES:
;       FLAGS
;----------------------------------------------------------------------------

BeginProc AVVXP500_PnP_New_DevNode

        Trace_Out "AVVXP500: PnP_New_DevNode"

        mov     eax, ebx
        mov     ebx, offset32 AVVXP500_PnP_ConfigHandler
        VxDCall MMDEVLDR_Register_Device_Driver
        mov     eax, CR_SUCCESS 

        stc
        ret
                                                           
EndProc AVVXP500_PnP_New_DevNode

VxD_PNP_CODE_ENDS

;==============================================================================
;                              R A R E   C O D E
;==============================================================================

VxD_RARE_CODE_SEG

;----------------------------------------------------------------------------
;
;   AVVXP500_Dyn_Device_Exit
;
;   DESCRIPTION:
;       Device exit notification when dynaloaded.
;
;   ENTRY:
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       All Registers
;
;----------------------------------------------------------------------------

BeginProc AVVXP500_Dyn_Device_Exit

	; BUGBUG!! Enable when device contention is in place
	   
if 0
        mov     eax, @@DOSMGR_End_V86_App
        mov     esi, AVVXP500_End_V86_App
        VMMCall Unhook_Device_Service
endif

        clc
        ret

EndProc AVVXP500_Dyn_Device_Exit

VxD_RARE_CODE_ENDS

end
