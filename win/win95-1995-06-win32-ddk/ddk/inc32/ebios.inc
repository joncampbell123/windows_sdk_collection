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
;   Title:	ebios.inc
;
;   Version:	1.00
;
;==============================================================================

Begin_Service_Table EBIOS

EBIOS_Service	  EBIOS_Get_Version, LOCAL
;
; Following service exists in EBIOS devices with version >= 2.00
;
EBIOS_Service	  EBIOS_Get_Unused_Mem, LOCAL

End_Service_Table EBIOS
