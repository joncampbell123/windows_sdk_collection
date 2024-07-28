page                    ,132
title                   Select Colour Routines
.286c

;       This routine will be passed an RGB colour in DL:AX (RED in AL,
;GREEN in AH, and BLUE in DL).  It will find the closest approximation to
;the passed colour in our colour table and pass back the RGB intensities
;in AX:DL and the LUT colour index in DH.  CL will contain the mono bit
;corresponding to the color index returned.

.xlist
include CMACROS.INC
include 8514.INC
include gdidefs.inc
.list

subttl          Data Area
page +
sBegin	Data
ife             GreyScale
externB Palette 			;in PALETTE.DAT
externB RealPalette
endif
externB WriteEnable
externB PhysicalIndexTable		;defined in palette.dat
externB ColorFlagTable
externB SystemPaletteColors

sEnd            Data

subttl          Code Area
page +
sBegin          Code
assumes         cs,Code
assumes         ds,nothing

externW 	_cstods 		;in BITBLT.ASM

cProc	ColourTranslate, <NEAR,PUBLIC>
cBegin                  
if	GreyScale
	xor	bx,bx			;get 0
	xchg	ah,bl			;into AH
	add	ax,bx			;add GREEN + RED
	xor	bx,bx			;get 0 again
	mov	dh,bl			;
	add	ax,dx			;add on BLUE
	mov	dl,bl			;clear DX for word length divide
	mov	bl,3*GREYINCR		;divide by 24
	div	bx			;now AX has our grey scale
	mov	ah,al
	mov	dx,ax
	cmp	al, BWTHRESHOLD
	jb	BypassAccBit
	or	dh, 10h 		; set bw bit on
BypassAccBit:
else
	push	ds			;save caller's DS
	mov	ds,cs:_cstods		;make DS --> Data
	assumes ds,Data

;AL has RED, AH has GREEN, DL has BLUE.

	cCall	GetIndexOfRGB
	mov	al, dh			;get the colour index to return in DH
	and	al, [WriteEnable]	;strip off accelerator bits, if any
	mov	cx, dx			;dx, will be destroyed by next call
	call	GetRGBOfIndex		;color index passed in al
	mov	dh, ch
	pop	ds			;restore caller's DS
endif					;GreyScale

CTExit:

;At this point AL contains the RED intensity that we're going to use.
;              AH contains the GREEN intensity that we're going to use.
;              DL contains the BLUE intensity that we're going to use.
;	       DH contains the colour index.
cEnd

cProc	GetIndexOfRGBFar, <FAR, PUBLIC>
cBegin	nogen
	push	ds
	mov	ds, cs:[_cstods]
	call	GetIndexOfRGB
	pop	ds
	ret
cEnd	nogen


cProc	GetIndexOfRGB, <NEAR, PUBLIC>, <si, di, cx, bx, ax>
	localW	lMinLo
	localW	wIndex
	localB	lMinHi

cBegin
	assumes cs, Code
	assumes ds, Data
	assumes es, nothing

	mov	bx, ax			; logical colors are in
	sub	cx, cx			; bl: red, bh: green, dl: blue
	mov	cl, [SystemPaletteColors]   ;CX=Size of Physical Palette
	xor	ax, ax
	dec	ax
	mov	lMinLo, ax		; initialize error term to some
	mov	lMinHi, al		; hideously large value (00ffffffh)
	mov	si, DataOFFSET Palette	;DS:SI-->Physical palette (RGB)
	mov	ax, cx			;we want to search our palette from
	shl	ax, 1			;white to black
	add	ax, cx
	dec	ax
	std				;go backwards, fetch blue, green, red
	add	si, ax			;DS:SI-->last byte of device's palette

LetsBoogy:
	sub	dh, dh			;initialize true error to 0
	lodsb				;get physical blue into AL
	sub	al, dl			;subtract red we want
	sbb	ah, ah			;make result an absolute value to
	xor	al, ah			; allow use of unsigned multiply
	sub	al, ah
	mul	al			; square differnce now
	mov	di, ax			; and save error squared in di

	lodsb				; now do the same thing with green
	sub	al, bh
	sbb	ah, ah			;make result an absolute value to
	xor	al, ah			; allow use of unsigned multiply
	sub	al, ah
	mul	al
	add	di, ax
	adc	dh, dh

	lodsb				; now compute delta R squared
	sub	al, bl
	sbb	ah, ah			;make result an absolute value to
	xor	al, ah			; allow use of unsigned multiply
	sub	al, ah
	mul	al
	add	di, ax
	adc	dh, 0
	or	di, di			; look for exact match
	jz	PossibleExactMatch
NotExactMatch:
	cmp	lMinHi, dh		; Compare current error term
	ja	SetNewlMin		; with minimal error found previously
	jb	MatchLoopBottom 	; and swap them if necessary
	cmp	lMinLo, di
	ja	SetNewlMin
MatchLoopBottom:
	loop	LetsBoogy
	jmp	short ExitCleanup
SetNewlMin:
	mov	wIndex, cx
	mov	lMinHi, dh
	mov	lMinLo, di
	loop	LetsBoogy
	jmp	short ExitCleanup
PossibleExactMatch:
	or	dh, dh
	jnz	NotExactMatch
	mov	bx, cx
	jmp	short EndItNow
ExitCleanup:
	mov	bx, wIndex
EndItNow:
	mov	ax, bx
	dec	ax
	mov	bx, DataOFFSET PhysicalIndexTable
	mov	dh, al			    ;DH=Log index
	xlatb				    ;now get the actual physical index
	xchg	dh, al			    ;AL=log index, DH=Physical index
	mov	bx, ax			    ;BX: log. index
	mov	dl, [bx].ColorFlagTable     ;DL: mono accel. bit
	cmp	[WriteEnable], 0ffh
	je	IndexOkay
	or	dh, dl			    ;set accelerator flags
IndexOkay:
	shr	dl, 4			    ;DL: mono bit in LSB
	cld				    ;reset direction flag

BuildColorTableEnd:
cEnd

cProc	ColorInfo, <FAR,PUBLIC,PASCAL,WIN>, <di>

	parmD	lpPDevice		;Pointer to physical device
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
	mov	es:[di][1], cl		;and return the best fit RGB colour
                                        ;in DL:AX
	xor	dh,dh			;make sure DH is 0
	jmp	short CIExit		;get out
CIWritePhysicalColorIndex:
	sub	cx, cx			;palette realized clrs. map --> black
	stosw
	jmp	short CIExit

CIRGBColour4Plane: 
	call	GetRGBOfIndex
	jmp	CIExit

CIReturnRGBColour:
	mov	ax,off_ColorIn		;get the color index passed into AX
	mov	cl, ah			;CL: mono flag
if	GreyScale
	mov	ah,al			;return the RGB equivlent
	mov	dl,al			;of the index passed in
else	       	                        ;not GreyScale
	cmp	 [WriteEnable],0fh
	je	CIRGBColour4Plane

	xor	ah,ah
        mov	dx,0FFFFh		;tell GDI it is a index
endif					;GreyScale

CIExit:
	les	di, lpPDevice		;ES:DI-->physical device structure
	cmp	byte ptr es:[di].bmBitsPixel, 1
	jne	CIDone			;color device. return colors in DX:AX
	shr	cl, 1			;mono device.  Use mono flag in CL to
	sbb	ax, ax			;to decide whether back or white is
	mov	dl, al			;returned in DX:AX

;At this point:
;       AL has the RED value.
;       AH has the GREEN value.
;	DL has the BLUE value.

CIDone:
cEnd

;This routine returns in DX:AX the RGB corresponding to the color index, or
;in the case of a palette realized color, the color index in AX and 0FFh in
;DH.  Note: this routine is set up such that it can work in 4 plane or in
;8 plane configuration of the 8514/A.
;Entry: AL:   color index
;	DS:   Data
;Exit:	DX:AX RGB or palette index
;Trashes:   BX

public	GetRGBOfIndex
GetRGBOfIndex	proc near
	assumes ds, Data
	mov	bl, [WriteEnable]
	sub	bh, bh
	and	al, bl
	mov	ah, [SystemPaletteColors]
	shr	ah, 1
	inc	bx			;BX: 16 or 256
	sub	bl, ah			;now bl=246 OR 8
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
	xor	dh,dh			;make sure DH is 0 in for system colors
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

sEnd            Code
end
