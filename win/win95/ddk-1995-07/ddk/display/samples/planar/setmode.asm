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

	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	SETMODE.ASM
;
; This module contains the routine which is called by the control
; panel when a device is to change modes.
;
; Exported Functions:	DeviceMode
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
;	This function is a nop for all display drivers.
;
; Restrictions:
;
;-----------------------------------------------------------------------;

	.xlist
	include cmacros.inc
	.list


createSeg _BLUEMOON,BlueMoonSeg,word,public,CODE
sBegin	BlueMoonSeg
assumes cs,BlueMoonSeg
page
;--------------------------Exported-Routine-----------------------------;
; INT DeviceMode(hWnd,hInst,lpDeviceType,lpOutputFile)
; HWND	hWnd;
; HINST hInst;
; LPSTR lpDeviceType;
; LPSTR lpOutputFile;
; 
; This routine is a nop for all display drivers.  It returns -1 to
; show success, just to keep everyone happy.
;
; Warnings:
;	none
; Effects:
;	none
; Calls:
;	none
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; INT DeviceMode(hWnd,hInst,lpDeviceType,lpOutputFile)
; HWND	hWnd;
; HINST hInst;
; LPSTR lpDeviceType;
; LPSTR lpOutputFile;
; {
;   return (-1);
; }
;-----------------------------------------------------------------------;


cProc	DeviceMode,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	parmW	h_wnd			;Handle to window
	parmW	h_inst			;Handle to instance
	parmD	lp_device_type		;Pointer to device (e.g. "HP7470")
	parmD	lp_output_file		;Pointer to output file (e.g. COM1:)


cBegin	<nogen>

	mov	ax,-1			;Show success
	ret	12			;Return, popping parameters

cEnd	<nogen>

sEnd	BlueMoonSeg
end
