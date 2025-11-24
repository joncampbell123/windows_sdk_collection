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
;----------------------------Module-Header------------------------------;
; Module Name: EGAINIT.ASM
;
; EGA initialization code.
; 
; This module handles disabling of the RC_SAVEBITMAP raster capability
; if the EGA doesn't have 256Kbytes of display memory.  This operation
; happens at run time, because the bit will always be set at assembly-
; time if the SaveScreenBitmap code is present in the driver, though
; a particular EGA board may not be able to use it.
;-----------------------------------------------------------------------;

	title	EGA Initialization Code

incDevice = 1				;allow assembly of needed constants

	.xlist
	include cmacros.inc
	include gdidefs.inc
	.list

	??_out	egainit

	externA		ScreenSelector	; the selector for display memory

sBegin	Data

	externW ssb_mask	;Mask for save screen bitmap bit

sEnd	Data

createSeg _INIT,InitSeg,byte,public,CODE
sBegin	InitSeg
assumes cs,InitSeg


	externW	physical_device

page

;--------------------------Public-Routine-------------------------------;
; dev_initialization - device specific initialization
;
; Any device specific initialization is performed.
;
; Entry:
;	None
; Returns:
;	AX = 1
; Registers Preserved:
;	SI,DI,BP,DS
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	int 10h
;-----------------------------------------------------------------------;

;----------------------------Pseudo-Code--------------------------------;
;	load registers to request amount of memory on EGA
;	call EGA BIOS
;	if less than 256Kbytes on EGA board
;		clear RC_SAVEBITMAP bit in GDI info table of driver
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

cProc	dev_initialization,<NEAR,PUBLIC>

cBegin

;	Call the EGA BIOS to see how much memory we have.  If we
;	have less than 256K, then we must make sure the RC_SAVEBITMAP
;	bit of the BitBLT capabilities word in the table is cleared.
;	This prevents USER from calling save_screen_bitmap when there
;	is probability zero (0) of success.

	mov	ah,12h			;"alternate select"
	mov	bl,10h			;"return EGA information"
	int	10h

	and	bl,00000011b		;destroy unneeded info
	cmp	bl,00000011b		;do we have 256K?
	je	ega_init_exit		;if yes, don't clear mask
	and	ssb_mask,not RC_SAVEBITMAP

ega_init_exit:

	mov	ax,1			;no way to have error

cEnd

sEnd	InitSeg
end
