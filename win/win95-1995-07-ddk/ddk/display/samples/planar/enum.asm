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
; Module Name:	ENUM.ASM
;
;   This module contains routines which enumerate a subset of the
;   objects which the device can enumerate.
;
; Exported Functions:	Control
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
;   A subset of the fonts, pens, and brushes which this driver can
;   support are enumerated for the caller, until all objects of the
;   requested type have been enumerated or the caller aborts.
;
; Restrictions:
;
;-----------------------------------------------------------------------;


;

incLogical	= 1			;Include control for gdidefs.inc
incFont 	= 1			;Include control for gdidefs.inc

	.xlist
	include cmacros.inc
	include gdidefs.inc
	include macros.inc
	include	mflags.inc
	.list


	??_out	enum

	externA COLOR_TBL_SIZE		;# entries in the color table


createSeg _BLUEMOON,BlueMoonSeg,word,public,CODE
sBegin	BlueMoonSeg
assumes cs,BlueMoonSeg

	externD BlueMoonSeg_color_table ;Color table
page
;--------------------------Exported-Routine-----------------------------;
; EnumDeviceFonts
;
;   Enumerate Device Fonts is called to enumerate the fonts available
;   on a given device.	For each appropriate font, the callback function
;   is called with the information for that font.  The callback function
;   is called until there are no more fonts or the callback function
;   returns zero.
;
;   Since this driver has no fonts to enumerate, all that need be done
;   is return a success flag (1).
;
; Entry:
;	None
; Returns:
;	AX = 1
; Error Returns:
;	None
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	None
; Calls:
;	None
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,Data
assumes es,nothing


cProc	EnumDFonts,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	parmD	lp_device		;Physical device
	parmD	lp_face_name		;Face name
	parmD	lp_callback_func	;Callback function
	parmD	lp_client_data		;Data to pass the callback function

cBegin	<nogen>

	mov	ax,1			;This is a nop for this driver
	ret	16

cEnd	<nogen>
page
;--------------------------Exported-Routine-----------------------------;
; EnumObject
;
;   The given style of object is enumerated through a callback
;   facility.  Since there are only a few objects within this
;   particular driver, they will all be enumerated.
;
;   If the Callback function returns a zero, then the enumeration
;   will be terminated.
;
; Entry:
;	None
; Returns:
;	AX = last value returned from callback function.
;	AX = 1 if nothing was enumerated.
; Error Returns:
;	None
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	None
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,Data
assumes es,nothing


cProc	EnumObj,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	parmD	lp_device
	parmW	style
	parmD	lp_callback_func
	parmD	lp_client_data

	localW	old_SP
	localV	obj_area,%(size LogBrush)
	errnz	<(SIZE LogBrush)-(SIZE LogPen)>	;Want the biggest!

cBegin

	mov	old_SP,sp		;Save SP for clean-up
	cmp	style,OBJ_PEN		;Pen?
	je	enum_pen		;  Yes
	cmp	style,OBJ_BRUSH 	;Brush?
	je	enum_brush		;  Yes
	mov	ax,1

exit_enum_obj:
	mov	sp,old_SP		;Remove any return addresses from stack

cEnd
page
;--------------------------Private-Routine------------------------------;
; call_client
;
;   The client callback routine is called with the current
;   logical object in obj_area.
;
; Entry:
;	object in obj_area
; Returns:
;	AX = non-zero
; Error Returns:
;	jumps to exit_enum_obj if user aborts enumeration
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	[lp_callback_func] - user supplied callback procedure
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing


call_client	proc	near

	lea	ax,obj_area
	farPtr	ptr1,ss,ax
	cCall	lp_callback_func,<ptr1,lp_client_data>
	or	ax,ax
	jz	exit_enum_obj		;Client is bailing out
	ret

call_client	endp
page
;--------------------------Private-Routine------------------------------;
; enum_pen
;
;   The pens which this driver can realize are enumerated for
;   the caller until either all pens have been enumerated or
;   the user aborts.
;
;
;   This driver supports five styles of pens, in the number of
;   colors appropriate for the driver.
;
;   The NULL Pen is not enumerated.
;
; Entry:
;	None
; Returns:
;	AX = non-zero
; Error Returns:
;	jumps to exit_enum_obj if user's aborts enumeration
; Registers Preserved:
;	DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,ES,FLAGS
; Calls:
;	do_color
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing


enum_pen proc	near

	xor	si,si				;Initialize pen style
	mov	obj_area.lopnWidth.xcoord,si	;Set nominal width
	mov	obj_area.lopnWidth.ycoord,si	;Set width

enum_pen_10:
	mov	obj_area.lopnStyle,si		;Set style

enum_pen_20:
	lea	di,obj_area.lopnColor		;--> pen color
	call	do_color			;Step all 8 colors
	inc	si				;Set next pen style
	cmp	si,LS_NOLINE			;At maximum line style?
	jb	enum_pen_10			;  No, continue

enum_pen_30:
	jmp	exit_enum_obj			;Done enumerating stuff

	errnz	LS_SOLID
	errnz	LS_DASHED-1
	errnz	LS_DOTTED-2
	errnz	LS_DOTDASHED-3
	errnz	LS_DASHDOTDOT-4
	errnz	LS_NOLINE-5			;Not enumerated
	errnz	MaxLineStyle-LS_NOLINE		;Should be no other pens

enum_pen endp
page
;--------------------------Private-Routine------------------------------;
; enum_brush
;
;   The brushes that the driver supports will be enumerated.
;
;   This driver supports 256K worth of ditherd brushes, so only
;   a few of them will enumerated.  These will be based on the
;   five grey scales defined in brush_colors.  All hatched brushes
;   will be enumerated.  The background color for hatched brushes
;   is not enumerated.
;
;   The Hollow Brush will not be enumerated.
;
; Entry:
;	None
; Returns:
;	AX = non-zero
; Error Returns:
;	jumps to exit_enum_obj if user's aborts enumeration
; Registers Preserved:
;	DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,ES,FLAGS
; Calls:
;	do_color
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


;	The Logical colors that will be returned for the enumeration
;	of the solid brushes


brush_colors label byte

	db	000h			;Black brush color
	db	040h			;DkGrey brush color
	db	080h			;Grey brush color
	db	0C0h			;LtGrey brush color
	db	0FFh			;White brush color

NUMBER_GREYS	=	$-brush_colors	;Number of greys to enumerate


assumes ds,nothing
assumes es,nothing


enum_brush proc near

	xor	ax,ax
	mov	obj_area.lbStyle,ax		;Set brush style
	errnz	BS_SOLID
	mov	obj_area.lbHatch,ax		;Clear hatch index
	mov	si,NUMBER_GREYS 		;Initialize red   index

enum_brush_10:
	mov	di,NUMBER_GREYS 		;Initialize green index

enum_brush_20:
	mov	bx,NUMBER_GREYS 		;Initialize blue  index

enum_brush_30:
	xor	ax,ax
	mov	al,brush_colors[bx][-1] 	;Get blue color
	mov	wptr obj_area.lbColor[2],ax	;Set blue color
	mov	al,brush_colors[si][-1] 	;Get red color
	mov	ah,brush_colors[di][-1] 	;Get green color
	mov	wptr obj_area.lbColor[0],ax	;Set red and green
	push	bx				;Save blue pointer
	call	call_client
	pop	bx				;Get back blue pointer
	dec	bx				;Out of blue?
	jnz	enum_brush_30			;  No, continue
	dec	di				;Out of green?
	jnz	enum_brush_20			;  No, continue
	dec	si				;Out of red?
	jnz	enum_brush_10			;  No, continue



;	Now enumerate the hatched brushes.  When enumerating hatched
;	brushes, the background color is not enumerated.

	mov	obj_area.lbStyle,2		;Set brush style
	errnz	BS_HATCHED-2			
	mov	si,HS_DIAGCROSS 		;Initialize hatch index

enum_brush_40:
	mov	obj_area.lbHatch,si		;Set style
	lea	di,obj_area.lbColor		;--> brush color
	call	do_color
	dec	si				;Set next hatch index
	jns	enum_brush_40			;For all hatch indexes
	jmp	enum_pen_30			;Done enumerating stuff

enum_brush endp
page
;--------------------------Private-Routine------------------------------;
; do_color
;
;   The client callback routine is called with the given object
;   with all 8 of the solid colors this driver supports.
;
; Entry:
;	SS:DI --> where to stuff color
; Returns:
;	AX = non-zero
; Error Returns:
;	Handled by call_client
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	call_client
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing


do_color proc	near

	push	si			;Save user's stuff
	mov	si,COLOR_TBL_SIZE	;Set initial color table index
	shiftl	si,2			;Indexing into dwords

do_color_10:
	cld
	mov	ax,wptr BlueMoonSeg_color_table[si][-4]
	mov	wptr ss:[di],ax
	mov	ax,wptr BlueMoonSeg_color_table[si][-2]
	mov	wptr ss:[di][2],ax
	call	call_client
	sub	si,4			;All colors processed yet?
	jg	do_color_10		;  No
	pop	si
	ret

do_color endp


sEnd	BlueMoonSeg
if	MASMFLAGS and PUBDEFS
	public	exit_enum_obj
	public	call_client
	public	enum_pen
	public	enum_pen_10
	public	enum_pen_20
	public	enum_pen_30
	public	enum_brush
	public	enum_brush_10
	public	enum_brush_20
	public	enum_brush_30
	public	enum_brush_40
	public	do_color
	public	do_color_10
endif
end
