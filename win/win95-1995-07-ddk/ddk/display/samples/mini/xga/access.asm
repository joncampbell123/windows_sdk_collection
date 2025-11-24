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

;----------------------------------------------------------------------------
; ACCESS.ASM 
; Begin and End framebuffer access functions.
;----------------------------------------------------------------------------
	.xlist
DOS5=1
	include cmacros.inc
	include macros.inc
	include dibeng.inc
	include xga.inc
	.list

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externFP DIB_BeginAccess	;in DIBENG.DLL
	externFP DIB_EndAccess		;in DIBENG.DLL
;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
	externD 	XGARegs 		;in VGA.ASM
	externB 	WindowsEnabledFlag	;in VGA.ASM
	externB 	CursorDefinedFlag	;in CURSOR.ASM
	globalB 	DIBAccessCallFlag,0	;
sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin	Code
	assumes cs,Code

;----------------------------------------------------------------------------
; BeginAccess(PDevice,Left,Top,Right,Bottom,Flags)
;
; GRAB surface semaphore
;
; RELEASE surface semaphore
; RETURN(Flags)
; 
;----------------------------------------------------------------------------
cProc	BeginAccess,<FAR,PUBLIC,NODATA>
;
	parmD	lpDevice
	parmW	Left
	parmW	Top
	parmW	Right
	parmW	Bottom
	parmW	Flags
;
cBegin
.386
	assumes ds,nothing
	mov	ax,_DATA
	mov	ds,ax			;first, make sure DS --> Data
	assumes ds,Data
	cmp	WindowsEnabledFlag,0	;does Windows own the CRTC?
	jne	@F			;nope, don't do this!
	les	di,XGARegs
	MakeHardwareNotBusy	es,di
@@:	test	Flags,CURSOREXCLUDE	;is this a request to exclude cursor?
	jz	BAExit			;nope, don't do this
	cmp	CursorDefinedFlag,2	;running with a software cursor?
	jne	BAExit			;nope, no need to call DIB engine
	mov	DIBAccessCallFlag,0ffh	;set the DIBAccessCallFlag
	arg	lpDevice		;call the DIB engine
	arg	Left			;
	arg	Top			;
	arg	Right			;
	arg	Bottom			;
	mov	ax,Flags		;
	and	ax,CURSOREXCLUDE	;(only pass the CURSOREXCLUDE order)
	arg	ax			;
	cCall	DIB_BeginAccess 	;
;
BAExit:
	mov	ax,Flags
.286c
cEnd

;----------------------------------------------------------------------------
; EndAccess(PDevice,Flags)
;
; GRAB surface semaphore
;
; RETURN(Flags)
;----------------------------------------------------------------------------
cProc	EndAccess,<FAR,PUBLIC,NODATA>
	parmD	lpDevice
	parmW	Flags
cBegin
.386
	assumes ds,nothing
	mov	ax,_DATA
	mov	ds,ax			;first, make sure DS --> Data
	assumes ds,Data
;
;We need to call the DIB Engine's EndAccess call to unexclude the cursor if
;we called its BeginAccess call:
;
	cmp	DIBAccessCallFlag,0	;
	je	EAExit			;nothing to do!
	test	Flags,CURSOREXCLUDE	;Unexclude cursor?
	jz	EAExit			;no
	push	lpDevice		;
	push	CURSOREXCLUDE		;
	call	DIB_EndAccess		;Let DIB Engine unexclude cursor.
	mov	DIBAccessCallFlag,0	;clear the flag
;
EAExit:
	mov	ax,Flags
.286c
cEnd

sEnd	Code
end
