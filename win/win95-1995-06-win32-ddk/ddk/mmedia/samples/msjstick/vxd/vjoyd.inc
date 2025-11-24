;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1994 - 1995  Microsoft Corporation.  All Rights Reserved.
;
;   VJOYD.INC
;   File:	vjoyd.inc
;   Content:	vjoyd include file
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   The following equates can either be used by pmode apps, rmode apps,
;   or VxD's.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

	include	vjoydapi.inc

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   The following equates are only useful in a VxD.  DDB_Sys_Crit_Init_Done
;   is defined by VMM.INC iff 'Not_Vxd' is NOT defined.  In this way, if
;   VMM.INC is included in a VxD before this include file, you will have
;   access to this information.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

ifdef DDB_Sys_Crit_Init_Done

Begin_Service_Table	VJOYD
;; Joystick VxDs use this service to register their polling routine and
;; their PNP Configuration Handler Procedure.
	VJOYD_Service	VJOYD_Register_Device_Driver, LOCAL
	VJOYD_Service	VJOYD_GetPosEx_Service, LOCAL
End_Service_Table VJOYD	

VJOYD_Init_Order              equ UNDEFINED_INIT_ORDER

ENDIF   ; DDB_Sys_Crit_Init_Done (VxD's only)
