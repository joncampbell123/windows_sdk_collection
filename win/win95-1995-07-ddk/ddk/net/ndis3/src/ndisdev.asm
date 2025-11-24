.LALL
;*****************************************************************************
;
;       (C) Copyright MICROSOFT Corp, 1995
;
;       Title:      LANCELNK.ASM sourced from:
;       Title:      NDISLNK.ASM - Assembly linkage to NDIS Wrapper for MACs
;                                  and Protocols
;
;       Version:    3.00
;
;       Date:       05-Nov-1991
;
;=============================================================================
    TITLE $LANCELNK
    .386P

INCLUDE VMM.INC
INCLUDE NDIS.INC
INCLUDE NETVXD.INC          ; Net VxD initialization oredr



DECLARE_VIRTUAL_DEVICE %MAC, 3, 10, <%MAC>_Control, Undefined_Device_Id, \
        MAC_Init_Order

IFDEF CHICAGO
VxD_LOCKED_DATA_SEG

Public bInitAlready	
	bInitAlready	 DB 0
	
VxD_LOCKED_DATA_ENDS
ENDIF

VxD_LOCKED_CODE_SEG

BeginProc C_Device_Init

IFDEF NDIS_STDCALL
extern _DriverEntry@8:NEAR
ELSE
extern _DriverEntry:NEAR
ENDIF

IFDEF CHICAGO
    mov  al, bInitAlready
	cmp  al, 0					; Make sure we' haven't been called already.
	jnz  Succeed_Init_Phase
	inc  bInitAlready			; Set the "Called Already" Flag
ENDIF

; Make sure the wrapper (Ndis.386) is loaded
    VxDcall NdisGetVersion
    jc   Fail_Init_Phase

    push 0
    push 0

IFDEF NDIS_STDCALL
    call _DriverEntry@8
ELSE
    call _DriverEntry
    add  esp,8
ENDIF

    cmp  eax,NDIS_STATUS_SUCCESS
    jne  Fail_Init_Phase
Succeed_Init_Phase:
    clc
    ret
Fail_Init_Phase:
    stc
    ret
EndProc C_Device_Init

Begin_Control_Dispatch %MAC
    Control_Dispatch Device_Init,       			C_Device_Init
IFDEF CHICAGO
    Control_Dispatch Sys_Dynamic_Device_Init,       C_Device_Init
ENDIF
End_Control_Dispatch %MAC



VxD_LOCKED_CODE_ENDS

;******************************************************************************
;                 R E A L   M O D E   I N I T   C O D E
;******************************************************************************

;******************************************************************************
;
;   MAC_Real_Init
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

VxD_REAL_INIT_SEG

BeginProc MAC_Real_Init

;
;   If another us is loaded then don't load -- Just abort our load
;
     test    bx, Duplicate_From_INT2F OR Duplicate_Device_ID
     jnz     SHORT Ndis_RI_Abort_Load



;   No other  is loaded.

     xor     bx, bx
     xor     si, si
     mov     ax, Device_Load_Ok
     ret

Ndis_RI_Abort_Load:
     xor     bx, bx
     xor     si, si
     mov     ax, Abort_Device_Load
     ret

EndProc MAC_Real_Init

VxD_REAL_INIT_ENDS

END MAC_Real_Init
