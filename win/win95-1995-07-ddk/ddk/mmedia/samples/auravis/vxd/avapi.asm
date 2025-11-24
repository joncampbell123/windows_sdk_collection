        page    60, 132

;******************************************************************************
        title   AVAPI.ASM - Ring 3 Driver Interfaces
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
;   Module:   AVAPI.ASM - Application Programming Interface
;
;   Version:  1.00
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
        include avvxp500.inc
        include equates.inc
        .list

EXTRN gAVDI:NEAR				; VxP500 information

EXTRN AVVXP500_Acquire:NEAR
EXTRN AVVXP500_Release:NEAR

;==============================================================================
;                       P A G E A B L E   D A T A
;==============================================================================

VxD_PAGEABLE_DATA_SEG

;------------------------------------------------------------------------------
;                 A P I   D I S P A T C H   T A B L E
;------------------------------------------------------------------------------

AVVXP500_API_TABLE LABEL DWORD
        dd  OFFSET32 API_Get_Version
        dd  OFFSET32 VXP500_API_Get_Info
        dd  OFFSET32 VXP500_API_Acquire
        dd  OFFSET32 VXP500_API_Release
AVVXP500_API_MAX       equ ($-AVVXP500_API_TABLE)/4

VxD_PAGEABLE_DATA_ENDS

;==============================================================================
;                        P A G E A B L E   C O D E
;==============================================================================

VxD_PAGEABLE_CODE_SEG

;---------------------------------------------------------------------------;
;
;   AVVXP500_API_Handler
;
;   DESCRIPTION:
;       This is the single entry point for a VM (pmode or rmode).
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

BeginProc AVVXP500_API_Handler, PUBLIC

	Trace_Out "AVVXP500_API_Handler:Function #AX"
        movzx   eax, [ebp.Client_DX]
        movzx   eax, ax
	cmp	eax, AVVXP500_API_MAX
	jae	SHORT AVVXP500_API_Fail
        call    dword ptr AVVXP500_API_Table[eax * 4]
        jc      SHORT AVVXP500_API_Fail

AVVXP500_API_Success:
        and     [ebp.Client_Flags], not CF_Mask ; clear carry
        ret

AVVXP500_API_Fail:
        or      [ebp.Client_Flags], CF_Mask     ; set carry
        ret

EndProc AVVXP500_API_Handler

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
;       Client_DX = AVVXP500_API_Get_Version (0)
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

        cCall   AVVXP500_Get_Version              ; get version in EAX
        mov     [ebp.Client_AX], ax             ; dump it in client's AX
        clc
        ret

EndProc API_Get_Version

BeginDoc
;---------------------------------------------------------------------------;
;
;   VXP500_API_Acquire, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Assigns ownership of VXP500 to a VM
;
;   ENTRY:
;       EBX = Current VM Handle (VM to assign ownership to)
;       EBP = Pointer to Client Register Structure.
;
;       Client_AX = VXP500 base port to acquire
;       Client_DX = AVVXP500_API_Acquire
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;       IF Client_EFLAGS = carry clear
;           success, ownership assigned
;       ELSE Client_EFLAGS = carry set
;           fail, Client_AX is error code:
;
;           AVVXP500_API_Err_Bad_Base_Port
;               The base port given is not virtualized by this VxD.
;
;           AVVXP500_API_Err_Already_Owned 
;               The hardware is currently owned by another VM.
;
;   USES:
;       Flags, EAX, EDX, ESI
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc VXP500_API_Acquire

        Assert_Client_Ptr ebp

        movzx   edx, [ebp.Client_AX]            ; VXP500 base in EDX

        Trace_Out "AVVXP500: AVVXP500_API_Acquire: IOAddress=#DX"

        or      edx, edx 
        jz      SHORT MMA_Bad_Port_Exit

        mov     ecx, [gAVDI.avdi_dwVXP500OwnerLast]

        ;
        ; EAX = flags
        ; EBX = VM handle to own VXP500
        ; ECX = Last owner
        ; EDX = VXP500 base to acquire
        ;

        mov     [ebp.Client_AX], 0              ; assume success
        call    AVVXP500_Acquire                ; assign ownership
        jc      SHORT MMA_Already_Owned
        ret

MMA_Already_Owned:
        mov     [ebp.Client_AX], AVVXP500_API_Err_Already_Owned
        jmp     SHORT MMA_Error_Exit

MMA_Bad_Port_Exit:
        mov     [ebp.Client_AX], AVVXP500_API_Err_Bad_Base_Port
        Debug_Out "AVVXP500: AVVXP500_API_Acquire: bad base port address--failing!"

MMA_Error_Exit:
        Debug_Out "AVVXP500: AVVXP500_API_Acquire: Failing..."
        stc
        ret

EndProc VXP500_API_Acquire

BeginDoc
;---------------------------------------------------------------------------;
;
;   VXP500_API_Release, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Releases ownership of an owned VXP500 so other VM's can use it.
;       Only the current owning VM can release the VXP500.
;
;   ENTRY:
;       EBX = Current VM Handle (VM to release ownership)
;       EBP = Pointer to Client Register Structure.
;
;       Client_AX = VXP500 base port to release
;       Client_DX = AVVXP500_API_Release
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;       IF Client_EFLAGS = carry clear
;           success, ownership released
;       ELSE Client_EFLAGS = carry set
;           fail, Client_AX is error code:
;
;           AVVXP500_API_Err_Bad_Base_Port
;               The base port given is not virtualized by this VxD.
;
;           AVVXP500_API_Err_Not_Yours
;               The VXP500 is NOT owned by caller's VM.
;
;   USES:
;       Flags, EAX, EDX, ESI
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc  VXP500_API_Release

        Assert_Client_Ptr ebp

        movzx   edx, [ebp.Client_AX]            ; VXP500 base in EDX

        Trace_Out "AVVXP500: AVVXP500_API_Release: IOAddress=#DX"

        or      edx, edx
        jz      SHORT MMR_Bad_Port_Exit

        mov     ecx, [gAVDI.avdi_dwVXP500OwnerCur]

        cmp     ecx, ebx                        ; Q: is VXP500 owned by VM?
        je      SHORT MMR_Release               ;   Y: then release it

        ;
        ; Currently owned by another VM (or not owned).
        ;

        mov     [ebp.Client_AX], AVVXP500_API_Err_Not_Yours
        jmp     SHORT MMR_Error_Exit

        ;
        ; EAX = flags
        ; EBX = VM handle to release
        ; ECX = current owning VM of hardware
        ; EDX = SS base to release
        ;

MMR_Release:
        call    AVVXP500_Release                ; release ownership

        mov     [ebp.Client_AX], 0              ; success
        clc
        ret

MMR_Bad_Port_Exit:
        mov     [ebp.Client_AX], AVVXP500_API_Err_Bad_Base_Port

MMR_Error_Exit:
        stc           
        ret

EndProc VXP500_API_Release

BeginDoc
;---------------------------------------------------------------------------;
;
;   VXP500_Get_Info, PMAPI, RMAPI
;
;   DESCRIPTION:
;       This function is used to get information about the Synthesis
;       support of this VxD.
;
;   ENTRY:
;       Client_ES = selector/segment of AVVXP500INFO structure
;       Client_BX = offset of AVVXP500INFO structure
;
;   EXIT:
;       IF carry clear
;           success
;           Client_AX = non-zero
;           Client_ES:BX ->filled in AVVXP500INFO structure
;       ELSE carry set
;           Client_AX = 0
;
;   USES:
;       Flags, EAX, EBX, ECX, ESI, EDI
;
;---------------------------------------------------------------------------;
EndDoc
BeginProc VXP500_API_Get_Info

        pushfd


        Assert_Client_Ptr ebp

        Trace_Out "AVVXP500: AVVXP500_API_Get_Info"

        mov     ax, [ebp.Client_AX]

VAGI_Got_DevNode:

	lea	esi, gAVDI
        Client_Ptr_Flat edi, ES, BX
        cmp     edi, -1
        je      SHORT VAGI_Exit_Failure

        ;
        ; Check the AVVXP500INFO structure size if too big, fail.
        ; If too small, assume we've added information to the
        ; structure, and this app doesn't recognize it - only
        ; copy what they've asked for...
        ;

        mov     ecx, [edi]
        or      ecx, ecx
        jz      SHORT VAGI_Exit_Failure
        cmp     ecx, (size AVVXP500INFO)
        ja      SHORT VAGI_Exit_Failure

	mov	[esi], ecx
	shr	ecx, 1
	rep	movsw
	adc	cl, cl
	rep	movsb

        mov     [ebp.Client_AX], 1          ; success
        popfd
        clc
        ret

VAGI_Exit_Failure:
        Debug_Out "AVVXP500: AVVXP500_API_Get_Info: function FAILED!!"
        mov     [ebp.Client_AX], 0          ; failed
        popfd
        stc
        ret

EndProc VXP500_API_Get_Info

VxD_PAGEABLE_CODE_ENDS

end
