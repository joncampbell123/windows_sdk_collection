        page    60, 132

;******************************************************************************
        title   MSMPU401.ASM - Main module
;******************************************************************************
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
;******************************************************************************
;
;   Title:    MSMPU401.386 - MICROSOFT MPU-401 386 Driver
;
;   Module:   MSMPU401.ASM - Main module
;
;   Version:  4.00
;******************************************************************************
;
;   Functional Description:
;
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
        include msmpu401.inc

        include msgmacro.inc
        include messages.inc

        include equates.inc
        .list

;==============================================================================
;             V I R T U A L   D E V I C E   D E C L A R A T I O N
;==============================================================================

Declare_Virtual_Device msmpu401, MSMPU401_Ver_Major, MSMPU401_Ver_Minor,\
                       MSMPU401_Control, MSMPU401_Device_ID,\
                       Undefined_Init_Order, MSMPU401_API_Handler,\
                       MSMPU401_API_Handler

;------------------------------------------------------------------------------
;                 E X T E R N A L    R E F E R E N C E S
;------------------------------------------------------------------------------

EXTRN MSMPU401_Dyn_Device_Init:NEAR
EXTRN MSMPU401_Dyn_Device_Exit:NEAR
EXTRN MSMPU401_API_Handler:NEAR
EXTRN MSMPU401_Add_Instance_To_VM_List:NEAR
EXTRN MSMPU401_PnP_New_DevNode:NEAR

;==============================================================================
;                   N O N P A G E A B L E   D A T A
;==============================================================================

VxD_LOCKED_DATA_SEG

;------------------------------------------------------------------------------
;             G L O B A L   D A T A   D E C L A R A T I O N S
;------------------------------------------------------------------------------

        public  gdwCBOffset
gdwCBOffset     dd      0       ; VM control block offset

        public  ghlMSMI
ghlMSMI          dd      0       ; Handle to SB16INFO list

        public  gpEndV86App
gpEndV86App     dd      0       ; old service proc      

gdwMSMIOffsets   label   dword
        dd      msmi_dn,                 fpMSMI_FromDWord
        dd      msmi_dwIRQHandle,        fpMSMI_FromDWord
        dd      msmi_wIOAddressMPU401,   fpMSMI_FromWord

VxD_LOCKED_DATA_ENDS

;===========================================================================;
;                   N O N P A G E A B L E   C O D E
;===========================================================================;

VxD_LOCKED_CODE_SEG

;----------------------------------------------------------------------------
;
;   MSMPU401_Control
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

BeginProc MSMPU401_Control

        Control_Dispatch VM_Critical_Init,        MSMPU401_Init_VM_Lists
        Control_Dispatch VM_Not_Executeable,      MSMPU401_VM_Not_Executeable
        Control_Dispatch Sys_VM_Init,             MSMPU401_Init_VM_Lists

        Control_Dispatch Sys_Dynamic_Device_Init, MSMPU401_Dyn_Device_Init
        Control_Dispatch Sys_Dynamic_Device_Exit, MSMPU401_Dyn_Device_Exit
        Control_Dispatch PnP_New_DevNode,         MSMPU401_PnP_New_DevNode

ifdef DEBUG
        Control_Dispatch Debug_Query,             MSMPU401_Debug_Dump
endif

        clc
        ret

EndProc MSMPU401_Control

;---------------------------------------------------------------------------;
;
;   MSMPU401_Get_pMSMI_From_XXX
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
;       CLC && EDI contain SB16INFO node, if successful
;       otherwise STC
;
;   USES:
;       EDI, FLAGS
;
;---------------------------------------------------------------------------;

BeginProc _MSMPU401_Get_pMSMI_From_XXX, PUBLIC

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
        mov     edx, gdwMSMIOffsets[ eax ]
        mov     eax, gdwMSMIOffsets[ eax + 4 ]
        mov     dwSearchFor, eax
        mov     ebx, dwCompare

        mov     esi, ghlMSMI 
        or      esi, esi
        jz      SHORT MGSFX_Exit_Failure

        cli
        VMMCall List_Get_First
        jz      SHORT MGSFX_Exit_Failure

MGSFX_Compare:
        mov     edi, [eax.hwl_pMSMI]
        test    dword ptr dwSearchFor, fpMSMI_FromWord
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
        
EndProc _MSMPU401_Get_pMSMI_From_XXX

;---------------------------------------------------------------------------;
;
;   MSMPU401_Get_VM_HW_State_From_pMSMI
;
;   DESCRIPTION:
;       Retrieves the list node associated with a pMSMI.
;       If list is empty, non-existant or the pMSMI does not
;       have an associated node, this function returns with
;       carry set.
;
;   ENTRY:
;       EBX = VM handle
;       EDI = pMSMI
;
;   EXIT:
;       CLC && ESI contains pointer to VM's hardware state node,
;       otherwise STC
;
;   USES:
;       ESI, FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSMPU401_Get_VM_HW_State_From_pMSMI, PUBLIC

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
        cmp     [eax.hws_pMSMI], edi
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
        Debug_Out "MSMPU401: Get_VM_HW_State_From_pMSMI failing!!"

        popfd
        xor     esi, esi
        stc

MGVHS_Exit:
        pop     ebx
        pop     eax
        ret

EndProc MSMPU401_Get_VM_HW_State_From_pMSMI

;---------------------------------------------------------------------------;
;
;   MSMPU401_End_V86_App
;
;   DESCRIPTION:
;       Watches the V86 app terminations...
;
;   ENTRY:
;	EBX = VM that performed the int 21, AX = 4B00h (Current VM)
;	ESI = high linear address of PSP which is terminating
;	 DX = PSP (segment value) of application which about to be run
;
;	All other registers are reserved for future use.  Do not assume
;	that the high word of the EDX register will be zero.
;
;   EXIT:
;       Nothing.
;
;   USES:
;       Nothing.
;
;---------------------------------------------------------------------------;

BeginProc MSMPU401_End_V86_App, High_Freq, Hook_Proc, gpEndV86App

        pushad
        pushfd

        VMMCall Test_Sys_VM_Handle
        je      SHORT EVA_Exit

        mov     esi, ghlMSMI
        or      esi, esi
        jz      SHORT EVA_Exit

        cli
        VMMCall List_Get_First
        popfd
        pushfd

        or      eax, eax
        jz      SHORT EVA_Exit

EVA_Compare_MPU401:
        mov     edi, [eax.hwl_pMSMI]
        cmp     [edi.msmi_dwMPU401OwnerCur], ebx
        jne     SHORT EVA_Next

        cCall   MSMPU401_Release

EVA_Next:
        cli
        VMMCall List_Get_Next
        popfd
        pushfd
        or      eax, eax
        jz      SHORT EVA_Exit
        jmp     SHORT EVA_Compare_MPU401

EVA_Exit:
        popfd
        popad
        jmp     [gpEndV86App]

EndProc MSMPU401_End_V86_App

;---------------------------------------------------------------------------;
;
;   MSMPU401_IRQ_Hw_Int_Proc
;
;   DESCRIPTION:
;       Reflects interrupt to current owner or handles it if no owner.
;
;   ENTRY:
;       EAX = IRQ Handle
;       EBX = Current VM Handle
;
;   EXIT:
;
;   USES:
;       Flags, EAX, EBX
;
;---------------------------------------------------------------------------;

BeginProc MSMPU401_IRQ_Hw_Int_Proc, High_Freq

        push    eax                             ; save IRQ handle
        push    edi

        cCall   _MSMPU401_Get_pMSMI_From_XXX, <eax, pMSMI_FromIRQ>
        or      edi, edi
        jz      SHORT MSMPU401_Int_Unowned

        mov     eax, [edi.msmi_dwMPU401OwnerCur]
        or      eax, eax                        ; Q: is there an owner?
        jz      SHORT MSMPU401_Int_Unowned      ;   N: set int request to curVM
        mov     ebx, eax                        ;   Y: set int request to owner

MSMPU401_Int_Unowned:
        pop     edi
        pop     eax                             ; restore IRQ handle

        Assert_VM_Handle ebx

        Trace_Out "<i"

        VxDcall VPICD_Set_Int_Request           ; set int request and return
        clc
        ret

EndProc MSMPU401_IRQ_Hw_Int_Proc

;---------------------------------------------------------------------------;
;
;   MSMPU401_IRQ_EOI_Proc
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

BeginProc MSMPU401_IRQ_EOI_Proc, High_Freq

        Trace_Out "i>"

        push    ecx
        VxDCall VPICD_Clear_Int_Request     ; clear virtual IRQ request
        VxDCall VPICD_Phys_EOI
        pop     ecx

        clc
        ret

EndProc MSMPU401_IRQ_EOI_Proc


;---------------------------------------------------------------------------;
;
;   MSMPU401_IRQ_Mask_Changed_Proc
;
;   DESCRIPTION:
;       If the _owning_ VM is masking or unmasking the IRQ, then we need
;       to set the physical state accordingly.  It is perfectly OK for
;       a non-owning VM to mask/unmask the IRQ and not disturb the owning
;       VM.  We can *NOT* assign ownership when the IRQ is [un]masked
;       because some apps will mask all IRQs so they can perform some
;       operations and then reset the PIC to the previous state.  This
;       operation has nothing to do with the SB16.
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

BeginProc MSMPU401_IRQ_Mask_Changed_Proc, High_Freq
               
        push    edi

        cCall   _MSMPU401_Get_pMSMI_From_XXX, <eax, pMSMI_FromIRQ>
        or      edi, edi
        jz      SHORT MSMPU401_Auto_Mask

        cmp     [edi.msmi_dwMPU401OwnerCur], ebx ; Q: is this the owner?
        jne     SHORT MSMPU401_Auto_Mask        ;   N: hmm...

        jecxz   MSMPU401_Mask_Unmasking

MSMPU401_Mask_Masking:

        Trace_Out "MSMPU401_IRQ_Mask_Changed_Proc: MASKING!"

        VxDcall VPICD_Physically_Mask
        jmp     SHORT MSMPU401_Mask_Exit

MSMPU401_Mask_Unmasking:

        Trace_Out "MSMPU401_IRQ_Mask_Changed_Proc: *UN*MASKING!"

        VxDcall VPICD_Physically_Unmask
        jmp     SHORT MSMPU401_Mask_Exit

MSMPU401_Auto_Mask:

        Trace_Out "MSMPU401_IRQ_Mask_Changed_Proc: Auto-Masking(#ECX)"

        ;
        ; No 'owner' of the SB16 hardware -- so we will FORCE the
        ; mask to the default state (ignoring the caller's request).
        ; This is only for the PHYSICAL state -- the virtual state
        ; remains as the VM expects.
        ;

        test    [edi.msmi_wFlags], MSMI_FLAG_IRQWASUNMASKED
        jnz     SHORT MSMPU401_Mask_Unmasking
        jz      SHORT MSMPU401_Mask_Masking

MSMPU401_Mask_Exit:
        pop     edi
        clc
        ret

EndProc MSMPU401_IRQ_Mask_Changed_Proc

VxD_LOCKED_CODE_ENDS

;===========================================================================;
;                          P A G E A B L E   C O D E
;===========================================================================;

VxD_PAGEABLE_CODE_SEG

;---------------------------------------------------------------------------;
;
;   MSMPU401_IO_Default_MPU401
;
;   DESCRIPTION:
;       Handle IO trapping of the SB16 ports.
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

BeginProc MSMPU401_IO_Default_MPU401, High_Freq

        Trace_Out "MSMPU401_IO_Default_MPU401, pMSMI = #ESI"

        push    eax                             ; save
        mov     eax, [esi.msmi_dwMPU401OwnerCur] ; get current owner...

        cmp     eax, ebx                        ; Q: does this VM own it?

ifdef DEBUG
        jne     SHORT MSMPU401_MPU_New_Owner  ;   N: then try to assign owner
        Debug_Out "MSMPU401: #EAX OWNS MPU401 AND TRAPPING IS ENABLED!?!"
endif

        ;
        ; Trapping should have been disabled for the owning VM, but if
        ; somehow (how?) we have lost track of ownership, allow access
        ; by the owner.
        ;

        je      SHORT MSMPU401_MPU_Allow_Access

        ;
        ; This is where the 'auto-acquire' comes into action. If the
        ; auto-acquire is not enabled, we will fail the IO, but we will
        ; be quiet about it.
        ;

MSMPU401_MPU_New_Owner:

        or      eax, eax                        ; Q: is there already an owner?
        jnz     SHORT MSMPU401_MPU_Not_Owner      ;   Y: yes, fail call!

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
        call    MSMPU401_Acquire                  ; acquire SB16
        pop     edx
        pop     ecx
        pop     ebx
        jc      SHORT MSMPU401_MPU_Not_Owner      ; fail if cannot acquire!

        Trace_Out "MSMPU401_IO_Default_MPU401: autoaquired by VM #EBX"

MSMPU401_MPU_Allow_Access:
        pop     eax                             ; restore

        Dispatch_Byte_IO Fall_Through, <SHORT MSMPU401_MPU_Real_Out>
        in      al, dx                          ; input from physical port
        jmp     SHORT MSMPU401_MPU_Exit

MSMPU401_MPU_Real_Out:
        out     dx, al                          ; output to physical port
        Assumes_Fall_Through MSMPU401_MPU_Exit

MSMPU401_MPU_Exit:
        ret

MSMPU401_MPU_Not_Owner:
        mov     edi, esi
        call    MSMPU401_Warning

MSMPU401_MPU_Fail_IO:
        pop     eax
        xor     eax, eax                        ; fail input with -1 value
        dec     eax
        ret

EndProc MSMPU401_IO_Default_MPU401

;---------------------------------------------------------------------------;
;
;   MSMPU401_Trapping_Enable_MPU401
;
;   DESCRIPTION:
;       Enables trapping of MPU401's ports in owning VM
;
;   ENTRY:
;       EBX = VM handle to enable trapping in
;       EDI = pMSMI
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSMPU401_Trapping_Enable_MPU401

        push    esi
        push    eax
        push    ecx

        Assert_VM_Handle ebx

        ;
        ; step through all MPU401 ports to re-enable trapping for VM
        ;

        mov     si, LAST_PORT_MPU401
        xor     ecx, ecx
        movzx   edx, [edi.msmi_wIOAddressMPU401]

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

EndProc MSMPU401_Trapping_Enable_MPU401

;---------------------------------------------------------------------------;
;
;   MSMPU401_Trapping_Disable_MPU401
;
;   DESCRIPTION:
;       Disables trapping of MPU401's ports in an owning VM
;
;   ENTRY:
;       EBX = VM handle to disable trapping in
;       EDI = pMSMI
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSMPU401_Trapping_Disable_MPU401

        push    esi
        push    eax
        push    ecx
        push    edx

        Assert_VM_Handle ebx

        ;
        ; step through all MPU401 related ports to disable trapping for VM
        ;

        mov     si, LAST_PORT_MPU401
        xor     ecx, ecx
        movzx   edx, [edi.msmi_wIOAddressMPU401]

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

EndProc MSMPU401_Trapping_Disable_MPU401

;---------------------------------------------------------------------------;
;
;   MSMPU401_Warning
;
;   DESCRIPTION:
;
;   ENTRY:
;       EDX = port being touched
;       EBX = VM to bring up warning dlg for
;       EDI = pMSMI
;
;   EXIT:
;
;   USES:
;       Flags, ESI, EDI, EAX, ECX
;
;---------------------------------------------------------------------------;

BeginProc MSMPU401_Warning

        ;
        ; Check to see if warnings are enabled (default).  If they are
        ; then see if we are currently sitting in a warning waiting for
        ; the user's response; if all is clear then put up the warning.
        ;

        test    [edi.msmi_wFlags], MSMI_FLAG_DISABLEWARNING
        jnz     SHORT MSMPU401_IO_Skip_Warning

        cCall   MSMPU401_Get_VM_HW_State_From_pMSMI

ifdef DEBUG
        jnc     SHORT @F

        Debug_Out "MSMPU401_Warning: failed to get hardware state for VM!"
@@:
endif
        jc      SHORT MSMPU401_IO_Skip_Warning

ifdef DEBUG
        mov     eax, [edi.msmi_dwMPU401OwnerCur]
        Trace_Out "MSMPU401: #EBX is touching MPU401 Port #DX--#EAX owns it!!"
endif

        test    [esi.hws_dwFlags], HWCB_FLAG_ALREADYWARNEDMPU401
        jnz     SHORT MSMPU401_IO_Skip_Warning

        GET_MESSAGE_PTR <gszNoAccessMessageMPU401>, ecx

        or      [esi.hws_dwFlags], HWCB_FLAG_ALREADYWARNEDMPU401

MSMPU401_IO_Display_Warning:

        ;
        ; Have the SHELL put up an appropriate warning...
        ; ECX -> message to display
        ;  

        mov     eax, MB_OK or MB_ICONEXCLAMATION        ; message box flags
        xor     esi, esi                                ; no callback
        xor     edi, edi                                ; default caption
        VxDcall SHELL_Message

MSMPU401_IO_Skip_Warning:

        ret

EndProc MSMPU401_Warning

;---------------------------------------------------------------------------;
;
;   MSMPU401_VM_Not_Executeable
;
;   DESCRIPTION:
;       This procedure checks whether the VM being destroyed owns the
;       Windows Sound System.  If it does, then dwxxxOwnerCur is cleared
;       and the DSP/OPL3 is reset.  Note that we reset because the VM
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

BeginProc MSMPU401_VM_Not_Executeable

        pushfd

        mov     esi, ghlMSMI
        or      esi, esi
        jz      VNE_No_Hardware

        cli

        VMMCall List_Get_First
        jz      SHORT VNE_No_Hardware

VNE_Compare_MPU401:
        popfd                                   ; STI if necessary
        pushfd

        push    eax                             ; save list node
        mov     edi, [eax.hwl_pMSMI]
        xor     eax, eax
        cmp     [edi.msmi_dwMPU401OwnerCur], ebx
        jne     SHORT VNE_No_Ownership

        mov     [edi.msmi_dwMPU401OwnerLast], -1  ; not owner anymore/not reset

        call    MSMPU401_Release

VNE_No_Ownership:
        pop     eax

        cli
        VMMCall List_Get_Next
        jz      SHORT VNE_No_Hardware
        jmp     SHORT VNE_Compare_MPU401

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

EndProc MSMPU401_VM_Not_Executeable

;---------------------------------------------------------------------------;
;
;   MSMPU401_Init_VM_Lists
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

BeginProc MSMPU401_Init_VM_Lists

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

        mov     esi, ghlMSMI 
        or      esi, esi

ifdef DEBUG
        jnz     SHORT @F
        Debug_Out "MSMPU401: initialize VM lists with no hardware registered???"
@@:
endif
        jz      SHORT IVL_Exit_Success

        cli
        VMMCall List_Get_First
        sti
        jz      SHORT IVL_Exit_Success

IVL_Instance_Loop:
        mov     edi, [eax.hwl_pMSMI]
        call    MSMPU401_Add_Instance_To_VM_List
        jc      SHORT IVL_Exit_Failure
        cli
        VMMCall List_Get_Next
        sti
        jz      SHORT IVL_Exit_Success
        jmp     SHORT IVL_Instance_Loop

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

EndProc MSMPU401_Init_VM_Lists

;---------------------------------------------------------------------------;
;
;   MSMPU401_Release
;
;   DESCRIPTION:
;       This function will release the MPU-401 from ownership by a VM.
;
;   ENTRY:
;       EBX = VM handle wanting to release
;       EDI = MSMI
;
;   EXIT:
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSMPU401_Release, PUBLIC


        push    eax
        push    edx
        push    esi

        Assert_VM_Handle ebx

        call    MSMPU401_Get_VM_HW_State_From_pMSMI
ifdef DEBUG
        jnc     SHORT @F

        Debug_Out "MSMPU401: Release can't get HW state???"
@@:
endif
        jc      RS_Exit

        ;
        ; release MPU401 (release woger, release bwian)
        ;
        ; EAX = flags
        ; EBX = VM handle
        ;

        cmp     [edi.msmi_dwMPU401OwnerCur], ebx
        jne     SHORT RS_Exit

        call    MSMPU401_Trapping_Enable_MPU401
        mov     [edi.msmi_dwMPU401OwnerCur], 0   ; zero out owner VM handle

        mov     eax, [edi.msmi_dwIRQHandle]     ; EAX = IRQ handle
        or      eax, eax
        jz      SHORT RS_No_IRQ

ifdef DEBUG
        push    ecx
        VxDcall VPICD_Get_Complete_Status
        test    ecx, VPICD_Stat_IRET_Pending+VPICD_Stat_In_Service+VPICD_Stat_Phys_In_Serv+VPICD_Stat_Virt_Req+VPICD_Stat_Phys_Req+VPICD_Stat_Virt_Dev_Req
        jz      SHORT @F
        Debug_Out "MSMPU401: Release: Releasing with IRQ in service!!! #ECX"
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


        VxDcall VPICD_Physically_Mask
        VxDcall VPICD_Phys_EOI
        VxDcall VPICD_Clear_Int_Request         ; clear any pending request

RS_No_IRQ:

        and     [esi.hws_dwFlags], not HWCB_FLAG_ALREADYWARNEDMPU401

RS_Exit:
        pop     esi
        pop     edx
        pop     eax

        clc
        ret

EndProc MSMPU401_Release


;---------------------------------------------------------------------------;
;
;   MSMPU401_Acquire
;
;   DESCRIPTION:
;       This function acquires the MPU-401 for a VM.
;
;   ENTRY:
;       EBX = VM handle to own SS
;       EDI = MSMI
;
;   EXIT:
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSMPU401_Acquire, PUBLIC

        push    eax
        push    ecx
        push    edx

        Assert_VM_Handle ebx

        ;
        ; acquire MPU401
        ;
        ; EAX = flags
        ; EBX = VM handle
        ; EDI = MSMI associated with MPU-401 base port
        ;

        cmp     [edi.msmi_dwMPU401OwnerCur], 0
        je      SHORT AS_Do_It

        cmp     [edi.msmi_dwMPU401OwnerCur], ebx
        je      DEBFAR AS_Success

        ;
        ; Failed to acquire MPU-401 because it is currently owned
        ;

AS_Fail:
        stc
        jc      DEBFAR AS_Exit

        ;
        ; EAX = flags
        ; EBX = VM handles
        ; EDI = MSMI
        ;

AS_Do_It:
        mov     [edi.msmi_dwMPU401OwnerCur], ebx         ; assign ownership

        cmp     [edi.msmi_dwMPU401OwnerLast], ebx
        je      SHORT AS_Dont_Reset

        movzx   edx, [edi.msmi_wIOAddressMPU401]

        Trace_Out "MSMPU401: release NOT RESETTING!!!"

AS_Dont_Reset:
        mov     [edi.msmi_dwMPU401OwnerLast], ebx

        call    MSMPU401_Trapping_Disable_MPU401

        ;
        ; Since the MPU-401 hardware is actually OWNED by a VM,
        ; we need to set the IRQ mask to an appropriate state for
        ; the new owner...
        ;

        ;
        ; Set the physical IRQ mask state to the current
        ; owner's virtual state
        ;

        mov     eax, [edi.msmi_dwIRQHandle]
        or      eax, eax
        jz      SHORT AS_Success

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
        pop     edx
        pop     ecx
        pop     eax
        ret

EndProc MSMPU401_Acquire

BeginDoc
;---------------------------------------------------------------------------;
;
;   MSMPU401_Get_Version
;
;   DESCRIPTION:
;       Get MSMPU401 device version.
;
;   ENTRY:
;
;   EXIT:
;       IF Carry clear
;           EAX is version; AH = Major, AL = Minor
;       ELSE
;           MSMPU401 device not installed
;
;   USES:
;       Flags, EAX
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc MSMPU401_Get_Version

        mov     eax, (MSMPU401_Ver_Major shl 8) or MSMPU401_Ver_Minor
        clc
        ret

EndProc MSMPU401_Get_Version

VxD_PAGEABLE_CODE_ENDS

ifdef DEBUG

;===========================================================================;
;              B E G I N:  D E B U G G I N G   A N N E X
;===========================================================================;

VxD_DEBUG_ONLY_CODE_SEG

;---------------------------------------------------------------------------;
;
;   MSMPU401_Debug_Dump
;
;   DESCRIPTION:
;       This function is invoked from WDEB386 by typing '.MSMPU401' at
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

BeginProc MSMPU401_Debug_Dump

        pushad

        Trace_Out "MSMPU401 Debug Dump-O-Matic:"

        mov     esi, ghlMSMI
        or      esi, esi
        jz      MDD_NoMSMI

        VMMCall List_Get_First
        jz      MDD_NoMSMI

MDD_DisplayMSMI:
        mov     edi, [eax.hwl_pMSMI]
        Trace_Out "pMSMI = #EDI"

        Dump_Struc_Head

        Dump_Struc edi, msmi_dwSize              
        Dump_Struc edi, msmi_wFlags              
        Dump_Struc edi, msmi_wIOAddressMPU401
        Dump_Struc edi, msmi_bIRQ                
        Dump_Struc edi, msmi_wVersionVxD         
        Dump_Struc edi, msmi_dwIRQHandle         
        Dump_Struc edi, msmi_dwMPU401OwnerCur      
        Dump_Struc edi, msmi_dwMPU401OwnerLast     
        Dump_Struc edi, msmi_dn                  
        Dump_Struc edi, msmi_hMPU401Stubs         

        Trace_Out " "

        VMMcall Get_Cur_VM_Handle
        Trace_Out "       Current VM: #EBX"
        VMMcall Get_Sys_VM_Handle
        Trace_Out "        System VM: #EBX"

        VMMCall List_Get_Next
        jnz     MDD_DisplayMSMI

MDD_Exit:
        popad
        ret

MDD_NoMSMI:
        Trace_Out "No hardware instances registered."
        jmp     SHORT MDD_Exit

EndProc MSMPU401_Debug_Dump

VxD_DEBUG_ONLY_CODE_ENDS

;===========================================================================;
;              E N D:  D E B U G G I N G   A N N E X
;===========================================================================;

endif

end
