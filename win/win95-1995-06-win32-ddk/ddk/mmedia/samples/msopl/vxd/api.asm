        page    60, 132

;******************************************************************************
        title   API.ASM - Ring 3 Driver Interfaces
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
;   Module:   API.ASM - Application Programming Interface
;
;   Version:  4.00
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
        include msopl.inc
        include equates.inc
        .list

EXTRN ghlMSOI:DWORD                              ; OPL Info list

EXTRN MSOPL_Acquire:NEAR
EXTRN MSOPL_Release:NEAR

;==============================================================================
;                       P A G E A B L E   D A T A
;==============================================================================

VxD_PAGEABLE_DATA_SEG

;------------------------------------------------------------------------------
;                 A P I   D I S P A T C H   T A B L E
;------------------------------------------------------------------------------

MSOPL_API_TABLE LABEL DWORD
        dd  OFFSET32 API_Get_Version
        dd  OFFSET32 OPL_API_Get_Info
        dd  OFFSET32 OPL_API_Acquire
        dd  OFFSET32 OPL_API_Release
MSOPL_API_MAX       equ ($-MSOPL_API_TABLE)/4

MSOPL_Func_Tables LABEL DWORD
        dd  1,                  OFFSET32 MSOPL_API_Table
        dd  MSOPL_API_MAX,      OFFSET32 MSOPL_API_Table

MSOPL_FUNC_HANDLERS_MAX equ ($-MSOPL_Func_Tables)/8

VxD_PAGEABLE_DATA_ENDS

;==============================================================================
;                        P A G E A B L E   C O D E
;==============================================================================

VxD_PAGEABLE_CODE_SEG

;---------------------------------------------------------------------------;
;
;   MSOPL_API_Handler
;
;   DESCRIPTION:
;       This is the single entry point for a VM (pmode or rmode) to make
;       the SB16 driver's life easier
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

BeginProc MSOPL_API_Handler, PUBLIC

        movzx   eax, [ebp.Client_DX]
        cmp     ah, MSOPL_FUNC_HANDLERS_MAX
        jae     SHORT MSOPL_API_Fail
        movzx   ecx, ah
        shl     ecx, 3
        lea     ecx, MSOPL_Func_Tables[ ecx ]
        movzx   eax, al
        cmp     eax, dword ptr [ecx]
        jae     SHORT MSOPL_API_Fail
        mov     ecx, dword ptr [ecx + 4]
        call    dword ptr [ecx][eax * 4]
        jc      SHORT MSOPL_API_Fail

MSOPL_API_Success:
        and     [ebp.Client_Flags], not CF_Mask ; clear carry
        ret

MSOPL_API_Fail:
        or      [ebp.Client_Flags], CF_Mask     ; set carry
        ret

EndProc MSOPL_API_Handler

BeginDoc
;---------------------------------------------------------------------------;
;
;   API_Get_Version, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Version call for API entry point
;
;   ENTRY:
;       EBX = Current VM Handle
;       EBP = Pointer to Client Register Structure.
;
;       Client_DX = MSOPL_API_Get_Version (0)
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
BeginProc API_Get_Version

        Assert_Client_Ptr ebp

        cCall   MSOPL_Get_Version              ; get version in EAX
        mov     [ebp.Client_AX], ax             ; dump it in client's AX
        clc
        ret

EndProc API_Get_Version

BeginDoc
;---------------------------------------------------------------------------;
;
;   OPL_API_Acquire, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Assigns ownership of OPL to a VM
;
;   ENTRY:
;       EBX = Current VM Handle (VM to assign ownership to)
;       EBP = Pointer to Client Register Structure.
;
;       Client_AX = OPL base port to acquire
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

        movzx   edx, [ebp.Client_AX]            ; OPL base in EDX

        Trace_Out "MSOPL: MSOPL_API_Acquire: IOAddress=#DX"

        or      edx, edx 
        jz      SHORT MMA_Bad_Port_Exit

        cCall   _MSOPL_Get_pMSOI_From_XXX, <edx, pMSOI_FromSynth>
        or      edi, edi                        ; Q: valid base port?
        jz      SHORT MMA_Bad_Port_Exit         ;    N: fail

        mov     ecx, [edi.msoi_dwSynthOwnerLast]

        ;
        ; EAX = flags
        ; EBX = VM handle to own OPL
        ; ECX = Last owner
        ; EDX = OPL base to acquire
        ; EDI = pMSOI
        ;

        mov     [ebp.Client_AX], 0              ; assume success
        call    MSOPL_Acquire                ; assign ownership
        jc      SHORT MMA_Already_Owned
        ret

MMA_Already_Owned:
        mov     [ebp.Client_AX], MSOPL_API_Err_Already_Owned
        jmp     SHORT MMA_Error_Exit

MMA_Bad_Port_Exit:
        mov     [ebp.Client_AX], MSOPL_API_Err_Bad_Base_Port
        Debug_Out "MSOPL: MSOPL_API_Acquire: bad base port address--failing!"

MMA_Error_Exit:
        Debug_Out "MSOPL: MSOPL_API_Acquire: Failing..."
        stc
        ret

EndProc OPL_API_Acquire

BeginDoc
;---------------------------------------------------------------------------;
;
;   OPL_API_Release, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Releases ownership of an owned OPL so other VM's can use it.
;       Only the current owning VM can release the OPL.
;
;   ENTRY:
;       EBX = Current VM Handle (VM to release ownership)
;       EBP = Pointer to Client Register Structure.
;
;       Client_AX = OPL base port to release
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
;               The OPL is NOT owned by caller's VM.
;
;   USES:
;       Flags, EAX, EDX, ESI
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc  OPL_API_Release

        Assert_Client_Ptr ebp

        movzx   edx, [ebp.Client_AX]            ; OPL base in EDX

        Trace_Out "MSOPL: MSOPL_API_Release: IOAddress=#DX"

        or      edx, edx
        jz      SHORT MMR_Bad_Port_Exit

        cCall   _MSOPL_Get_pMSOI_From_XXX, <edx, pMSOI_FromSynth>
        or      edi, edi                        ; Q: valid base port?
        jz      SHORT MMR_Bad_Port_Exit         ;    N: fail

        mov     ecx, [edi.msoi_dwSynthOwnerCur]

        cmp     ecx, ebx                        ; Q: is OPL owned by VM?
        je      SHORT MMR_Release               ;   Y: then release it

        ;
        ; Currently owned by another VM (or not owned).
        ;

        mov     [ebp.Client_AX], MSOPL_API_Err_Not_Yours
        jmp     SHORT MMR_Error_Exit

        ;
        ; EAX = flags
        ; EBX = VM handle to release
        ; ECX = current owning VM of hardware
        ; EDX = SS base to release
        ;

MMR_Release:
        call    MSOPL_Release                ; release ownership

        mov     [ebp.Client_AX], 0              ; success
        clc
        ret

MMR_Bad_Port_Exit:
        mov     [ebp.Client_AX], MSOPL_API_Err_Bad_Base_Port

MMR_Error_Exit:
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

        Trace_Out "MSOPL: MSOPL_API_Get_Info"

        mov     ax, [ebp.Client_AX]
        cmp     ax, MSOPL_API_GetInfoF_DevNode
        jne     SHORT MAGI_Exit_Failure

        mov     ecx, [ebp.Client_ECX]
        Trace_Out "MSOPL: MSOPL_API_Get_Info: specified devnode #ECX"
        
MAGI_No_DevNode:
        cli
        mov     esi, ghlMSOI 
        or      esi, esi
        jz      SHORT MAGI_Exit_Failure
        VMMCall List_Get_First
        jz      SHORT MAGI_Exit_Failure

MAGI_Get_DevNode:
        mov     edi, [eax.hwl_pMSOI]
        cmp     [edi.msoi_dn], ecx
        je      SHORT MAGI_Got_DevNode
        VMMCall List_Get_Next
        jz      SHORT MAGI_Exit_Failure
        jmp     SHORT MAGI_Get_DevNode

MAGI_Got_DevNode:
        popfd
        pushfd
        mov     esi, edi
        Client_Ptr_Flat edi, ES, BX
        cmp     edi, -1
        je      SHORT MAGI_Exit_Failure

        ;
        ; Check the MSOPLINFO structure size if too big, fail.
        ; If too small, assume we've added information to the
        ; structure, and this app doesn't recognize it - only
        ; copy what they've asked for...
        ;

        mov     ecx, [edi]
        or      ecx, ecx
        jz      SHORT MAGI_Exit_Failure
        cmp     ecx, (size MSOPLINFO)
        ja      SHORT MAGI_Exit_Failure


BUG <Optimize me...>
        mov     ax, [esi.msoi_wHardwareOptions]
        mov     [edi.msoi_wHardwareOptions], ax
        mov     ax, [esi.msoi_wIOAddressSynth]
        mov     [edi.msoi_wIOAddressSynth], ax
        mov     eax, [esi.msoi_dn]
        mov     [edi.msoi_dn], eax

        mov     [ebp.Client_AX], 1          ; success
        popfd
        clc
        ret

MAGI_Exit_Failure:

        Debug_Out "MSOPL: MSOPL_API_Get_Info: function FAILED!!"
        mov     [ebp.Client_AX], 0          ; failed
        popfd
        stc
        ret

EndProc OPL_API_Get_Info

VxD_PAGEABLE_CODE_ENDS

end
