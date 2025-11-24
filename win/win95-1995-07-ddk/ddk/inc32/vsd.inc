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
;   Title:	Virtual Sound Device Exported Services
;
;   Version:	1.00
;
;==============================================================================

Begin_Service_Table VSD

VSD_Service VSD_Get_Version, LOCAL
VSD_Service VSD_Bell, LOCAL
VSD_Service VSD_SoundOn, LOCAL
VSD_Service VSD_TakeSoundPort, LOCAL

End_Service_Table VSD
