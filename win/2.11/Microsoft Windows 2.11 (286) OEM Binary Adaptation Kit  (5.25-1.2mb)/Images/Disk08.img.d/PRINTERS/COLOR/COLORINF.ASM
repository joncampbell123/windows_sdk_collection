page	,132
;***************************************************************************
;									   *
;		Copyright (C) 1984-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************

TITLE	ColorInf - Device Color Routine
%out	ColorInf



;	061585	WM	Wasn't checking the PDevice when returning the
;			closest logical color.	Must return black/white
;			for monochrome bitmaps



	.xlist
	include cmacros.inc
	include gdidefs.inc
	include color.inc
	.list


sBegin	code
assumes cs,code


	extrn	SumRGBColorsAlt:near
page

;	dmColorInfo - Return Color Information
;
;	dmColorInfo accepts a logical RGB color value and returns the
;	logical RGB color value that the device can most closely represent.
;	dmColorInfo also returns the device dependent, physical representation
;	of bits necessary to display the specified color on the device.  This
;	information will be passed back into the driver by GDI.  GDI will
;	do no interpreting of the physical color.
;
;	dmColorinfo may also be requested to convert a physical color into
;	a logical RGB color.  This is indicated by a NULL lpColor.  Since
;	this driver maintains logical and physical colors as one in the
;	same (i.e. logical white is xxFFFFFFH and physical white is
;	xxFFFFFFH), we can just pass the physical color back as the
;	logical color!	GDI will only call us to convert our own physical
;	color that was returned by this driver, so this is safe.
;
;	The lpDevice may be a pointer to our device or to a memory bitmap.
;	If it is to a monochrome memory bitmap, then the color must be
;	interpreted as black/white, else it must be interpreted as one of
;	the RGB colors supported in color mode.
;
;
;	Entry:	per parameters
;
;	Exit:	dx:ax = logical color
;
;	Uses:	ax,bx,dx,es,flags


cProc	dmColorInfo,<FAR,PUBLIC>,<di>

	parmd	lpDevice		;Pointer to device
	parmd	ColorIn 		;Input color (RGB or physical)
	parmd	lpPhysBits		;Pointer to physical bit field


cBegin	dmColorInfo

	mov	ax,OFF_ColorIn		;Set al = Red,	ah = Green
	mov	dx,SEG_ColorIn		;Set dl = Blue, dh = Mono

	mov	bx,OFF_lpPhysBits	;Save the color or return logical?
	or	bx,SEG_lpPhysBits
	jz	ColorInfo1		;Return logical color

	call	sumRGBColorsAlt 	;Get the colors to use
	les	di,lpPhysBits		;Store physical color here
	cld
	stosw
	mov	word ptr es:[di],dx
;	jmp	short ColorInfo2					;061585

	errnz	Red			;Colors must be in this order
	errnz	Green-1
	errnz	Blue-2
	errnz	Mono-3



;	The request was to convert a physical color into a logical
;	color.	If the lpDevice is to a monochrome memory bitmap,
;	then the color must be interpreted as black\white (via
;	the mono bit)


ColorInfo1:
	les	di,lpDevice		;See if color
	cmp	es:bmPlanes[di],3
	je	ColorInfo2		;It is color, use the triplet

	mov	al,dh			;Get mono portion of the color
	shl	al,1			;  and turn into b/w
	errnz	MonoBit-01000000b	;  by sign extending it
	cbw				;  and rearranging things
	cwd
	mov	ax,dx			;al=red, ah=green, dl=blue

ColorInfo2:
	xor	dh,dh			;Clear out any monochrome data


cEnd	dmColorInfo

sEnd	code
end
