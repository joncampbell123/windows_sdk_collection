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
;   Title:    MSOPL.386 - MICROSOFT OPL2/OPL3 386 Driver
;
;   Module:   STARTUP.ASM - Initialization routines
;
;   Version:  4.00
;******************************************************************************
;
;   Functional Description:
;      Initialization procedures for MSOPL.386.
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

        include msopl.inc
        include equates.inc
        .list

;------------------------------------------------------------------------------
;                 E X T E R N A L    R E F E R E N C E S
;------------------------------------------------------------------------------

EXTRN Is_Synth_Valid:NEAR

EXTRN MSOPL_IO_Default:NEAR

EXTRN MSOPL_Release:NEAR

EXTRN MSOPL_End_V86_App:NEAR

EXTRN gdwCBOffset:DWORD                 ; VM control block offset
EXTRN ghlMSOI:DWORD                     ; OPL2/OPL3 Info list
EXTRN gpEndV86App:DWORD                 ; old DOSMGR_End_V86_App service ptr

;==============================================================================
;                       P A G E A B L E   D A T A
;==============================================================================

VxD_PAGEABLE_DATA_SEG

;
; Port map for OPL2/OPL3 -- used when installing trap handlers.
;

Begin_VxD_IO_Table MSOPL_Port_Table

        VxD_IO  OPL_0,  MSOPL_IO_Default
        VxD_IO  OPL_1,  MSOPL_IO_Default
        VxD_IO  OPL_2,  MSOPL_IO_Default
        VxD_IO  OPL_3,  MSOPL_IO_Default

End_VxD_IO_Table MSOPL_Port_Table

VxD_PAGEABLE_DATA_ENDS

;==============================================================================
;                                 I C O D E
;==============================================================================

VxD_INIT_CODE_SEG

;----------------------------------------------------------------------------
;
;   MSOPL_Dyn_Device_Init
;
;   DESCRIPTION:
;       Device initialization entry point when dynaloaded.
;       Allocates the VM control block for MSOPL.386.
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

BeginProc MSOPL_Dyn_Device_Init

        Trace_Out "MSOPL: Dyn_Device_Init"

        ;
        ; Allocate our control block
        ;

        VMMCall _Allocate_Device_CB_Area, <<size MSOPL_CB_STRUCT>, 0>

        or      eax, eax                                ; Q: Got it?
        jnz     SHORT DDI_GotCB                         ;    Y: Continue

DDI_Exit_Failure:
        Trace_Out "MSOPL: Dyn_Device_Init failing"
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
        mov     ghlMSOI, esi

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
        mov     esi, MSOPL_End_V86_App
        VMMCall Hook_Device_Service

DDI_Exit:
        clc
        ret

DDI_VM_List_Alloc_Failure:
        pop     ebx
        jmp     DDI_Exit_Failure

EndProc MSOPL_Dyn_Device_Init

VxD_INIT_CODE_ENDS

;==============================================================================
;                             P N P   C O D E
;==============================================================================

VxD_PNP_CODE_SEG

;---------------------------------------------------------------------------;
;
;   _MSOPL_IsOwned
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

BeginProc _MSOPL_IsOwned, PUBLIC

        dn              equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        push    edi

        cCall   _MSOPL_Get_pMSOI_From_XXX, <dn, pMSOI_FromDevNode>
        or      edi, edi

ifdef DEBUG
        jnz     SHORT @F

        Debug_Out "MSOPL: IsOwned, unable to locate devnode?"
@@:
endif
        jz      SHORT MIO_NotOwned

        cmp     [edi.msoi_dwSynthOwnerCur], 0
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

EndProc _MSOPL_IsOwned

;---------------------------------------------------------------------------;
;
;   MSOPL_Add_Instance_To_VM_List
;
;   DESCRIPTION:
;       Creates and adds a hardware state node to the VM list
;       associated with the pMSOI.
;
;   ENTRY:
;       EBX = VM handle to add an instance
;       EDI = pMSOI
;
;   EXIT:
;       STC if error, otherwise CLC
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSOPL_Add_Instance_To_VM_List

        push    eax
        push    ecx
        push    esi                             ; !!!! stack ordering
        push    edi                             ; !!!! assumed below...
                                                
        mov     edi, gdwCBOffset

        mov     esi, [ebx + edi].mscb_hlhws
        or      esi, esi
ifdef DEBUG
        jnz     SHORT @F
        Debug_Out "MSOPL: hardware state list handle NULL in VM????"

@@:
endif
        jz      SHORT MAITVL_Exit_Failure

        VMMCall List_Allocate
ifdef DEBUG
        jnc     SHORT @F
        Debug_Out "MSOPL: unable to allocate hardware state node"
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

        mov     edi, [esp]                      ; get pMSOI from stack
        mov     [eax.hws_pMSOI], edi
        
        VMMCall List_Attach_Tail

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

EndProc MSOPL_Add_Instance_To_VM_List

;---------------------------------------------------------------------------;
;
;   MSOPL_Remove_Instance_From_VM_List
;
;   DESCRIPTION:
;       Removes the associated hardware state node from the VM list.
;
;   ENTRY:
;       EBX = VM handle to remove an instance
;       EDI = pMSOI
;
;   EXIT:
;       nothing.
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSOPL_Remove_Instance_From_VM_List

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
        Debug_Out "MSOPL: hardware state list handle NULL in VM????"

@@:
endif
        jz      SHORT MRIFVL_Exit

        cli                                     ; playing with the list...

        VMMCall List_Get_First
        jz      SHORT MRIFVL_Exit

MRIFVL_Compare:
        cmp     [eax.hws_pMSOI], edi
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

EndProc MSOPL_Remove_Instance_From_VM_List

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
;   Allocate_Synth
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

BeginProc _Allocate_Synth

        dn              equ     [ebp + 8]
        wBaseSynth      equ     [ebp + 12]
        dwFlags         equ     [ebp + 16]

        pListNode       equ     dword ptr [ebp - 4]

        push    ebp
        mov     ebp, esp
        sub     esp, 4

        pushad

        Trace_Out "MSOPL: Allocate_Synth"

        mov     pListNode, 0

        mov     esi, ghlMSOI
        or      esi, esi
ifdef DEBUG
        jnz     SHORT @F
        Debug_Out "MSOPL: list handle NULL in MSOPL_Allocate_SB16????"

@@:
endif
        jz      AM_Exit_Failure

        movzx   edx, word ptr wBaseSynth
        cCall   _MSOPL_Get_pMSOI_From_XXX, <edx, pMSOI_FromSynth>
        or      edi, edi

ifdef DEBUG
        jz      SHORT @F

        Debug_Out "MSOPL: duplicate base in Allocate_Synth!"
@@:                       
endif
        jnz     AM_Exit_Failure

        VMMCall List_Allocate
        jc      AM_Exit_Failure

        mov     pListNode, eax
        VMMCall List_Attach_Tail

        mov     [eax.hwl_pMSOI], 0

        VMMCall _HeapAllocate, <size MSOPLINFO, HeapZeroInit>

        or      eax, eax
        jz      AM_Exit_Failure

        ;
        ; store pMSOI in list node
        ;

        mov     edi, eax
        mov     eax, pListNode
        mov     [eax.hwl_pMSOI], edi

        ;
        ; store devnode in MSOI
        ;

        mov     eax, dn
        mov     [edi.msoi_dn], eax

        mov     ax, wBaseSynth
        mov     [edi.msoi_wIOAddressSynth], ax

        mov     ax, word ptr dwFlags
        mov     [edi.msoi_wHardwareOptions], ax

        cCall   MSOPL_Get_Version
        mov     [edi].msoi_wVersionVxD, ax

ifdef DEBUG
        pushad

        movzx   edx, [edi].msoi_wIOAddressSynth

        Trace_Out "_Allocate_Synth: base #DX"

        popad
endif

        mov     esi, OFFSET32 MSOPL_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]
        movzx   edx, word ptr wBaseSynth

        Trace_Out "Install_Synth_IO_Handlers: base I/O =#DX"

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
        mov     [edi.msoi_hOPLStubs], eax
        or      eax, eax

ifdef DEBUG
        jnz     SHORT @F
        Debug_Out "MSOPL: can't trap OPL2/OPL3 ports!"
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
        call    MSOPL_Add_Instance_To_VM_List
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
        call    DeAllocate_Synth

AM_Not_Allocated:
        mov     [esp.PushAD_EAX], False

        Debug_Out "MSOPL: Allocate_Synth FAILED!"
        jmp     SHORT AM_Exit

EndProc _Allocate_Synth

;---------------------------------------------------------------------------;
;
;   DeAllocate_Synth
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

BeginProc DeAllocate_Synth

        pListNode       equ     [ebp - 4]

        push    ebp
        mov     ebp, esp
        sub     esp, 4

        pushad

        mov     pListNode, edi
        mov     edi, [edi.hwl_pMSOI]
        or      edi, edi
        jz      DM_Free_Node

        mov     eax, [edi.msoi_hOPLStubs]
        or      eax, eax
        jz      SHORT DM_No_SynthTraps

        ;
        ; Build table for Remove_Mult_IO_Handlers
        ;

        mov     esi, OFFSET32 MSOPL_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]
        movzx   edx, [edi.msoi_wIOAddressSynth]

        Trace_Out "Remove_Synth_IO_Handlers: base I/O =#DX"

        push    edi                             ; save list node
        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

DM_IO_Loop:
        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    DM_IO_Loop

        mov     eax, [esp]
        mov     eax, [eax.msoi_hOPLStubs]
        cCall   Remove_Mult_IO_Handlers_Ex
        pop     edi

DM_No_SynthTraps:

        VMMCall _HeapFree, <edi, 0>

DM_Free_Node:

        ;
        ; Remove the list node.
        ;

        mov     esi, ghlMSOI
        mov     eax, pListNode
        push    eax
        VMMCall List_Remove
        pop     eax
        VMMCall List_Deallocate

        popad
        add     esp, 4
        pop     ebp
        ret

EndProc DeAllocate_Synth

;---------------------------------------------------------------------------;
;
;   MSOPL_Set_Config
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
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc _MSOPL_Set_Config, PUBLIC

        dn              equ     [ebp + 8]
        wBaseSynth      equ     [ebp + 12]

        push    ebp
        mov     ebp, esp

        pushad

        Trace_Out "MSOPL: Set_Config"

        ;
        ; Assume failure...
        ;

        mov     [esp.PushAD_EAX], CR_FAILURE

        ;
        ; verify I/O address(es)
        ;

        movsx   edx, word ptr wBaseSynth               ; get Synth base
        call    Is_Synth_Valid
        jc      MSC_Exit_NotThere

        ;
        ; Now, put hooks into hardware accesses...
        ;

        cCall   _Allocate_Synth, <dn, wBaseSynth, eax>
        or      eax, eax
        jz      SHORT MSC_Exit

MSC_Exit_Success:
        mov     [esp.PushAD_EAX], CR_SUCCESS
        Trace_Out "MSOPL: Set_Config successful"

MSC_Exit:
        popad
        pop     ebp
        ret

MSC_Exit_NotThere:
        Debug_Out "MSOPL: Set_Config is returning CR_DEVICE_NOT_THERE"

        mov     [esp.PushAD_EAX], CR_DEVICE_NOT_THERE
        jmp     SHORT MSC_Exit

EndProc _MSOPL_Set_Config

;---------------------------------------------------------------------------;
;
;   _MSOPL_Remove_Config
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

BeginProc _MSOPL_Remove_Config, PUBLIC

        dn              equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        push    esi
        push    edi
        push    ebx
        pushfd

        cCall   _MSOPL_IsOwned, <dn>
        or      eax, eax

ifdef DEBUG
        jz      SHORT @F

        Debug_Out "MSOPL: devnode is owned and we're trying to remove?"
@@:
endif
        jz      SHORT MRC_NotOwned

        cCall   _MSOPL_Get_pMSOI_From_XXX, <dn, pMSOI_FromDevNode>
        or      edi, edi
        jz      SHORT MRC_NotOwned

        mov     ebx, [edi.msoi_dwSynthOwnerCur]
        or      ebx, ebx
        jz      SHORT MRC_NotOwned
        call    MSOPL_Release

MRC_NotOwned:

        ;
        ; Search for the devnode's list node in the hardware instance
        ; list... if it's not found, we've got a CONFIG_REMOVE from
        ; CONFIGMG when we haven't allocated any instance information,
        ; return success anyway.
        ;

        mov     ebx, dn

        mov     esi, ghlMSOI                     
        or      esi, esi                        ; Q: is list null?
        jz      SHORT MRC_Exit                  ;    Y: remove was successful.

        cli
        VMMCall List_Get_First
        jz      SHORT MRC_Exit

MRC_Compare:
        mov     edi, [eax.hwl_pMSOI]
        cmp     [edi.msoi_dn], ebx
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
        call    MSOPL_Remove_Instance_From_VM_List
        VMMCall Get_Next_VM_Handle
        cmp     ebx, [esp]
        jne     SHORT MRC_Remove_VMState_Loop
        pop     ebx                             ; get rid of Sys VM handle

        mov     edi, eax
        call    DeAllocate_Synth
        mov     eax, NOT False

MRC_Exit:
        popfd
        pop     ebx
        pop     edi
        pop     esi

        pop     ebp
        ret

EndProc _MSOPL_Remove_Config

;----------------------------------------------------------------------------
;
;   MSOPL_PnP_ConfigHandler
;
;   DESCRIPTION:
;      The procedure which we register with mmdevldr to receive our
;      CONFIG_ messages. This simply calls the MSOPL_Config_Handler
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

BeginProc MSOPL_PnP_ConfigHandler, CCALL, PUBLIC

        ArgVar  Function,DWORD
        ArgVar  SubFunction,DWORD
        ArgVar  MyDevNode,DWORD
        ArgVar  RefData,DWORD
        ArgVar  Flags,DWORD

        EnterProc

        cCall   _MSOPL_Config_Handler, <[Function],[SubFunction],\
                                           [MyDevNode],[RefData],[Flags]>
        LeaveProc

        clc

        Return

EndProc MSOPL_PnP_ConfigHandler

;----------------------------------------------------------------------------
;
;   MSOPL_PnP_New_DevNode
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

BeginProc MSOPL_PnP_New_DevNode

        Trace_Out "MSOPL: PnP_New_DevNode"

        mov     eax, ebx
        mov     ebx, offset32 MSOPL_PnP_ConfigHandler
        VxDCall MMDEVLDR_Register_Device_Driver
        mov     eax, CR_SUCCESS 

        stc
        ret
                                                           
EndProc MSOPL_PnP_New_DevNode

VxD_PNP_CODE_ENDS

;==============================================================================
;                              R A R E   C O D E
;==============================================================================

VxD_RARE_CODE_SEG

;----------------------------------------------------------------------------
;
;   MSOPL_Dyn_Device_Exit
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

BeginProc MSOPL_Dyn_Device_Exit

        mov     eax, @@DOSMGR_End_V86_App
        mov     esi, MSOPL_End_V86_App
        VMMCall Unhook_Device_Service

        ;
        ; Remove hardware instance list
        ;
        
        xor     esi, esi
        xchg    esi, ghlMSOI
        or      esi, esi
        jz      SHORT DDE_Exit

ifdef DEBUG
        VMMCall List_Get_First
        jz      SHORT @F

        Debug_Out "MSOPL: Dyn_Device_Exit - ghlMSOI has nodes!!!"
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

EndProc MSOPL_Dyn_Device_Exit

VxD_RARE_CODE_ENDS

end
