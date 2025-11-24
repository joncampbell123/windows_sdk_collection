;***************************************************************************
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;   Copyright (c) 1994 - 1995	Microsoft Corporation.	All Rights Reserved.
;  File:       vjoydapi.h
;  Content:	VJOYDAPI service equates and structures
;*************************************************************************
MULTIMEDIA_OEM_ID	EQU	0440H		       ; MS Reserved OEM # 34
VJOYD_Device_ID	EQU	MULTIMEDIA_OEM_ID+9  ; VJOYD API Device
VJOYD_Ver_Major	EQU	1
VJOYD_Ver_Minor	EQU	0
; VJOYDAPI_Get_Version
;;
;   ENTRY:
;	AX = 0
;   RETURNS:
;	SUCCESS: AX == TRUE
;	ERROR:   AX == FALSE
VJOYDAPI_GetVersion	EQU	0
VJOYDAPI_IOCTL_GetVersion	EQU	0
; VJOYDAPI_GetPosEx
;;
;   ENTRY:
;	AX = 1
;	DX = joystick id (0,1)
;	ES:BX = pointer to JOYINFOEX struct
;   RETURNS:
;	SUCCESS: EAX == MMSYSERR_NOERROR
;	ERROR:   EAX == JOYERR_PARMS
;			JOYERR_UNPLUGGED
VJOYDAPI_GetPosEx	EQU	1
VJOYDAPI_IOCTL_GetPosEx	EQU	1
; VJOYDAPI_GetPos
;;
;   ENTRY:
;	AX = 1
;	DX = joystick id (0,1)
;	ES:BX = pointer to JOYINFO struct
;   RETURNS:
;	SUCCESS: EAX == MMSYSERR_NOERROR
;	ERROR:   EAX == JOYERR_PARMS
;			JOYERR_UNPLUGGED
VJOYDAPI_GetPos	EQU	2
VJOYDAPI_IOCTL_GetPos	EQU	2
; VJOYDAPI_SetData
;;
;   ENTRY:
;	AX = 3
;	ES:BX = pointer to joystick data (private)
;;
;   RETURNS:
;	SUCCESS: Always
VJOYDAPI_SetData	EQU	3				; ;Internal
VJOYDAPI_IOCTL_SetData	EQU	3				; ;Internal
; VJOYDAPI_GetHWCaps
;;
;   ENTRY:
;	AX = 4
;	ES:BX = pointer to hw caps data (private)
;;
;   RETURNS:
;	SUCCESS: Always
VJOYDAPI_GetHWCaps	EQU	4				; ;Internal
