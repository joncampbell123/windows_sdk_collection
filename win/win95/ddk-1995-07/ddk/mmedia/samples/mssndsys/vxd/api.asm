        page    60, 132

;******************************************************************************
        title   API.ASM - Initialization routines
;******************************************************************************
;---------------------------------------------------------------------------;
;
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1992 - 1995	Microsoft Corporation.	All Rights Reserved.
;
;---------------------------------------------------------------------------;
;
;   Title:    MSSNDSYS.386 - MICROSOFT Windows Sound System 386 Driver
;
;   Module:   API.ASM - Application Programming Interface
;
;   Version:  4.00
;
;   Date:     April 23, 1993
;
;******************************************************************************
;
;   Functional Description:
;      Provides V86/PM API for DOS apps or Windows driver.
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
        include mssndsys.inc
        include msopl.inc
        include equates.inc
        .list

EXTRN ghlSSI:DWORD                      ; SoundSystem Info list

EXTRN MSSNDSYS_Acquire_SndSys:NEAR
EXTRN MSSNDSYS_Release_SndSys:NEAR

EXTRN _pipeOpen:NEAR
EXTRN _pipeClose:NEAR

;==============================================================================
;                       P A G E A B L E   D A T A
;==============================================================================

VxD_PAGEABLE_DATA_SEG

;------------------------------------------------------------------------------
;                 A P I   D I S P A T C H   T A B L E
;------------------------------------------------------------------------------

MSSNDSYS_API_Table  LABEL DWORD
        dd  OFFSET32 MSSNDSYS_API_Get_Version
        dd  OFFSET32 MSSNDSYS_API_Get_Info
        dd  OFFSET32 MSSNDSYS_API_Acquire
        dd  OFFSET32 MSSNDSYS_API_Release
        dd  OFFSET32 MSSNDSYS_API_Copy_To_Buffer
        dd  OFFSET32 MSSNDSYS_API_Copy_From_Buffer
        dd  OFFSET32 MSSNDSYS_API_Get_DMA_Count
MSSNDSYS_API_MAX    equ ($-MSSNDSYS_API_TABLE)/4

MSOPL_API_Table LABEL DWORD
        dd  OFFSET32 MSSNDSYS_API_Get_Version
        dd  OFFSET32 OPL_API_Get_Info
        dd  OFFSET32 OPL_API_Acquire
        dd  OFFSET32 OPL_API_Release
MSOPL_API_MAX       equ ($-MSOPL_API_TABLE)/4

PIPE_API_Table LABEL DWORD
        dd  OFFSET32 VxD_API_Open_Pipe
        dd  OFFSET32 VxD_API_Close_Pipe
PIPE_API_MAX        equ ($-PIPE_API_TABLE)/4
                    
MSSNDSYS_Func_Tables LABEL DWORD
        dd  MSSNDSYS_API_MAX,   OFFSET32 MSSNDSYS_API_Table
        dd  MSOPL_API_MAX,      OFFSET32 MSOPL_API_Table
        dd  PIPE_API_MAX,       OFFSET32 PIPE_API_Table
MSSNDSYS_FUNC_HANDLERS_MAX equ ($-MSSNDSYS_Func_Tables)/8

VxD_PAGEABLE_DATA_ENDS

;==============================================================================
;                        P A G E A B L E   C O D E
;==============================================================================

VxD_PAGEABLE_CODE_SEG

;---------------------------------------------------------------------------;
;
;   MSSNDSYS_API_Handler
;
;   DESCRIPTION:
;       This is the single entry point for a VM (pmode or rmode) to make
;       a SndSys driver's life easier
;
;   ENTRY:
;       EBX = Current VM Handle
;       EBP = Pointer to Client Register Structure.
;
;       Client_DX = Function number
;
;       And API specific Client_?? registers.
;
;   EXIT:
;       Client_EFLAGS _PLUS_ any API specific Client_?? registers.
;
;   USES:
;       FLAGS
;
;---------------------------------------------------------------------------;

BeginProc MSSNDSYS_API_Handler, PUBLIC

        movzx   eax, [ebp.Client_DX]
        cmp     ah, MSSNDSYS_FUNC_HANDLERS_MAX
        jae     SHORT MSSNDSYS_API_Fail
        movzx   ecx, ah
        shl     ecx, 3
        lea     ecx, MSSNDSYS_Func_Tables[ ecx ]
        movzx   eax, al
        cmp     eax, dword ptr [ecx]
        jae     SHORT MSSNDSYS_API_Fail
        mov     ecx, dword ptr [ecx + 4]
        call    dword ptr [ecx][eax * 4]
        jc      SHORT MSSNDSYS_API_Fail

MSSNDSYS_API_Success:
        and     [ebp.Client_Flags], not CF_Mask ; clear carry
        ret

MSSNDSYS_API_Fail:
        or      [ebp.Client_Flags], CF_Mask     ; set carry
        ret

EndProc MSSNDSYS_API_Handler

BeginDoc
;---------------------------------------------------------------------------;
;
;   MSSNDSYS_API_Copy_To_Buffer, PMAPI, RMAPI
;
;   DESCRIPTION:
;       This function is used to copy a buffer to the DMA buffer allocated
;       by MSSNDSYS.  The intention of this API is to provide a mechanism
;       for V86 VM's to implement auto-init DMA using our DMA buffer.
;
;   ENTRY:
;       Client_ES = selector/segment of buffer to copy from
;       Client_DI = offset of buffer to copy from
;       Client_BX = offset of DMA buffer to copy into
;       Client_CX = size of copy
;
;   EXIT:
;       IF carry clear
;           success
;           Client_AX = non-zero, buffer was successfully copied
;       ELSE carry set
;           Client_AX = 0, failure
;
;   USES:
;       Flags, EAX, ECX, ESI, EDI
;
;---------------------------------------------------------------------------;
EndDoc

BeginProc MSSNDSYS_API_Copy_To_Buffer

        Assert_Client_Ptr ebp
        Assert_VM_Handle ebx

        Trace_Out "MSSNDSYS: Copy_To_Buffer API called"

        ;
        ; NOTE!  For compatibility, this routine only checks
        ; the first hardware instance, there is no V86 method
        ; acquire any other instance of the hardware.
        ;

        mov     esi, ghlSSI 
        or      esi, esi
        jz      SHORT MACTB_Exit_Failure
        pushfd
        cli
        VMMCall List_Get_First
        popfd
        or      eax, eax
        jz      SHORT MACTB_Exit_Failure

        mov     edi, [eax.hwl_pSSI]
        cmp     ebx, [edi.ssi_dwCODECOwnerCur]
        jne     SHORT MACTB_Exit_Failure

        ;
        ; Grab location of user buffer and validate.
        ;

        Client_Ptr_Flat esi, ES, DI
        cmp     esi, -1
        je      SHORT MACTB_Exit_Failure

        ;
        ; Test for overwrite... fail if user is giving
        ; us bogus values.
        ;

        movzx   ecx, [ebp.Client_BX]
        movzx   eax, [ebp.Client_CX]
        add     ecx, eax
        cmp     ecx, [edi.ssi_dwDMABufferLen]
        ja      SHORT MACTB_Exit_Failure

        ;
        ; Everything seems to check out... go ahead and copy.
        ;

        movzx   ecx, [ebp.Client_CX]
        mov     edi, [edi.ssi_lpDMABufferLinear]
        movzx   eax, [ebp.Client_BX]
        add     edi, eax

        ;
        ; Make sure they specified a size.
        ;

        or      ecx, ecx
        jz      SHORT MACTB_Exit_Failure

        cld
        shr     ecx, 1
        rep     movsw
        adc     cl, cl
        rep     movsb

        mov     [ebp.Client_AX], 1          ; success
        clc
        ret


MACTB_Exit_Failure:
        Debug_Out "MSSNDSYS: Copy_To_Buffer: API FAILED!!"
        mov     [ebp.Client_AX], 0          ; failed
        stc
        ret

EndProc MSSNDSYS_API_Copy_To_Buffer

BeginDoc
;---------------------------------------------------------------------------;
;
;   MSSNDSYS_API_Copy_From_Buffer, PMAPI, RMAPI
;
;   DESCRIPTION:
;       This function is used to copy from the DMA buffer allocated
;       by MSSNDSYS to the user's buffer.  The intention of this API
;       is to provide a mechanism for V86 VM's to implement auto-init
;       DMA using our DMA buffer.
;
;   ENTRY:
;       Client_ES = selector/segment of buffer to copy to
;       Client_DI = offset of buffer to copy to
;       Client_BX = offset into DMA buffer to copy from
;       Client_CX = size of copy
;
;   EXIT:
;       IF carry clear
;           success
;           Client_AX = non-zero, buffer was successfully copied
;       ELSE carry set
;           Client_AX = 0, failure
;
;   USES:
;       Flags, EAX, ECX, ESI, EDI
;
;---------------------------------------------------------------------------;
EndDoc

BeginProc MSSNDSYS_API_Copy_From_Buffer

        Assert_Client_Ptr ebp
        Assert_VM_Handle ebx

        Trace_Out "MSSNDSYS: Copy_From_Buffer API called"

        ;
        ; NOTE!  For compatibility, this routine only checks
        ; the first harware instance, there is no V86 method
        ; acquire any other instance of the hardware.
        ;

        mov     esi, ghlSSI 
        or      esi, esi
        jz      SHORT MACFB_Exit_Failure
        pushfd
        cli
        VMMCall List_Get_First
        popfd
        or      eax, eax
        jz      SHORT MACFB_Exit_Failure

        mov     esi, [eax.hwl_pSSI]
        cmp     ebx, [esi.ssi_dwCODECOwnerCur]
        jne     SHORT MACFB_Exit_Failure

        ;
        ; Grab location of user buffer and validate.
        ;

        Client_Ptr_Flat edi, ES, DI
        cmp     edi, -1
        je      SHORT MACFB_Exit_Failure

        ;
        ; Test for overwrite... fail if user is giving
        ; us bogus values.
        ;

        movzx   ecx, [ebp.Client_BX]
        movzx   eax, [ebp.Client_CX]
        add     ecx, eax
        cmp     ecx, [esi.ssi_dwDMABufferLen]
        ja      SHORT MACFB_Exit_Failure

        ;
        ; Everything seems to check out... go ahead and copy.
        ;

        movzx   ecx, [ebp.Client_CX]
        mov     esi, [esi.ssi_lpDMABufferLinear]
        movzx   eax, [ebp.Client_BX]
        add     esi, eax

        ;
        ; Make sure they specified a size.
        ;

        or      ecx, ecx
        jz      SHORT SHORT MACFB_Exit_Failure

        cld
        shr     ecx, 1
        rep     movsw
        adc     cl, cl
        rep     movsb

        mov     [ebp.Client_AX], 1          ; success
        clc
        ret


MACFB_Exit_Failure:
        Debug_Out "MSSNDSYS: Copy_From_Buffer API FAILED!!"
        mov     [ebp.Client_AX], 0          ; failed
        stc
        ret

EndProc MSSNDSYS_API_Copy_From_Buffer

BeginDoc
;---------------------------------------------------------------------------;
;
;   MSSNDSYS_API_Get_DMA_Count, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Returns current DMA count of controller for given DevNode.
;
;   ENTRY:
;       EBX = Current VM Handle
;       EBP = Pointer to Client Register Structure.
;
;       Client_DX = MSS_API_Get_DMA_Count
;       Client_BX = Subfunction
;           sfSS_GDC_Invalid    equ     0000h
;           sfSS_GDC_DAC_Count  equ     0001h
;           sfSS_GDC_ADC_Count  equ     0002h
;       Client_ECX = DevNode
;
;   EXIT:
;       Client_EFLAGS = CY if error, else NC
;       Client_AX = DMA count
;
;   USES:
;       FLAGS, EAX
;
;---------------------------------------------------------------------------;
EndDoc

BeginProc MSSNDSYS_API_Get_DMA_Count

        push    edi

        Assert_Client_Ptr ebp

        mov     eax, [ebp.Client_ECX]
        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <eax, pSSI_FromDevNode>

ifdef DEBUG
        jnc     SHORT @F
        Debug_Out "MSSNDSYS_API_Get_DMA_Count: invalid devnode (#EAX)"
@@:
endif
        jc      SHORT MAGDC_Exit_Failure

        movzx   eax, [ebp.Client_BX]
        or      eax, eax

ifdef DEBUG
        jnz     SHORT @F
        Debug_Out "MSSNDSYS_API_Get_DMA_Count: invalid subfunction (#AX)"
@@:
endif
        jz      SHORT MAGDC_Exit_Failure

        cmp     eax, sfSS_GDC_DAC_Count
        jne     SHORT MAGDC_ADC_Count

        mov     eax, [edi.ssi_dwDMADACHandle]
        jmp     SHORT MAGDC_Get_Count

MAGDC_ADC_Count:
        mov     eax, [edi.ssi_dwDMAADCHandle]
        or      eax, eax
        jnz     SHORT MAGDC_Get_Count
        mov     eax, [edi.ssi_dwDMADACHandle]

MAGDC_Get_Count:
        or      eax, eax

ifdef DEBUG
        jnz     SHORT @F
        Debug_Out "MSSNDSYS_API_Get_DMA_Count: dma not virtualized"
@@:
endif
        jz      SHORT MAGDC_Exit_Failure

        push    ecx
        VxDCall VDMAD_Get_Phys_Count            ; get current byte count in CX
        mov     [ebp.Client_AX], cx             ; dump it in client's AX

        pop     ecx

        clc
        jmp     SHORT MAGDC_Exit

MAGDC_Exit_Failure:
        Debug_Out "MSSNDSYS: Get_DMA_Count API FAILED!!"
        mov     [ebp.Client_AX], 0FFFFh
        stc

MAGDC_Exit:
        pop     edi
        ret

EndProc MSSNDSYS_API_Get_DMA_Count

BeginDoc
;---------------------------------------------------------------------------;
;
;   MSSNDSYS_API_Get_Version, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Version call for API entry point
;
;   ENTRY:
;       EBX = Current VM Handle
;       EBP = Pointer to Client Register Structure.
;
;       Client_DX = MSS_API_Get_Version (0)
;
;   EXIT:
;       Client_EFLAGS = carry clear
;       Client_AX = Version
;
;   USES:
;       FLAGS, EAX
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc MSSNDSYS_API_Get_Version

        Assert_Client_Ptr ebp

        cCall   MSSNDSYS_Get_Version            ; get version in EAX
        mov     [ebp.Client_AX], ax             ; dump it in client's AX
        clc
        ret

EndProc MSSNDSYS_API_Get_Version

BeginDoc
;---------------------------------------------------------------------------;
;
;   MSSNDSYS_API_Acquire, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Assigns ownership to VM
;
;   ENTRY:
;       EBX = Current VM Handle (VM to assign ownership to)
;       EBP = Pointer to Client Register Structure.
;
;       Client_AX = CODEC base port to acquire (ex. 534h)
;       Client_BX = Flags
;           fSS_ASS_Acquire_CODEC       equ 00000001b
;           fSS_ASS_Acquire_OPL3        equ 00000010b
;       Client_DX = MSS_API_Acquire (1)
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;       IF Client_EFLAGS = carry clear
;           success, ownership assigned
;       ELSE Client_EFLAGS = carry set
;           fail, Client_AX is error code:
;
;           MSS_API_ASS_Err_Bad_SndSys      equ 01h
;               The SS base specified is not being virtualized by
;               MSSNDSYS.
;
;           MSS_API_ASS_Err_Already_Owned  equ 02h
;               The SS is currently owned by another VM.
;
;   USES:
;       Flags, EAX, EDX, ESI
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc MSSNDSYS_API_Acquire

        Assert_Client_Ptr ebp
        movzx   eax, [ebp.Client_BX]            ; flags in EAX

ifdef DEBUG
        test    eax, not (fSS_ASS_Acquire_OPL3 + fSS_ASS_Acquire_CODEC)
        jz      SHORT @F                    ; no bogus flags
        Debug_Out "MSSNDSYS: API_Acquire: Reserved flags used! #EAX"
        mov     [ebp.Client_AX], -1
        jmp     SHORT MAA_Error_Exit
@@:
endif

        movzx   edx, [ebp.Client_AX]            ; CODEC base in EDX

        Trace_Out "MSSNDSYS: API_Acquire: IOAddress=#DX  Flags=#AX"

        or      edx, edx 
        jz      SHORT MAA_Bad_SS_Exit

        test    eax, fSS_ASS_Acquire_CODEC
        jz      SHORT MAA_Try_OPL3

        ;
        ; If it's coming from and older driver, they are probably
        ; using the wIOAddress base which is the AutoSelect base.
        ; SO, try it first, if it fails then try moving to the CODEC
        ; from the given base.  Confusing?  Yes, but we've gotta
        ; maintain backwards compatibility.
        ;

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <edx, pSSI_FromCODEC>
        or      edi, edi                        ; Q: They give us a valid
                                                ;    CODEC base port?
        jnz     SHORT MAA_Got_SSI_From_CODEC    ;    Y: continue
        add     edx, 4                          ;    N: try moving to CODEC

MAA_Have_CODEC_Base:
        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <edx, pSSI_FromCODEC>
        or      edi, edi                        ; Q: valid base?
        jz      SHORT MAA_Bad_SS_Exit           ;    N: fail

MAA_Got_SSI_From_CODEC:
        mov     ecx, [edi.ssi_dwCODECOwnerLast]
        jmp     SHORT MAA_Verify_SS             ;    Y: continue

;---------------------------------------------------------------------------

MAA_Try_OPL3:
        test    eax, fSS_ASS_Acquire_OPL3
        jz      SHORT MAA_Bad_SS_Exit

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <edx, pSSI_FromOPL3>
        or      edi, edi                        ; Q: valid base port?
        jz      SHORT MAA_Bad_SS_Exit           ;    N: fail

        mov     ecx, [edi.ssi_dwOPL3OwnerLast]
        jmp     SHORT MAA_Verify_SS

MAA_Bad_SS_Exit:
        mov     [ebp.Client_AX], MSS_API_ASS_Err_Bad_Base_Port
        Debug_Out "MSSNDSYS: API_Acquire: bad base port address--failing!"

MAA_Error_Exit:
        stc
        ret

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   EAX = flags
;   EBX = VM handle to own SS
;   ECX = Last owner
;   EDX = SS base to acquire
;   EDI = SSI
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

MAA_Verify_SS:
        mov     [ebp.Client_AX], 0              ; assume success
        call    MSSNDSYS_Acquire_SndSys         ; assign ownership
        jc      SHORT MAA_Already_Owned
        ret

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   SS and/or Ad Lib is currently owned by another VM!
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

MAA_Already_Owned:
        mov     [ebp.Client_AX], MSS_API_ASS_Err_Already_Owned
        jmp     SHORT MAA_Error_Exit

EndProc MSSNDSYS_API_Acquire

BeginDoc
;---------------------------------------------------------------------------;
;
;   MSSNDSYS_API_Release, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Releases ownership of an owned SS so other VM's can use it.
;       Only the current owning VM can release a SS!
;
;   ENTRY:
;       EBX = Current VM Handle (VM to release ownership)
;       EBP = Pointer to Client Register Structure.
;
;       Client_AX = Base of SS to release (for example, 0240h)
;       Client_BX = Flags
;           fSS_ASS_Acquire_CODEC       equ 00000001b
;           fSS_ASS_Acquire_OPL3        equ 00000010b
;       Client_DX = MSS_API_Release (2)
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;       IF Client_EFLAGS = carry clear
;           success, ownership released
;       ELSE Client_EFLAGS = carry set
;           fail, Client_AX is error code:
;
;           MSS_API_RSS_Err_Bad_Base_Port      equ 01h
;               The SS base specified is not being virtualized by
;               MSSNDSYS.
;
;           MSS_API_RSS_Err_Not_Yours      equ 02h
;               The SS is NOT owned by callers VM.
;
;   USES:
;       Flags, EAX, EDX, ESI
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc  MSSNDSYS_API_Release

        Assert_Client_Ptr ebp
        movzx   eax, [ebp.Client_BX]        ; flags in EAX

ifdef DEBUG
        test    eax, not (fSS_ASS_Acquire_OPL3 + fSS_ASS_Acquire_CODEC)
        jz      SHORT @F                    ; no bogus flags
        Debug_Out "MSSNDSYS: API_Release: Reserved flags used! #EAX"
        mov     [ebp.Client_AX], -1
        jmp     SHORT MAR_Error_Exit
@@:
endif

        movzx   edx, [ebp.Client_AX]        ; CODEC base in EDX

        Trace_Out "MSSNDSYS: API_Release: IOAddress=#DX  Flags=#AX"

        or      edx, edx
        jz      SHORT MAR_Bad_SS_Exit

        test    eax, fSS_ASS_Acquire_CODEC
        jz      SHORT MAR_Try_OPL3

        VMMCall Test_Sys_VM_Handle
        je      SHORT MAR_Have_CODEC_Base

        ;
        ; If it's coming from any VM other than the System VM,
        ; they are probably using the wIOAddress base which is
        ; the AutoSelect base... so, try it first then try moving
        ; to the CODEC from the given base.  Confusing?  Yes. But
        ; we've gotta maintain backwards compatibility.
        ;


        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <edx, pSSI_FromCODEC>
        or      edi, edi                        ; Q: They give us a valid
                                                ;    CODEC base port?
        jnz     SHORT MAR_Got_SSI_From_CODEC    ;    Y: continue
        add     edx, 4                          ;    N: try moving to CODEC

MAR_Have_CODEC_Base:
        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <edx, pSSI_FromCODEC>
        or      edi, edi                        ; Q: valid base port?
        jz      SHORT MAR_Bad_SS_Exit           ;    N: fail

MAR_Got_SSI_From_CODEC:
        mov     ecx, [edi.ssi_dwCODECOwnerCur]
        jmp     SHORT MAR_Verify_SS

;---------------------------------------------------------------------------

MAR_Try_OPL3:
        test    eax, fSS_ASS_Acquire_OPL3
        jz      SHORT MAR_Bad_SS_Exit

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <edx, pSSI_FromOPL3>
        or      edi, edi                        ; Q: valid base port?
        jz      SHORT MAR_Bad_SS_Exit           ;    N: fail

        mov     ecx, [edi.ssi_dwOPL3OwnerCur]
        jmp     SHORT MAR_Verify_SS

MAR_Bad_SS_Exit:
        mov     [ebp.Client_AX], MSS_API_RSS_Err_Bad_Base_Port

MAR_Error_Exit:
        stc
        ret


        ;
        ; EAX = flags
        ; EBX = VM handle to release
        ; ECX = current owning VM of hardware
        ; EDX = SS base to release
        ;

MAR_Verify_SS:
        cmp     ecx, ebx                        ; Q: is SS owned by VM?
        je      SHORT MAR_Release               ;   Y: then release it

        ;
        ; Currently owned by another VM (or not owned).
        ;

        mov     [ebp.Client_AX], MSS_API_RSS_Err_Not_Yours
        jmp     SHORT MAR_Error_Exit

MAR_Release:
        call    MSSNDSYS_Release_SndSys    ; release ownership

        mov     [ebp.Client_AX], 0         ; success
        clc
        ret


EndProc MSSNDSYS_API_Release

BeginDoc
;---------------------------------------------------------------------------;
;
;   MSSNDSYS_Get_SndSys_Info, PMAPI, RMAPI
;
;   DESCRIPTION:
;       This function is used to get information about the SndSys Audio
;       card configuration. This should be used instead of trying to
;       determine the information in a driver by poking at the hardware.
;       This has the advantage of being much faster anyway because this
;       VxD has already done all of the work.
;
;   ENTRY:
;       Client_ES = selector/segment of MSSNDSYSINFO structure
;       Client_BX = offset of MSSNDSYSINFO structure
;       Client_AX = flags
;           In version 4.0, a MSS_API_GetInfoF_DevNode was added to
;           allow the caller to specify the devnode used to retrieve
;           information. Otherwise, flags must be 0.
;       Client_ECX = DevNode
;           In version 4.0, if MSS_API_GetInfoF_DevNode is specified,
;           ECX contains the DevNode.
;
;   EXIT:
;       IF carry clear
;           success
;           Client_AX = non-zero
;           Client_ES:BX ->filled in MSSNDSYSINFO structure
;       ELSE carry set
;           Client_AX = 0
;
;   USES:
;       Flags, EAX, EBX, ECX, ESI, EDI
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc MSSNDSYS_API_Get_Info

        pushfd

        Assert_Client_Ptr ebp

        Trace_Out "MSSNDSYS: API_Get_Info: called"

        mov     ax, [ebp.Client_AX]
        and     ax, NOT MSS_API_GetInfoF_Mask
        jnz     SHORT MSGI_Exit_Failure

        xor     ecx, ecx
        mov     ax, [ebp.Client_AX]
        and     ax, MSS_API_GetInfoF_DevNode
        jz      SHORT MSGI_No_DevNode

        mov     ecx, [ebp.Client_ECX]
        Trace_Out "MSSNDSYS: API_Get_Info: client specified devnode #ECX"
        
MSGI_No_DevNode:
        cli
        mov     esi, ghlSSI 
        or      esi, esi
        jz      SHORT MSGI_Exit_Failure
        VMMCall List_Get_First
        jz      SHORT MSGI_Exit_Failure
        mov     edi, [eax.hwl_pSSI]
        jecxz   SHORT MSGI_Got_DevNode

MSGI_Get_DevNode:
        cmp     [edi.ssi_dn], ecx
        je      SHORT MSGI_Got_DevNode
        VMMCall List_Get_Next
        jz      SHORT MSGI_Exit_Failure
        mov     edi, [eax.hwl_pSSI]
        jmp     SHORT MSGI_Get_DevNode

MSGI_Got_DevNode:
        popfd
        pushfd
        mov     esi, edi
        Client_Ptr_Flat edi, ES, BX
        cmp     edi, -1
        je      SHORT MSGI_Exit_Failure

        ;
        ; Check the SNDSYSINFO structure size if too big, fail.
        ; If too small, assume we've added information to the
        ; structure, and this app doesn't recognize it - only
        ; copy what they've asked for...
        ;
        ; NOTE!  The SNDSYSINFO now contains a dwSize parameter
        ; in the beginning of the structure.  Anyone trying to use
        ; the 1.00 structure on the 1.01 VxD API is hosed. Attempt
        ; to detect this by checking against zero for dwSize.
        ;

        mov     ecx, [edi]
        or      ecx, ecx
        jz      SHORT MSGI_Exit_Failure
        cmp     ecx, (size SNDSYSINFO)
        ja      SHORT MSGI_Exit_Failure

        cld
        rep     movsb

        mov     [ebp.Client_AX], 1          ; success
        popfd
        clc
        ret


MSGI_Exit_Failure:

        Debug_Out "MSSNDSYS: API_Get_Info: function FAILED!!"
        mov     [ebp.Client_AX], 0          ; failed
        popfd
        stc
        ret

EndProc MSSNDSYS_API_Get_Info

;=============================================================================
;=============================================================================

BeginDoc
;---------------------------------------------------------------------------;
;
;   OPL_API_Acquire, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Assigns ownership of OPL3 to a VM
;
;   ENTRY:
;       EBX = Current VM Handle (VM to assign ownership to)
;       EBP = Pointer to Client Register Structure.
;
;       Client_AX = OPL3 base port to acquire
;       Client_DX = MSOPL_API_Acquire
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;       IF Client_EFLAGS = carry clear
;           success, ownership assigned
;       ELSE Client_EFLAGS = carry set
;           fail, Client_AX is error code:
;
;           MSOPL_API_Err_Bad_Base_Port
;               The base port given is not virtualized by this VxD.
;
;           MSOPL_API_Err_Already_Owned 
;               The hardware is currently owned by another VM.
;
;   USES:
;       Flags, EAX, EDX, ESI
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc OPL_API_Acquire

        Assert_Client_Ptr ebp

        movzx   edx, [ebp.Client_AX]            ; OPL3 base in EDX

        Trace_Out "MSSNDSYS: MSOPL_API_Acquire: IOAddress=#DX"

        or      edx, edx 
        jz      SHORT MOA_Bad_Port_Exit

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <edx, pSSI_FromOPL3>
        or      edi, edi                        ; Q: valid base port?
        jz      SHORT MOA_Bad_Port_Exit         ;    N: fail

        mov     ecx, [edi.ssi_dwOPL3OwnerLast]

        ;
        ; EAX = flags
        ; EBX = VM handle to own OPL3
        ; ECX = Last owner
        ; EDX = OPL3 base to acquire
        ; EDI = pSSI
        ;

        mov     [ebp.Client_AX], 0              ; assume success
        mov     eax, fSS_ASS_Acquire_OPL3
        call    MSSNDSYS_Acquire_SndSys         ; assign ownership
        jc      SHORT MOA_Already_Owned
        ret

MOA_Already_Owned:
        mov     [ebp.Client_AX], MSOPL_API_Err_Already_Owned
        jmp     SHORT MOA_Error_Exit

MOA_Bad_Port_Exit:
        mov     [ebp.Client_AX], MSOPL_API_Err_Bad_Base_Port
        Debug_Out "MSSNDSYS: MSOPL_API_Acquire: bad base port address--failing!"

MOA_Error_Exit:
        Debug_Out "MSSNDSYS: MSOPL_API_Acquire: Failing..."
        stc
        ret

EndProc OPL_API_Acquire

BeginDoc
;---------------------------------------------------------------------------;
;
;   OPL_API_Release, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Releases ownership of an owned OPL3 so other VM's can use it.
;       Only the current owning VM can release the OPL3.
;
;   ENTRY:
;       EBX = Current VM Handle (VM to release ownership)
;       EBP = Pointer to Client Register Structure.
;
;       Client_AX = OPL3 base port to release
;       Client_DX = MSOPL_API_Release
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;       IF Client_EFLAGS = carry clear
;           success, ownership released
;       ELSE Client_EFLAGS = carry set
;           fail, Client_AX is error code:
;
;           MSOPL_API_Err_Bad_Base_Port
;               The base port given is not virtualized by this VxD.
;
;           MSOPL_API_Err_Not_Yours
;               The OPL3 is NOT owned by caller's VM.
;
;   USES:
;       Flags, EAX, EDX, ESI
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc  OPL_API_Release

        Assert_Client_Ptr ebp

        movzx   edx, [ebp.Client_AX]            ; OPL3 base in EDX

        Trace_Out "MSSNDSYS: MSOPL_API_Release: IOAddress=#DX"

        or      edx, edx
        jz      SHORT MOR_Bad_Port_Exit

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <edx, pSSI_FromOPL3>
        or      edi, edi                        ; Q: valid base port?
        jz      SHORT MOR_Bad_Port_Exit         ;    N: fail

        mov     ecx, [edi.ssi_dwOPL3OwnerCur]

        cmp     ecx, ebx                        ; Q: is OPL3 owned by VM?
        je      SHORT MOR_Release               ;   Y: then release it

        ;
        ; Currently owned by another VM (or not owned).
        ;

        mov     [ebp.Client_AX], MSOPL_API_Err_Not_Yours
        jmp     SHORT MOR_Error_Exit

        ;
        ; EAX = flags
        ; EBX = VM handle to release
        ; ECX = current owning VM of hardware
        ; EDX = SS base to release
        ;

MOR_Release:
        mov     eax, fSS_ASS_Acquire_OPL3
        call    MSSNDSYS_Release_SndSys         ; release ownership

        mov     [ebp.Client_AX], 0              ; success
        clc
        ret

MOR_Bad_Port_Exit:
        mov     [ebp.Client_AX], MSOPL_API_Err_Bad_Base_Port

MOR_Error_Exit:
        stc
        ret

EndProc OPL_API_Release

BeginDoc
;---------------------------------------------------------------------------;
;
;   OPL_Get_Info, PMAPI, RMAPI
;
;   DESCRIPTION:
;       This function is used to get information about the Synthesis
;       support of this VxD.
;
;   ENTRY:
;       Client_ES = selector/segment of MSOPLINFO structure
;       Client_BX = offset of MSOPLINFO structure
;       Client_AX = flags
;           MSOPL_API_GetInfoF_DevNode specifies the devnode used to
;           retrieve information.  This flag must be specified, all
;           others are invalid.
;
;       Client_ECX = DevNode
;
;   EXIT:
;       IF carry clear
;           success
;           Client_AX = non-zero
;           Client_ES:BX ->filled in MSOPLINFO structure
;       ELSE carry set
;           Client_AX = 0
;
;   USES:
;       Flags, EAX, EBX, ECX, ESI, EDI
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc OPL_API_Get_Info

        pushfd

        Assert_Client_Ptr ebp

        Trace_Out "MSSNDSYS: MSOPL_API_Get_Info"

        mov     ax, [ebp.Client_AX]
        cmp     ax, MSOPL_API_GetInfoF_DevNode
        jne     SHORT MOGI_Exit_Failure

        mov     ecx, [ebp.Client_ECX]
        Trace_Out "MSSNDSYS: MSOPL_API_Get_Info: specified devnode #ECX"
        
MOGI_No_DevNode:
        cli
        mov     esi, ghlSSI 
        or      esi, esi
        jz      SHORT MOGI_Exit_Failure
        VMMCall List_Get_First
        jz      SHORT MOGI_Exit_Failure
        mov     edi, [eax.hwl_pSSI]

MOGI_Get_DevNode:
        cmp     [edi.ssi_dn], ecx
        je      SHORT MOGI_Got_DevNode
        VMMCall List_Get_Next
        jz      SHORT MOGI_Exit_Failure
        mov     edi, [eax.hwl_pSSI]
        jmp     SHORT MOGI_Get_DevNode

MOGI_Got_DevNode:
        popfd
        pushfd
        mov     esi, edi
        Client_Ptr_Flat edi, ES, BX
        cmp     edi, -1
        je      SHORT MOGI_Exit_Failure

        ;
        ; Check the MSOPLINFO structure size if too big, fail.
        ; If too small, assume we've added information to the
        ; structure, and this app doesn't recognize it - only
        ; copy what they've asked for...
        ;

        test    [esi.ssi_wHardwareOptions], DAK_FMSYNTH
        jz      SHORT MOGI_Exit_Failure

        mov     ecx, [edi]
        or      ecx, ecx
        jz      SHORT MOGI_Exit_Failure
        cmp     ecx, (size MSOPLINFO)
        ja      SHORT MOGI_Exit_Failure

        mov     [edi.msoi_wHardwareOptions], MSOPL_HWOPTIONSF_OPL3DETECTED
        mov     ax, [esi.ssi_wIOAddressOPL3]
        mov     [edi.msoi_wIOAddressSynth], ax
        mov     eax, [esi.ssi_dn]
        mov     [edi.msoi_dn], eax

        mov     [ebp.Client_AX], 1          ; success
        popfd
        clc
        ret

MOGI_Exit_Failure:

        Debug_Out "MSSNDSYS: MSOPL_API_Get_Info: function FAILED!!"
        mov     [ebp.Client_AX], 0          ; failed
        popfd
        stc
        ret

EndProc OPL_API_Get_Info

BeginDoc
;---------------------------------------------------------------------------;
;
;   VxD_API_Open_Pipe, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Opens a "pipe" between two clients.
;
;   ENTRY:
;
;   EXIT:
;       IF carry clear
;           Client_AX = 0
;       ELSE carry set
;           Client_AX = 1
;
;   USES:
;       Flags, EAX, EBX, ECX, ESI, EDI
;
;---------------------------------------------------------------------------;
EndDoc

BeginProc VxD_API_Open_Pipe

dn      equ     dword ptr [esi]                 ; dev node
psz     equ     dword ptr [esI + 4]             ; pipe name
pos     equ     dword ptr [esi + 8]             ; pipe open struct

        Client_Ptr_Flat esi, SS, SP

        push    dword ptr [ebp.Client_DS]
        push    [ebp.Client_EDX]

        mov     eax, psz
        mov     [ebp.Client_DX], ax
        shr     eax, 16
        mov     [ebp.Client_DS], ax
        Client_Ptr_Flat eax, DS, DX
        mov     psz, eax

        mov     eax, pos
        mov     [ebp.Client_DX], ax
        shr     eax, 16
        mov     [ebp.Client_DS], ax
        Client_Ptr_Flat eax, DS, DX
        mov     pos, eax

        pop     [ebp.Client_EDX]
        pop     dword ptr [ebp.Client_DS]

        mov     eax, dn

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <eax, pSSI_FromDevNode>
        or      edi, edi
        jz      SHORT AOP_Failure

        lea     eax, [edi.ssi_hlPipe]

        cCall   _pipeOpen, <dn, eax, psz, pos>
        or      eax, eax
        jz      SHORT AOP_Failure

        mov     [ebp.Client_AX], ax
        shr     eax, 16
        mov     [ebp.Client_DX], ax
        clc
        ret

AOP_Failure:
        Debug_Out "MSSNDSYS: VxD_API_Open_Pipe: function FAILED!!"
        mov     [ebp.Client_AX], 0
        mov     [ebp.Client_DX], 0
        stc
        ret
        
EndProc VxD_API_Open_Pipe

BeginDoc
;---------------------------------------------------------------------------;
;
;   VxD_API_Close_Pipe, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Closes an open "pipe".
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;       Flags, EAX, EBX, ECX, ESI, EDI
;
;---------------------------------------------------------------------------;
EndDoc

BeginProc VxD_API_Close_Pipe

dn      equ     dword ptr [esi]                 ; dev node
pn      equ     dword ptr [esi + 4]             ; pipe node

        Client_Ptr_Flat esi, SS, SP

        mov     eax, dn

        cCall   _MSSNDSYS_Get_pSSI_From_XXX, <eax, pSSI_FromDevNode>
        or      edi, edi
        jz      SHORT ACP_Failure

        lea     eax, [edi.ssi_hlPipe]

        cCall   _pipeClose, <eax, pn>

ACP_Failure:
        mov     [ebp.Client_AX], 0
        clc
        ret
        
EndProc VxD_API_Close_Pipe

VxD_PAGEABLE_CODE_ENDS

end
