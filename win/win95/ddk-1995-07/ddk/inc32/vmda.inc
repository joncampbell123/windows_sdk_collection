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

SHELL_Call_Dev_VDD	EQU	0000AH
SHELL_Debug_Out	EQU	8
VDD_MOD_MAX	EQU	256
VDD_CTRL_STATE_MAX	EQU	128
WMX_USER	EQU	0400H
INCLUDE VDDGRB.INC

.ERRE VDA_Type_Chng	EQ	((WMX_USER+20)+2)
.ERRE VDA_Display_Event EQ	((WMX_USER+20)+6)

