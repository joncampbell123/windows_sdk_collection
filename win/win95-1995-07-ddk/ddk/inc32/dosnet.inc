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
;   Title:	DOSNET.Inc - Public services for DOSNET
;
;   Version:	1.00
;
;==============================================================================

Begin_Service_Table DOSNET

DOSNET_Service	DOSNET_Get_Version, LOCAL
DOSNET_Service	DOSNET_Send_FILESYSCHANGE, LOCAL
DOSNET_Service	DOSNET_Do_PSP_Adjust, LOCAL

End_Service_Table DOSNET
