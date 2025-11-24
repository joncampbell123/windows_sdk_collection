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
;  INKREADY.ASM
;
;  This implements the new InkReady API for PenWindows
;
;----------------------------------------------------------------------------

.xlist
        include cmacros.inc
.list

sBegin		Data

;
; interesting variables from trail.asm
;

IS_EXCLUDED	equ 000000001b		;Cursor is excluded.
IS_BUSY      	equ 000000010b		;Critical section.
IS_NULL	    	equ 000000100b		;Cursor is null.
HAS_MOVED	equ 000001000b		;Cursor has moved.
IS_VISIBLE   	equ 000010000b		;Cursor is visible.


;
; lpfnUpdateInking is the lpfn passed to InkReady
;

globalD		lpfnUpdateInking,0

;
; fInkAvailable is TRUE when there is ink data available,
; but could not be drawn because the screen was busy.
;

globalB		fInkAvailable,0

;
; lpDeviceForPenWindows is a the lpDevice for the whole screen.
; This display driver hack will be removed when GDI returns this
; value.
;

globalD		lpDeviceForPenWindows,0

sEnd		Data

sBegin		Code

assumes		cs,Code
assumes		ds,nothing
assumes		es,nothing

page

;--------------------------Public-Routine-------------------------------;
; BOOL InkReady(FARPROC lpfn);
;
; InkReady announces to the PenWindows-compliant display driver that
; inking data is available, and when it is safe to perform graphics
; operations, lpfn should be called.  If it is currently not safe to
; perform a graphics operation, this function should return false, but
; call lpfn once it is safe (i.e. when UnExclude (cursors.asm) is
; called).
;
; Entry: parameter
;
; Returns: TRUE if lpfn was called right away
;		(i.e. the screen was not busy)
;
; Registers Destroyed: AX,FLAGS, and maybe BX,CX,DX (lpfn might)
;
; Registers Preserved: SI,DI,CS,DS,ES,SS,SP,BP
;
; Calls: lpfn, if screen is not now busy
;
;-----------------------------------------------------------------------;

cProc	InkReady,<FAR,PUBLIC,WIN,PASCAL>,<>
	parmD	lpfn

cBegin
	assumes	ds,Data

%out >>> INKING is currently broken. <<<
if 0
	mov	al,IS_BUSY
	xchg	al,CURSOR_STATUS
	test	al,IS_EXCLUDED or IS_BUSY
	jnz	IR_cant_ink_now
	or	CURSOR_STATUS,al

	; the screen was not busy.  the xchg marked it as busy,
	; so call lpfn, and then mark it as not busy.

	push	ax
	call	dword ptr lpfn
	pop	ax

	xchg	al,CURSOR_STATUS
	mov	ax,1
	jmp	short IR_return

IR_cant_ink_now:

	xchg	al,CURSOR_STATUS

	; whoops!  we can't ink now, so let's save away the lpfn
	; for later use, and set the "InkAvailable" flag.
	; UnExclude (trail.asm - called after every graphics
	; operation) checks for this flag, and will call lpfn
	; if the InkAvailable flag is set.

	mov	ax,word ptr lpfn[0]
	mov	word ptr lpfnUpdateInking[0],ax
	mov	ax,word ptr lpfn[2]
	mov	word ptr lpfnUpdateInking[2],ax

	mov	fInkAvailable,1
	xor	ax,ax

IR_return:
endif

cEnd

cProc	GetLPDevice,<FAR,PUBLIC,WIN,PASCAL>,<>
cBegin
	mov	ax,word ptr lpDeviceForPenWindows[0]
	mov	dx,word ptr lpDeviceForPenWindows[2]
cEnd

sEnd		Code

	END

