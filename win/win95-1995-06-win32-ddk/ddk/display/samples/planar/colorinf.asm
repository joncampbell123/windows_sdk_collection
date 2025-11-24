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
; Module Name:	COLORINF.ASM
;
; This module contains the color information routine.
;
; Exported Functions:	Control
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
;   ColorInfo is called by GDI to either convert a logical color
;   (an RGB triplet) to a physical color, or a phsyical color to
;   a logical color.
;
; Restrictions:
;
;-----------------------------------------------------------------------;


	.xlist
	include cmacros.inc
	include gdidefs.inc
	include display.inc
	include macros.inc
	include	mflags.inc
	.list


	??_out	colorinf


	ifdef	GEN_COLOR
	externA COLOR_FORMAT		;Color format (0103h or 0104h)
	endif


sBegin	Code
assumes cs,Code


	externNP sum_RGB_colors_alt
page

;--------------------------Exported-Routine-----------------------------;
;
; ColorInfo
;
;   ColorInfo accepts a logical RGB color value and returns the
;   logical RGB color value that the device can most closely represent.
;   ColorInfo also returns the device dependent, physical representation
;   of bits necessary to display the specified color on the device.
;   This information will be passed back into the driver by GDI.  GDI
;   will do no interpreting of the physical color.
;
;   Colorinfo may also be requested to convert a physical color into
;   a logical RGB color.  This is indicated by a NULL lp_phys_bits.
;   Since this driver maintains logical and physical colors as one
;   in the same (i.e. logical white is xxFFFFFFH and physical white
;   is xxFFFFFFH), we can just pass the physical color back as the
;   logical color!  GDI will only call us to convert our own physical
;   color that was returned by this driver, so this is safe.
;
;   The lpDevice may be a pointer to our device or to a memory bitmap.
;   If it is to a monochrome memory bitmap, then the color must be
;   interpreted as black/white, else it must be interpreted as one of
;   the RGB colors supported in color mode.
;
; Entry:
;	None
; Returns:
;	DX:AX = physical color or logical color as appropriate
; Error Returns:
;	None
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	sum_RGB_colors_alt
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,Data
assumes es,nothing


cProc	ColorInfo,<FAR,PUBLIC,WIN,PASCAL>,<di>

	parmD	lp_device		;Pointer to device
	parmD	color_in		;Input color (RGB or physical)
	parmD	lp_phys_bits		;Pointer to physical bit field

cBegin

	mov	ax,off_color_in 	;Set AL = Red,	AH = Green
	mov	dx,seg_color_in 	;Set DL = Blue, DH = Special

	les	di,lp_phys_bits 	;Store physical color here if not NULL
	assumes es,nothing

	mov	bx,es			;Save the color or return logical?
	or	bx,di
	jz	color_info_10		;Return logical color

	call	sum_RGB_colors_alt	;Get the colors to use
	cld
	stosw				;Store C0, C1
	mov	wptr es:[di],dx 	;Store C2, Special

	errnz	pcol_C0 		;This ordering is assumed
	errnz	pcol_C1-pcol_C0-1
	errnz	pcol_C2-pcol_C1-1
	errnz	pcol_C3-pcol_C2-1


;	Fall through and convert the physical color into black/white
;	if the PDevice is to a b/w bitmap.  If the PDevice is to a
;	color device, this is not necessary.


;	The request was to convert a physical color into a logical
;	color.	If the lpDevice is to a monochrome memory bitmap,
;	then the color must be interpreted as black\white (via
;	the mono bit)


color_info_10:

	ifdef	GEN_COLOR
	lds	di,lp_device		;--> physical device
	assumes ds,nothing

	cmp	wptr [di].bmPlanes,COLOR_FORMAT
	je	color_info_20		;Our color format, physical = logical
	errnz	bmBitsPixel-bmPlanes-1
	endif

	shiftl	dh,4			;Set 'C' if white
	errnz	MONO_BIT-00010000b
	sbb	ax,ax			;AX = 0000 if black, FFFF if white
	mov	dx,ax

color_info_20:
	xor	dh,dh			;Clear out any monochrome data


cEnd

sEnd	Code
if 	MASMFLAGS and PUBDEFS
	public	color_info_10
	public	color_info_20
endif
end

