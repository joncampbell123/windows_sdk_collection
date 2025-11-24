        page    60, 132

;******************************************************************************
	title	WRAPS - 'C' wrapper for VMM services
;******************************************************************************
;---------------------------------------------------------------------------;
;
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1992- 1995	Microsoft Corporation.	All Rights Reserved.
;
;---------------------------------------------------------------------------;
;
;   Title:    WRAPS - 'C' wrapper for VMM services
;******************************************************************************
;
;   Functional Description:
;
;      Provides a 'C' interface to VMM services
;
;******************************************************************************

        .386p

;==============================================================================
;                     I N C L U D E S   &   E Q U A T E S
;==============================================================================

        .XLIST
        INCLUDE VMM.Inc
        .LIST

VxD_PAGEABLE_CODE_SEG

BeginProc _vmmBeginNestExec, PUBLIC

        VMMJmp  Begin_Nest_Exec

EndProc _vmmBeginNestExec

BeginProc _vmmEndNestExec, PUBLIC

        VMMJmp  End_Nest_Exec

EndProc _vmmEndNestExec

BeginProc _vmmResumeExec, PUBLIC

        VMMJmp  Resume_Exec

EndProc _vmmResumeExec

;------------------------------------------------------------------------------
;
;   VOID CDECL vmmSaveClientState( PCRS_32 pSaveCRS )
;
;   DESCRIPTION:
;      Saves the client register structure into the save
;      structure (pSaveCRS).
;
;   PARAMETERS:
;      PCRS_32 pSaveCRS
;         pointer to save structure
;
;   RETURN VALUE:
;      Nothing.
;
;------------------------------------------------------------------------------

BeginProc _vmmSaveClientState, PUBLIC

        pSaveCRS        equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        push    edi

        mov     edi, pSaveCRS
        VMMCall Save_Client_State

        pop     edi

        pop     ebp
        ret

EndProc _vmmSaveClientState

;------------------------------------------------------------------------------
;
;   VOID CDECL vmmRestoreClientState( PCRS_32 pSavedCRS )
;
;   DESCRIPTION:
;      Restores the client register structure from the save
;      structure (pSavedCRS).
;
;   PARAMETERS:
;      PCRS_32 pSavedCRS
;         pointer to save structure
;
;   RETURN VALUE:
;      Nothing.
;
;------------------------------------------------------------------------------

BeginProc _vmmRestoreClientState, PUBLIC

        pSavedCRS       equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        push    esi

        mov     esi, pSavedCRS
        VMMCall Restore_Client_State

        pop     esi

        pop     ebp
        ret

EndProc _vmmRestoreClientState

;------------------------------------------------------------------------------
;
;   VOID CDECL vmmSimulateFarCall( DWORD dwSeg, DWORD dwOff )
;
;   DESCRIPTION:
;      Simulates a far call into the current VM.
;
;   PARAMETERS:
;      DWORD dwSeg
;         segment/selector of code segment to call
;
;      DWORD dwOff
;         offset of procedure, if the code segment is 16-bit
;         the high word must be zero.
;
;   RETURN VALUE:
;      Nothing.
;
;------------------------------------------------------------------------------

BeginProc _vmmSimulateFarCall, PUBLIC

        dwSeg   equ     [esp + 4]
        dwOff   equ     [esp + 8]

        movzx   ecx, word ptr dwSeg
        mov     edx, dwOff
        VMMJmp  Simulate_Far_Call

EndProc _vmmSimulateFarCall

;------------------------------------------------------------------------------
;
;   VOID CDECL vmmSimulatePush( DWORD dwValue )
;
;   DESCRIPTION:
;      Simulates a push into the current VM.
;
;   PARAMETERS:
;      DWORD dwValue
;         value to push, if V86 or if VM is running 16-bit mode app,
;         only loword of value is pushed.
;
;   RETURN VALUE:
;      Nothing.
;
;------------------------------------------------------------------------------

BeginProc _vmmSimulatePush, PUBLIC

        dwValue         equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        mov     eax, dwValue
        VMMCall Simulate_Push

        pop     ebp
        ret

EndProc _vmmSimulatePush


BeginProc _vmmGetCurVMHandle, PUBLIC

        push    ebx
        VMMCall Get_Cur_VM_Handle
        mov     eax, ebx
        pop     ebx
        ret

EndProc _vmmGetCurVMHandle

VxD_PAGEABLE_CODE_ENDS

end
