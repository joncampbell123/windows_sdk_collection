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

	.xlist
;----------------------------Module-Header------------------------------;
; Module Name: COPYDEV.BLT
;
; Subroutine to copy physical device information to local frame.
;
; This file is part of a set that makes up the Windows BitBLT function
; at driver-level.
;-----------------------------------------------------------------------;
	.list

	.xlist
if	MASMFLAGS and PUBDEFS
	public	copy_dev
ifdef	GEN_COLOR_BLT
	public	copy_dev_20
ifdef	THIS_IS_DOS_3_STUFF
else
	public	not_the_device
endif	;THIS_IS_DOS_3_STUFF
	public	copy_dev_bad_clr_format
endif	;GEN_COLOR_BLT
endif
	.list

;----------------------------Private-Routine----------------------------;
; copy_device
;
; Copy device information to frame.
;
; Entry:
;	DS:SI --> device
; 	ES:DI --> frame DEV structure
;	BH     =  gl_the_flags, accumulated so far
; Returns:
;	BH     =  gl_the_flags, accumulated so far
;	Carry clear if no error
; Error Returns:
;	Carry set if error (bad color format)
; Registers Preserved:
;	BX,CX,DS,ES,BP
; Registers Destroyed:
;	AX,DX,SI,DI,flags
; Calls:
;	None
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


copy_dev	proc near

	lodsw				;Get type
	errnz	bmType			;Must be the 1st word
	cmp	ax,1			;Set 'C' if the device
	cmc
	rcl	bh,1			;Move in type
	errnz	F0_SRC_IS_DEV-00001000b
	errnz	F0_DEST_IS_DEV-0000010b

	movsw				;Width in bits
	errnz	bmWidth-bmType-2
	errnz	width_bits

	movsw				;Height in scans
	errnz	bmHeight-bmWidth-2
	errnz	height-width_bits-2

	movsw				;Width in bytes
	errnz	bmWidthBytes-bmHeight-2
	errnz	width_b-height-2

	lodsw				;Get Planes/pixels
	cmp	ax,0101H		;Monochrome?
ifdef	GEN_COLOR_BLT
	je	copy_dev_20		;  Yes	('C' clear)
	externA COLOR_FORMAT
	cmp	ax,COLOR_FORMAT		;Our color?
	jne	copy_dev_bad_clr_format	;  No, complain about color format
	stc				;  (show color)
copy_dev_20:
else
	jne	copy_dev_bad_clr_format
	clc
endif
	rcl	bh,1			;Rotate in color flag
ifdef	GEN_COLOR_BLT
	errnz	F0_SRC_IS_COLOR-00000100b
	errnz	F0_DEST_IS_COLOR-00000001b
endif
	errnz	bmPlanes-bmWidthBytes-2
	errnz	bmBitsPixel-bmPlanes-1

ifdef	THIS_IS_DOS_3_STUFF
	movsw				;lpBits
	movsw
	errnz	bmBits-bmBitsPixel-1
	errnz	lp_bits-width_b-2
else
	movsw				;lpBits
	lodsw				;Save the selector if this
	stosw				;  is the physical device
	errnz	bmBits-bmBitsPixel-1
	errnz	lp_bits-width_b-2

	test	bh,00000010b
	jz	not_the_device
	mov	ScreenSelector,ax
	errnz	F0_SRC_IS_DEV-00001000b
	errnz	F0_DEST_IS_DEV-0000010b
not_the_device:
endif
	movsw				;Width of a plane if small color bitmap
	errnz	bmWidthPlanes-bmBits-4
	errnz	plane_w-lp_bits-4

	add	si,6			;-->segment index
	movsw				;Segment index if huge bitmap
	errnz	bmSegmentIndex-bmWidthPlanes-8
	errnz	seg_index-plane_w-2

	movsw				;Number of scans in a segment
	errnz	bmScanSegment-bmSegmentIndex-2
	errnz	scans_seg-seg_index-2

	movsw				;Number of unused bytes if huge bitmap
	errnz	bmFillBytes-bmScanSegment-2
	errnz	fill_bytes-scans_seg-2

	mov	al,bh			;Set IS_COLOR and IS_DEVICE
ifdef	GEN_COLOR_BLT
	and	al,IS_COLOR+IS_DEVICE	;  flags in the Device Flags
else
	and	al,IS_DEVICE		;  flags in the Device Flags
endif
	stosb
	errnz	dev_flags-fill_bytes-2
ifdef	GEN_COLOR_BLT
	errnz	IS_COLOR-F0_DEST_IS_COLOR ;Must be same bits
endif
	clc
	ret

copy_dev_bad_clr_format:
	stc
	ret

copy_dev	endp
