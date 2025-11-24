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

PAGE 58,132
;******************************************************************************
;
;   Title:	VNETBIOS.INC - Virtual NetBIOS Device Service Declarations
;
;   Version:	1.00
;
;==============================================================================

Begin_Service_Table VNETBIOS

VNETBIOS_Service VNETBIOS_Get_Version, LOCAL
VNETBIOS_Service VNETBIOS_Register, LOCAL
VNETBIOS_Service VNETBIOS_Submit, LOCAL
VNETBIOS_Service VNETBIOS_Enum, LOCAL
VNETBIOS_Service VNETBIOS_Deregister, LOCAL
VNETBIOS_Service VNETBIOS_Register2, LOCAL
VNETBIOS_Service VNETBIOS_Map, LOCAL
VNETBIOS_Service VNETBIOS_Enum2, LOCAL

End_Service_Table VNETBIOS
