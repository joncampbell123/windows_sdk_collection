;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   vadlibd.inc
;
;   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
;
;   Description:
;       This VxD handles contention for the AdLib Synth between VMs.  Note
;       this is NOT a full virtualizing driver.
;
;   Notes:
;       If you are including this file in a VxD, you MUST include VMM.INC
;       first.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   The following equates can either be used by pmode apps, rmode apps,
;   or VxD's.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

Multimedia_OEM_ID           equ 0440h            ; MS Reserved OEM # 34
VADLIBD_Device_ID           equ Multimedia_OEM_ID + 6

VADLIBD_Ver_Major           equ 1                ; version 1.0 of this VxD
VADLIBD_Ver_Minor           equ 0

;
; AdLib related equates
;
VADLIBD_Max_Synths          equ 4                ; currently only 4 at once

;
; AdLib default base and IRQ
;
VADLIBD_DEFAULT_BASE        equ 388h             ; normal default port

;
; PMode and RMode API indexes
;
ADLIB_API_Get_Version       equ 0                ; get version info on VADLIBD
ADLIB_API_Acquire_Synth     equ 1
ADLIB_API_Release_Synth     equ 2
ADLIB_API_Get_Synth_Array   equ 3

;
; Error returns for ADLIB_API_Acquire_Synth (returned in Client_AX)
; VADLIBD_Acquire_Synth returns these error codes also...
;
ADLIB_AS_Err_Bad_Synth      equ 01h
ADLIB_AS_Err_Already_Owned  equ 02h

;
; Error returns for ADLIB_API_Release_Synth (returned in Client_AX)
; VADLIBD_Release_Synth returns these error codes also...
;
ADLIB_RS_Err_Bad_Synth      equ 01h
ADLIB_RS_Err_Not_Yours      equ 02h

;
; Error returns for ADLIB_Install_Handler (returned in Client_AX)
;
ADLIB_IH_Err_Bad_Synth      equ 01h
ADLIB_IH_Err_Already_Owned  equ 02h
ADLIB_IH_Err_Wrong_Phase    equ 03h

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   The following equates are only useful in a VxD.  DDB_Sys_Crit_Init_Done
;   is defined by VMM.INC iff 'Not_Vxd' is NOT defined.  In this way, if
;   VMM.INC is included in a VxD before this include file, you will have
;   access to this information.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

ifdef DDB_Sys_Crit_Init_Done

;
; The init order for VADLIBD *MUST* be before any VxD that uses it.
;
VADLIBD_Init_Order              equ WINLOAD_Init_Order

;
; Flags for VADLIBD_Virtualize_Synth VxD service (EBX)
;
fADLIB_VS_Install_Handler       equ 0000000000000001b

;
; Error returns in EAX for VADLIBD_Virtualize_Synth VxD service
;
ADLIB_VS_Err_No_Error               equ 00h
ADLIB_VS_Err_Already_Virtualized    equ 01h
ADLIB_VS_Err_No_Available_Slots     equ 03h
ADLIB_VS_Err_Memory_Alloc_Failed    equ 04h
ADLIB_VS_Err_Ports_Hooked           equ 05h

;
; Global state data
;
ADLIB_STRUC STRUC

wAdLibBasePort      dw  ?       ; base port address for synth
wAdLibFlags         dw  0       ; flags for trapping, etc.
dwAdLibOwner        dd  0       ; VM handle owning this AdLib Synth
dwAdLibHandler      dd  ?       ; pointer to custom handler
rgdwAdLibReserved2  dd  2 DUP(?); reserved

ADLIB_STRUC ENDS

;
; VxD service table
;
Begin_Service_Table VADLIBD

VADLIBD_Service VADLIBD_Get_Version, LOCAL
VADLIBD_Service VADLIBD_Virtualize_Synth, LOCAL
VADLIBD_Service VADLIBD_Get_Synth_Focus, LOCAL
VADLIBD_Service VADLIBD_Acquire_Synth, LOCAL
VADLIBD_Service VADLIBD_Release_Synth, LOCAL
VADLIBD_Service VADLIBD_Install_Handler, LOCAL

End_Service_Table VADLIBD

endif   ; DDB_Sys_Crit_Init_Done (VxD's only)
