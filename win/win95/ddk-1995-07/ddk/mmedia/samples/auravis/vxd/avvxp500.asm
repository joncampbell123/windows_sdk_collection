        page    60, 132

;******************************************************************************
        title   AVVXP500.ASM - Main module
;******************************************************************************
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
;******************************************************************************
;
;   Title:    AVVXP500.386 - AURAVISION VxP500 386 Driver
;
;   Module:   AVVXP500.ASM - Main module
;
;   Version:  1.00
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
        include avvxp500.inc

        include msgmacro.inc
        include messages.inc

        include equates.inc
        .list

;==============================================================================
;             V I R T U A L   D E V I C E   D E C L A R A T I O N
;==============================================================================

Declare_Virtual_Device avvxp500, AVVXP500_Ver_Major, AVVXP500_Ver_Minor,\
                       AVVXP500_Control, AVVXP500_Device_ID,\
                       Undefined_Init_Order, AVVXP500_API_Handler,\
                       AVVXP500_API_Handler

;------------------------------------------------------------------------------
;                 E X T E R N A L    R E F E R E N C E S
;------------------------------------------------------------------------------

EXTRN AVVXP500_Dyn_Device_Init:NEAR
EXTRN AVVXP500_Dyn_Device_Exit:NEAR
EXTRN AVVXP500_API_Handler:NEAR
EXTRN AVVXP500_PnP_New_DevNode:NEAR

;==============================================================================
;                   N O N P A G E A B L E   D A T A
;==============================================================================

VxD_LOCKED_DATA_SEG

;------------------------------------------------------------------------------
;             G L O B A L   D A T A   D E C L A R A T I O N S
;------------------------------------------------------------------------------

	public	gAVDI
gAVDI		db	SIZE AVVXP500INFO dup (?)

        public  gdwCBOffset
gdwCBOffset     dd      0       ; VM control block offset

if 0
        public  gpEndV86App
gpEndV86App     dd      0       ; old service proc      
endif

ifdef DEBUG
        public Bogus_DevNode
Bogus_DevNode	dd	12345
endif

VxD_LOCKED_DATA_ENDS

;===========================================================================;
;                   N O N P A G E A B L E   C O D E
;===========================================================================;

VxD_LOCKED_CODE_SEG

;----------------------------------------------------------------------------
;
;   AVVXP500_Control
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

BeginProc AVVXP500_Control

        Control_Dispatch VM_Not_Executeable,      AVVXP500_VM_Not_Executeable
        Control_Dispatch Sys_Dynamic_Device_Init, AVVXP500_Dyn_Device_Init
        Control_Dispatch Sys_Dynamic_Device_Exit, AVVXP500_Dyn_Device_Exit
        Control_Dispatch PnP_New_DevNode,         AVVXP500_PnP_New_DevNode

ifdef DEBUG
        Control_Dispatch Debug_Query,             AVVXP500_Debug_Dump
endif

        clc
        ret

EndProc AVVXP500_Control

if 0

;
; BUGBUG! enable when contention management is in place.
;

;---------------------------------------------------------------------------;
;
;   AVVXP500_End_V86_App
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

BeginProc AVVXP500_End_V86_App, High_Freq, Hook_Proc, gpEndV86App

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

EVA_Compare_VXP500:
        cmp     [edi.avdi_dwVXP500OwnerCur], ebx
        jne     SHORT EVA_Next

        cCall   AVVXP500_Release

EVA_Next:
        cli
        VMMCall List_Get_Next
        popfd
        pushfd
        or      eax, eax
        jz      SHORT EVA_Exit
        jmp     SHORT EVA_Compare_VXP500

EVA_Exit:
        popfd
        popad
        jmp     [gpEndV86App]

EndProc AVVXP500_End_V86_App
endif
;---------------------------------------------------------------------------;
;
;   AVVXP500_IRQ_Hw_Int_Proc
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

BeginProc AVVXP500_IRQ_Hw_Int_Proc, High_Freq

;
; BUGBUG! enable when contention management is in place
;

if 1
        push    eax                             ; save IRQ handle
        push    edi

        mov     eax, [gAVDI.avdi_dwVXP500OwnerCur]
        or      eax, eax                        ; Q: is there an owner?
        jz      SHORT AVVXP500_Int_Unowned      ;   N: set int request to curVM
        mov     ebx, eax                        ;   Y: set int request to owner

AVVXP500_Int_Unowned:
        pop     edi
        pop     eax                             ; restore IRQ handle

endif
        Assert_VM_Handle ebx

        Trace_Out "<i"

        VxDcall VPICD_Set_Int_Request           ; set int request and return
        clc
        ret

EndProc AVVXP500_IRQ_Hw_Int_Proc

;---------------------------------------------------------------------------;
;
;   AVVXP500_IRQ_EOI_Proc
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

BeginProc AVVXP500_IRQ_EOI_Proc, High_Freq

        Trace_Out "i>"

        push    ecx
        VxDCall VPICD_Clear_Int_Request     ; clear virtual IRQ request
        VxDCall VPICD_Phys_EOI
        pop     ecx

        clc
        ret

EndProc AVVXP500_IRQ_EOI_Proc


;---------------------------------------------------------------------------;
;
;   AVVXP500_IRQ_Mask_Changed_Proc
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

BeginProc AVVXP500_IRQ_Mask_Changed_Proc, High_Freq
               
        push    edi

if 0
        cCall   _AVVXP500_Get_pavdi_From_XXX, <eax, pavdi_FromIRQ>
        or      edi, edi
        jz      SHORT AVVXP500_Auto_Mask
endif
        cmp     [gAVDI.avdi_dwVXP500OwnerCur], ebx ; Q: is this the owner?
        jne     SHORT AVVXP500_Auto_Mask        ;   N: hmm...

        jecxz   AVVXP500_Mask_Unmasking

AVVXP500_Mask_Masking:

        Trace_Out "AVVXP500_IRQ_Mask_Changed_Proc: MASKING!"

        VxDcall VPICD_Physically_Mask
        jmp     SHORT AVVXP500_Mask_Exit

AVVXP500_Mask_Unmasking:

        Trace_Out "AVVXP500_IRQ_Mask_Changed_Proc: *UN*MASKING!"

        VxDcall VPICD_Physically_Unmask
        jmp     SHORT AVVXP500_Mask_Exit

AVVXP500_Auto_Mask:

        Trace_Out "AVVXP500_IRQ_Mask_Changed_Proc: Auto-Masking(#ECX)"

        ;
        ; No 'owner' of the SB16 hardware -- so we will FORCE the
        ; mask to the default state (ignoring the caller's request).
        ; This is only for the PHYSICAL state -- the virtual state
        ; remains as the VM expects.
        ;

        test    [gAVDI.avdi_wFlags], AVDI_FLAG_IRQWASUNMASKED
        jnz     SHORT AVVXP500_Mask_Unmasking
        jz      SHORT AVVXP500_Mask_Masking

AVVXP500_Mask_Exit:
        pop     edi
        clc
        ret

EndProc AVVXP500_IRQ_Mask_Changed_Proc

VxD_LOCKED_CODE_ENDS

;===========================================================================;
;                          P A G E A B L E   C O D E
;===========================================================================;

VxD_PAGEABLE_CODE_SEG

;
; BUGBUG!  Enable when contention management is put in place.
;

if 0

;---------------------------------------------------------------------------;
;
;   AVVXP500_IO_Default_VXP500
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

BeginProc AVVXP500_IO_Default_VXP500, High_Freq

        Trace_Out "AVVXP500_IO_Default_VXP500, pMSMI = #ESI"

        push    eax                             ; save
        mov     eax, [esi.avdi_dwVXP500OwnerCur] ; get current owner...

        cmp     eax, ebx                        ; Q: does this VM own it?

ifdef DEBUG
        jne     SHORT AVVXP500_MPU_New_Owner  ;   N: then try to assign owner
        Debug_Out "AVVXP500: #EAX OWNS VXP500 AND TRAPPING IS ENABLED!?!"
endif

        ;
        ; Trapping should have been disabled for the owning VM, but if
        ; somehow (how?) we have lost track of ownership, allow access
        ; by the owner.
        ;

        je      SHORT AVVXP500_MPU_Allow_Access

        ;
        ; This is where the 'auto-acquire' comes into action. If the
        ; auto-acquire is not enabled, we will fail the IO, but we will
        ; be quiet about it.
        ;

AVVXP500_MPU_New_Owner:

        or      eax, eax                        ; Q: is there already an owner?
        jnz     SHORT AVVXP500_MPU_Not_Owner      ;   Y: yes, fail call!

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
        call    AVVXP500_Acquire                  ; acquire SB16
        pop     edx
        pop     ecx
        pop     ebx
        jc      SHORT AVVXP500_MPU_Not_Owner      ; fail if cannot acquire!

        Trace_Out "AVVXP500_IO_Default_VXP500: autoaquired by VM #EBX"

AVVXP500_MPU_Allow_Access:
        pop     eax                             ; restore

        Dispatch_Byte_IO Fall_Through, <SHORT AVVXP500_MPU_Real_Out>
        in      al, dx                          ; input from physical port
        jmp     SHORT AVVXP500_MPU_Exit

AVVXP500_MPU_Real_Out:
        out     dx, al                          ; output to physical port
        Assumes_Fall_Through AVVXP500_MPU_Exit

AVVXP500_MPU_Exit:
        ret

AVVXP500_MPU_Not_Owner:
        mov     edi, esi
        call    AVVXP500_Warning

AVVXP500_MPU_Fail_IO:
        pop     eax
        xor     eax, eax                        ; fail input with -1 value
        dec     eax
        ret

EndProc AVVXP500_IO_Default_VXP500

;---------------------------------------------------------------------------;
;
;   AVVXP500_Trapping_Enable_VXP500
;
;   DESCRIPTION:
;       Enables trapping of VXP500's ports in owning VM
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

BeginProc AVVXP500_Trapping_Enable_VXP500

        push    esi
        push    eax
        push    ecx

        Assert_VM_Handle ebx

        ;
        ; step through all VXP500 ports to re-enable trapping for VM
        ;

        mov     si, LAST_PORT_VXP500
        xor     ecx, ecx
        movzx   edx, [edi.avdi_wIOAddressVXP500]

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

EndProc AVVXP500_Trapping_Enable_VXP500

;---------------------------------------------------------------------------;
;
;   AVVXP500_Trapping_Disable_VXP500
;
;   DESCRIPTION:
;       Disables trapping of VXP500's ports in an owning VM
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

BeginProc AVVXP500_Trapping_Disable_VXP500

        push    esi
        push    eax
        push    ecx
        push    edx

        Assert_VM_Handle ebx

        ;
        ; step through all VXP500 related ports to disable trapping for VM
        ;

        mov     si, LAST_PORT_VXP500
        xor     ecx, ecx
        movzx   edx, [edi.avdi_wIOAddressVXP500]

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

EndProc AVVXP500_Trapping_Disable_VXP500

;---------------------------------------------------------------------------;
;
;   AVVXP500_Warning
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

BeginProc AVVXP500_Warning

        ;
        ; Check to see if warnings are enabled (default).  If they are
        ; then see if we are currently sitting in a warning waiting for
        ; the user's response; if all is clear then put up the warning.
        ;

        test    [edi.avdi_wFlags], avdi_FLAG_DISABLEWARNING
        jnz     SHORT AVVXP500_IO_Skip_Warning

        cCall   AVVXP500_Get_VM_HW_State_From_pMSMI

ifdef DEBUG
        jnc     SHORT @F

        Debug_Out "AVVXP500_Warning: failed to get hardware state for VM!"
@@:
endif
        jc      SHORT AVVXP500_IO_Skip_Warning

ifdef DEBUG
        mov     eax, [edi.avdi_dwVXP500OwnerCur]
        Trace_Out "AVVXP500: #EBX is touching VXP500 Port #DX--#EAX owns it!!"
endif

        test    [esi.hws_dwFlags], HWCB_FLAG_ALREADYWARNEDVXP500
        jnz     SHORT AVVXP500_IO_Skip_Warning

        GET_MESSAGE_PTR <gszNoAccessMessageVXP500>, ecx
        jmp     SHORT AVVXP500_IO_Display_Warning

        or      [esi.hws_dwFlags], HWCB_FLAG_ALREADYWARNEDVXP500

AVVXP500_IO_Display_Warning:

        ;
        ; Have the SHELL put up an appropriate warning...
        ; ECX -> message to display
        ;  

        mov     eax, MB_OK or MB_ICONEXCLAMATION        ; message box flags
        xor     esi, esi                                ; no callback
        xor     edi, edi                                ; default caption
        VxDcall SHELL_Message

AVVXP500_IO_Skip_Warning:

        ret

EndProc AVVXP500_Warning

endif

;---------------------------------------------------------------------------;
;
;   AVVXP500_VM_Not_Executeable
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

BeginProc AVVXP500_VM_Not_Executeable

        cmp     [gAVDI.avdi_dwVXP500OwnerCur], ebx
        jne     SHORT VNE_No_Ownership

        mov     [gAVDI.avdi_dwVXP500OwnerLast], -1  ; not owner anymore

        call    AVVXP500_Release

VNE_No_Ownership:
        clc
        ret

EndProc AVVXP500_VM_Not_Executeable

;---------------------------------------------------------------------------;
;
;   AVVXP500_Release
;
;   DESCRIPTION:
;       This function will release the MPU-401 from ownership by a VM.
;
;   ENTRY:
;       EBX = VM handle wanting to release
;
;   EXIT:
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc AVVXP500_Release, PUBLIC

        push    eax
        push    edx

        Assert_VM_Handle ebx

        ;
        ; release VXP500 (release woger, release bwian)
        ;
        ; EAX = flags
        ; EBX = VM handle
        ;

        cmp     [gAVDI.avdi_dwVXP500OwnerCur], ebx
        jne     SHORT RS_Exit

	; BUGBUG!! Enable when trapping is enabled.
;        call    AVVXP500_Trapping_Enable_VXP500
        mov     [gAVDI.avdi_dwVXP500OwnerCur], 0   ; zero out owner VM handle

        mov     eax, [gAVDI.avdi_dwIRQHandle]     ; EAX = IRQ handle
        or      eax, eax
        jz      SHORT RS_No_IRQ

ifdef DEBUG
        push    ecx
        VxDcall VPICD_Get_Complete_Status
        test    ecx, VPICD_Stat_IRET_Pending+VPICD_Stat_In_Service+VPICD_Stat_Phys_In_Serv+VPICD_Stat_Virt_Req+VPICD_Stat_Phys_Req+VPICD_Stat_Virt_Dev_Req
        jz      SHORT @F
        Debug_Out "AVVXP500: Release: Releasing with IRQ in service!!! #ECX"
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

        and     [gAVDI.hws_dwFlags], not HWCB_FLAG_ALREADYWARNEDVXP500

RS_Exit:
        pop     edx
        pop     eax

        clc
        ret

EndProc AVVXP500_Release


;---------------------------------------------------------------------------;
;
;   AVVXP500_Acquire
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

BeginProc AVVXP500_Acquire, PUBLIC

        push    eax
        push    ecx
        push    edx

        Assert_VM_Handle ebx

        ;
        ; acquire VXP500
        ;
        ; EAX = flags
        ; EBX = VM handle
        ; EDI = MSMI associated with MPU-401 base port
        ;

        cmp     [gAVDI.avdi_dwVXP500OwnerCur], 0
        je      SHORT AS_Do_It

        cmp     [gAVDI.avdi_dwVXP500OwnerCur], ebx
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
        mov     [gAVDI.avdi_dwVXP500OwnerCur], ebx         ; assign ownership

        cmp     [gAVDI.avdi_dwVXP500OwnerLast], ebx
        je      SHORT AS_Dont_Reset

        movzx   edx, [gAVDI.avdi_wIOAddressVXP500]

        Trace_Out "AVVXP500: release NOT RESETTING!!!"

AS_Dont_Reset:
        mov     [gAVDI.avdi_dwVXP500OwnerLast], ebx

	;; BUGBUG!!! Enable when trapping is enabled...

;        call    AVVXP500_Trapping_Disable_VXP500

        ;
        ; Set the physical IRQ mask state to the current
        ; owner's virtual state
        ;

        mov     eax, [gAVDI.avdi_dwIRQHandle]
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

EndProc AVVXP500_Acquire

BeginDoc
;---------------------------------------------------------------------------;
;
;   AVVXP500_Get_Version
;
;   DESCRIPTION:
;       Get AVVXP500 device version.
;
;   ENTRY:
;
;   EXIT:
;       IF Carry clear
;           EAX is version; AH = Major, AL = Minor
;       ELSE
;           AVVXP500 device not installed
;
;   USES:
;       Flags, EAX
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc AVVXP500_Get_Version

        mov     eax, (AVVXP500_Ver_Major shl 8) or AVVXP500_Ver_Minor
        clc
        ret

EndProc AVVXP500_Get_Version

VxD_PAGEABLE_CODE_ENDS

ifdef DEBUG

;===========================================================================;
;              B E G I N:  D E B U G G I N G   A N N E X
;===========================================================================;

VxD_DEBUG_ONLY_CODE_SEG

;---------------------------------------------------------------------------;
;
;   AVVXP500_Debug_Dump
;
;   DESCRIPTION:
;       This function is invoked from WDEB386 by typing '.AVVXP500' at
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

BeginProc AVVXP500_Debug_Dump

        pushad

        Trace_Out "AVVXP500 Debug Dump-O-Matic:"

	lea	edi, gAVDI
        Trace_Out "gAVDI = #EDI"

        Dump_Struc_Head

        Dump_Struc edi, avdi_dwSize              
        Dump_Struc edi, avdi_wFlags              
        Dump_Struc edi, avdi_wIOAddressVXP500
        Dump_Struc edi, avdi_bIRQ
	Dump_Struc edi, avdi_dwMemBase                
        Dump_Struc edi, avdi_wVersionVxD         
        Dump_Struc edi, avdi_dwIRQHandle         
        Dump_Struc edi, avdi_dwVXP500OwnerCur      
        Dump_Struc edi, avdi_dwVXP500OwnerLast     
        Dump_Struc edi, avdi_dn                  

        Trace_Out " "

        VMMcall Get_Cur_VM_Handle
        Trace_Out "       Current VM: #EBX"
        VMMcall Get_Sys_VM_Handle
        Trace_Out "        System VM: #EBX"

MDD_Exit:
        popad
        ret

EndProc AVVXP500_Debug_Dump

VxD_DEBUG_ONLY_CODE_ENDS

;===========================================================================;
;              E N D:  D E B U G G I N G   A N N E X
;===========================================================================;

endif

end
