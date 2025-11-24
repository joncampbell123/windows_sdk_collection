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

Begin_Service_Table VMCPD
VMCPD_Service	 VMCPD_Get_Version, LOCAL
VMCPD_Service	 VMCPD_Get_Virt_State, LOCAL
VMCPD_Service	 VMCPD_Set_Virt_State, LOCAL
VMCPD_Service	 VMCPD_Get_CR0_State, LOCAL
VMCPD_Service	 VMCPD_Set_CR0_State, LOCAL
VMCPD_Service	 VMCPD_Get_Thread_State, LOCAL
VMCPD_Service	 VMCPD_Set_Thread_State, LOCAL
VMCPD_Service	 _VMCPD_Get_FP_Instruction_Size, LOCAL
VMCPD_Service	 VMCPD_Set_Thread_Precision, LOCAL
End_Service_Table VMCPD
