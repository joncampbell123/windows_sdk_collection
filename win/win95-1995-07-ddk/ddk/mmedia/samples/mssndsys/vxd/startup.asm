        page    60, 132

;******************************************************************************
        title   STARTUP.ASM - Initialization routines
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
;   Title:    MSSNDSYS.VXD - MICROSOFT Windows Sound System 386 Driver
;
;   Module:   STARTUP.ASM - Initialization routines
;
;   Version:  1.10
;
;   Date:     April 23, 1993
;
;******************************************************************************
;
;   Functional Description:
;      Initialization procedures for MSSNDSYS.VXD.
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
        include vkd.inc
        include vpicd.inc
        include dosmgr.inc
        include configmg.inc
        include mmdevldr.inc

        include mssndsys.inc
        include equates.inc
        include sbvirt.inc
        .list

;------------------------------------------------------------------------------
;                 E X T E R N A L    R E F E R E N C E S
;------------------------------------------------------------------------------

EXTRN _Is_AutoSelect_Valid:NEAR
EXTRN Is_CODEC_Valid:NEAR
EXTRN Is_OPL3_Valid:NEAR
EXTRN Is_Compaq_ISeries:NEAR
EXTRN _Is_AGA_Valid:NEAR
EXTRN _Validate_Compaq_Config:NEAR
EXTRN _Validate_AutoSel_IRQ:NEAR
EXTRN _Validate_AutoSel_DMA:NEAR
EXTRN _Validate_IRQ:NEAR
EXTRN _Validate_DMA:NEAR
EXTRN Get_CODEC_Class:NEAR
ifdef MSSNDSYS
EXTRN _SBVIRT_PnP_ConfigHandler:NEAR
endif

EXTRN CODEC_Reset:NEAR
EXTRN CODEC_Save:NEAR
EXTRN MSSNDSYS_Get_VM_HW_State_From_pSSI:NEAR

ifdef MSSNDSYS
EXTRN MSSNDSYS_Hot_Key_Handler:NEAR
endif

EXTRN CODEC_RegRead:NEAR
EXTRN CODEC_RegWrite:NEAR
EXTRN CODEC_InitRegs:BYTE

EXTRN MSSNDSYS_IO_Default_AutoSelect:NEAR
EXTRN MSSNDSYS_IO_Default_CODEC:NEAR
EXTRN MSSNDSYS_IO_Default_OPL3:NEAR
EXTRN MSSNDSYS_IO_Default_SB:NEAR
EXTRN MSSNDSYS_IRQ_Hw_Int_Proc:NEAR
ifdef MSSNDSYS
EXTRN MSSNDSYS_Shared_Hw_Int_Proc:NEAR
endif
EXTRN MSSNDSYS_IRQ_EOI_Proc:NEAR
EXTRN MSSNDSYS_IRQ_Mask_Changed_Proc:NEAR

EXTRN MSSNDSYS_Release_SndSys:NEAR

EXTRN MSSNDSYS_End_V86_App:NEAR
EXTRN MSSNDSYS_Virtual_DMA_Trap:NEAR

ifdef AZTECH
EXTRN SetMode_To_SBPro:NEAR
EXTRN StoreConfigRegs:NEAR
EXTRN AZT_SSI_Init: NEAR
endif

EXTRN gdwCBOffset:DWORD                 ; VM control block offset
EXTRN ghlSSI:DWORD                      ; SoundSystem Info list
EXTRN gpEndV86App:DWORD                 ; old DOSMGR_End_V86_App service ptr

;==============================================================================
;                       P A G E A B L E   D A T A
;==============================================================================

VxD_PAGEABLE_DATA_SEG

;
; Hot key handles
;

        public  gdwVolDnHKHandle
gdwVolDnHKHandle        dd      0

        public  gdwVolUpHKHandle
gdwVolUpHKHandle        dd      0

;
; Port map for AutoSelect, CODEC and OPL3 used when
; installing trap handlers.
;

Begin_VxD_IO_Table MSSNDSYS_AutoSelect_Port_Table

        ;
        ; Only the system VM will be able to see AutoSelect.
        ; No auto-acquire is needed here since all other VMs
        ; will not be able to detect AutoSelect.
        ;

        VxD_IO  SS_PAL_CONFIG,      MSSNDSYS_IO_Default_AutoSelect
        VxD_IO  SS_PAL_RESERVED01,  MSSNDSYS_IO_Default_AutoSelect
        VxD_IO  SS_PAL_RESERVED02,  MSSNDSYS_IO_Default_AutoSelect
        VxD_IO  SS_PAL_ID,          MSSNDSYS_IO_Default_AutoSelect

End_VxD_IO_Table MSSNDSYS_AutoSelect_Port_Table

Begin_VxD_IO_Table MSSNDSYS_CODEC_Port_Table

        VxD_IO  SS_CODEC_ADDRESS,   MSSNDSYS_IO_Default_CODEC
        VxD_IO  SS_CODEC_DATA,      MSSNDSYS_IO_Default_CODEC
        VxD_IO  SS_CODEC_STATUS,    MSSNDSYS_IO_Default_CODEC
        VxD_IO  SS_CODEC_DIRECT,    MSSNDSYS_IO_Default_CODEC

End_VxD_IO_Table MSSNDSYS_CODEC_Port_Table

Begin_VxD_IO_Table MSSNDSYS_OPL3_Port_Table

        VxD_IO  SS_OPL3_0,   MSSNDSYS_IO_Default_OPL3
        VxD_IO  SS_OPL3_1,   MSSNDSYS_IO_Default_OPL3
        VxD_IO  SS_OPL3_2,   MSSNDSYS_IO_Default_OPL3
        VxD_IO  SS_OPL3_3,   MSSNDSYS_IO_Default_OPL3

End_VxD_IO_Table MSSNDSYS_OPL3_Port_Table

Begin_VxD_IO_Table MSSNDSYS_SB_Port_Table

        VxD_IO  DSP_PORT_CMSD0,      MSSNDSYS_IO_Default_SB   
        VxD_IO  DSP_PORT_CMSR0,      MSSNDSYS_IO_Default_SB   
        VxD_IO  DSP_PORT_CMSD1,      MSSNDSYS_IO_Default_SB   
        VxD_IO  DSP_PORT_CMSR1,      MSSNDSYS_IO_Default_SB   
        VxD_IO  DSP_PORT_MIXREG,     MSSNDSYS_IO_Default_SB   
        VxD_IO  DSP_PORT_MIXDATA     MSSNDSYS_IO_Default_SB   
        VxD_IO  DSP_PORT_RESET,      MSSNDSYS_IO_Default_SB   
        VxD_IO  DSP_PORT_07h,        MSSNDSYS_IO_Default_SB   
        VxD_IO  DSP_PORT_FMD0,       MSSNDSYS_IO_Default_SB   
        VxD_IO  DSP_PORT_FMR0,       MSSNDSYS_IO_Default_SB   
        VxD_IO  DSP_PORT_READ,       MSSNDSYS_IO_Default_SB   
        VxD_IO  DSP_PORT_0Bh,        MSSNDSYS_IO_Default_SB   
        VxD_IO  DSP_PORT_WRITE,      MSSNDSYS_IO_Default_SB   
        VxD_IO  DSP_PORT_0Dh,        MSSNDSYS_IO_Default_SB   
        VxD_IO  DSP_PORT_DATAAVAIL,  MSSNDSYS_IO_Default_SB
        VxD_IO  DSP_PORT_0Fh,        MSSNDSYS_IO_Default_SB

End_VxD_IO_Table MSSNDSYS_SB_Port_Table


;
;   WARNING: it is assumed that the IRQ values are at the same offsets
;   as their config code counter parts. that is, 07h must be at the
;   same offset in gabIRQValid as 48h is in gabIRQConfigCodes, etc.
;

        public  gabDMAValid
gabDMAValid             db      0, 1, 3, -1

gabDMAConfigCodes       db      1, 2, 3                     

        public  gabIRQValid
gabIRQValid             db      07h, 09h, 0Ah, 0Bh, -1

        public  gabIRQConfigCodes
gabIRQConfigCodes       db      48h, 50h, 58h, 60h, -1

MSSNDSYS_IRQ_Descriptor VPICD_IRQ_Descriptor <,VPICD_Opt_Ref_Data,\
                            OFFSET32 MSSNDSYS_IRQ_Hw_Int_Proc,,     \
                            OFFSET32 MSSNDSYS_IRQ_EOI_Proc,         \
                            OFFSET32 MSSNDSYS_IRQ_Mask_Changed_Proc,,\
                            0> 

;
; error logging messages, not localizable
;

msgUnableToAllocAutoSel db      "unable to allocate AutoSel", 10, 0
msgUnableToAllocCODEC   db      "unable to allocate CODEC", 10, 0
msgUnableToAllocSB      db      "unable to allocate SB", 10, 0
msgUnableToAllocOPL3    db      "unable to allocate OPL3", 10, 0
msgUnableToAllocIRQ     db      "unable to allocate IRQ", 10, 0
msgUnableToAllocDMA     db      "unable to allocate DMA", 10, 0
msgUnableToAllocBuffer  db      "unable to allocate buffer", 10, 0

VxD_PAGEABLE_DATA_ENDS

;==============================================================================
;                                 I C O D E
;==============================================================================

VxD_INIT_CODE_SEG

;----------------------------------------------------------------------------
;
;   MSSNDSYS_Dyn_Device_Init
;
;   DESCRIPTION:
;       Device initialization entry point when dynaloaded.
;       Allocates the VM control block for MSSNDSYS.VXD.
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

BeginProc MSSNDSYS_Dyn_Device_Init

        Trace_Out "MSSNDSYS: Dyn_Device_Init"

        ;
        ; Allocate our control block
        ;

        VMMCall _Allocate_Device_CB_Area, <<size MSSNDSYS_CB_STRUCT>, 0>

        or      eax, eax                                ; Q: Got it?
        jnz     SHORT DDI_GotCB                         ;    Y: Continue

DDI_Exit_Failure:
        Trace_Out "MSSNDSYS: Dyn_Device_Init failing"
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
        mov     ghlSSI, esi

        ;
        ; Alloc VM hardware instance list for each existing VM,
        ; this maintains CODEC state information in each VM for
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
        mov     esi, MSSNDSYS_End_V86_App
        VMMCall Hook_Device_Service

ifdef MSSNDSYS
        ;
        ; Trap Cntrl-Alt-PageUp and Cntrl-Alt-PageDn for
        ; master volume control when running SB virtualization.
        ;

        mov     al, 49h                                 ; page-up
        mov     ah, ExtendedKey_B
        ShiftState <SS_Toggle_mask + SS_Either_Ctrl + SS_Either_Alt>,\
                   <SS_Ctrl + SS_Alt>
        mov     cl, CallOnPress + CallOnRepeat + Local_Key
        mov     esi, OFFSET32 MSSNDSYS_Hot_Key_Handler
        xor     edx, edx
        xor     edi, edi
        VxDCall VKD_Define_Hot_Key
        jc      SHORT DDI_Exit
        mov     gdwVolUpHKHandle, eax

        mov     al, 51h                                 ; page-down
        mov     ah, ExtendedKey_B
        ShiftState <SS_Toggle_mask + SS_Either_Ctrl + SS_Either_Alt>,\
                   <SS_Ctrl + SS_Alt>
        mov     cl, CallOnPress + CallOnRepeat + Local_Key
        mov     esi, OFFSET32 MSSNDSYS_Hot_Key_Handler
        xor     edx, edx
        xor     edi, edi
        VxDCall VKD_Define_Hot_Key
        jc      SHORT DDI_Exit
        mov     gdwVolDnHKHandle, eax
endif

DDI_Exit:
        clc
        ret

DDI_VM_List_Alloc_Failure:
        pop     ebx
        jmp     DDI_Exit_Failure

EndProc MSSNDSYS_Dyn_Device_Init

VxD_INIT_CODE_ENDS

;==============================================================================
;                             P N P   C O D E
;==============================================================================

VxD_PNP_CODE_SEG

;---------------------------------------------------------------------------;
;
;   _MSSNDSYS_IsOwned
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

BeginProc _MSSNDSYS_IsOwned, PUBLIC

        dn              equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        push    edi

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <dn, pSSI_FromDevNode>
        or      edi, edi

ifdef DEBUG
        jnz     SHORT @F

        Debug_Out "MSSNDSYS: IsOwned, unable to locate devnode?"
@@:
endif
        jz      SHORT MIO_NotOwned

        cmp     [edi.ssi_dwCODECOwnerCur], 0
        jne     SHORT MIO_Owned
        cmp     [edi.ssi_dwOPL3OwnerCur], 0
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

EndProc _MSSNDSYS_IsOwned

ifdef MSSNDSYS
;---------------------------------------------------------------------------;
;
;   _MSSNDSYS_WantSB
;
;   DESCRIPTION:
;       Checks DevNode to determine if the Sound Blaster emulation
;       is requested and/or necessary.
;
;   ENTRY:
;       DWORD dn
;           devnode
;
;       PDWORD pdwFlags
;           relative SB virtualization flags
;
;   EXIT:
;       EAX is True if wanted, False otherwise
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc _MSSNDSYS_WantSB, PUBLIC

        dn              equ     [ebp + 8]
        pdwFlags        equ     [ebp + 12]

        push    ebp
        mov     ebp, esp

        push    esi
        push    edi

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <dn, pSSI_FromDevNode>
        or      edi, edi

ifdef DEBUG
        jnz     SHORT @F

        Debug_Out "MSSNDSYS: WantSB, unable to locate devnode?"
@@:
endif
        jz      SHORT MWS_NotWanted

        mov     esi, pdwFlags
        mov     dword ptr [esi], 0

        test    [edi.ssi_wFlags], SSI_FLAG_HWSB
        jnz     SHORT MWS_NotWanted

MWS_Wanted:
        mov     eax, NOT False
        
        test    [edi.ssi_wFlags], SSI_FLAG_BUSTYPE_PCMCIA
        jz      SHORT MWS_Exit

        or      dword ptr [esi], SBVIRT_FLAG_BUSTYPE_PCMCIA
        or      dword ptr [esi], SBVIRT_FLAG_VIRT_DMA

        jmp     SHORT MWS_Exit

MWS_NotWanted:
        mov     eax, False

MWS_Exit:
        pop     edi
        pop     esi

        pop     ebp
        ret

EndProc _MSSNDSYS_WantSB

endif

;---------------------------------------------------------------------------;
;
;   _MSSNDSYS_Get_DevNode_Flags
;
;   DESCRIPTION:
;       Returns ssi_wFlags in *pdwFlags if successful.
;
;   ENTRY:
;       DWORD dn
;           devnode
;
;       PDWORD pdwFlags
;           pointer to dword to receive flags
;
;   EXIT:
;       EAX is True if successful, False otherwise
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc _MSSNDSYS_Get_DevNode_Flags, PUBLIC

        dn              equ     [ebp + 8]
        pdwFlags        equ     [ebp + 12]

        push    ebp
        mov     ebp, esp

        push    esi
        push    edi

        mov     esi, pdwFlags
        mov     dword ptr [esi], 0

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <dn, pSSI_FromDevNode>
        or      edi, edi

ifdef DEBUG
        jnz     SHORT @F

        Debug_Out "_MSSNDSYS_Get_DevNode_Flags, unable to locate devnode?"
@@:
endif
        jz      SHORT GDF_Failure

        movzx   eax, [edi.ssi_wFlags]
        mov     dword ptr [esi], eax
        mov     eax, NOT False
        jmp     SHORT GDF_Exit

GDF_Failure:
        mov     eax, False

GDF_Exit:
        pop     edi
        pop     esi

        pop     ebp
        ret

EndProc _MSSNDSYS_Get_DevNode_Flags

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Allocate_DMA_Buffer
;
;   DESCRIPTION:
;       This function allocates a DMA buffer suitable for the SndSys
;       Audio card. It attempts to allocate enough contiguous pages
;       to hold the requested size (EAX = size in _kilobytes_). If the
;       requested size cannot be allocated, the number of pages is halved
;       until the allocation succeeds.
;
;       If no pages can be allocated, EAX, EBX, ECX, and EDX will be 0 on
;       return.
;
;   ENTRY:
;       EAX = desired size in _kilobytes_ of DMA buffer to allocate. this
;             size cannot be exceed 64.
;
;   EXIT:
;       IF carry clear
;           EAX = memory handle of the memory block allocated
;           EBX = _physical address_ of memory block
;           ECX = actual size in _bytes_ of memory block allocated
;           EDX = _ring 0 linear address_ of memory block
;       ELSE carry set
;           EAX = EBX = ECX = EDX = 0
;
;   USES:
;       Flags, EAX, EBX, ECX, EDX
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Allocate_DMA_Buffer

        cmp     eax, 64
        jle     SHORT MADB_SizeOk

        Trace_Out "MSSNDSYS: Allocate_DMA_Buffer: requested size #EAX too big!"
        mov     eax, 64

MADB_SizeOk:

        add     eax, 3                  ; round up to get
        shr     eax, 2                  ; # of pages

MADB_Loop:

        mov     ebx, eax                ; EBX = # of pages to allocate
                                        ; (examples:       3     7    11
                                        ;                 12K   28K   44K
        dec     eax                     ; # pages - 1     10b  111b 1011b
        bsr     cx, ax                  ; max power of 2   1     2     3
        inc     cl                      ; shift cnt        2     3     4
        mov     eax, 1
        shl     eax, cl                 ; mask + 1       100b 1000b 10000b
        dec     eax                     ; mask            11b  111b  1111b
                                        ; alignment       16K   32K    64K

        mov     ecx, ebx
        Trace_Out "MSSNDSYS: Allocate_DMA_Buffer: pages=#ECX alignment=#EAX"

        ;
        ; At this point, we have the following information:
        ;
        ;     EAX = alignment mask for allocation
        ;     ECX = number of pages to allocate
        ;
        ; NOTE: Now retrieving the max phys limit from VDMAD.
        ;

        push    eax
        VxDCall VDMAD_Get_Max_Phys_Page
        mov     edx, 0fffh
        cmp     eax, edx
        jae     SHORT MADB_B16MB
        mov     edx, eax

MADB_B16MB:
        pop     eax

        push    ecx                     ; save this
        sub     esp, 4
        mov     ebx, esp
        VMMcall _PageAllocate <ecx, PG_SYS, 0, eax, 0, edx, ebx,\
                               <PageUseAlign+PageContig+PageFixed>>
        pop     ebx
        pop     ecx
        or      eax, eax
        jnz     short MADB_Success

        Trace_Out "MSSNDSYS: Allocate_DMA_Buffer: allocation failed! pages=#ECX"

        mov     eax, ecx
        shr     eax, 1                  ; div # pages by 2
        jnz     SHORT MADB_Loop

        Debug_Out "MSSNDSYS: Allocate_DMA_Buffer: COULD NOT ALLOCATE DMA BUFFER!!!"

        xor     ecx, ecx
        stc
        ret


MADB_Success:

        shl     ecx, 12                 ; pages-->bytes (mul by 4096)

        ;
        ; Return the following information to the caller:
        ;
        ;     EAX = memory handle of the memory block allocated
        ;     EBX = _physical address_ of memory block
        ;     ECX = size in _bytes_ of memory block allocated
        ;     EDX = _ring 0 linear address_ of memory block
        ;

        clc                             ; success
        ret

EndProc MSSNDSYS_Allocate_DMA_Buffer

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Add_Instance_To_VM_List
;
;   DESCRIPTION:
;       Creates and adds a hardware state node to the VM list
;       associated with the pSSI.
;
;   ENTRY:
;       EBX = VM handle to add an instance
;       EDI = pSSI
;
;   EXIT:
;       STC if error, otherwise CLC
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Add_Instance_To_VM_List

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
        Debug_Out "MSSNDSYS: hardware state list handle NULL in VM????"

@@:
endif
        jz      SHORT MAITVL_Exit_Failure

        cli     
        VMMCall List_Allocate
        sti
ifdef DEBUG
        jnc     SHORT @F
        Debug_Out "MSSNDSYS: unable to allocate hardware state node"
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

        mov     edi, [esp]                      ; get pSSI from stack

        push    eax
        push    edi
        push    esi
        mov     [eax.hws_pSSI], edi
        lea     edi, [eax.hws_abCODECState]
        mov     esi, OFFSET32 CODEC_InitRegs
        mov     ecx, CODEC_NUM_IDXREGS
        shr     ecx, 2
        rep     movsd
        pop     esi
        pop     edi
        pop     eax

ifdef AZTECH
        ; clock/format reg - st,22kHz

        mov     [eax.hws_abCODECState][CODEC_REG_CAPDATAFMT], 04bh
endif

        test    [edi.ssi_wFlags], SSI_FLAG_BUSTYPE_PCMCIA
        jz      SHORT MAITVL_NotPCMCIA

        ;
        ; Modify init registers for .WAV Jammer's OPL3 routing to AUX2
        ;

        mov     byte ptr [eax.hws_abCODECState][4], 0
        mov     byte ptr [eax.hws_abCODECState][5], 0

MAITVL_NotPCMCIA:

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

EndProc MSSNDSYS_Add_Instance_To_VM_List

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Remove_Instance_From_VM_List
;
;   DESCRIPTION:
;       Removes the associated hardware state node from the VM list.
;
;   ENTRY:
;       EBX = VM handle to remove an instance
;       EDI = pSSI
;
;   EXIT:
;       nothing.
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_Remove_Instance_From_VM_List

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
        Debug_Out "MSSNDSYS: hardware state list handle NULL in VM????"

@@:
endif
        jz      SHORT MRIFVL_Exit

        cli                                     ; playing with the list...

        VMMCall List_Get_First
        jz      SHORT MRIFVL_Exit

MRIFVL_Compare:
        cmp     [eax.hws_pSSI], edi
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

EndProc MSSNDSYS_Remove_Instance_From_VM_List

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
;   MSSNDSYS_Allocate_SndSys
;
;   DESCRIPTION:
;       Allocates a SNDSYSINFO structure for the given base CODEC
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

BeginProc _MSSNDSYS_Allocate_SndSys

        dn              equ     [ebp + 8]
        wBaseCODEC      equ     [ebp + 12]
        wBaseAutoSel    equ     [ebp + 16]
        wBaseAGA        equ     [ebp + 20]
        wBaseOPL3       equ     [ebp + 24]
        wBaseSB         equ     [ebp + 28]
        wIRQ            equ     [ebp + 32]
        wPlaybackDMA    equ     [ebp + 36]
        wCaptureDMA     equ     [ebp + 40]
        dwDMABufferSize equ     [ebp + 44]
        fdwOptions      equ     [ebp + 48]
        fwHardwareOpts  equ     [ebp + 52]

        pListNode       equ     dword ptr [ebp - 4]

        push    ebp
        mov     ebp, esp
        sub     esp, 4

        pushad

        Trace_Out "MSSNDSYS: Allocate_SndSys"

        mov     pListNode, 0

        mov     esi, ghlSSI
        or      esi, esi
ifdef DEBUG
        jnz     SHORT @F
        Debug_Out "MSSNDSYS: list handle NULL in MSSNDSYS_Allocate_SndSys????"

@@:
endif
        jz      MAS_Exit_Failure

        VMMCall List_Allocate
        jc      MAS_Exit_Failure

        mov     pListNode, eax
        VMMCall List_Attach_Tail

        mov     [eax.hwl_pSSI], 0

        VMMCall _HeapAllocate, <size SNDSYSINFO, HeapZeroInit>

        or      eax, eax
        jz      MAS_Exit_Failure

        ;
        ; store pSSI in list node
        ;

        mov     edi, eax
        mov     eax, pListNode
        mov     [eax.hwl_pSSI], edi

        ;
        ; store devnode in SSI
        ;

        mov     eax, dn
        mov     [edi.ssi_dn], eax

        ;
        ; set the options flags...
        ;

        mov     ax, word ptr fdwOptions
        mov     [edi.ssi_wFlags], ax


        ;
        ; store things that we found while validating...
        ;

        mov     ax, word ptr fwHardwareOpts
        mov     [edi.ssi_wHardwareOptions], ax

        ;
        ; Adjust the wBaseCODEC address to be as expected
        ; for backwards compatibility with VM clients.
        ; This value is always wBaseCODEC - 4.
        ;

        mov     ax, wBaseCODEC
        mov     [edi.ssi_wIOAddress], ax
        sub     [edi.ssi_wIOAddress], 4

        mov     [edi.ssi_wCODECBase], ax

        mov     ax, wIRQ
        mov     [edi.ssi_bIRQ], al

        test    dword ptr fdwOptions, SSI_FLAG_BUSTYPE_PCMCIA
        jnz     SHORT MAS_DualDMA

        mov     ax, wPlaybackDMA
        mov     [edi.ssi_bDMADAC], al
        mov     ax, wCaptureDMA
        mov     [edi.ssi_bDMAADC], al

        cmp     ax, wPlaybackDMA
        je      SHORT MAS_NotDualDMA

MAS_DualDMA:
        or      [edi.ssi_wHardwareOptions], DAK_DUALDMA

MAS_NotDualDMA:
        mov     ax, wBaseAGA
        mov     [edi.ssi_wAGABase], ax

        cmp     word ptr wBaseAutoSel, -1
        je      SHORT MAS_NoAutoSel

        ;
        ; This was already verified, but we need to grab
        ; the version...
        ;

        cCall   _Is_AutoSelect_Valid, <wBaseAutoSel>
        or      eax, eax
        jz      MAS_Exit_Failure

        or      [edi.ssi_wHardwareOptions], DAK_AUTOSELECT

        mov     ah, al                                  ; unpack the 
                                                        ;     product.revision
        and     ah, PAL_ID_PRODUCTBITS
        shr     ah, 3                                   ; normalize product #
        and     al, PAL_ID_REVBITS                      ; strip off revision
        mov     [edi.ssi_wVersionPAL], ax


MAS_NoAutoSel:
        cCall   MSSNDSYS_Get_Version
        mov     [edi].ssi_wVersionVxD, ax

ifdef DEBUG
        pushad
        movzx   edx, [edi].ssi_wCODECBase
        mov     bl, [edi].ssi_bIRQ

        test    dword ptr fdwOptions, SSI_FLAG_BUSTYPE_PCMCIA
        jnz     SHORT MAS_Debug_PCMCIA

        mov     ch, [edi].ssi_bDMAADC
        mov     cl, [edi].ssi_bDMADAC

        Trace_Out "MSSNDSYS_Allocate_SndSys: CODEC #DX  IRQ #BLh  DMA(ADC/DAC) #CX"

        jmp     SHORT MAS_Debug_Exit

MAS_Debug_PCMCIA:
        Trace_Out "MSSNDSYS_Allocate_SndSys (PCMCIA): CODEC #DX  IRQ #BLh"

MAS_Debug_Exit:
        popad
endif

        mov     [MSSNDSYS_IRQ_Descriptor.VID_Options], VPICD_Opt_Ref_Data
        mov     [MSSNDSYS_IRQ_Descriptor.VID_Hw_Int_Proc], OFFSET32 MSSNDSYS_IRQ_Hw_Int_Proc

ifdef MSSNDSYS

        ;
        ; Get PCCARD info and set IRQ sharing flag if necessary...
        ;

        test    dword ptr fdwOptions, SSI_FLAG_BUSTYPE_PCMCIA
        jz      DEBFAR MAS_NotPCMCIA

        sub     esp, 8
        mov     eax, esp

        mov     edx, eax
        add     edx, 4

        cCall   _MSSNDSYS_Get_PCCard_Info, <dn, eax, edx>

        ;
        ; compute SRAM base
        ;

        mov     dx, wBaseCODEC
        add     dx, word ptr [esp]
        mov     [edi.ssi_wPCMCIA_SRAMBase], dx

        ;
        ; store ID
        ;

        mov     dx, word ptr [esp+4]
        mov     [edi.ssi_wPCMCIA_Id], dx
        add     esp, 8

        cmp     eax, CR_SUCCESS
ifdef DEBUG
        je      SHORT @F
        Debug_Out "MSSNDSYS: Failed to retrieve PCCARD info in Allocate_SndSys"
@@:
endif
        jne     MAS_Exit_Failure

        cmp     dx, PCMCIA_ID_SOUNDSCSI
        jne     SHORT MAS_NotPCMCIA

        cCall   _MSSNDSYS_Get_SCSI_Base, <dn>
        cmp     eax, -1
ifdef DEBUG
        jne     SHORT @F
        Debug_Out "MSSNDSYS: Failed to retrieve SCSI base address"
@@:
endif
        je      MAS_Exit_Failure

        add     eax, 14h                        ; SCSI status is base + 14h
        mov     [edi.ssi_wSCSIStatus], ax

        or      [edi.ssi_wFlags], SSI_FLAG_IRQSHARING
        or      [MSSNDSYS_IRQ_Descriptor.VID_Options], VPICD_Opt_Can_Share
        mov     [MSSNDSYS_IRQ_Descriptor.VID_Hw_Int_Proc], OFFSET32 MSSNDSYS_Shared_Hw_Int_Proc

        Trace_Out "MSSNDSYS: sharing IRQ for Sound/SCSI II"

MAS_NotPCMCIA:

endif

        ;
        ; Virtualize IRQ...  note that for shared interrupts, the
        ; last to be virtualized is the first in the chain.
        ;

        push    edi
        movzx   ecx, [edi.ssi_bIRQ]
        mov     [MSSNDSYS_IRQ_Descriptor.VID_IRQ_Number], cx
        mov     [MSSNDSYS_IRQ_Descriptor.VID_Hw_Int_Ref], edi
        mov     edi, OFFSET32 MSSNDSYS_IRQ_Descriptor
        VxDCall VPICD_Virtualize_IRQ
        pop     edi

        jnc     SHORT MAS_Store_IRQ
        cCall   _MSSNDSYS_Log_Error, <OFFSET32 msgUnableToAllocIRQ>
        jmp     MAS_Exit_Failure

MAS_Store_IRQ:

        ;
        ; store VPICD's IRQ handle and update progress...
        ;

        mov     [edi.ssi_dwIRQHandle], eax

        ;
        ; Get auto-masking IRQ state...
        ;

        VMMCall Get_Sys_VM_Handle
        VxDCall VPICD_Get_Complete_Status
        Trace_Out "MSSNDSYS_Allocate_SndSys: IRQ complete status=#ECX"
        test    ecx, VPICD_Stat_Phys_Mask
        jnz     SHORT MAS_IRQWasMasked

        or      [edi.ssi_wFlags], SSI_FLAG_IRQWASUNMASKED

MAS_IRQWasMasked:

        test    [edi.ssi_wFlags], SSI_FLAG_IRQSHARING
        jz      SHORT MAS_IRQNotShared

        VxDCall VPICD_Physically_Unmask
        jmp     SHORT MAS_InstallIO

MAS_IRQNotShared:

        ;
        ; Clear up problems with IRQ generation...
        ;

        VxDCall VPICD_Physically_Mask
        VxDCall VPICD_Phys_EOI

MAS_InstallIO:

        ;
        ; Build table for Install_Mult_IO_Handlers
        ;
        ;
        ; NOTE! This assumes a contiguous block of I/O address space.
        ;

        test    [edi.ssi_wHardwareOptions], DAK_AUTOSELECT
        jz      SHORT MAS_Install_CODEC_Handlers

        movzx   ecx, [MSSNDSYS_AutoSelect_Port_Table.VxD_IO_Ports]
        mov     esi, OFFSET32 MSSNDSYS_AutoSelect_Port_Table
        movzx   edx, word ptr wBaseAutoSel

        Trace_Out "MSSNDSYS: Install_AutoSelect_IO_Handlers: base I/O =#DX"

        push    edi                             ; save list node
        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

MAS_AutoSelect_IO_Loop:
        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    MAS_AutoSelect_IO_Loop

        ;
        ; Tell VMM to trap AutoSelect ports.
        ;

        mov     edx, [esp]                      ; reference data
        cCall   Install_Mult_IO_Handlers_Ex
        pop     edi
        mov     [edi.ssi_hAutoSelectStubs], eax
        or      eax, eax
        jnz     SHORT MAS_Install_CODEC_Handlers

        cCall   _MSSNDSYS_Log_Error, <OFFSET32 msgUnableToAllocAutoSel>
        jmp     MAS_Exit_Failure

MAS_Install_CODEC_Handlers:
        mov     esi, OFFSET32 MSSNDSYS_CODEC_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]
        movzx   edx, word ptr wBaseCODEC

        Trace_Out "Install_CODEC_IO_Handlers: base I/O =#DX"

        call    Get_CODEC_Class                 ; determine class
        mov     [edi.ssi_bVersionCODEC], ah
        xor     ah, ah
        mov     [edi.ssi_wCODECClass], ax

        ;
        ; Build table for Install_Mult_IO_Handlers
        ;
        ; NOTE! This assumes a contiguous block of I/O address space.
        ;

        push    edi                             ; save list node
        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

MAS_CODEC_IO_Loop:
        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    MAS_CODEC_IO_Loop

        mov     edx, [esp]                      ; reference data
        cCall   Install_Mult_IO_Handlers_Ex
        pop     edi
        mov     [edi.ssi_hCODECStubs], eax
        or      eax, eax
        jnz     SHORT MAS_Install_OPL3_Handlers

        cCall   _MSSNDSYS_Log_Error, <OFFSET32 msgUnableToAllocCODEC>
        jmp     MAS_Exit_Failure

MAS_Install_OPL3_Handlers:

        movsx   edx, word ptr wBaseOPL3
        cmp     edx, -1
        je      SHORT MAS_Install_SB_Handlers

        mov     [edi.ssi_wIOAddressOPL3], dx
        or      [edi.ssi_wHardwareOptions], DAK_FMSYNTH

        mov     esi, OFFSET32 MSSNDSYS_OPL3_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]

        Trace_Out "Install_OPL3_IO_Handlers: base I/O =#DX"

        ;
        ; Build table for Install_Mult_IO_Handlers
        ;
        ; NOTE! This assumes a contiguous block of I/O address space.
        ;

        push    edi                             ; save list node
        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

MAS_OPL3_IO_Loop:
        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    MAS_OPL3_IO_Loop

        mov     edx, [esp]                      ; reference data
        cCall   Install_Mult_IO_Handlers_Ex
        pop     edi
        mov     [edi.ssi_hOPL3Stubs], eax
        or      eax, eax
        jnz     SHORT MAS_Install_SB_Handlers

        cCall   _MSSNDSYS_Log_Error, <OFFSET32 msgUnableToAllocOPL3>
        jmp     MAS_Exit_Failure

MAS_Install_SB_Handlers:
        movsx   edx, word ptr wBaseSB
        cmp     edx, -1
        je      SHORT MAS_Allocate_DMA_Buffer

        mov     [edi.ssi_wIOAddressSB], dx
        or      [edi.ssi_wFlags], SSI_FLAG_HWSB

        mov     esi, OFFSET32 MSSNDSYS_SB_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]

        Trace_Out "Install_SB_IO_Handlers: base I/O =#DX"

        ;
        ; Build table for Install_Mult_IO_Handlers
        ;
        ; NOTE! This assumes a contiguous block of I/O address space.
        ;

        push    edi                             ; save list node
        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

MAS_SB_IO_Loop:
        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    MAS_SB_IO_Loop

        mov     edx, [esp]                      ; reference data
        cCall   Install_Mult_IO_Handlers_Ex
        pop     edi
        mov     [edi.ssi_hSBStubs], eax
        or      eax, eax

        jnz     SHORT MAS_Allocate_DMA_Buffer
        cCall   _MSSNDSYS_Log_Error, <OFFSET32 msgUnableToAllocSB>
        jmp     MAS_Exit_Failure

MAS_Allocate_DMA_Buffer:

        test    dword ptr fdwOptions, SSI_FLAG_BUSTYPE_PCMCIA
ifdef DEBUG
        jz      SHORT @F
        Trace_Out "MSSNDSYS: PCMCIA adapter installed, no DMA in this rev."
@@:
endif
        jnz     MAS_Alloc_VMState

        Trace_Out "MSSNDSYS: About to allocate DMA buffer"

        ;
        ; Allocate the DMA buffer
        ;

        mov     eax, dwDMABufferSize
        call    MSSNDSYS_Allocate_DMA_Buffer
        mov     [edi.ssi_dwDMABufferHandle], eax
        mov     [edi.ssi_lpDMABufferPhys], ebx
        mov     [edi.ssi_dwDMABufferLen], ecx
        mov     [edi.ssi_lpDMABufferLinear], edx


        jnc     SHORT MAS_Alloc_Selector

        cCall   _MSSNDSYS_Log_Error, <OFFSET32 msgUnableToAllocBuffer>
        jmp     MAS_Exit_Failure

MAS_Alloc_Selector:
                                        
        VMMCall _BuildDescriptorDWORDS, <edx, ecx, <D_PRES + D_DATA + D_W + D_SEG>, 0, 0>
        VMMCall _Allocate_GDT_Selector, <edx, eax, 0>

        mov     [edi.ssi_wDMABufferSelector], ax

        ;
        ; trap DAC DMA channel
        ;

        movzx   eax, word ptr wPlaybackDMA
        mov     esi, OFFSET32 MSSNDSYS_Virtual_DMA_Trap
        VxDCall VDMAD_Virtualize_Channel
        mov     [edi.ssi_dwDMADACHandle], eax
        jnc     SHORT MAS_Alloc_ADC_Channel

        cCall   _MSSNDSYS_Log_Error, <OFFSET32 msgUnableToAllocDMA>
        jmp     MAS_Exit_Failure

MAS_Alloc_ADC_Channel:

        mov     ax, word ptr wCaptureDMA
        cmp     ax, word ptr wPlaybackDMA
        jz      SHORT MAS_Alloc_VMState

        ;
        ; trap ADC DMA channel
        ;

        movzx   eax, al
        mov     esi, OFFSET32 MSSNDSYS_Virtual_DMA_Trap
        VxDCall VDMAD_Virtualize_Channel
        mov     [edi.ssi_dwDMAADCHandle], eax
        jnc     SHORT MAS_Alloc_VMState

        cCall   _MSSNDSYS_Log_Error, <OFFSET32 msgUnableToAllocDMA>
        jmp     MAS_Exit_Failure

MAS_Alloc_VMState:

        ;
        ; Allocate the hardware state instance node for
        ; each active VM.
        ;

        VMMCall Get_Sys_VM_Handle
        push    ebx                             ; save SysVM handle

MAS_Alloc_VMState_Loop:
        call    MSSNDSYS_Add_Instance_To_VM_List
        jc      SHORT MAS_VMState_Failure
        VMMCall Get_Next_VM_Handle
        cmp     ebx, [esp]
        jne     SHORT MAS_Alloc_VMState_Loop
        pop     ebx                             ; get rid of Sys VM handle

MAS_Exit_Success:
ifdef AZTECH
        call    AZT_SSI_Init
        jc      SHORT MAS_Exit_Failure
endif
        mov     [esp.PushAD_EAX], NOT False

MAS_Exit:
        popad
        add     esp, 4
        pop     ebp
        ret

MAS_VMState_Failure:
        pop     ebx                             ; get rid of Sys VM handle

MAS_Exit_Failure:
        mov     edi, pListNode
        or      edi, edi
        jz      SHORT MAS_Not_Allocated
        call    MSSNDSYS_Deallocate_SndSys

MAS_Not_Allocated:
        mov     [esp.PushAD_EAX], False

        Debug_Out "MSSNDSYS: Allocate_SndSys FAILED!"
        jmp     SHORT MAS_Exit

EndProc _MSSNDSYS_Allocate_SndSys

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Deallocate_SndSys
;
;   DESCRIPTION:
;       Deallocates SNDSYSINFO structure and all associated traps,
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

BeginProc MSSNDSYS_Deallocate_SndSys

        pListNode       equ     [ebp - 4]

        push    ebp
        mov     ebp, esp
        sub     esp, 4

        pushad

        Assert_Ints_Enabled

        mov     pListNode, edi
        mov     edi, [edi.hwl_pSSI]
        or      edi, edi
        jz      MDS_Free_Node

        xor     esi, esi
        xchg    esi, [edi.ssi_hlPipe]
        or      esi, esi
        jz      SHORT MDS_No_hlPipe
        VMMCall List_Destroy

MDS_No_hlPipe:
        mov     eax, [edi.ssi_dwDMAADCHandle]
        or      eax, eax
        jz      SHORT MDS_No_ADCDMA
        VxDCall VDMAD_Unvirtualize_Channel

MDS_No_ADCDMA:
        mov     eax, [edi.ssi_dwDMADACHandle]
        or      eax, eax
        jz      SHORT MDS_No_DACDMA
        VxDCall VDMAD_Unvirtualize_Channel

MDS_No_DACDMA:
        movzx   eax, [edi.ssi_wDMABufferSelector]
        or      eax, eax
        jz      SHORT MDS_No_DMABuffer
        VMMCall _Free_GDT_Selector, <eax, 0>
        mov     eax, [edi.ssi_dwDMABufferHandle]
        VMMCall _PageFree, <eax, 0>

MDS_No_DMABuffer:
        mov     eax, [edi.ssi_hSBStubs]
        or      eax, eax
        jz      SHORT MDS_No_SBTraps

        ;
        ; Build table for Remove_Mult_IO_Handlers
        ;

        mov     esi, OFFSET32 MSSNDSYS_SB_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]
        movzx   edx, [edi.ssi_wIOAddressSB]

        Trace_Out "Remove_SB_IO_Handlers: base I/O =#DX"

        push    edi                             ; save list node
        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

MDS_SB_IO_Loop:
        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    MDS_SB_IO_Loop

        mov     eax, [esp]
        mov     eax, [eax.ssi_hSBStubs]
        cCall   Remove_Mult_IO_Handlers_Ex
        pop     edi

MDS_No_SBTraps:
        mov     eax, [edi.ssi_hOPL3Stubs]
        or      eax, eax
        jz      SHORT MDS_No_OPL3Traps

        ;
        ; Build table for Remove_Mult_IO_Handlers
        ;

        mov     esi, OFFSET32 MSSNDSYS_OPL3_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]
        movzx   edx, [edi.ssi_wIOAddressOPL3]

        Trace_Out "Remove_OPL3_IO_Handlers: base I/O =#DX"

        push    edi                             ; save list node
        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

MDS_OPL3_IO_Loop:
        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    MDS_OPL3_IO_Loop

        mov     eax, [esp]
        mov     eax, [eax.ssi_hOPL3Stubs]
        cCall   Remove_Mult_IO_Handlers_Ex
        pop     edi

MDS_No_OPL3Traps:
        mov     eax, [edi.ssi_hCODECStubs]
        or      eax, eax
        jz      SHORT MDS_No_CODECTraps

        ;
        ; Build table for Remove_Mult_IO_Handlers
        ;

        mov     esi, OFFSET32 MSSNDSYS_CODEC_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]
        movzx   edx, [edi.ssi_wCODECBase]

        Trace_Out "Remove_CODEC_IO_Handlers: base I/O =#DX"

        push    edi                             ; save list node
        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

MDS_CODEC_IO_Loop:
        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    MDS_CODEC_IO_Loop

        mov     eax, [esp]
        mov     eax, [eax.ssi_hCODECStubs]
        cCall   Remove_Mult_IO_Handlers_Ex
        pop     edi

MDS_No_CODECTraps:
        mov     eax, [edi.ssi_hAutoSelectStubs]
        or      eax, eax
        jz      SHORT MDS_No_AutoSelTraps

        ;
        ; Build table for Remove_Mult_IO_Handlers
        ;

        mov     esi, OFFSET32 MSSNDSYS_AutoSelect_Port_Table
        movzx   ecx, [esi.VxD_IO_Ports]
        movzx   edx, [edi.ssi_wIOAddress]

        push    edi                             ; save list node
        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

        Trace_Out "MSSNDSYS:  Remove_AutoSelect_IO_Handlers: base I/O =#DX"

MDS_AutoSelect_IO_Loop:
        mov     [esi.VxD_IO_Port], dx
        inc     edx
        add     esi, (size VxD_IO_Struc)
        loop    MDS_AutoSelect_IO_Loop

        mov     eax, [esp]
        mov     eax, [eax.ssi_hAutoSelectStubs]
        cCall   Remove_Mult_IO_Handlers_Ex
        pop     edi

MDS_No_AutoSelTraps:
        ;
        ; Unvirtualize IRQ
        ;

        mov     eax, [edi.ssi_dwIRQHandle]
        or      eax, eax
        jz      SHORT MDS_Free_SSI

        cli              
        VxDCall VPICD_Phys_EOI
        VxDCall VPICD_Physically_Mask
        VxDCall VPICD_Force_Default_Behavior
        sti
        
MDS_Free_SSI:

ifdef AZTECH
        call    SetMode_To_SBPro
        call    StoreConfigRegs
endif

        VMMCall _HeapFree, <edi, 0>

MDS_Free_Node:

        ;
        ; Remove the list node.
        ;

        mov     esi, ghlSSI
        mov     eax, pListNode
        push    eax
        VMMCall List_Remove
        pop     eax
        VMMCall List_Deallocate

        popad
        add     esp, 4
        pop     ebp
        ret

EndProc MSSNDSYS_Deallocate_SndSys

;---------------------------------------------------------------------------;
;
;   _Configure_AutoSelect
;
;   DESCRIPTION:
;       Programs the AutoSelect options as specified by CONFIGMG.
;
;   ENTRY:
;
;   EXIT:
;       EAX is True if successful, False otherwise
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc _Configure_AutoSelect, PUBLIC

        wBaseAutoSelect equ     [ebp + 8]
        wIRQ            equ     [ebp + 12]
        wPlaybackDMA    equ     [ebp + 16]
        wCaptureDMA     equ     [ebp + 20]

        push    ebp
        mov     ebp, esp

        pushad

        Trace_Out "MSSNDSYS: _Configure_AutoSelect"

        xor     eax, eax
        xor     ecx, ecx

        mov     al, byte ptr wIRQ

CAS_IRQLoop:
        cmp     gabIRQValid[ ecx ], -1
        je      SHORT CAS_Exit_Failure
        cmp     al, gabIRQValid[ ecx ]
        je      SHORT CAS_GotIRQ
        inc     ecx
        jmp     SHORT CAS_IRQLoop

CAS_GotIRQ:
        mov     ah, gabIRQConfigCodes[ ecx ]
        and     ah, 3Fh

        xor     ecx, ecx

        mov     al, byte ptr wPlaybackDMA

CAS_PBK_DMALoop:
        cmp     gabDMAValid[ ecx ], -1
        je      SHORT CAS_Exit_Failure
        cmp     al, gabDMAValid[ ecx ]
        je      SHORT CAS_GotPBKDMA
        inc     ecx
        jmp     SHORT CAS_PBK_DMALoop

CAS_GotPBKDMA:
        or      ah, gabDMAConfigCodes[ ecx ]
        mov     cx, word ptr wPlaybackDMA
        cmp     cx, word ptr wCaptureDMA
        je      SHORT CAS_SingleDMA

BUG <Need to validate 0 or 1 as capture channel>
        or      ah, 04h                                 ; dual DMA

CAS_SingleDMA:
        mov     al, ah

        movzx   edx, word ptr wBaseAutoSelect
        .errnz  SS_PAL_CONFIG
        out     dx, al

CAS_Exit_Success:
        clc

CAS_Exit:
        popad

        pop     ebp
        ret

CAS_Exit_Failure:
        stc
        jmp     SHORT CAS_Exit

EndProc _Configure_AutoSelect

;---------------------------------------------------------------------------;
;
;   _Configure_WAVJammer
;
;   DESCRIPTION:
;       Wakes up and configures the .WAV Jammer card.
;
;   ENTRY:
;       wBaseCODEC
;          base address of CODEC
;
;   EXIT:
;       EAX is True if successful, False otherwise
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc _Configure_WAVJammer, PUBLIC

        wBaseCODEC      equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        pushad

        Trace_Out "MSSNDSYS: _Configure_WAVJammer"

        movzx   edx, word ptr wBaseCODEC

        xor     al, al
        add     edx, 0Ah
        out     dx, al
        mov     ecx, 1000
        sub     edx, 0Ah

CWJ_WaitForReady:
        mov     al, 0Ah
        out     dx, al
        in      al, dx
        cmp     al, 0Ah
        jz      SHORT CWJ_Exit
        loop    CWJ_WaitForReady

CWJ_Exit:
        popad

        pop     ebp
        ret

EndProc _Configure_WAVJammer

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Set_Config
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

BeginProc _MSSNDSYS_Set_Config, PUBLIC

        dn              equ     [ebp + 8]
        wBaseCODEC      equ     [ebp + 12]
        wBaseAutoSel    equ     [ebp + 16]
        wBaseAGA        equ     [ebp + 20]
        wBaseOPL3       equ     [ebp + 24]
        wBaseSB         equ     [ebp + 28]
        wIRQ            equ     [ebp + 32]
        wPlaybackDMA    equ     [ebp + 36]
        wCaptureDMA     equ     [ebp + 40]
        dwDMABufferSize equ     [ebp + 44]
        fdwOptions      equ     [ebp + 48]

        fwHardwareOpts  equ     [ebp - 4]

        push    ebp
        mov     ebp, esp

        sub     esp, 4

        pushad

        Trace_Out "MSSNDSYS: Set_Config"

        ;
        ; Assume failure...
        ;

        mov     [esp.PushAD_EAX], CR_FAILURE

        ;
        ; Check for duplicate...
        ; 

        movzx   edx, word ptr wBaseCODEC
        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <edx, pSSI_FromCODEC>
        or      edi, edi
ifdef DEBUG
        jz      SHORT @F

        Debug_Out "MSSNDSYS: duplicate CODEC base in Allocate_Sndsys!"
@@:                       
endif
        jnz     MSC_Exit

        mov     dword ptr fwHardwareOpts, 0

        test    dword ptr fdwOptions, SSI_FLAG_BUSTYPE_ISAPNP
        jnz     MSC_Allocate_Hardware
        test    dword ptr fdwOptions, SSI_FLAG_BUSTYPE_PCMCIA
        jnz     MSC_Configure_PCMCIA

        ;
        ; verify I/O address(es)
        ;

        movzx   edx, word ptr wBaseCODEC                ; get CODEC base
        call    Is_CODEC_Valid                          ; Q: valid?
        jc      MSC_Exit_NotThere                       ;    N: bail...

        movsx   edx, word ptr wBaseOPL3                 ; get OPL3 base
        cmp     edx, -1                                 ; Q: == -1?
        je      SHORT MSC_No_OPL3                       ;    Y: no OPL3
        call    Is_OPL3_Valid                           ; Q: valid?
        jc      MSC_Exit_NotThere                       ;    N: bail...

MSC_No_OPL3:
        movsx   edx, word ptr wBaseAutoSel              ; get AutoSel base
        cmp     edx, -1                                 ; Q: == -1?
        je      SHORT MSC_No_AutoSelect                 ;    Y: no auto

        test    dword ptr fdwOptions, SSI_FLAG_BUSTYPE_PNPBIOS
        jnz     SHORT MSC_Configure_For_PNPBIOS

        cCall   _Is_AutoSelect_Valid, <edx>             ; Q: valid?
        or      eax, eax
        jz      MSC_Exit_NotThere

        cCall   _Validate_AutoSel_IRQ, <wBaseAutoSel,\
                                        wIRQ>
        or      eax, eax
        jz      MSC_Exit

        cCall   _Validate_AutoSel_DMA, <wBaseAutoSel,\
                                        wPlaybackDMA,\
                                        wCaptureDMA>
        or      eax, eax
        jz      MSC_Exit

MSC_Configure_For_PNPBIOS:

        cCall   _Configure_AutoSelect, <wBaseAutoSel,\
                                        wIRQ,\
                                        wPlaybackDMA,\
                                        wCaptureDMA>

        jmp     MSC_Allocate_Hardware

MSC_No_AutoSelect:
        cmp     word ptr wBaseAGA, -1
        je      SHORT MSC_Brute_Force

        cCall   _Is_AGA_Valid, <wBaseCODEC, wBaseAGA, fIAV_ForReal>
        or      eax, eax
        jz      SHORT MSC_Try_Compaq_Jumpered

        or      dword ptr fwHardwareOpts, DAK_COMPAQBA

        cCall   _Validate_Compaq_Config, <wBaseAGA,\
                                          wIRQ,\
                                          wPlaybackDMA,\
                                          wCaptureDMA>  ; Q: cfg look valid?
        or      eax, eax
        jz      MSC_Exit                                ;    N: failure
        jmp     SHORT MSC_Allocate_Hardware

MSC_Try_Compaq_Jumpered:
        call    Is_Compaq_ISeries                       ; Q: I-Series?
        jc      SHORT MSC_Brute_Force                   ;    N: Standard DAK

        or      dword ptr fwHardwareOpts, DAK_COMPAQBA + DAK_COMPAQI

        ;
        ; Fall-through to validate IRQ/DMA for these jumpered I-boxes
        ;

MSC_Brute_Force:
        cCall   _Validate_IRQ, <wBaseCODEC, wIRQ>       ; Q: IRQ valid?
        or      eax, eax
        jz      SHORT MSC_Exit                          ;    N: failure

        cCall   _Validate_DMA, <wBaseCODEC,\
                                wPlaybackDMA,\          ; Q: DMA valid?
                                wCaptureDMA>
        or      eax, eax
        jz      SHORT MSC_Exit                          ;    N: failure
        jmp     SHORT MSC_Allocate_Hardware

MSC_Configure_PCMCIA:
        xor     eax, eax
        mov     [edi.ssi_dwDMABufferHandle], eax
        mov     [edi.ssi_lpDMABufferPhys], eax
        mov     [edi.ssi_lpDMABufferLinear], eax

        ;
        ; .WAVJammer "DMA buffer" is 2x16x8 SRAM 
        ;

        mov     [edi.ssi_dwDMABufferLen], 8000h

        cCall   _Configure_WAVJammer, <wBaseCODEC>

MSC_Allocate_Hardware:

        ;
        ; Now, put hooks into hardware accesses...
        ;

        cCall   _MSSNDSYS_Allocate_SndSys, <dn, wBaseCODEC, wBaseAutoSel,\
                                            wBaseAGA, wBaseOPL3, wBaseSB,\
                                            wIRQ, wPlaybackDMA, wCaptureDMA,\
                                            dwDMABufferSize, fdwOptions,\
                                            fwHardwareOpts>
        or      eax, eax
        jz      SHORT MSC_Exit

MSC_Exit_Success:
        mov     [esp.PushAD_EAX], CR_SUCCESS
        Trace_Out "MSSNDSYS: Set_Config successful"

MSC_Exit:
        popad
        add     esp, 4
        pop     ebp
        ret

MSC_Exit_NotThere:
        Debug_Out "MSSNDSYS: Set_Config is returning CR_DEVICE_NOT_THERE"

        mov     [esp.PushAD_EAX], CR_DEVICE_NOT_THERE
        jmp     SHORT MSC_Exit
        
EndProc _MSSNDSYS_Set_Config

;---------------------------------------------------------------------------;
;
;   _MSSNDSYS_Remove_Config
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

BeginProc _MSSNDSYS_Remove_Config, PUBLIC

        dn      equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        push    esi
        push    edi
        push    ebx
        pushfd

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <dn, pSSI_FromDevNode>
        or      edi, edi
        jz      MRC_Exit_Success

        cCall   _MSSNDSYS_IsOwned, <dn>
        or      eax, eax

ifdef DEBUG
        jz      SHORT @F

        Debug_Out "MSSNDSYS: devnode is owned and we're trying to remove?"
@@:
endif
        jz      SHORT MRC_NotOwned

        mov     ebx, [edi.ssi_dwCODECOwnerCur]
        or      ebx, ebx
        jz      SHORT MRC_NotCODEC
        mov     eax, fSS_ASS_Acquire_CODEC
        call    MSSNDSYS_Release_SndSys

MRC_NotCODEC:
        mov     ebx, [edi.ssi_dwOPL3OwnerCur]
        or      ebx, ebx
        jz      SHORT MRC_NotOwned
        mov     eax, fSS_ASS_Acquire_OPL3
        call    MSSNDSYS_Release_SndSys

MRC_NotOwned:

        cli

        mov     eax, [edi.ssi_dwDMADACHandle]
        or      eax, eax
        jz      SHORT MRC_NoDMADAC

        VxDCall VDMAD_Phys_Mask_Channel

MRC_NoDMADAC:
        mov     eax, [edi.ssi_dwDMAADCHandle]
        or      eax, eax
        jz      SHORT MRC_NoDMAADC

        VxDCall VDMAD_Phys_Mask_Channel

MRC_NoDMAADC:

        ; 
        ; Remove IRQ/DMA configuration from AutoSelect
        ;

        test    [edi.ssi_wHardwareOptions], DAK_AUTOSELECT
        jz      SHORT MRC_NoAutoSelect
        movzx   edx, [edi.ssi_wIOAddress]
        Trace_Out "MSSNDSYS: clearing AutoSel IRQ/DMA setting #EDX"
        xor     eax, eax
        out     dx, al

MRC_NoAutoSelect:
        
        ;
        ; Search for the devnode's list node in the hardware instance
        ; list... if it's not found, we've got a CONFIG_REMOVE from
        ; CONFIGMG when we haven't allocated any instance information,
        ; return success anyway.
        ;

        mov     ebx, dn

        mov     esi, ghlSSI                     
        or      esi, esi                        ; Q: is list null?
        jz      SHORT MRC_Exit                  ;    Y: remove was successful.

        ;
        ; Walks the list and finds the pListNode for the device.
        ; stores in EAX
        ;

        Assert_Ints_Disabled
        VMMCall List_Get_First
        jz      SHORT MRC_Exit

MRC_Compare:
        mov     edi, [eax.hwl_pSSI]
        cmp     [edi.ssi_dn], ebx
        je      SHORT MRC_FoundNode

        VMMCall List_Get_Next
        jz      SHORT MRC_Exit
        jmp     SHORT MRC_Compare

MRC_FoundNode:
        sti


        ;
        ; Remove the hardware state instance node for each active VM.
        ;

        VMMCall Get_Sys_VM_Handle
        push    ebx                             ; save SysVM handle

MRC_Remove_VMState_Loop:
        call    MSSNDSYS_Remove_Instance_From_VM_List
        VMMCall Get_Next_VM_Handle
        cmp     ebx, [esp]
        jne     SHORT MRC_Remove_VMState_Loop
        pop     ebx                             ; get rid of Sys VM handle

        mov     edi, eax                        ; This is pListNode
                                                ; from search above @
                                                ; MRC_Compare
        call    MSSNDSYS_Deallocate_SndSys

        jz      SHORT MRC_Exit_Success

MRC_Exit_Success:
        mov     eax, NOT False

MRC_Exit:
        popfd
        pop     ebx
        pop     edi
        pop     esi

        pop     ebp
        ret

EndProc _MSSNDSYS_Remove_Config

;----------------------------------------------------------------------------
;
;   MSSNDSYS_PnP_ConfigHandler
;
;   DESCRIPTION:
;      The procedure which we register with mmdevldr to receive our
;      CONFIG_ messages. This simply calls the MSSNDSYS_Config_Handler
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

BeginProc MSSNDSYS_PnP_ConfigHandler, CCALL, PUBLIC

        ArgVar  cf,DWORD
        ArgVar  scf,DWORD
        ArgVar  dn,DWORD
        ArgVar  refdata,DWORD
        ArgVar  flags,DWORD

        EnterProc
        cCall   _MSSNDSYS_Config_Handler, <cf, scf, dn, refdata, flags>
        LeaveProc

        clc

        Return

EndProc MSSNDSYS_PnP_ConfigHandler

;----------------------------------------------------------------------------
;
;   MSSNDSYS_PnP_New_DevNode
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
;      eax = CR return value
;
;   USES:
;       FLAGS
;----------------------------------------------------------------------------

BeginProc MSSNDSYS_PnP_New_DevNode

        Trace_Out "MSSNDSYS: PnP_New_DevNode"

        push    ebp
        mov     ebp, esp
        sub     esp, 4

ifdef MSSNDSYS
        mov     eax, esp
        VxDCall _CONFIGMG_Get_Parent, <eax, ebx, 0>
        cmp     eax, CR_NO_SUCH_DEVNODE
        jz      SHORT MPND_Not_Child
        cmp     eax, CR_SUCCESS
        jz      SHORT MPND_Check_For_Enum
        jmp     SHORT MPND_Exit

MPND_Check_For_Enum:
        push    edi
        mov     eax, [esp + 4]
        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <eax, pSSI_FromDevNode>
        or      edi, edi
        pop     edi
        jz      SHORT MPND_Not_Child

        Trace_Out "MSSNDSYS: PnP_New_DevNode - child DevNode starting..."

        VxDCall _CONFIGMG_Register_Device_Driver, <ebx, OFFSET32 _SBVIRT_PnP_ConfigHandler, eax, CM_REGISTER_DEVICE_DRIVER_REMOVABLE + CM_REGISTER_DEVICE_DRIVER_DISABLEABLE>
        jmp     SHORT MPND_Exit

MPND_Not_Child:

endif
        mov     eax, ebx
        mov     ebx, OFFSET32 _MSSNDSYS_PnP_ConfigHandler
        VxDCall MMDEVLDR_Register_Device_Driver
        mov     eax, CR_SUCCESS 

MPND_Exit:
        add     esp, 4
        pop     ebp
        stc
        ret
                                                           
EndProc MSSNDSYS_PnP_New_DevNode

VxD_PNP_CODE_ENDS

;==============================================================================
;                              R A R E   C O D E
;==============================================================================

VxD_RARE_CODE_SEG

;----------------------------------------------------------------------------
;
;   MSSNDSYS_Dyn_Device_Exit
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

BeginProc MSSNDSYS_Dyn_Device_Exit

        Assert_Ints_Enabled

        mov     eax, @@DOSMGR_End_V86_App
        mov     esi, MSSNDSYS_End_V86_App
        VMMCall Unhook_Device_Service

ifdef MSSNDSYS
        mov     eax, gdwVolDnHKHandle
        VxDCall VKD_Remove_Hot_Key

        mov     eax, gdwVolUpHKHandle
        VxDCall VKD_Remove_Hot_Key
endif

        ;
        ; Remove hardware instance list
        ;
        
        xor     esi, esi
        xchg    esi, ghlSSI
        or      esi, esi
        jz      SHORT DDE_Exit

ifdef DEBUG
        VMMCall List_Get_First
        jz      SHORT @F

        Debug_Out "MSSNDSYS: Dyn_Device_Exit - ghlSSI has nodes!!!"
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

EndProc MSSNDSYS_Dyn_Device_Exit

;==============================================================================
;                   A P M   S U P P O R T   F U N C T I O N S
;==============================================================================

;---------------------------------------------------------------------------;
;
;   _MSSNDSYS_Suspend
;
;   DESCRIPTION:
;       Prepares DevNode for suspend.
;
;   ENTRY:
;       DWORD dn
;           devnode
;
;   EXIT:
;       Nothing.
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc _MSSNDSYS_Suspend, PUBLIC

        dn              equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        push    ebx
        push    edi
        push    esi

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <dn, pSSI_FromDevNode>
        or      edi, edi

ifdef DEBUG
        jnz     SHORT @F

        Debug_Out "MSSNDSYS: Suspend, unable to locate devnode?"
@@:
endif
        jz      DEBFAR MSUS_Exit

        mov     ebx, [edi.ssi_dwCODECOwnerCur]
        or      ebx, ebx
        jz      DEBFAR MSUS_Exit
        VMMCall Test_Sys_VM_Handle
        je      DEBFAR MSUS_Exit

        ;
        ; We are suspending and we have a CODEC owner...
        ;

        Trace_Out "MSSNDSYS: suspending with non-Sys VM CODEC owner #EBX"

        call    MSSNDSYS_Get_VM_HW_State_From_pSSI
ifdef DEBUG
        jnc     SHORT @F
        Debug_Out "MSSNDSYS: Suspend can't save, pSSI invalid?"
@@:
endif

        jc      SHORT MSUS_Exit

        ;
        ; ESI = VM's hardware state node (phws)
        ;

        call    CODEC_Save

        ;
        ; Save DMA state
        ;

BUG <Should probably save ADC DMA state as well>

        push    ecx
        push    edx
        push    edi

        mov     eax, [edi.ssi_dwDMADACHandle]
        or      eax, eax
        jz      SHORT MSUS_DAC_Masked
        mov     edi, esi
        VxDCall VDMAD_Get_Virt_State
        mov     [edi.hws_wDMAMode], dx
        mov     [edi.hws_dwDMACount], ecx
        mov     [edi.hws_dwDMAAddr], esi
        mov     esi, edi

        test    [esi.hws_wDMAMode], DMA_masked
        jnz     SHORT MSUS_DAC_Masked

        VxDCall VDMAD_Mask_Channel

MSUS_DAC_Masked:
        pop     edi

        ;
        ; stop DMA and interrupts
        ;

        movzx   edx, [edi.ssi_wCODECBase]
        mov     ah, CODEC_REG_INTERFACE
        xor     al, al
        call    CODEC_RegWrite
        inc     ah
        call    CODEC_RegWrite

        pop     edx
        pop     ecx

        or      [edi.ssi_wFlags], SSI_FLAG_SUSPENDED

MSUS_Exit:
        pop     esi
        pop     edi
        pop     ebx

        pop     ebp
        ret

EndProc _MSSNDSYS_Suspend

;---------------------------------------------------------------------------;
;
;   _MSSNDSYS_Resume
;
;   DESCRIPTION:
;       Prepares DevNode for resume.
;
;   ENTRY:
;       DWORD dn
;           devnode
;
;   EXIT:
;       Nothing.
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc _MSSNDSYS_Resume, PUBLIC

        dn              equ     [ebp + 8]

        wBaseIO         equ     [ebp - 4]
        wIRQ            equ     [ebp - 8]
        wPlaybackDMA    equ     [ebp - 12]
        wCaptureDMA     equ     [ebp - 16]

        push    ebp
        mov     ebp, esp
        sub     esp, 16

        push    ebx
        push    edi
        push    esi

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <dn, pSSI_FromDevNode>
        or      edi, edi

ifdef DEBUG
        jnz     SHORT @F

        Debug_Out "MSSNDSYS: Resume, unable to locate devnode?"
@@:
endif
        jz      MRES_Exit

        ;
        ; Reconfigure hardware if necessary...
        ;

        movzx   eax, [edi.ssi_wIOAddress]
        mov     wBaseIO, eax
        movzx   eax, [edi.ssi_bIRQ]
        mov     wIRQ, eax
        movzx   eax, [edi.ssi_bDMADAC]
        mov     wPlaybackDMA, eax
        movzx   eax, [edi.ssi_bDMAADC]
        mov     wCaptureDMA, eax

        test    [edi.ssi_wFlags], SSI_FLAG_BUSTYPE_ISAPNP

        jnz     SHORT MRES_Check_Resume

        test    [edi.ssi_wFlags], SSI_FLAG_BUSTYPE_PCMCIA
        jnz     SHORT MRES_Config_PCMCIA

        test    [edi.ssi_wHardwareOptions], DAK_AUTOSELECT
        jz      SHORT MRES_Check_Resume

        cCall   _Configure_AutoSelect, <wBaseIO,\
                                        wIRQ,\
                                        wPlaybackDMA,\
                                        wCaptureDMA>

        jmp     SHORT MRES_Check_Resume

MRES_Config_PCMCIA:

        cCall   _Configure_WAVJammer, <wBaseIO>

MRES_Check_Resume:

        mov     ebx, [edi.ssi_dwCODECOwnerCur]
        or      ebx, ebx
        jz      MRES_Exit
        VMMCall Test_Sys_VM_Handle
        je      MRES_Exit

        ;
        ; If we didn't save anything then we're done...
        ;

        test    [edi.ssi_wFlags], SSI_FLAG_SUSPENDED
        jz      MRES_Exit

        ;
        ; We are resuming and we have a CODEC owner...
        ;

        Trace_Out "MSSNDSYS: resuming with non-Sys VM CODEC owner #EBX"

        call    MSSNDSYS_Get_VM_HW_State_From_pSSI
ifdef DEBUG
        jnc     SHORT @F
        Debug_Out "MSSNDSYS: Resume can't restore, pSSI invalid?"
@@:
endif

        jc      MRES_Exit

        ;
        ; ESI = VM's hardware state node (phws)
        ;

        ;
        ; force ACAL...
        ; 

        or      byte ptr [esi.hws_abCODECState + CODEC_REG_INTERFACE], \
                                                           AD1848_CONFIG_ACAL

        call    CODEC_Reset

        ;
        ; Restore DMA state
        ;

BUG <Should probably restore ADC DMA state as well>

        push    ecx
        push    edx
        push    edi

        mov     eax, [edi.ssi_dwDMADACHandle]
        or      eax, eax
        jz      SHORT MRES_DAC_Masked

        mov     edi, esi
        mov     dx, [edi.hws_wDMAMode]
        mov     ecx, [edi.hws_dwDMACount]
        mov     esi, [edi.hws_dwDMAAddr]
        VxDCall VDMAD_Set_Virt_State
        mov     esi, edi

        test    dl, DMA_masked
        jnz     SHORT MRES_DAC_Masked

        VxDCall VDMAD_Unmask_Channel

MRES_DAC_Masked:
        pop     edi

        ;
        ; restart DMA and interrupts
        ;

        movzx   edx, [edi.ssi_wCODECBase]

        mov     ecx, CODEC_REG_INTERFACE
        mov     ah, cl
        and     [esi.hws_abCODECState][ecx], NOT AD1848_CONFIG_ACAL
        mov     al, [esi.hws_abCODECState][ecx]
        call    CODEC_RegWrite
        inc     ecx
        inc     ah
        mov     al, [esi.hws_abCODECState][ecx]
        call    CODEC_RegWrite

        pop     edx
        pop     ecx

        and     [edi.ssi_wFlags], NOT SSI_FLAG_SUSPENDED

MRES_Exit:
        pop     esi
        pop     edi
        pop     ebx
        
        add     esp, 16
        pop     ebp
        ret

EndProc _MSSNDSYS_Resume

VxD_RARE_CODE_ENDS

end
