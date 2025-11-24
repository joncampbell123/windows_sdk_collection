;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;******************************************************************************
;
;   Title:	vdd2.inc - VDD definitions for Secondary VDD
;
;   Version:	1.00
;
;==============================================================================


Begin_Service_Table VDD2

;***************
;VDD2 services for other devices and VMM(Get_Version must be first)
;
VDD2_Service	VDD2_Get_Version	; Get version number and ID string ptr
End_Service_Table VDD2
