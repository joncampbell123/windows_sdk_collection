/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/*==============================================================================
 * VFBACKUP VxD declaration constants
 *============================================================================*/

// we want to initialize just after IOS in order to check if NEC is present
#define VFBACKUP_INIT_ORDER	IFSMGR_INIT_ORDER + 0x800000

#define VFBACKUP_MAJOR_VER	0x4
#define VFBACKUP_MIN_VER 	0

/*==============================================================================
 * PUBLIC defines
 *============================================================================*/

// defines for *pvfb_LockFlags

#define FLP_DIRECT_PIO_ACCESS 0x1	// indicates that PIO access occurred
#define FLP_DIRECT_PIR_ACCESS_BIT 0x0  // without a Lock, so use the timeout
#define FLP_NEC_INT_EXPECTED  0x2	// interrupt is expected from the NEC
#define FLP_NEC_INT_EXPECTED_BIT 0x1	// driver code
#define FLP_THREAD_LOCK	0x100	// indicates a thread based lock
#define FLP_THREAD_LOCK_BIT	0x8
#define FLP_ALLOW_MOTOR_OFF	0x4	// indicates that motor_off should be allowed
#define FLP_ALLOW_MOTOR_OFF_BIT 0x2
#define FLP_TRAP_ALL_PIO		0x8 // indicates that all port access will fail
#define FLP_TRAP_ALL_PIO_BIT	0x3	

;#define VFBACKUP_Name_Based	1

/* ASM

;==============================================================================
; VFBACKUP SERVICE TABLE
;==============================================================================

Begin_Service_Table VFBACKUP
VFBACKUP_Service	VFBACKUP_Get_Version, LOCAL
VFBACKUP_Service	VFBACKUP_Lock_NEC, LOCAL
VFBACKUP_Service	VFBACKUP_UnLock_NEC, LOCAL
VFBACKUP_Service	VFBACKUP_Register_NEC, LOCAL
VFBACKUP_Service	VFBACKUP_Register_VFD, LOCAL
VFBACKUP_Service	VFBACKUP_Lock_All_Ports, LOCAL
End_Service_Table VFBACKUP

*/


