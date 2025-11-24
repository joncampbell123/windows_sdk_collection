;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1994 - 1995  Microsoft Corporation.  All Rights Reserved.
;
;   File:	poll.inc
;   Content:	include file for polling routines
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

JOY1_X_MASK   equ <0001h>   	    ; mask for joystick 1 X port bits
JOY1_Y_MASK   equ <0002h>   	    ; mask for joystick 1 Y port bits
JOY2_X_MASK   equ <0004h>           ; mask for joystick 2 X port bits
JOY2_Y_MASK   equ <0008h>           ; mask for joystick 2 Y port bits
JOY_ALL	      equ <000fh>	    ; masks for all bits

extern _dwXpos:DWORD
extern _dwYpos:DWORD
extern _dwZpos:DWORD
extern _dwRpos:DWORD
extern _dwButtons:DWORD
extern _dwTimeOut:DWORD
extern _bPass1:DWORD
extern _cXYBits:BYTE
extern _c3Bits:BYTE
extern _cAllBits:BYTE
extern _cButtonShift:BYTE
extern _cXshift:BYTE
extern _cYshift:BYTE
extern _cZshift:BYTE
extern _cRshift:BYTE
extern _cXbit:BYTE
extern _cYbit:BYTE
extern _cZbit:BYTE
extern _cRbit:BYTE
extern _dwJoyPort:DWORD
