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

MED	STRUC
MED_Physical	DD	?
MED_Next	DD	?
MED_Adjacent	DD	?
MED_Type	DW	?
MED_Size	DW	?
MED_Flags	DW	?
MED_type_spec	DW	?
MED	ENDS
MED_FREE_POOL	EQU	0001H
MED_FREE_FAST	EQU	0002H
MED_ALLOC	EQU	0004H
MED_PANIC	EQU	0008H
MED_EXTERNAL	EQU	0010H
MED_SMART	EQU	0020H
MED_FREE_TYPE	EQU	0
MED_IOP	EQU	1
MED_ILB	EQU	2
MED_AEP	EQU	3
MED_DCB	EQU	4
MED_DDB	EQU	5
MED_DRP	EQU	6
MED_DVT	EQU	7
MED_IRB	EQU	8
MED_MISC	EQU	9
MED_ILR	EQU	10
MED_IOC	EQU	11
MED_NEW_IOP	EQU	12
MED_VRP	EQU	13
MED_PCD	EQU	14
MED_MAX_TYPE	EQU	14
MED_GRAN_DELTA	EQU	16
MED_FAST_GRANS	EQU	64
MED_SHIFT_CNT	EQU	4-2
