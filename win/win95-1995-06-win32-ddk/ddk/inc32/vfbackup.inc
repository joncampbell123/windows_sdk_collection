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

VFBACKUP_INIT_ORDER	EQU	<IFSMGR_INIT_ORDER+800000H>
VFBACKUP_MAJOR_VER	EQU	4H
VFBACKUP_MIN_VER	EQU	0
FLP_DIRECT_PIO_ACCESS	EQU	1H
FLP_DIRECT_PIR_ACCESS_BIT	EQU	0H
FLP_NEC_INT_EXPECTED	EQU	2H
FLP_NEC_INT_EXPECTED_BIT	EQU	1H
FLP_THREAD_LOCK	EQU	100H
FLP_THREAD_LOCK_BIT	EQU	8H
FLP_ALLOW_MOTOR_OFF	EQU	4H
FLP_ALLOW_MOTOR_OFF_BIT	EQU	2H
FLP_TRAP_ALL_PIO	EQU	8H
FLP_TRAP_ALL_PIO_BIT	EQU	3H





Begin_Service_Table VFBACKUP
VFBACKUP_Service	VFBACKUP_Get_Version, LOCAL
VFBACKUP_Service	VFBACKUP_Lock_NEC, LOCAL
VFBACKUP_Service	VFBACKUP_UnLock_NEC, LOCAL
VFBACKUP_Service	VFBACKUP_Register_NEC, LOCAL
VFBACKUP_Service	VFBACKUP_Register_VFD, LOCAL
VFBACKUP_Service	VFBACKUP_Lock_All_Ports, LOCAL
End_Service_Table VFBACKUP


