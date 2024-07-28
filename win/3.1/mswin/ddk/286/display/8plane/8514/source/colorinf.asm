page            ,132
title           Colour Information Functions
.286c


.xlist
include         CMACROS.INC
include         8514.INC
.list

RPALETTE	equ	02ebh
DPALETTE	equ	02edh

LOWEREIGHT	equ	8
UPPEREIGHT	equ	248


sBegin          Data
ife             GreyScale
externB Palette 			;in PALETTE.DAT
endif
externB BitsPixel
externB WriteEnable			;in DATA.ASM
externB SystemColorTranslateTable
externB SystemPaletteColors
sEnd            Data


subttl          Code Area
page +
sBegin  Code
assumes cs,Code
assumes ds,Data

externNP    ColourTranslate		;in COLOUR.ASM


cProc	ColorInfo, <FAR,PUBLIC,PASCAL,WIN>, <di>

        parmD   lpDevice                ;Pointer to device
        parmD   ColorIn                 ;Input color (RGB or physical)
        parmD   lpPColour               ;Pointer to physical colour index

cBegin               
	cmp	seg_lpPColour,0 	;are we to return the RGB colour for
					;the index passed?
	je	CIReturnRGBColour	;yes! return RGB color for index passed

CIReturnPhysicalColour:
	les	di,lpPColour		;now ES:DI points to PCOLOR data
					;structure
	mov	ax, off_ColorIn 	;get passed RGB color into DX:AX
	mov	dx, seg_ColorIn 	;now AL has RED, AH has GREEN,
					;    DL has BLUE of color that GDI
					;wants to match up to.
	or	dh, dh
	js	CIWritePhysicalColorIndex
	cCall	ColourTranslate 	;now AL has RED, AH has GREEN
					;    DL has BLUE of best fit color
					;DH has index for this color
	mov	es:[di],dh		;copy physical color into structure
                                        ;and return the best fit RGB colour
                                        ;in DL:AX
	xor	dh,dh			;make sure DH is 0
	jmp	short CIExit		;get out
CIWritePhysicalColorIndex:
	stosb
	xor	dh,dh			;make sure DH is 0
	jmp	short CIExit

CIReturnRGBColour:
	mov	ax,off_ColorIn		;get the color index passed into AX
	sub	ah, ah
if	GreyScale
	mov	ah,al			;return the RGB equivlent
	mov	dl,al			;of the index passed in
else					;not GreyScale
	call	GetRGBOfIndex		;ax=color index

;	 mov	 si,DataOFFSET PALETTE	 ;now DS:SI points to base of table
;	 mov	 cx,ax			 ;for multiply by 3 operation
;	 shl	 ax,1			 ;multiply by 2
;	 add	 ax,cx			 ;multiply by 3
;	 add	 si,ax			 ;now DS:SI points to entry for our
;					 ;colour
;	 lodsw				 ;get R into AL, G into AH
;	 mov	 dl,[si]		 ;get B into DL
endif					;GreyScale

CIExit:             

;At this point:
;       AL has the RED value.
;       AH has the GREEN value.
;       DL has the BLUE value.

cEnd

public	GetRGBOfIndexFar
GetRGBOfIndexFar    proc far
	call	GetRGBOfIndex
	ret
GetRGBOfIndexFar    endp

public	GetRGBOfIndex
GetRGBOfIndex	proc near
	assumes ds, Data
	and	al, [WriteEnable]
	mov	cl, [BitsPixel]
	mov	ah, [SystemPaletteColors]
	shr	ah, 1
	mov	bx, 01h
	shl	bx, cl
	sub	bl, ah			;now bl=UPPEREIGHT (248 OR 8)
	cmp	al, ah			;if we are dealing with a system color
	jb	GetSystemColorRGB	;then we need to translate it such that
	cmp	al, bl			;the physical color maps to its logical
	jb	GetRealPhysicalColor	;color.

	sub	bl, ah
	sub	al, bl
GetSystemColorRGB:
	sub	ah, ah
	mov	bx, DataOFFSET Palette
	add	bx, ax
	shl	ax, 1
	add	bx, ax
	mov	ax, [bx]
	mov	dl, [bx+2]
	xor	dh,dh			;make sure DH is 0
	ret

GetRealPhysicalColor:			;AL: physical color index

;At this point it is certain that we are running in 8 plane mode.  The RGB of
;one of the colors reserved for the palette manager is requested.  However,
;what is in the DAC now may not be up to date or, in case of memory DCs, not
;appropriate at all.  So, return 0FF0000IIh in DX:AX to indicate to GDI that
;it has to find the actual RGB itself.

	sub	ah, ah			;clear high byte of AX
	cwd				;now DX=0
	dec	dh			;Now DX:AX=0FF0000IIh
	ret				;we're all done now.
GetRGBOfIndex	endp

sEnd    Code
end
