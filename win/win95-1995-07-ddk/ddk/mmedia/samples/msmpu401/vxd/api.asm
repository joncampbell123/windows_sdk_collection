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
;   Title:    MSMPU401.386 - MICROSOFT MPU-401 386 Driver
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
        include msmpu401.inc
        include equates.inc
        .list

EXTRN ghlMSMI:DWORD                              ; MPU401 Info list

EXTRN MSMPU401_Acquire:NEAR
EXTRN MSMPU401_Release:NEAR

;==============================================================================
;                       P A G E A B L E   D A T A
;==============================================================================

VxD_PAGEABLE_DATA_SEG

;------------------------------------------------------------------------------
;                 A P I   D I S P A T C H   T A B L E
;------------------------------------------------------------------------------

MSMPU401_API_TABLE LABEL DWORD
        dd  OFFSET32 API_Get_Version
        dd  OFFSET32 MPU401_API_Get_Info
        dd  OFFSET32 MPU401_API_Acquire
        dd  OFFSET32 MPU401_API_Release
MSMPU401_API_MAX       equ ($-MSMPU401_API_TABLE)/4

MSMPU401_Func_Tables LABEL DWORD
        dd  1,                  OFFSET32 MSMPU401_API_Table
        dd  0,                  OFFSET32 MSMPU401_API_Fail
        dd  0,                  OFFSET32 MSMPU401_API_Fail
        dd  MSMPU401_API_MAX,   OFFSET32 MSMPU401_API_Table
MSMPU401_FUNC_HANDLERS_MAX equ ($-MSMPU401_Func_Tables)/8

VxD_PAGEABLE_DATA_ENDS

;==============================================================================
;                        P A G E A B L E   C O D E
;==============================================================================

VxD_PAGEABLE_CODE_SEG

;---------------------------------------------------------------------------;
;
;   MSMPU401_API_Handler
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

BeginProc MSMPU401_API_Handler, PUBLIC

        movzx   eax, [ebp.Client_DX]
        cmp     ah, MSMPU401_FUNC_HANDLERS_MAX
        jae     SHORT MSMPU401_API_Fail
        movzx   ecx, ah
        shl     ecx, 3
        lea     ecx, MSMPU401_Func_Tables[ ecx ]
        movzx   eax, al
        cmp     eax, dword ptr [ecx]
        jae     SHORT MSMPU401_API_Fail
        mov     ecx, dword ptr [ecx + 4]
        call    dword ptr [ecx][eax * 4]
        jc      SHORT MSMPU401_API_Fail

MSMPU401_API_Success:
        and     [ebp.Client_Flags], not CF_Mask ; clear carry
        ret

MSMPU401_API_Fail:
        or      [ebp.Client_Flags], CF_Mask     ; set carry
        ret

EndProc MSMPU401_API_Handler

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
;       Client_DX = MSMPU401_API_Get_Version (0)
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

        cCall   MSMPU401_Get_Version              ; get version in EAX
        mov     [ebp.Client_AX], ax             ; dump it in client's AX
        clc
        ret

EndProc API_Get_Version

BeginDoc
;---------------------------------------------------------------------------;
;
;   MPU401_API_Acquire, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Assigns ownership of MPU401 to a VM
;
;   ENTRY:
;       EBX = Current VM Handle (VM to assign ownership to)
;       EBP = Pointer to Client Register Structure.
;
;       Client_AX = MPU401 base port to acquire
;       Client_DX = MSMPU401_API_Acquire
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;       IF Client_EFLAGS = carry clear
;           success, ownership assigned
;       ELSE Client_EFLAGS = carry set
;           fail, Client_AX is error code:
;
;           MSMPU401_API_Err_Bad_Base_Port
;               The base port given is not virtualized by this VxD.
;
;           MSMPU401_API_Err_Already_Owned 
;               The hardware is currently owned by another VM.
;
;   USES:
;       Flags, EAX, EDX, ESI
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc MPU401_API_Acquire

        Assert_Client_Ptr ebp

        movzx   edx, [ebp.Client_AX]            ; MPU401 base in EDX

        Trace_Out "MSMPU401: MSMPU401_API_Acquire: IOAddress=#DX"

        or      edx, edx 
        jz      SHORT MMA_Bad_Port_Exit

        cCall   _MSMPU401_Get_pMSMI_From_XXX, <edx, pMSMI_FromMPU401>
        or      edi, edi                        ; Q: valid base port?
        jz      SHORT MMA_Bad_Port_Exit         ;    N: fail

        mov     ecx, [edi.msmi_dwMPU401OwnerLast]

        ;
        ; EAX = flags
        ; EBX = VM handle to own MPU401
        ; ECX = Last owner
        ; EDX = MPU401 base to acquire
        ; EDI = pMSMI
        ;

        mov     [ebp.Client_AX], 0              ; assume success
        call    MSMPU401_Acquire                ; assign ownership
        jc      SHORT MMA_Already_Owned
        ret

MMA_Already_Owned:
        mov     [ebp.Client_AX], MSMPU401_API_Err_Already_Owned
        jmp     SHORT MMA_Error_Exit

MMA_Bad_Port_Exit:
        mov     [ebp.Client_AX], MSMPU401_API_Err_Bad_Base_Port
        Debug_Out "MSMPU401: MSMPU401_API_Acquire: bad base port address--failing!"

MMA_Error_Exit:
        Debug_Out "MSMPU401: MSMPU401_API_Acquire: Failing..."
        stc
        ret

EndProc MPU401_API_Acquire

BeginDoc
;---------------------------------------------------------------------------;
;
;   MPU401_API_Release, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Releases ownership of an owned MPU401 so other VM's can use it.
;       Only the current owning VM can release the MPU401.
;
;   ENTRY:
;       EBX = Current VM Handle (VM to release ownership)
;       EBP = Pointer to Client Register Structure.
;
;       Client_AX = MPU401 base port to release
;       Client_DX = MSMPU401_API_Release
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;       IF Client_EFLAGS = carry clear
;           success, ownership released
;       ELSE Client_EFLAGS = carry set
;           fail, Client_AX is error code:
;
;           MSMPU401_API_Err_Bad_Base_Port
;               The base port given is not virtualized by this VxD.
;
;           MSMPU401_API_Err_Not_Yours
;               The MPU401 is NOT owned by caller's VM.
;
;   USES:
;       Flags, EAX, EDX, ESI
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc  MPU401_API_Release

        Assert_Client_Ptr ebp

        movzx   edx, [ebp.Client_AX]            ; MPU401 base in EDX

        Trace_Out "MSMPU401: MSMPU401_API_Release: IOAddress=#DX"

        or      edx, edx
        jz      SHORT MMR_Bad_Port_Exit

        cCall   _MSMPU401_Get_pMSMI_From_XXX, <edx, pMSMI_FromMPU401>
        or      edi, edi                        ; Q: valid base port?
        jz      SHORT MMR_Bad_Port_Exit         ;    N: fail

        mov     ecx, [edi.msmi_dwMPU401OwnerCur]

        cmp     ecx, ebx                        ; Q: is MPU401 owned by VM?
        je      SHORT MMR_Release               ;   Y: then release it

        ;
        ; Currently owned by another VM (or not owned).
        ;

        mov     [ebp.Client_AX], MSMPU401_API_Err_Not_Yours
        jmp     SHORT MMR_Error_Exit

        ;
        ; EAX = flags
        ; EBX = VM handle to release
        ; ECX = current owning VM of hardware
        ; EDX = SS base to release
        ;

MMR_Release:
        call    MSMPU401_Release                ; release ownership

        mov     [ebp.Client_AX], 0              ; success
        clc
        ret

MMR_Bad_Port_Exit:
        mov     [ebp.Client_AX], MSMPU401_API_Err_Bad_Base_Port

MMR_Error_Exit:
        stc           
        ret

EndProc MPU401_API_Release

BeginDoc
;---------------------------------------------------------------------------;
;
;   MPU401_Get_Info, PMAPI, RMAPI
;
;   DESCRIPTION:
;       This function is used to get information about the Synthesis
;       support of this VxD.
;
;   ENTRY:
;       Client_ES = selector/segment of MSMPU401INFO structure
;       Client_BX = offset of MSMPU401INFO structure
;       Client_AX = flags
;           MSMPU401_API_GetInfoF_DevNode specifies the devnode used to
;           retrieve information.  This flag must be specified, all
;           others are invalid.
;
;       Client_ECX = DevNode
;
;   EXIT:
;       IF carry clear
;           success
;           Client_AX = non-zero
;           Client_ES:BX ->filled in MSMPU401INFO structure
;       ELSE carry set
;           Client_AX = 0
;
;   USES:
;       Flags, EAX, EBX, ECX, ESI, EDI
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc MPU401_API_Get_Info

        pushfd

        Assert_Client_Ptr ebp

        Trace_Out "MSMPU401: MSMPU401_API_Get_Info"

        mov     ax, [ebp.Client_AX]
        cmp     ax, MSMPU401_API_GetInfoF_DevNode
        jne     SHORT MAGI_Exit_Failure

        mov     ecx, [ebp.Client_ECX]
        Trace_Out "MSMPU401: MSMPU401_API_Get_Info: specified devnode #ECX"
        
MAGI_No_DevNode:
        cli
        mov     esi, ghlMSMI 
        or      esi, esi
        jz      SHORT MAGI_Exit_Failure
        VMMCall List_Get_First
        jz      SHORT MAGI_Exit_Failure

MAGI_Get_DevNode:
        mov     edi, [eax.hwl_pMSMI]
        cmp     [edi.msmi_dn], ecx
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
        ; Check the MSMPU401INFO structure size if too big, fail.
        ; If too small, assume we've added information to the
        ; structure, and this app doesn't recognize it - only
        ; copy what they've asked for...
        ;

        mov     ecx, [edi]
        or      ecx, ecx
        jz      SHORT MAGI_Exit_Failure
        cmp     ecx, (size MSMPU401INFO)
        ja      SHORT MAGI_Exit_Failure

        mov     [edi.msmi_wHardwareOptions], 0
        mov     ax, [esi.msmi_wIOAddressMPU401]
        mov     [edi.msmi_wIOAddressMPU401], ax
        mov     al, [esi.msmi_bIRQ]
        mov     [edi.msmi_bIRQ], al
        mov     eax, [esi.msmi_dn]
        mov     [edi.msmi_dn], eax

        mov     [ebp.Client_AX], 1          ; success
        popfd
        clc
        ret

MAGI_Exit_Failure:

        Debug_Out "MSMPU401: MSMPU401_API_Get_Info: function FAILED!!"
        mov     [ebp.Client_AX], 0          ; failed
        popfd
        stc
        ret

EndProc MPU401_API_Get_Info

VxD_PAGEABLE_CODE_ENDS

end
