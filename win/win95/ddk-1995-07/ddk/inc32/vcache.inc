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

ifndef VCache_Device_ID
VCache_Device_ID	EQU	048BH
endif
.errnz VCache_Device_ID - 048Bh

VCLF_LOCKED	EQU	00000010H
VCL_MAXCACHENAMELEN	EQU	80
ifndef NOSERVICE
Begin_Service_Table  VCache
VCache_Service		 VCache_Get_Version, LOCAL
VCache_Service		 VCache_Register, LOCAL
VCache_Service		 VCache_GetSize, LOCAL
VCache_Service		 VCache_CheckAvail, LOCAL
VCache_Service		 VCache_FindBlock, LOCAL
VCache_Service		 VCache_FreeBlock, LOCAL
VCache_Service		 VCache_MakeMRU, LOCAL
VCache_Service		 VCache_Hold, LOCAL
VCache_Service		 VCache_Unhold, LOCAL
VCache_Service		 VCache_Enum, LOCAL
VCache_Service		 VCache_TestHandle, LOCAL
VCache_Service		 VCache_VerifySums, LOCAL
VCache_Service		 VCache_RecalcSums, LOCAL
VCache_Service		 VCache_TestHold, LOCAL
VCache_Service		 VCache_GetStats, LOCAL
VCache_Service		 VCache_Deregister, LOCAL
VCache_Service		 VCache_AdjustMinimum, LOCAL
VCache_Service		 VCache_SwapBuffers, LOCAL
VCache_Service		 VCache_RelinquishPage, LOCAL
VCache_Service		 VCache_UseThisPage, LOCAL
VCache_Service		 _VCache_CreateLookupCache
VCache_Service		 _VCache_CloseLookupCache
VCache_Service		 _VCache_DeleteLookupCache
VCache_Service		 _VCache_Lookup
VCache_Service		 _VCache_UpdateLookup
End_Service_Table VCache
endif
VCache_VERSION	EQU	100H
VCFB_Create	EQU	01H
VCFB_Hold	EQU	02H
VCFB_MakeMRU	EQU	04H
VCFB_LowPri	EQU	08H
VCFB_MustCreate	EQU	10H
VCFB_RemoveFromLRU	EQU	20H
B_FSKey1	equ	dword ptr 08H
B_FSKey2	equ	dword ptr 0CH
B_BufPtr 	equ	dword ptr 10H

B_FSDData	equ	014H
SizeFSDData	equ	(7*4)

B_Dirty		equ	byte ptr 032H

VCACHE_BUFSIZE	EQU	4096
