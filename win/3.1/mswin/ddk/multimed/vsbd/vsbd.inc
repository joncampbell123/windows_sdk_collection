;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   vsbd.inc
;
;   Copyright (c) 1991-1992 Microsoft Corporation.  All rights reserved.
;
;   Description:
;       This VxD handles contention for a Sound Blaster card between VMs.
;       Note this is NOT a full virtualizing driver.
;
;   Notes:
;       If you are including this file in a VxD, you MUST include VMM.INC
;       first.  It also makes calls into vadlibd.386, which was shipped
;       with Windows 3.1.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   The following equates can either be used by pmode apps, rmode apps,
;   or VxD's.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

Multimedia_OEM_ID               equ 0440h       ; MS Reserved OEM # 34
VSBD_Device_ID                  equ Multimedia_OEM_ID + 5

VSBD_Ver_Major                  equ 1           ; version 1.1 of this VxD
VSBD_Ver_Minor                  equ 1

;
; VSBD related equates
;
VSBD_Max_Sound_Blaster          equ 1           ; currently only 1 at a time

;
; PMode and RMode API indexes
;
SB_API_Get_Version              equ 0           ; get version info on VSBD
SB_API_Acquire_Sound_Blaster    equ 1
SB_API_Release_Sound_Blaster    equ 2
SB_API_Get_Sound_Blaster_Info   equ 3

;
; Flags for acquire sound blaster
;
fSB_ASB_Acquire_AdLib_Synth     equ 00000001b
fSB_ASB_Acquire_DSP             equ 00000010b
fSB_ASB_Auto_Reset_DSP          equ 00000100b

;
; Error returns for SB_API_Acquire_Sound_Blaster (returned in Client_AX)
;
SB_API_ASB_Err_Bad_SB           equ 0001h
SB_API_ASB_Err_Already_Owned    equ 0002h
SB_API_ASB_Err_State_Unknown    equ 8000h

;
; Error returns for SB_API_Release_Sound_Blaster (returned in Client_AX)
;
SB_API_RSB_Err_Bad_SB           equ 01h
SB_API_RSB_Err_Not_Yours        equ 02h


;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   The following equates are only useful in a VxD.  DDB_Sys_Crit_Init_Done
;   is defined by VMM.INC iff 'Not_Vxd' is NOT defined.  In this way, if
;   VMM.INC is included in a VxD before this include file, you will have
;   access to this information.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

ifdef DDB_Sys_Crit_Init_Done

;
; The init order for VSBD *MUST* be after VADLIBD.386
;
VSBD_Init_Order     equ     Undefined_Init_Order

;
; VxD service table
;
Begin_Service_Table VSBD

VSBD_Service VSBD_Get_Version, LOCAL
VSBD_Service VSBD_Get_Sound_Blaster_Focus, LOCAL

End_Service_Table VSBD

endif ; DDB_Sys_Crit_Init_Done (VxD's only)
