page	,132
;***************************************************************************
;									   *
;		Copyright (C) 1984-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************

TITLE	Enumerate
%out	Enum


;	Define the portions of gdidefs.inc that will be needed

incLogical	= 1			;include logical object definitions
incFont 	= 1			;include font definitions

	.xlist
	include cmacros.inc
	include gdidefs.inc
	.list


sBegin	code
assumes cs,code


	extrn	BrushColors:word	;Dithers for the solid brushes
	extrn	ColorTable:dword	;Color table
page

;	dmEnumObj - Enumerate Object
;
;	The given style of object is enumerated through a callback
;	facility.  Since there are only a few objects within this
;	particular driver, they will all be enumerated.  If there
;	are lots of objects (i.e. 1000 brushes, you may not want
;	to enumerate them all)
;
;	If the Callback function returns a zero, then the enumeration
;	will be terminated.
;
;	Entry:	per parameters
;
;	Exit:	(ax) = last value returned from callback function.
;		(ax) = 1 if nothing was enumerated.
;
;	Uses:	ax,bx,cx,dx,es,flags

cProc	dmEnumObj,<FAR,PUBLIC>,<si,di>

	parmd	lpDevice
	parmw	style
	parmd	lpCallBackFunction
	parmd	lpClientData

	localw	OldSP
	localv	ObjArea,%(SIZE LOGBRUSH)
	errnz	<(SIZE LOGBRUSH)-(SIZE LOGPEN)-2>	;Want the biggest!


cBegin	dmEnumObj

	mov	OldSP,sp		;Save SP for clean-up
	cmp	Style,OBJ_PEN		;Pen?
	je	EnumPen 		;  Yes
	cmp	Style,OBJ_BRUSH 	;Brush?
	je	EnumBrush		;  Yes
	mov	ax,1

ExitEnumObj:
	mov	sp,OldSP		;Remove any return addresses from stack

cEnd	dmEnumObj
page

;	CallClient - Call Client's Callback Routine
;
;	The client callback routine is called with the current
;	logical object.
;
;	Entry:	Object in ObjArea
;
;	Exit:	To caller if client did not bail out
;		To exit if client bailed out
;
;	Uses:	ax,bx,cx,dx,es,flags

CallClient:
	lea	ax,ObjArea
	regptr	ptr1,ss,ax
	cCall	lpCallbackFunction,<ptr1,lpClientData>,nounderscore
	or	ax,ax			;Client bailing out?
	jz	ExitEnumObj		;  Yes
	ret				;  No
page

;	EnumPen - Enumerate The Driver's Pens
;
;	The pens that the driver supports will be enumerated.
;	This driver supports five styles of pens in 8 colors.
;	The NULL Pen is not enumerated.

EnumPen:
	xor	si,si				;Initialize pen style
	mov	ObjArea.lopnWidth.xcoord,si	;Set nominal width
	mov	ObjArea.lopnWidth.ycoord,si	;Set width

EnumPen0:
	mov	ObjArea.lopnStyle,si		;Set style

EnumPen1:
	lea	di,ObjArea.lopnColor		;--> pen color
	call	DoColor 			;Step all 8 colors
	inc	si				;Set next pen style
	cmp	si,LS_NOLINE			;At maximum line style?
	jb	EnumPen0			;  No, continue

EnumPen3:
	jmp	ExitEnumObj			;Done enumerating stuff

	errnz	LS_SOLID
	errnz	LS_DASHED-1
	errnz	LS_DOTTED-2
	errnz	LS_DOTDASHED-3
	errnz	LS_DASHDOTDOT-4
	errnz	LS_NOLINE-5			;Not enumerated
	errnz	MaxLineSTyle-LS_NOLINE		;Should be no other pens
page

;	EnumBrush - Enumerate The Driver's Brushes
;
;	The brushes that the driver supports will be enumerated.
;
;	This driver supports 256K worth of ditherd brushes, so only
;	a few of them will enumerated.	These will be based on the
;	five grey scales defined in BrushColors.  All hatched brushes
;	will be enumerated.  The background color for hatched brushes
;	is not enumerated.
;
;	The Hollow Brush will not be enumerated.


EnumBrush:
	xor	ax,ax
	mov	ObjArea.lbStyle,ax		;Set brush style
	errnz	BS_SOLID
	mov	ObjArea.lbHatch,ax		;Clear hatch index
	mov	si,5*2				;Initialize red   index

EnumBrush1:
	mov	di,5*2				;Initialize green index

EnumBrush2:
	mov	bx,5*2				;Initialize blue  index

EnumBrush3:
	mov	al,byte ptr BrushColors-2[si]	;Get red color
	mov	ah,byte ptr BrushColors-2[di]	;Get green color
	mov	word ptr ObjArea.lbColor,ax	;Set red and green
	mov	al,byte ptr BrushColors-2[bx]	;Get blue color
	xor	ah,ah
	mov	word ptr ObjArea.lbColor+2,ax	;Set blue color
	push	bx				;Save blue pointer
	call	CallClient
	pop	bx				;Get back blue pointer
	mov	dx,2
	sub	bx,dx				;Out of blue?
	jnz	EnumBrush3			;  No, continue
	sub	di,dx				;Out of green?
	jnz	EnumBrush2			;  No, continue
	sub	si,dx				;Out of red?
	jnz	EnumBrush1			;  No, continue



;	Now enumerate the hatched brushes.  When enumerating hatched
;	brushes, the background color is not enumerated.

	mov	ObjArea.lbStyle,BS_HATCHED	;Set brush style
	mov	si,HS_DIAGCROSS 		;Initialize hatch index

EnumBrush4:
	mov	ObjArea.lbHatch,si		;Set style
	lea	di,ObjArea.lbColor		;--> brush color
	call	DoColor
	dec	si				;Set next hatch index
	jns	EnumBrush4			;For all hatch indexes
	jmp	EnumPen3			;Done enumerating stuff
page

;	DoColor - Call Client With All The Solid Colors
;
;	The client callback routine is called with the given object
;	with all 8 of the solid colors this driver supports.
;
;	Entry:	ss:di --> where to stuff color
;
;	Exit:	None
;
;	Uses:	ax,bx,cx,dx

DoColor proc	near

	push	si			;Save user's stuff
	mov	si,8*4-4		;Set initial color table index

DoColor1:
	mov	ax,word ptr ColorTable[si]
	mov	word ptr es:[di],ax
	mov	ax,word ptr ColorTable+2[si]
	mov	word ptr es:2[di],ax
	call	CallClient
	sub	si,4			;All colors processed yet?
	jge	DoColor1		;  No
	pop	si
	ret

DoColor endp


sEnd	code
end
