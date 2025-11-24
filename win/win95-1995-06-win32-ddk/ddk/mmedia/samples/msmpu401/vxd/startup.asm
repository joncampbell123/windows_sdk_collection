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
;   Title:    MSMPU401.386 - MICROSOFT MPU-401 386 Driver
;
;   Module:   STARTUP.ASM - Initialization routines
;
;   Version:  4.00
;******************************************************************************
;
;   Functional Description:
;      Initialization procedures for MSMPU401.386.
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

        include msmpu401.inc
        include equates.inc
        .list

;------------------------------------------------------------------------------
;                 E X T E R N A L    R E F E R E N C E S
;------------------------------------------------------------------------------

EXTRN Is_MPU401_Valid:NEAR
EXTRN _Validate_IRQ:NEAR

EXTRN MSMPU401_IO_Default_MPU401:NEAR

EXTRN MSMPU401_IRQ_Hw_Int_Proc:NEAR
EXTRN MSMPU401_IRQ_EOI_Proc:NEAR
EXTRN MSMPU401_IRQ_Mask_Changed_Proc:NEAR

EXTRN MSMPU401_Release:NEAR

EXTRN MSMPU401_End_V86_App:NEAR

EXTRN gdwCBOffset:DWORD                 ; VM control block offset
EXTRN ghlMSMI:DWORD                     ; MPU-401 Info list
EXTRN gpEndV86App:DWORD                 ; old DOSMGR_End_V86_App service ptr

;==============================================================================
;                       P A G E A B L E   D A T A
;==============================================================================

VxD_PAGEABLE_DATA_SEG

;
; Port map for MPU-401 -- used when installing trap handlers.
;

Begin_VxD_IO_Table MSMPU401_Port_Table

        VxD_IO  MPU401_DATA, MSMPU401_IO_Default_MPU401
        VxD_IO  MPU401_STATUS, MSMPU401_IO_Default_MPU401

End_VxD_IO_Table MSMPU401_Port_Table

MSMPU401_IRQ_Descriptor VPICD_IRQ_Descriptor <,,                    \
                            OFFSET32 MSMPU401_IRQ_Hw_Int_Proc,,     \
                            OFFSET32 MSMPU401_IRQ_EOI_Proc,         \
                            OFFSET32 MSMPU401_IRQ_Mask_Changed_Proc,,>

VxD_PAGEABLE_DATA_ENDS

;==============================================================================
;                                 I C O D E
;==============================================================================

VxD_INIT_CODE_SEG

;----------------------------------------------------------------------------
;
;   MSMPU401_Dyn_Device_Init
;
;   DESCRIPTION:
;       Device initialization entry point when dynaloaded.
;       Allocates the VM control block for MSMPU401.386.
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

BeginProc MSMPU401_Dyn_Device_Init

        Trace_Out "MSMPU401: Dyn_Device_Init"

        ;
        ; Allocate our control block
        ;

        VMMCall _Allocate_Device_CB_Area, <<size MSMPU401_CB_STRUCT>, 0>

        or      eax, eax                                ; Q: Got it?
        jnz     SHORT DDI_GotCB                         ;    Y: Continue

DDI_Exit_Failure:
        Trace_Out "MSMPU401: Dyn_Device_Init failing"
        stc                                             ;    N: fail load
        ret

DDI_GotCB:
        mov     [gdwCBOffset], eax                      ;    Y: store offset

        ;
        ; Alloc hardware instance list
        ;

        mov     eax, LF_Alloc_Error + LF_Async
        mov     ecx, size HARDWARE_INSTANCE_NODE
        VMMCall List_Create
        jc      SHORT DDI_Exit_Failure
        mov     ghlMSMI, esi

        ;
        ; Alloc VM hardware instance list for each existing VM,
        ; this maintains state information in each VM for
        ; each hardware instance.
        ;

        VMMCall Get_Sys_VM_Handle
        mov     edi, gdwCBOffset
        push    ebx

DDI_VM_Loop:
        mov     eax, LF_Alloc_Error + LF_Async
        mov     ecx, size VM_HWSTATE_NODE

        VMMCall List_Create
        jc      DDI_VM_List_Alloc_Failure
        mov     [ebx + edi].mscb_hlhws, esi
        VMMCall Get_Next_VM_Handle
        cmp     ebx, [esp]
        jne     SHORT DDI_VM_Loop
        pop     ebx

        ;
        ; watch when V86 apps terminate...
        ;

        mov     eax, @@DOSMGR_End_V86_App
        mov     esi, MSMPU401_End_V86_App
        VMMCall Hook_Device_Service

DDI_Exit:
        clc
        ret

DDI_VM_List_Alloc_Failure:
        pop     ebx
        jmp     DDI_Exit_Failure

EndProc MSMPU401_Dyn_Device_Init

VxD_INIT_CODE_ENDS

;==============================================================================
;                             P N P   C O D E
;==============================================================================

VxD_PNP_CODE_SEG

;---------------------------------------------------------------------------;
;
;   _MSMPU401_IsOwned
;
;   DESCRIPTION:
;       Checks ownership of given devnode.
;
;   ENTRY:
;       DWORD dn
;           devnode
;
;   EXIT:
;       EAX is True if owned, False otherwise
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc _MSMPU401_IsOwned, PUBLIC

        dn              equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        push    edi

        cCall   _MSMPU401_Get_pMSMI_From_XXX, <dn, pMSMI_FromDevNode>
        or      edi, edi

ifdef DEBUG
        jnz     SHORT @F

        Debug_Out "MSMPU401: IsOwned, unable to locate devnode?"
@@:
endif
        jz      SHORT MIO_NotOwned

        cmp     [edi.msmi_dwMPU401OwnerCur], 0
        jne     SHORT MIO_Owned

MIO_NotOwned:
        mov     eax, False
        jmp     SHORT MIO_Exit

MIO_Owned:
        mov     eax, NOT False

MIO_Exit:
        pop     edi

        pop     ebp
        ret

EndProc _MSMPU401_IsOwned

;---------------------------------------------------------------------------;
;
;   MSMPU401_Add_Instance_To_VM_List
;
;   DESCRIPTION:
;       Creates and adds a hardware state node to the VM list
;       associated with the pMSMI.
;
;   ENTRY:
;       EBX = VM handle to add an instance
;       EDI = pMSMI
;
;   EXIT:
;       STC if error, otherwise CLC
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSMPU401_Add_Instance_To_VM_List

        push    eax
        push    ecx
        push    esi                             ; !!!! stack ordering
        push    edi                             ; !!!! assumed below...

        Assert_Ints_Enabled
                                                
        mov     edi, gdwCBOffset

        mov     esi, [ebx + edi].mscb_hlhws
        or      esi, esi
ifdef DEBUG
        jnz     SHORT @F
        Debug_Out "MSMPU401: hardware state list handle NULL in VM????"

@@:
endif
        jz      SHORT MAITVL_Exit_Failure
        
        cli
        VMMCall List_Allocate
        sti
ifdef DEBUG
        jnc     SHORT @F
        Debug_Out "MSMPU401: unable to allocate hardware state node"
@@:
endif
        jc      SHORT MAITVL_Exit_Failure

        ;
        ; clear the list node, it's undefined by definition
        ;

        push    eax
        mov     edi, eax
        xor     eax, eax
        mov     ecx, size VM_HWSTATE_NODE
        shr     ecx, 2
        rep     stosd
        pop     eax

        mov     edi, [esp]                      ; get pMSMI from stack
        mov     [eax.hws_pMSMI], edi
        
        cli
        VMMCall List_Attach_Tail
        sti

        clc

MAITVL_Exit:
        pop     edi
        pop     esi
        pop     ecx
        pop     eax
        ret

MAITVL_Exit_Failure:
        stc
        jmp     SHORT MAITVL_Exit

EndProc MSMPU401_Add_Instance_To_VM_List

;---------------------------------------------------------------------------;
;
;   MSMPU401_Remove_Instance_From_VM_List
;
;   DESCRIPTION:
;       Removes the associated hardware state node from the VM list.
;
;   ENTRY:
;       EBX = VM handle to remove an instance
;       EDI = pMSMI
;
;   EXIT:
;       nothing.
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSMPU401_Remove_Instance_From_VM_List

        push    eax
        push    ecx
        push    esi                             
        push    edi                             
        pushfd
                   
        mov     esi, gdwCBOffset
        mov     esi, [ebx + esi].mscb_hlhws

        or      esi, esi
ifdef DEBUG
        jnz     SHORT @F
        Debug_Out "MSMPU401: hardware state list handle NULL in VM????"

@@:
endif
        jz      SHORT MRIFVL_Exit

        cli                                     ; playing with the list...

        VMMCall List_Get_First
        jz      SHORT MRIFVL_Exit

MRIFVL_Compare:
        cmp     [eax.hws_pMSMI], edi
        je      SHORT MRIFVL_FoundNode
        VMMCall List_Get_Next
        jz      SHORT MRIFVL_Exit
        jmp     SHORT MRIFVL_Compare

MRIFVL_FoundNode:
        push    eax
        VMMCall List_Remove
        pop     eax
        VMMCall List_Deallocate

MRIFVL_Exit:
        popfd
        pop     edi
        pop     esi
        pop     ecx
        pop     eax
        ret

EndProc MSMPU401_Remove_Instance_From_VM_List

;---------------------------------------------------------------------------;
;
;   Install_Mult_IO_Handlers_Ex
;
;   DESCRIPTION:
;       Installs multiple I/O handlers using VMM's service, BUT, allocates
;       stub functions to provide a DWORD reference value on the callback.
;
;   ENTRY:
;       Same as Install_Mult_IO_Handlers_ with addition of
;       reference data in EDX and "I/O stub" table ptr is returned
;       in EAX.
;
;   CALLBACK:
;       ebx = VM
;       ecx = IOType
;       edx = Port
;       ebp = OFFSET32 crs
;       eax = Data      (output data)
;       esi = reference data
;
;   EXIT:
;       EAX contains ptr to "I/O stub" table.
;
;   USES:
;       EAX, FLAGS
;
;---------------------------------------------------------------------------;

BeginProc Install_Mult_IO_Handlers_Ex, PUBLIC

        jmp     SHORT   imihe_WrapHandlers

IO_Stub:        
        mov     esi, 0                          ; dummy data
IO_Stub_RefData equ     ($ - IO_Stub) - 4       

        jmp     imihe_Exit                      ; dummy location
IO_Stub_JmpAddr equ     ($ - IO_Stub) - 4

IO_Stub_Len     equ     $-IO_Stub

.errnz  (IO_Stub_Len - 10)

imihe_WrapHandlers:

cbIOStubMem     equ     [ebp - 4]
pIOStubMem      equ     [ebp - 8]
dwRefData       equ     [ebp - 12]

        push    ebp
        mov     ebp, esp
        sub     esp, 12

        pushad

        mov     dwRefData, edx

        ;
        ; Allocate stub memory
        ;

        movzx   ecx, [edi.VxD_IO_Ports]
        mov     eax, ecx                        ; ports *= 10 (IO_Stub_Size)
        shl     eax, 2
        add     eax, ecx
        shl     eax, 1
        mov     cbIOStubMem, eax

        VMMCall _HeapAllocate, <eax, HeapZeroInit + HeapLockedIfDP>
        or      eax, eax
        jz      imihe_Exit_Failure
        mov     pIOStubMem, eax

        push    edi                             ; save pIOTraps
        movzx   ecx, [edi.VxD_IO_Ports]
        push    ecx                             ; save cIOTraps

        mov     edi, pIOStubMem

imihe_CopyLoop:                                 ; replicate code...
        push    ecx
        mov     ecx, IO_Stub_Len
        lea     esi, IO_Stub
        cld
        rep     movsb
        pop     ecx
        loop    imihe_CopyLoop

        pop     ecx

        mov     edi, [esp]
        mov     esi, pIOStubMem

        add     edi, (size VxD_IOT_Hdr)

imihe_WrapLoop:
        mov     eax, dwRefData
        mov     [esi.IO_Stub_RefData], eax      ; modify stub for RefData
        mov     eax, [edi.VxD_IO_Proc]
        sub     eax, esi
        sub     eax, IO_Stub_JmpAddr + 4
        mov     [esi.IO_Stub_JmpAddr], eax      ; modify JMP to IO_Port

        ;
        ; Tell VMM to trap the port...
        ;

        movzx   edx, [edi.VxD_IO_Port]
        VMMCall Install_IO_Handler
        jc      imihe_FailedInstall

        add     edi, (size VxD_IO_Struc)
        add     esi, IO_Stub_Len
        loop    imihe_WrapLoop

        pop     edi
        mov     eax, pIOStubMem
        mov     [esp.PushAD_EAX], eax
        jmp     short imihe_Exit


imihe_FailedInstall:
        Debug_Out "Install handlers failed... removing existing traps."

        pop     edi
        movzx   eax, [edi.VxD_IO_Ports]
        sub     ecx, eax
        neg     ecx
        jecxz   imihe_NoneTrapped

        add     edi, (size VxD_IOT_Hdr)

imihe_RemoveTraps:
        movzx   edx, [edi.VxD_IO_Port]
        VMMCall Remove_IO_Handler
        add     edi, (size VxD_IO_Struc)
        loop    imihe_RemoveTraps

imihe_NoneTrapped:
        VMMCall _HeapFree, <pIOStubMem, 0>

imihe_Exit_Failure:
        xor     eax, eax
        mov     [esp.PushAD_EAX], eax

imihe_Exit:
        popad

        add     esp, 12                         ; this trashes carry!
        pop     ebp
        ret

EndProc Install_Mult_IO_Handlers_Ex

;---------------------------------------------------------------------------;
;
;   Remove_Mult_IO_Handlers_Ex
;
;   DESCRIPTION:
;       Removes I/O handlers installed using Install_Mult_IO_Handlers_Ex.
;
;   ENTRY:
;       Same as Remove_Mult_IO_Handlers with addition of the "handle"
;       of the I/O stub table in EAX.
;
;   EXIT:
;       Nothing.
;
;   USES:
;       EAX, FLAGS
;
;---------------------------------------------------------------------------;

BeginProc Remove_Mult_IO_Handlers_Ex, PUBLIC

        push    ecx
        push    edx
        push    edi

        push    eax

        movzx   ecx, [edi.VxD_IO_Ports]

        ;
        ; Tell VMM to remove the port traps...
        ;

        add     edi, (size VxD_IOT_Hdr)

rmihe_RemoveTraps:
        movzx   edx, [edi.VxD_IO_Port]
        VMMCall Remove_IO_Handler
        add     edi, (size VxD_IO_Struc)
        loop    rmihe_RemoveTraps

        pop     eax
        VMMCall _HeapFree, <eax, 0>
        
        pop     edi
        pop     edx
        pop     ecx

        ret

EndProc Remove_Mult_IO_Handlers_Ex

;---------------------------------------------------------------------------;
;
;   Allocate_MPU401
;
;   DESCRIPTION:
;       Allocates a SB16INFO structure for the given base DSP
;       address, installs IRQ and DMA handlers, and installs I/O
;       traps for all ports associated with the device.
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

BeginProc _Allocate_MPU401

        dn              equ     [ebp + 8]
        wBaseMPU401     equ     [ebp + 12]
        wIRQ            equ     [ebp + 16]

        pListNode       equ     dword ptr [ebp - 4]

        push    ebp
        mov     ebp, esp
        sub     esp, 4

        pushad

        Trace_Out "MSMPU401: Allocate_MPU401"

        mov     pListNode, 0

        mov     esi, ghlMSMI
        or      esi, esi
ifdef DEBUG
        jnz     SHORT @F
        Debug_Out "MSMPU401: list handle NULL in MSMPU401_Allocate_SB16????"

@@:
endif
        jz      AM_Exit_Failure

        movzx   edx, word ptr wBaseMPU401
        cCall   _MSMPU401_Get_pMSMI_From_XXX, <edx, pMSMI_FromMPU401>
        or      edi, edi

ifdef DEBUG
        jz      SHORT @F

        Debug_Out "MSMPU401: duplicate base in Allocate_MPU401!"
@@:                       
endif
        jnz     AM_Exit_Failure

        VMMCall List_Allocate
        jc      AM_Exit_Failure

        mov     pListNode, eax
        VMMCall List_Attach_Tail

        mov     [eax.hwl_pMSMI], 0

        VMMCall _HeapAllocate, <size MSMPU401INFO, HeapZeroInit>

        or      eax, eax
        jz      AM_Exit_Failure

        ;
        ; store pMSMI in list node
        ;

        mov     edi, eax
        mov     eax, pListNode
        mov     [eax.hwl_pMSMI], edi

        ;
        ; store devnode in MSMI
        ;

        mov     eax, dn
        mov     [edi.msmi_dn], eax

        mov     ax, wBaseMPU401
        mov     [edi.msmi_wIOAddressMPU401], ax
        mov     ax, wIRQ
        mov     [edi.msmi_bIRQ], al

        cCall   MSMPU401_Get_Version
        mov     [edi].msmi_wVersionVxD, ax

ifdef DEBUG
        pushad

        movzx   edx, [edi].msmi_wIOAddressMPU401
        mov     bl, [edi].msmi_bIRQ

        Trace_Out "_Allocate_MPU401: base #DX  IRQ #BLh"

        popad
endif

        ;
        ; Virtualize IRQ...
        ;

        xor     eax, eax
        movsx   ecx, [edi.msmi_bIRQ]
        cmp     ecx, -1
        je      SHORT AM_No_IRQ
        push    edi
        mov     [MSMPU401_IRQ_Descriptor.VID_IRQ_Number], cx
        mov     edi, OFFSET32 MSMPU401_IRQ_Descriptor
        VxDCall VPICD_Virtualize_IRQ
        pop     edi

ifdef DEBUG
        jnc     SHORT @F
        Trace_Out "MSMPU401: failed to virtualize IRQ!"
@@:
endif
        jc      AM_Exit_Failure

        ;
        ; store VPICD's IRQ handle and update progress...
        ;

        mov     [edi.msmi_dwIRQHandle], eax

        ;
        ; Get auto-masking IRQ state...
        ;

        VMMCall Get_Sys_VM_Handle
        VxDCall VPICD_Get_Complete_Status
        Trace_Out "MSMPU401_Allocate_SB16: IRQ complete status=#ECX"
        test    ecx, VPICD_Stat_Phys_Mask
        jnz     SHORT AM_No_IRQ

        or      [edi.msmi_wFlags], MSMI_FLAG_IRQWASUNMASKED

AM_No_IRQ:


        mov     esi, OFFSET32 MSMPU401_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]
        movzx   edx, word ptr wBaseMPU401

        Trace_Out "Install_MPU401_IO_Handlers: base I/O =#DX"

        ;
        ; Build table for Install_Mult_IO_Handlers
        ;
        ;
        ; NOTE! This assumes a contiguous block of I/O address space.
        ;

        push    edi                             ; save list node
        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

AM_IO_Loop:
        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    AM_IO_Loop

        mov     edx, [esp]                      ; reference data
        cCall   Install_Mult_IO_Handlers_Ex
        pop     edi
        mov     [edi.msmi_hMPU401Stubs], eax
        or      eax, eax

ifdef DEBUG
        jnz     SHORT @F
        Debug_Out "MSMPU401: can't trap MPU-401 ports!"
@@:
endif
        jz      AM_Exit_Failure

AM_Alloc_VMState:

        ;
        ; Allocate the hardware state instance node for
        ; each active VM.
        ;

        VMMCall Get_Sys_VM_Handle
        push    ebx                             ; save SysVM handle

AM_Alloc_VMState_Loop:
        call    MSMPU401_Add_Instance_To_VM_List
        jc      SHORT AM_VMState_Failure
        VMMCall Get_Next_VM_Handle
        cmp     ebx, [esp]
        jne     SHORT AM_Alloc_VMState_Loop
        pop     ebx                             ; get rid of Sys VM handle

AM_Exit_Success:
        mov     [esp.PushAD_EAX], NOT False

AM_Exit:
        popad
        add     esp, 4
        pop     ebp
        ret

AM_VMState_Failure:
        pop     ebx                             ; get rid of Sys VM handle

AM_Exit_Failure:
        mov     edi, pListNode
        or      edi, edi
        jz      SHORT AM_Not_Allocated
        call    DeAllocate_MPU401

AM_Not_Allocated:
        mov     [esp.PushAD_EAX], False

        Debug_Out "MSMPU401: Allocate_MPU401 FAILED!"
        jmp     SHORT AM_Exit

EndProc _Allocate_MPU401

;---------------------------------------------------------------------------;
;
;   DeAllocate_MPU401
;
;   DESCRIPTION:
;       Deallocates SB16INFO structure and all associated traps,
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

BeginProc DeAllocate_MPU401

        pListNode       equ     [ebp - 4]

        push    ebp
        mov     ebp, esp
        sub     esp, 4

        pushad

        Assert_Ints_Enabled

        mov     pListNode, edi
        mov     edi, [edi.hwl_pMSMI]
        or      edi, edi
        jz      DM_Free_Node

        mov     eax, [edi.msmi_hMPU401Stubs]
        or      eax, eax
        jz      SHORT DM_No_MPU401Traps

        ;
        ; Build table for Remove_Mult_IO_Handlers
        ;

        mov     esi, OFFSET32 MSMPU401_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]
        movzx   edx, [edi.msmi_wIOAddressMPU401]

        Trace_Out "Remove_MPU401_IO_Handlers: base I/O =#DX"

        push    edi                             ; save list node
        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

DM_IO_Loop:
        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    DM_IO_Loop

        mov     eax, [esp]
        mov     eax, [eax.msmi_hMPU401Stubs]
        cCall   Remove_Mult_IO_Handlers_Ex
        pop     edi

DM_No_MPU401Traps:

        ;
        ; Unvirtualize IRQ
        ;

        mov     eax, [edi.msmi_dwIRQHandle]
        or      eax, eax
        jz      SHORT DM_Free_MSMI
        VxDCall VPICD_Force_Default_Behavior

DM_Free_MSMI:

        VMMCall _HeapFree, <edi, 0>

DM_Free_Node:

        ;
        ; Remove the list node.
        ;

        mov     esi, ghlMSMI
        mov     eax, pListNode
        push    eax
        VMMCall List_Remove
        pop     eax
        VMMCall List_Deallocate

        popad
        add     esp, 4
        pop     ebp
        ret

EndProc DeAllocate_MPU401

;---------------------------------------------------------------------------;
;
;   MSMPU401_Set_Config
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

BeginProc _MSMPU401_Set_Config, PUBLIC

        dn              equ     [ebp + 8]
        wBaseMPU401     equ     [ebp + 12]
        wIRQ            equ     [ebp + 16]

        push    ebp
        mov     ebp, esp

        pushad

        Trace_Out "MSMPU401: Set_Config"

        ;
        ; Assume failure...
        ;

        mov     [esp.PushAD_EAX], CR_FAILURE

        ;
        ; verify I/O address(es)
        ;

        movsx   edx, word ptr wBaseMPU401               ; get MPU401 base
        call    Is_MPU401_Valid
        jc      MSC_Exit_NotThere

        movsx   eax, word ptr wIRQ
        or      eax, eax
        js      SHORT MSC_Allocate_Hardware

        cCall   _Validate_IRQ, <wBaseMPU401, wIRQ>      ; Q: IRQ valid?
        or      eax, eax
        jz      SHORT MSC_Exit                          ;    N: failure

MSC_Allocate_Hardware:

        ;
        ; Now, put hooks into hardware accesses...
        ;

        cCall   _Allocate_MPU401, <dn, wBaseMPU401, wIRQ>
        or      eax, eax
        jz      SHORT MSC_Exit

MSC_Exit_Success:
        mov     [esp.PushAD_EAX], CR_SUCCESS
        Trace_Out "MSMPU401: Set_Config successful"

MSC_Exit:
        popad
        pop     ebp
        ret

MSC_Exit_NotThere:
        Debug_Out "MSMPU401: Set_Config is returning CR_DEVICE_NOT_THERE"

        mov     [esp.PushAD_EAX], CR_DEVICE_NOT_THERE
        jmp     SHORT MSC_Exit

EndProc _MSMPU401_Set_Config

;---------------------------------------------------------------------------;
;
;   _MSMPU401_Remove_Config
;
;   DESCRIPTION:
;       Deallocates the DevNode resources and cleans up.
;
;   ENTRY:
;       DWORD dn
;           devnode
;
;   EXIT:
;       Nothing.
;
;   USES:
;       EAX, EBX, Flags
;
;---------------------------------------------------------------------------;

BeginProc _MSMPU401_Remove_Config, PUBLIC

        dn              equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        push    esi
        push    edi
        push    ebx
        pushfd

        cCall   _MSMPU401_IsOwned, <dn>
        or      eax, eax

ifdef DEBUG
        jz      SHORT @F

        Debug_Out "MSMPU401: devnode is owned and we're trying to remove?"
@@:
endif
        jz      SHORT MRC_NotOwned

        cCall   _MSMPU401_Get_pMSMI_From_XXX, <dn, pMSMI_FromDevNode>
        or      edi, edi
        jz      SHORT MRC_NotOwned

        mov     ebx, [edi.msmi_dwMPU401OwnerCur]
        or      ebx, ebx
        jz      SHORT MRC_NotOwned
        call    MSMPU401_Release

MRC_NotOwned:

        ;
        ; Search for the devnode's list node in the hardware instance
        ; list... if it's not found, we've got a CONFIG_REMOVE from
        ; CONFIGMG when we haven't allocated any instance information,
        ; return success anyway.
        ;

        mov     ebx, dn

        mov     esi, ghlMSMI                     
        or      esi, esi                        ; Q: is list null?
        jz      SHORT MRC_Exit                  ;    Y: remove was successful.

        cli
        VMMCall List_Get_First
        jz      SHORT MRC_Exit

MRC_Compare:
        mov     edi, [eax.hwl_pMSMI]
        cmp     [edi.msmi_dn], ebx
        je      SHORT MRC_FoundNode

        VMMCall List_Get_Next
        jz      SHORT MRC_Exit
        jmp     SHORT MRC_Compare

MRC_FoundNode:
        popfd
        pushfd


        ;
        ; Remove the hardware state instance node for each active VM.
        ;

        VMMCall Get_Sys_VM_Handle
        push    ebx                             ; save SysVM handle

MRC_Remove_VMState_Loop:
        call    MSMPU401_Remove_Instance_From_VM_List
        VMMCall Get_Next_VM_Handle
        cmp     ebx, [esp]
        jne     SHORT MRC_Remove_VMState_Loop
        pop     ebx                             ; get rid of Sys VM handle

        mov     edi, eax
        call    DeAllocate_MPU401
        mov     eax, NOT False

MRC_Exit:

        popfd
        pop     ebx
        pop     edi
        pop     esi

        pop     ebp
        ret

EndProc _MSMPU401_Remove_Config

;----------------------------------------------------------------------------
;
;   MSMPU401_PnP_ConfigHandler
;
;   DESCRIPTION:
;      The procedure which we register with mmdevldr to receive our
;      CONFIG_ messages. This simply calls the MSMPU401_Config_Handler
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

BeginProc MSMPU401_PnP_ConfigHandler, CCALL, PUBLIC

        ArgVar  Function,DWORD
        ArgVar  SubFunction,DWORD
        ArgVar  MyDevNode,DWORD
        ArgVar  RefData,DWORD
        ArgVar  Flags,DWORD

        EnterProc

        cCall   _MSMPU401_Config_Handler, <[Function],[SubFunction],\
                                           [MyDevNode],[RefData],[Flags]>
        LeaveProc

        clc

        Return

EndProc MSMPU401_PnP_ConfigHandler

;----------------------------------------------------------------------------
;
;   MSMPU401_PnP_New_DevNode
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

BeginProc MSMPU401_PnP_New_DevNode

        Trace_Out "MSMPU401: PnP_New_DevNode"

        mov     eax, ebx
        mov     ebx, offset32 MSMPU401_PnP_ConfigHandler
        VxDCall MMDEVLDR_Register_Device_Driver
        mov     eax, CR_SUCCESS 

        stc
        ret
                                                           
EndProc MSMPU401_PnP_New_DevNode

VxD_PNP_CODE_ENDS

;==============================================================================
;                              R A R E   C O D E
;==============================================================================

VxD_RARE_CODE_SEG

;----------------------------------------------------------------------------
;
;   MSMPU401_Dyn_Device_Exit
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

BeginProc MSMPU401_Dyn_Device_Exit

        mov     eax, @@DOSMGR_End_V86_App
        mov     esi, MSMPU401_End_V86_App
        VMMCall Unhook_Device_Service

        ;
        ; Remove hardware instance list
        ;
        
        xor     esi, esi
        xchg    esi, ghlMSMI
        or      esi, esi
        jz      SHORT DDE_Exit

ifdef DEBUG
        VMMCall List_Get_First
        jz      SHORT @F

        Debug_Out "MSMPU401: Dyn_Device_Exit - ghlMSMI has nodes!!!"
@@:
endif
        VMMCall List_Destroy

        ;
        ; Remove VM hardware instance list for each existing VM.
        ;

        VMMCall Get_Sys_VM_Handle
        mov     edi, gdwCBOffset
        push    ebx

DDE_VM_Loop:
        xor     esi, esi
        xchg    esi, [ebx + edi].mscb_hlhws
        or      esi, esi
        jz      SHORT DDE_Next_VM
        VMMCall List_Destroy

DDE_Next_VM:
        VMMCall Get_Next_VM_Handle
        cmp     ebx, [esp]
        jne     SHORT DDE_VM_Loop
        pop     ebx

DDE_Exit:
        clc
        ret

EndProc MSMPU401_Dyn_Device_Exit

VxD_RARE_CODE_ENDS

end
