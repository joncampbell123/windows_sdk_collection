        page    60, 132

;******************************************************************************
        title   MSOPL.ASM - Main module
;******************************************************************************
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
;******************************************************************************
;
;   Title:    MSOPL.386 - MICROSOFT OPL2/OPL3 386 Driver
;
;   Module:   MSOPL.ASM - Main module
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
        include msopl.inc

        include msgmacro.inc
        include messages.inc

        include equates.inc
        .list

;==============================================================================
;             V I R T U A L   D E V I C E   D E C L A R A T I O N
;==============================================================================

Declare_Virtual_Device msopl, MSOPL_Ver_Major, MSOPL_Ver_Minor,\
                       MSOPL_Control, MSOPL_Device_ID,\
                       Undefined_Init_Order, MSOPL_API_Handler,\
                       MSOPL_API_Handler

;------------------------------------------------------------------------------
;                 E X T E R N A L    R E F E R E N C E S
;------------------------------------------------------------------------------

EXTRN MSOPL_Dyn_Device_Init:NEAR
EXTRN MSOPL_Dyn_Device_Exit:NEAR
EXTRN MSOPL_API_Handler:NEAR
EXTRN MSOPL_Add_Instance_To_VM_List:NEAR
EXTRN MSOPL_PnP_New_DevNode:NEAR

;==============================================================================
;                   N O N P A G E A B L E   D A T A
;==============================================================================

VxD_LOCKED_DATA_SEG

;------------------------------------------------------------------------------
;             G L O B A L   D A T A   D E C L A R A T I O N S
;------------------------------------------------------------------------------

        public  gdwCBOffset
gdwCBOffset     dd      0       ; VM control block offset

        public  ghlMSOI
ghlMSOI          dd      0       ; Handle to MSOPLINFO list

        public  gpEndV86App
gpEndV86App     dd      0       ; old service proc      

gdwMSOIOffsets   label   dword
        dd      msoi_dn,                 fpMSOI_FromDWord
        dd      msoi_wIOAddressSynth,    fpMSOI_FromWord

VxD_LOCKED_DATA_ENDS

;===========================================================================;
;                   N O N P A G E A B L E   C O D E
;===========================================================================;

VxD_LOCKED_CODE_SEG

;----------------------------------------------------------------------------
;
;   MSOPL_Control
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

BeginProc MSOPL_Control

        Control_Dispatch VM_Critical_Init,        MSOPL_Init_VM_Lists
        Control_Dispatch VM_Not_Executeable,      MSOPL_VM_Not_Executeable
        Control_Dispatch Sys_VM_Init,             MSOPL_Init_VM_Lists

        Control_Dispatch Sys_Dynamic_Device_Init, MSOPL_Dyn_Device_Init
        Control_Dispatch Sys_Dynamic_Device_Exit, MSOPL_Dyn_Device_Exit
        Control_Dispatch PnP_New_DevNode,         MSOPL_PnP_New_DevNode

ifdef DEBUG
        Control_Dispatch Debug_Query,             MSOPL_Debug_Dump
endif

        clc
        ret

EndProc MSOPL_Control

;---------------------------------------------------------------------------;
;
;   MSOPL_Get_pMSOI_From_XXX
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
;       CLC && EDI contain MSOPLINFO node, if successful
;       otherwise STC
;
;   USES:
;       EDI, FLAGS
;
;---------------------------------------------------------------------------;

BeginProc _MSOPL_Get_pMSOI_From_XXX, PUBLIC

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
        mov     edx, gdwMSOIOffsets[ eax ]
        mov     eax, gdwMSOIOffsets[ eax + 4 ]
        mov     dwSearchFor, eax
        mov     ebx, dwCompare

        mov     esi, ghlMSOI 
        or      esi, esi
        jz      SHORT MGSFX_Exit_Failure

        cli
        VMMCall List_Get_First
        jz      SHORT MGSFX_Exit_Failure

MGSFX_Compare:
        mov     edi, [eax.hwl_pMSOI]
        test    dword ptr dwSearchFor, fpMSOI_FromWord
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
        
EndProc _MSOPL_Get_pMSOI_From_XXX

;---------------------------------------------------------------------------;
;
;   MSOPL_Get_VM_HW_State_From_pMSOI
;
;   DESCRIPTION:
;       Retrieves the list node associated with a pMSOI.
;       If list is empty, non-existant or the pMSOI does not
;       have an associated node, this function returns with
;       carry set.
;
;   ENTRY:
;       EBX = VM handle
;       EDI = pMSOI
;
;   EXIT:
;       CLC && ESI contains pointer to VM's hardware state node,
;       otherwise STC
;
;   USES:
;       ESI, FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSOPL_Get_VM_HW_State_From_pMSOI, PUBLIC

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
        cmp     [eax.hws_pMSOI], edi
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
        Debug_Out "MSOPL: Get_VM_HW_State_From_pMSOI failing!!"

        popfd
        xor     esi, esi
        stc

MGVHS_Exit:
        pop     ebx
        pop     eax
        ret

EndProc MSOPL_Get_VM_HW_State_From_pMSOI

;---------------------------------------------------------------------------;
;
;   MSOPL_End_V86_App
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

BeginProc MSOPL_End_V86_App, High_Freq, Hook_Proc, gpEndV86App

        pushad
        pushfd

        VMMCall Test_Sys_VM_Handle
        je      SHORT EVA_Exit

        mov     esi, ghlMSOI
        or      esi, esi
        jz      SHORT EVA_Exit

        cli
        VMMCall List_Get_First
        popfd
        pushfd

        or      eax, eax
        jz      SHORT EVA_Exit

EVA_Compare_OPL:
        mov     edi, [eax.hwl_pMSOI]
        cmp     [edi.msoi_dwSynthOwnerCur], ebx
        jne     SHORT EVA_Next

        cCall   MSOPL_Release

EVA_Next:
        cli
        VMMCall List_Get_Next
        popfd
        pushfd
        or      eax, eax
        jz      SHORT EVA_Exit
        jmp     SHORT EVA_Compare_OPL

EVA_Exit:
        popfd
        popad
        jmp     [gpEndV86App]

EndProc MSOPL_End_V86_App

VxD_LOCKED_CODE_ENDS

;===========================================================================;
;                          P A G E A B L E   C O D E
;===========================================================================;

VxD_PAGEABLE_CODE_SEG

;------------------------------------------------------------------------------
;
;   OPL_IODelay
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

BeginProc OPL_IODelay, PUBLIC

        push    eax
        in      al, dx
        IO_Delay
        in      al, dx
        IO_Delay
        pop     eax

        ret

EndProc OPL_IODelay

;------------------------------------------------------------------------------
;
;   OPL_RegWrite
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

BeginProc OPL_RegWrite, PUBLIC

        push    ebx
        push    edx
        push    eax

        mov     eax, ebx
        shl     ah, 1
        add     dl, ah
        out     dx, al                          ; Select the bank/register
        sub     dl, ah
        call    OPL_IODelay
        pop     eax
        inc     dl
        out     dx, al
        call    OPL_IODelay

        pop     edx
        pop     ebx
        ret

EndProc OPL_RegWrite

;---------------------------------------------------------------------------;
;
;   Force_OPL3_Into_OPL2_Mode
;
;   DESCRIPTION:
;       This function will place the OPL3 into OPL2 mode so that applications
;       in DOS VM's can see the OPL3 in its power on state when it is auto-
;       acquired. If an application looks for an OPL2 (AdLib compat.) and
;       the OPL3 is not in OPL2 compatibility mode, then the application
;       will not be able to 'see' the OPL2.
;
;   ENTRY:
;       EDI = PMSOI
;
;   EXIT:
;       The OPL3 will be in OPL2 mode
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc Force_OPL3_Into_OPL2_Mode


        push    eax
        push    edx

        Trace_Out "MSOPL: forcing OPL3 into OPL2 mode!"

        ;
        ; "Quiet" the OPL3
        ;

        push    ebx
        push    ecx

        ; Tell the FM chip to use 4-operator mode, and then 
        ; fill in any other random variables.

        movzx   edx, [edi.msoi_wIOAddressSynth]

        mov     ebx, AD_NEW
        mov     al, 1
        call    OPL_RegWrite

        mov     ebx, AD_MASK
        mov     al, 60h
        call    OPL_RegWrite

        mov     ebx, AD_CONNECTION
        mov     al, 3fh
        call    OPL_RegWrite

        mov     ebx, AD_NTS
        xor     al, al
        call    OPL_RegWrite

        ;
        ; Turn off the drums, and use high vibrato/modulation
        ;

        mov     ebx, AD_DRUM
        mov     al, 0c0h
        call    OPL_RegWrite

        ;
        ; Turn off all oscillators
        ;

        mov     ecx, 15h
        mov     ebx, AD_LEVEL
        mov     al, 3fh

VFO_Oscillator_Loop:
        xor     bh, bh
        call    OPL_RegWrite           ; out( AD_LEVEL + i, 3fh )
        mov     bh, 1                   
        call    OPL_RegWrite           ; out( AD_LEVEL2 + i, 3fh )
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
        call    OPL_RegWrite           ; out( AD_BLOCK + i, 00h )
        mov     bh, 1
        call    OPL_RegWrite           ; out( AD_BLOCK2 + i, 00h )
        inc     bl
        loop    VFO_Voices_Loop

        ;
        ; Kick into OPL2 mode
        ;

        mov     ebx, AD_CONNECTION      ; out( AD_CONNECTION, 00h )
        xor     eax, eax
        call    OPL_RegWrite
        mov     ebx, AD_NEW             ; out( AD_NEW, 00h )
        call    OPL_RegWrite

        ;
        ; Delay cause OPL3 doesn't like instructions immediately
        ; after kicking it into OPL2 mode.
        ; 

        call    OPL_IODelay
        call    OPL_IODelay
        call    OPL_IODelay

        pop     ecx
        pop     ebx

        pop     edx
        pop     eax
        ret

EndProc Force_OPL3_Into_OPL2_Mode

;---------------------------------------------------------------------------;
;
;   MSOPL_IO_Default
;
;   DESCRIPTION:
;       Handle IO trapping of the OPL2/OPL3 ports.
;
;   ENTRY:
;       EBX = VM Handle.
;       ECX = Type of I/O
;       EDX = Port number
;       ESI = reference data (MSOPLINFO structure)
;       EBP = Pointer to client register structure
;
;   EXIT:
;       EAX = data input or output depending on type of I/O
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSOPL_IO_Default, High_Freq

        Trace_Out "MSOPL_IO_Default, pMSOI = #ESI"

        push    eax                             ; save
        mov     eax, [esi.msoi_dwSynthOwnerCur] ; get current owner...

        cmp     eax, ebx                        ; Q: does this VM own it?

ifdef DEBUG
        jne     SHORT MSOPL_New_Owner  ;   N: then try to assign owner
        Debug_Out "MSOPL: #EAX OWNS OPL AND TRAPPING IS ENABLED!?!"
endif

        ;
        ; Trapping should have been disabled for the owning VM, but if
        ; somehow (how?) we have lost track of ownership, allow access
        ; by the owner.
        ;

        je      SHORT MSOPL_Allow_Access

        ;
        ; This is where the 'auto-acquire' comes into action. If the
        ; auto-acquire is not enabled, we will fail the IO, but we will
        ; be quiet about it.
        ;

MSOPL_New_Owner:

        or      eax, eax                   ; Q: is there already an owner?
        jnz     SHORT MSOPL_Not_Owner      ;   Y: yes, fail call!

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
        call    MSOPL_Acquire                  ; acquire OPL2/OPL3
        pop     edx
        pop     ecx
        pop     ebx
        jc      SHORT MSOPL_Not_Owner      ; fail if cannot acquire!

        Trace_Out "MSOPL_IO_Default: autoaquired by VM #EBX"

MSOPL_Allow_Access:
        pop     eax                             ; restore

        Dispatch_Byte_IO Fall_Through, <SHORT MSOPL_Real_Out>
        in      al, dx                          ; input from physical port
        jmp     SHORT MSOPL_Exit

MSOPL_Real_Out:
        out     dx, al                          ; output to physical port
        Assumes_Fall_Through MSOPL_Exit

MSOPL_Exit:
        ret

MSOPL_Not_Owner:
        mov     edi, esi
        call    MSOPL_Warning

MSOPL_Fail_IO:
        pop     eax
        xor     eax, eax                        ; fail input with -1 value
        dec     eax
        ret

EndProc MSOPL_IO_Default

;---------------------------------------------------------------------------;
;
;   MSOPL_Trapping_Enable
;
;   DESCRIPTION:
;       Enables trapping of OPL's ports in owning VM
;
;   ENTRY:
;       EBX = VM handle to enable trapping in
;       EDI = pMSOI
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSOPL_Trapping_Enable

        push    esi
        push    eax
        push    ecx

        Assert_VM_Handle ebx

        ;
        ; step through all OPL ports to re-enable trapping for VM
        ;

        mov     si, LAST_PORT_OPL2
        test    [edi.msoi_wHardwareOptions], MSOPL_HWOPTIONSF_OPL3DETECTED
        jz      SHORT TEO_OPL2
        mov     si, LAST_PORT_OPL3

TEO_OPL2:
        xor     ecx, ecx
        movzx   edx, [edi.msoi_wIOAddressSynth]

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

EndProc MSOPL_Trapping_Enable

;---------------------------------------------------------------------------;
;
;   MSOPL_Trapping_Disable
;
;   DESCRIPTION:
;       Disables trapping of OPL's ports in an owning VM
;
;   ENTRY:
;       EBX = VM handle to disable trapping in
;       EDI = pMSOI
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSOPL_Trapping_Disable

        push    esi
        push    eax
        push    ecx
        push    edx

        Assert_VM_Handle ebx

        ;
        ; step through all OPL related ports to disable trapping for VM
        ;

        mov     si, LAST_PORT_OPL2
        test    [edi.msoi_wHardwareOptions], MSOPL_HWOPTIONSF_OPL3DETECTED
        jz      SHORT TDO_OPL2
        mov     si, LAST_PORT_OPL3

TDO_OPL2:
        xor     ecx, ecx
        movzx   edx, [edi.msoi_wIOAddressSynth]

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

EndProc MSOPL_Trapping_Disable

;---------------------------------------------------------------------------;
;
;   MSOPL_Warning
;
;   DESCRIPTION:
;
;   ENTRY:
;       EDX = port being touched
;       EBX = VM to bring up warning dlg for
;       EDI = pMSOI
;
;   EXIT:
;
;   USES:
;       Flags, ESI, EDI, EAX, ECX
;
;---------------------------------------------------------------------------;

BeginProc MSOPL_Warning

        ;
        ; Check to see if warnings are enabled (default).  If they are
        ; then see if we are currently sitting in a warning waiting for
        ; the user's response; if all is clear then put up the warning.
        ;

        test    [edi.msoi_wFlags], MSOI_FLAG_DISABLEWARNING
        jnz     SHORT MSOPL_IO_Skip_Warning

        cCall   MSOPL_Get_VM_HW_State_From_pMSOI

ifdef DEBUG
        jnc     SHORT @F

        Debug_Out "MSOPL_Warning: failed to get hardware state for VM!"
@@:
endif
        jc      SHORT MSOPL_IO_Skip_Warning

ifdef DEBUG
        mov     eax, [edi.msoi_dwSynthOwnerCur]
        Trace_Out "MSOPL: #EBX is touching OPL Port #DX--#EAX owns it!!"
endif

        test    [esi.hws_dwFlags], HWCB_FLAG_ALREADYWARNEDOPL
        jnz     SHORT MSOPL_IO_Skip_Warning

        GET_MESSAGE_PTR <gszNoAccessMessageOPL>, ecx

        or      [esi.hws_dwFlags], HWCB_FLAG_ALREADYWARNEDOPL

MSOPL_IO_Display_Warning:

        ;
        ; Have the SHELL put up an appropriate warning...
        ; ECX -> message to display
        ;  

        mov     eax, MB_OK or MB_ICONEXCLAMATION        ; message box flags
        xor     esi, esi                                ; no callback
        xor     edi, edi                                ; default caption
        VxDcall SHELL_Message

MSOPL_IO_Skip_Warning:

        ret

EndProc MSOPL_Warning

;---------------------------------------------------------------------------;
;
;   MSOPL_VM_Not_Executeable
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

BeginProc MSOPL_VM_Not_Executeable

        pushfd

        mov     esi, ghlMSOI
        or      esi, esi
        jz      VNE_No_Hardware

        cli

        VMMCall List_Get_First
        jz      SHORT VNE_No_Hardware

VNE_Compare:
        popfd                                   ; STI if necessary
        pushfd

        push    eax                             ; save list node
        mov     edi, [eax.hwl_pMSOI]
        xor     eax, eax
        cmp     [edi.msoi_dwSynthOwnerCur], ebx
        jne     SHORT VNE_No_Ownership

        mov     [edi.msoi_dwSynthOwnerLast], -1  ; not owner anymore/not reset

        call    MSOPL_Release

VNE_No_Ownership:
        pop     eax

        cli
        VMMCall List_Get_Next
        jz      SHORT VNE_No_Hardware
        jmp     SHORT VNE_Compare

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

EndProc MSOPL_VM_Not_Executeable

;---------------------------------------------------------------------------;
;
;   MSOPL_Init_VM_Lists
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

BeginProc MSOPL_Init_VM_Lists

        pushad
        pushfd

        mov     edi, gdwCBOffset

        mov     eax, LF_Alloc_Error + LF_Async
        mov     ecx, size VM_HWSTATE_NODE

        cli

        VMMCall List_Create
        jc      IVL_Exit_Failure_NoList

        mov     [ebx + edi].mscb_hlhws, esi

        ;
        ; If no hardware instances, just exit with success but
        ; we're loaded, so it shouldn't happen.
        ;

        mov     esi, ghlMSOI 
        or      esi, esi

ifdef DEBUG
        jnz     SHORT @F
        Debug_Out "MSOPL: initialize VM lists with no hardware registered???"
@@:
endif
        jz      SHORT IVL_Exit_Success

        VMMCall List_Get_First
        jz      SHORT IVL_Exit_Success

IVL_Instance_Loop:
        mov     edi, [eax.hwl_pMSOI]
        call    MSOPL_Add_Instance_To_VM_List
        jc      SHORT IVL_Exit_Failure
        VMMCall List_Get_Next
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

EndProc MSOPL_Init_VM_Lists

;---------------------------------------------------------------------------;
;
;   MSOPL_Release
;
;   DESCRIPTION:
;       This function will release the OPL2/3 from ownership by a VM.
;
;   ENTRY:
;       EBX = VM handle wanting to release
;       EDI = MSOI
;
;   EXIT:
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSOPL_Release, PUBLIC


        push    eax
        push    edx
        push    esi

        Assert_VM_Handle ebx

        call    MSOPL_Get_VM_HW_State_From_pMSOI
ifdef DEBUG
        jnc     SHORT @F

        Debug_Out "MSOPL: Release can't get HW state???"
@@:
endif
        jc      RS_Exit

        ;
        ; release OPL (release woger, release bwian)
        ;
        ; EAX = flags
        ; EBX = VM handle
        ;

        cmp     [edi.msoi_dwSynthOwnerCur], ebx
        jne     SHORT RS_Exit

        call    MSOPL_Trapping_Enable
        mov     [edi.msoi_dwSynthOwnerCur], 0   ; zero out owner VM handle

        and     [esi.hws_dwFlags], not HWCB_FLAG_ALREADYWARNEDOPL

RS_Exit:
        pop     esi
        pop     edx
        pop     eax

        clc
        ret

EndProc MSOPL_Release


;---------------------------------------------------------------------------;
;
;   MSOPL_Acquire
;
;   DESCRIPTION:
;       This function acquires the OPL2/3 for a VM.
;
;   ENTRY:
;       EBX = VM handle to own SS
;       EDI = MSOI
;
;   EXIT:
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSOPL_Acquire, PUBLIC

        push    eax
        push    ecx
        push    edx

        Assert_VM_Handle ebx

        ;
        ; acquire OPL
        ;
        ; EAX = flags
        ; EBX = VM handle
        ; EDI = MSOI associated with OPL2/3 base port
        ;

        cmp     [edi.msoi_dwSynthOwnerCur], 0
        je      SHORT AS_Do_It

        cmp     [edi.msoi_dwSynthOwnerCur], ebx
        je      DEBFAR AS_Success

        ;
        ; Failed to acquire OPL2/3 because it is currently owned
        ;

AS_Fail:
        stc
        jc      DEBFAR AS_Exit

        ;
        ; EAX = flags
        ; EBX = VM handles
        ; EDI = MSOI
        ;

AS_Do_It:
        mov     [edi.msoi_dwSynthOwnerCur], ebx         ; assign ownership

        cmp     [edi.msoi_dwSynthOwnerLast], ebx
        je      SHORT AS_Dont_Reset

        test    [edi.msoi_wHardwareOptions], MSOPL_HWOPTIONSF_OPL3DETECTED
        jz      SHORT AS_Dont_Reset

        call    Force_OPL3_Into_OPL2_Mode

AS_Dont_Reset:
        mov     [edi.msoi_dwSynthOwnerLast], ebx

        call    MSOPL_Trapping_Disable

        ;
        ; Since the OPL2/3 hardware is actually OWNED by a VM,
        ; we need to set the IRQ mask to an appropriate state for
        ; the new owner...
        ;

AS_Success:
        clc

AS_Exit:
        pop     edx
        pop     ecx
        pop     eax
        ret

EndProc MSOPL_Acquire

BeginDoc
;---------------------------------------------------------------------------;
;
;   MSOPL_Get_Version
;
;   DESCRIPTION:
;       Get MSOPL device version.
;
;   ENTRY:
;
;   EXIT:
;       IF Carry clear
;           EAX is version; AH = Major, AL = Minor
;       ELSE
;           MSOPL device not installed
;
;   USES:
;       Flags, EAX
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc MSOPL_Get_Version

        mov     eax, (MSOPL_Ver_Major shl 8) or MSOPL_Ver_Minor
        clc
        ret

EndProc MSOPL_Get_Version

VxD_PAGEABLE_CODE_ENDS

ifdef DEBUG

;===========================================================================;
;              B E G I N:  D E B U G G I N G   A N N E X
;===========================================================================;

VxD_DEBUG_ONLY_CODE_SEG

;---------------------------------------------------------------------------;
;
;   MSOPL_Debug_Dump
;
;   DESCRIPTION:
;       This function is invoked from WDEB386 by typing '.MSOPL' at
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

BeginProc MSOPL_Debug_Dump

        pushad

        Trace_Out "MSOPL Debug Dump-O-Matic:"

        mov     esi, ghlMSOI
        or      esi, esi
        jz      MDD_NoMSOI

        VMMCall List_Get_First
        jz      MDD_NoMSOI

MDD_DisplayMSOI:
        mov     edi, [eax.hwl_pMSOI]
        Trace_Out "pMSOI = #EDI"

        Dump_Struc_Head

        Dump_Struc edi, msoi_dwSize              
        Dump_Struc edi, msoi_wFlags              
        Dump_Struc edi, msoi_wIOAddressSynth
        Dump_Struc edi, msoi_wVersionVxD         
        Dump_Struc edi, msoi_dwSynthOwnerCur      
        Dump_Struc edi, msoi_dwSynthOwnerLast     
        Dump_Struc edi, msoi_dn                  
        Dump_Struc edi, msoi_hOPLStubs         

        Trace_Out " "

        VMMcall Get_Cur_VM_Handle
        Trace_Out "       Current VM: #EBX"
        VMMcall Get_Sys_VM_Handle
        Trace_Out "        System VM: #EBX"

        VMMCall List_Get_Next
        jnz     MDD_DisplayMSOI

MDD_Exit:
        popad
        ret

MDD_NoMSOI:
        Trace_Out "No hardware instances registered."
        jmp     SHORT MDD_Exit

EndProc MSOPL_Debug_Dump

VxD_DEBUG_ONLY_CODE_ENDS

;===========================================================================;
;              E N D:  D E B U G G I N G   A N N E X
;===========================================================================;

endif

end
