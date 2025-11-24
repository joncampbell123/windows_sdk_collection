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

; :ts=4

;**	nethlp.h - NET$HLP$ driver definitions
;
;	WARNING: This file is used to create nethlp.inc
;			 1) Make sure it is h2inc-able
;			 2) Don't change nethlp.inc, only change nethlp.h
;

;NOINC
;#ifndef NDISHLP_INC
;#define NDISHLP_INC	1
;INC


;**	Contacting the Net$Hlp$ driver
;
;	The direct call entry point for the Net$Hlp$ driver is located
;	by issuing a Receive Control Data IOCtl (AX=4402h) to the driver
;	with DS:DX pointing to an RIOCData structure and
;	CX=sizeof(struct RIOCData).  RIOCData.iod_magic must be set to
;	RIOCMAGICIN prior to issuing the IOCtl request.
;	If the request is successful, Net$Hlp$ will set RIOCData.iod_magic
;	to RIOCMAGICOUT and will fill in RIOCData.iod_addr with the
;	direct call entrypoint.
;
;	The direct call entrypoint is called C style with a FAR return
;	address on the stack.  The first parameter is always a function code.
;

RIOCData 	struc
	iod_magic dd ?	; Magic recognition variable			
	iod_addr dd ?	; Address of drivers handler routine	
RIOCData	ends

;**	Magic numbers for redir recognition  (used in iod_magic)
;
;		Inbound = "Ri##"  where ## = ASCII X.Y version number
;		Outbound= "##Ro"  where ## = ASCII X.Y version number
;
;	NOTE:	The magic numbers will change when there is a change
;			in the interface to the Net$Hlp$ driver
;
;	NOTE:	RIOCMAGICIN = RIOCMAGICINL + ((ulong)RIOCMAGICINH << 16)
;			RIOCMAGICOUT = RIOCMAGICOUTL + ((ulong)RIOCMAGICOUTH << 16)
;

RIOCMAGICINL	equ	0E96Fh
RIOCMAGICINH	equ	03733h

RIOCMAGICOUTL	equ	03733h
RIOCMAGICOUTH	equ	0EF6Fh

RIOCMAGICIN		equ	03733E96Fh
RIOCMAGICOUT	equ	0EF6F3733h

;**	Direct call stuff.
;
;	The following definitions describe the Net$Hlp$ drivers direct
;	access entrypoint.

;typedef unsigned long _cdecl ReqHand(unsigned short func, ...);
;typedef ReqHand FAR *pReqHand;


;*		Functions to the driver handler

RDRHNDGETVARS	equ	0	; Get addrs of the redirs vars
RDRHNDINSTALL	equ	1	; Install
RDRHNDUNINSTALL	equ	2	; DeInstall
RDRHNDSETLPT	equ	3	; Put LPT into chain
RDRHNDREMOVELPT	equ	4	; Remove LPT from chain
RDRHNDTRANSITION	equ	5	; Transition to VRedir
VNBGETINFO	equ	6	; VNB Get Info
VNBWRITEMCBT	equ	7	; VNB Write MCB Table
VNBWRITEHOOK	equ	8	; VNB Write Interrupt hooks
VNBHOOKSONOFF	equ	9	; VNB Interrupt hooks on


;**	VNBGETINFO - Get addresses of VNB structures
;
; Entry	TOS		- word:  function code = VNBGETINFO
; Exit		ax = offset in driver of pminfo structure
;			dx = offset in driver of VLDT structure
; Uses		ax, bx, cx, dx, es, flags

;**	VNBWRITEMCBT - Save the address of the MCB Table
;
; Entry	TOS		- word:  function code = VNBWRITEMCBT
;			TOS + 2	- dword: far address of MCB Table
; Exit		none
; Uses		ax, bx, cx, dx, es, flags

;**	VNBWRITEHOOK - Set the routing information of an interrupt hook
;
;		NOTICE: This call must be made for EACH interrupt that
;				needs routing information
;
; Entry	TOS		- word:  function code = VNBWRITEHOOK
;			TOS + 2	- dword: far address of handler routine
;			TOS + 6	- word:  interrupt level
; Exit		none
; Uses		ax, bx, cx, dx, es, flags

;**	VNBHOOKSONOFF - Enable interrupt hooks
;
; Entry	TOS		- word:  function code = VNBHOOKSON
;			TOS+2	- word:  flag, 0 = turn hooks off, 1 = turn hooks on
; Exit		ax		- previous state of hooks
; Uses		ax, bx, cx, dx, es, flags

;NOINC
;#endif
;INC
