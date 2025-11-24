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
; GETRESID.ASM
;----------------------------------------------------------------------------
	.xlist
	include cmacros.inc
	include	macros.inc
	.list
;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
	externW	wDpi			;in vga.asm
sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin	Code
assumes cs,Code
;----------------------------------------------------------------------------
; GetDriverResourceID
;----------------------------------------------------------------------------
cProc	GetDriverResourceID,<FAR,WIN,PASCAL,PUBLIC>
        parmW  wResID
        parmD  lpResType
cBegin
	assumes	ds,Data
	mov	ax,wResID		;Get res id into ax.
	cmp	ax,3			;Is it the fonts resource?
	jne	short @f		;no. leave it alone.
        cmp     wDpi,96			;is dpi = 96?
	je	short @f		;yes. leave it alone.
	mov	ax,2003			;no. remap it to 2003.
@@:	xor	dx,dx		        ;dx must be zero.                
cEnd GetDriverResourceID
sEnd	Code
end
