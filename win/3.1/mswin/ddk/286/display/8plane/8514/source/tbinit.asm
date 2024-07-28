;*********************************Module*Header********************************
;This module contains the routines for palette manipulations initiated by GDI
;as well as the Brush modification routines.  Color values are used as an index
;into a color table (a LUT) and the color value thus obtained is used in the
;brush whenever it is used by BitBlt (for now)
;Created:	11-04-88
;By		GunterZ     (Gunter Zieber)
;History:	11-05-88    Got Hook into BitBlt working, verified concept
;		11-07-88    Got Hook into StrBlt, TranslateDrawMode works!!!!
;			    Hooks in output translate Brush, Pen and DrawMode
;******************************************************************************

.286
.xlist
include cmacros.inc
incDrawMode	equ	1		; enable drawing mode definitions
include gdidefs.inc
include drvpal.inc			; Brush structures and misc defintions
include 8514port.inc			; Port definitions used in this module
include 8514.inc
include brush.inc
.list

TABLESIZE	    equ     256
READCOMMAND	    equ     (FILL_X_RECT or BYTE_SWAP or PC_TRANS+BIT16 or STROKE_ALG or INCX or INCY or YMAJAXIS or DRAWCMD)
WRITECOMMAND	    equ     (READCOMMAND or WRITCMD)
FORGROUNDMIX	    equ     (PTRANS_ACTIVE or SRC)
BACKGROUNDMIX	    equ     (PTRANS_ACTIVE OR SRC)
DEFAULTMIX	    equ     (F_CLR_ACTIVE or SRC)
ONE_WORD	    equ     080h
NUMCOLORS	    equ     256

sBegin	Data

externB TranslateTable
externB ITranslateTable
externB BrushCopy
externB PaletteFlags
externB NewDrawMode
externB PenCopy
externB BigFontInfoTable
externB ShadowMemoryTrashed
externB HardwarePaletteRead		;in DATA.ASM
externB SystemPaletteColors
externB RealPalette

sEnd	Data

createSeg	_INIT,InitSeg,word public,CODE
sBegin	InitSeg
assumes cs,InitSeg
assumes ds,Data

cProc	LUTInit, <NEAR, PUBLIC>, <di>

cBegin
	sub	ax, ax
	push	bx
	push	di
	push	es
	push	ds
	pop	es
	lea	di, ITranslateTable
	lea	bx, TranslateTable
InitLoop:
	mov	[bx], al		; initialize Index LUT as identity LUT
	stosb
	inc	bx
	inc	ax
	cmp	ax, 256
	jb	InitLoop
	pop	es
	pop	di
	pop	bx
cEnd
sEnd	InitSeg

sBegin	Code
assumes cs, Code
assumes ds, Data

;*********************************Public*Routine*******************************
;Translate pen makes a copy of the pen currently used and returns a long
;pointer to the translated pen in dx:ax.  The current translate table is used
;for the color conversion.
;******************************************************************************

cProc	TranslatePen, <FAR, PUBLIC>, <es, si, di, bx>
	parmD	lpPen
cBegin
	mov	dx, seg_lpPen
	mov	ax, off_lpPen
	or	ax, dx
	jz	TranslatePenEnd
	lea	bx, TranslateTable
	les	si, lpPen
	lods	word ptr es:[si]	;AL: pen style, AH: pen color
	mov	cl, es:[si]		;CL: pen color's mono bit in LSB
	lea	di, PenCopy		;DS:SI-->internal copy of pen struc
	mov	byte ptr [di], al	;don't change the Style
	mov	al, ah
	xlatb
	mov	[di][1], al		;translate the color
	mov	[di][2], cl		;mono bit is 0 if palette realized col.
	mov	dx, ds			; return a long pointer to the copy of
	mov	ax, di			; the modified pen we just made
TranslatePenEnd:
cEnd

;*********************************Public*Routine*******************************
;Tranlate DrawMode will pass a pointer to a DrawMode structure to the calling
;routine in dx:ax.  The forground and background color index are translated
;using the current translate table.  A long pointer to the DrawMode structure
;(as received from GDI) is passed as parameter on the stack.
;******************************************************************************

cProc	TranslateDrawMode, <FAR, PUBLIC, WIN, PASCAL>, <es, si, di, cx, bx>
	parmD	lpDrawMode
cBegin
	mov	dx, seg_lpDrawMode
	mov	ax, off_lpDrawMode
	or	ax, dx
	jz	TranslateDrawModeEnd
	push	ds
	pop	es			; es: Data
	lds	si, lpDrawMode
	mov	cx, size DRAWMODE
	shr	cx, 1
	lea	di, NewDrawMode
	push	di
rep	movsw				; copy all of the drawmode structure
	push	es
	pop	ds
	pop	si			; ds:si->NewDrawMode
	lea	bx, TranslateTable
	mov	al, byte ptr [si].bkColor
	xlatb				; translate background color
	mov	byte ptr [si].bkColor, al
	mov	al, byte ptr [si].TextColor
	xlatb				; translate foreground Color
	mov	byte ptr [si].TextColor, al
	mov	dx, ds
	mov	ax, si
TranslateDrawModeEnd:
cEnd

;*********************************Public*Routine*******************************
;Translate Brush expects a long pointer to the physical brush and returns
;a pointer to the translated brush back in dx:bx, and the translated BgColor in
;ah to be consistent with ConvertBrush (in Brush.asm)
;This routine will handle solid as well as patterned brushes.
;This call has to precede the call to ConvertBrush (called by
;HiresBLTProcessing in 8514.blt)
;******************************************************************************

cProc	TranslateBrush, <FAR, PUBLIC>, <es, si, di, cx, bx>
	parmD	lpBrush
cBegin
	assumes cs, Code
	assumes ds, Data
	assumes es, nothing

	mov	dx, seg_lpBrush
	mov	ax, off_lpBrush
	or	ax, dx
	jz	TranslateBrushError
	lea	di, BrushCopy		; es:di points to bcStyle
	push	ds			;make es=ds=Data
	pop	es
	assumes es, Data
	sub	ch, ch			; ch=0
	lds	si, lpBrush		; now ds:si->physical brush
	assumes ds, nothing
	mov	dl, [si].bnStyle	; keep brush style in dl
	and	dl, 0fh 		; look only at lower nibble
	test	dl, 02h 		; exit in case of monochrome pattern br
	jnz	SetupPointers		; note: ds:si points to the PBrush
	lea	bx, TranslateTable
	mov	cl, size NewBrush	; copy the entire brush
rep	movsb				; es:di -> local copy
	push	es
	pop	ds
	assumes ds, Data
	lea	si, BrushCopy		; now ds:si->BrushCopy
	cmp	dl, 05h 		; is it a dithered brush?
	je	TranslateDitherPattern	; if yes, go translate the pattern
	cmp	dl, 01h 		; 0->solid br. 1->hollow br.
	ja	TryColorPattern
	mov	al, [si].bnColor	; we're dealing with a solid or hollow
	xlatb				; put the new color value into al
	mov	[si].bnColor, al	; store in private copy of that brush
	jmp	Short SetupPointers	; we're done now!

TranslateBrushError:
	sub	ax, ax
	cwd
	jmp	short TranslateBrushEnd
public	TranslateDitherPattern
TranslateDitherPattern:

; at this point: es=ds=Data, si=offset brush copy, bx=offset Translate table

	push	si
	inc	si			; first we want to translate the
	mov	di, si			; closest pure color
	.errnz	bnColor-bnStyle-1
	lodsb
	xlatb
	stosb
	add	si, (bnColorBits-2)	;now translate the pattern bitmap
	mov	di, si
	mov	cx, (PSIZE SHR 1)
TranslateDitherPatternLoop:
	lodsb
	xlatb
	stosb
	loop	TranslateDitherPatternLoop
	pop	si			; get pointer to brush copy in DS:SI
	jmp	short SetupPointers

TryColorPattern:
	test	dl, 04h 		; 4->colored patterned brush
	jnz	DoColorPattern		; don't deal with something weird
	lds	si, lpBrush
	jmp	short SetupPointers
public	DoColorPattern
DoColorPattern:
	mov	al, [si].bnBgColor	; get the background color and xlate it
	xlatb
	mov	[si].bnBgColor, al	; store in our copy of brush
	mov	al, [si].bnFgColor	; do the same with foreground color
	xlatb
	mov	[si].bnFgColor, al
SetupPointers:
	mov	dx, ds			; return a pointer to our copy of the
	mov	ax, si			; initial brush. dx->seg, ax->off
TranslateBrushEnd:
cEnd

sEnd	Code

externFP    CursorExclude		; in ROUTINES.ASM
externFP    CursorUnExclude		; in ROUTINES.ASM

createSeg   _DEIGHT, DynamicEight, word, public, CODE
sBegin	DynamicEight

cProc	UpdateColors, <FAR, PUBLIC>, <si, di>
	parmW	wStartX
	parmW	wStartY
	parmW	wExtX
	parmW	wExtY
	parmD	lpTranslate

	localW	wToDoX
	localB	bFlags
	localW	nLineCounter
	localV	pbXlateTable, TABLESIZE
cBegin
	assumes cs, DynamicEight
	assumes ds, Data
	assumes es, nothing

	and	[ShadowMemoryTrashed], 0fdh
	mov	ax, wStartX
	mov	bx, wStartY
	push	ax
	push	bx
	add	ax, wExtX
	add	bx, wExtY
	push	ax
	push	bx
	cCall	<far ptr CursorExclude>
	push	ss
	pop	es
	mov	cx, TABLESIZE
	mov	dx, ds			;DX=Data
	lds	si, lpTranslate 	; we're passed an array of words, but
	assumes ds, nothing
	lea	di, pbXlateTable	; in order to use xlat we need an
	mov	bx, di
WordToByteLoop: 			; array of bytes.
	lodsw				; do in place word to byte conversion
	stosb
	loop	WordToByteLoop
	mov	ds, dx
	assumes ds, Data
	mov	es, dx
	assumes es, Data
	mov	dx, STATUS_REG
WaitQueueEmpty: 			; before we start writing to the 8514
	in	ax, dx			; we want to make sure that the input
	or	al, al			; queue is empty, so that we don't need
	jnz	WaitQueueEmpty		; to check each time we write anything
	dec	al
	mov	dx, RD_MASK_ENABLE
	out	dx, al
	mov	dx, WR_MASK_ENABLE
	out	dx, al
WaitReadQueueEmpty:
	mov	dx, STATUS_REG
	in	ax, dx
	and	ah, DATA_AVAIL
	jz	ReadQueueIsEmpty	; if there are still any data in the
	mov	dx, PIX_TRANS_REG	; 8514's output queue, get rid of them
	in	ax, dx			; now
	jmp	short WaitReadQueueEmpty

YLoopDoneNode:
	jmp	YLoopDone

ReadQueueIsEmpty:
	mov	dx, FRGD_MIX_REG	; set the forground mix register to
	mov	ax, FORGROUNDMIX	; source-copy
	out	dx, ax
	mov	dx, BKGD_MIX_REG
	mov	ax, BACKGROUNDMIX
	out	dx, ax
	mov	ax, wStartY
	mov	cx, wExtY		; cx contains number of lines to do
	mov	nLineCounter, cx
	add	ax, cx
	mov	wExtY, ax		; now: wExtY = wEndY
	mov	ax, wStartY
YLoop:
	cmp	ax, wExtY		; check whether we're done
	jge	YLoopDoneNode
	sub	si, si
	mov	cx, wExtX
	mov	ax, wStartX
	push	ax			; save this number for write command
	mov	dx, CUR_X_POS
	out	dx, ax
	mov	ax, wStartY		; get the Y pos and set the register
	mov	dx, CUR_Y_POS
	out	dx, ax
	mov	ax, cx			; now get X-ext minus one
	dec	ax			; and set the register
	mov	dx, MAJ_AXIS_PCNT
	out	dx, ax
	sub	ax, ax			; do one line at a time, i.e., Y-ext
	mov	dx, MISC_REG		; minus one is equal zero
	out	dx, ax
	mov	ax, READCOMMAND 	; now issue read command to 8514 and
	mov	dx, COMMAND_REG 	; get pixels
	out	dx, ax			; we'll read two pixels at a time
	shr	cx, 1			; odd # of pixels ?
	adc	si, si
	mov	nLineCounter, cx	; save for write command
	mov	di, DataOFFSET BigFontInfoTable
	mov	dx, STATUS_REG
WaitForData:
	in	ax, dx
	and	ah, DATA_AVAIL
	jz	WaitForData

	mov	dx, PIX_TRANS_REG
	jcxz	CheckEvenPixel1
GetPixelLoop:
	in	ax, dx			; get two pixels
	xlatb	ss:[bx] 		; translate the one in al
	xchg	al, ah
	xlatb	ss:[bx]
	xchg	al, ah
	stosw
	loop	GetPixelLoop

CheckEvenPixel1:
	or	si, si			; check if odd or even pixel count
	jz	EvenPixelCount1
	in	ax, dx
	xlatb	ss:[bx]
	stosb

EvenPixelCount1:
	mov	di, si			; now di is our odd/even flag
	mov	dx, CUR_X_POS		; re-initialize start x position
	pop	ax
	out	dx, ax
	mov	dx, CUR_Y_POS		; re-initialize start Y position
	mov	ax, wStartY
	out	dx, ax
	mov	dx, COMMAND_REG 	; now issue the write command
	mov	ax, WRITECOMMAND
	out	dx, ax
	mov	cx, nLineCounter	; get the number of words to write
	mov	si, DataOFFSET BigFontInfoTable
	mov	dx, STATUS_REG	       ; for now, there will be no checking
SetPixelLoop:
	in	ax, dx		       ; whether there is any space in the
	and	al, ONE_WORD	       ; output FIFO or not
	jnz	SetPixelLoop
	mov	dx, PIX_TRANS_REG
	jcxz	CheckEvenPixel2
rep	outsw

CheckEvenPixel2:
	or	di, di
	jz	EvenPixelCount2
	lodsb
	out	dx, ax
EvenPixelCount2:

	mov	ax, wStartY
	inc	ax
	mov	wStartY, ax
	jmp	YLoop
YLoopDone:
	mov	dx, FRGD_MIX_REG	; set the forground mix register to
	mov	ax, DEFAULTMIX		; default mix
	out	dx, ax
	mov	dx, BKGD_MIX_REG	; do the same with background mix
	out	dx, ax			; register
	cCall	<far ptr CursorUnExclude>
cEnd

;*********************************Public*Routine*******************************
;This routine is used to pass the current translate table from GDI to the
;driver.  At this point flags are set that the translate table has changed.
;******************************************************************************

cProc	SetPalTrans, <FAR, PUBLIC, WIN, PASCAL>, <si, di>
;	 parmW	 nNumber		 ; number of entries to make--start at 0
	parmD	lpTransTable		; array of indices (in WORDS)
cBegin
	assumes cs, DynamicEight
	assumes ds, Data
	assumes es, nothing

	lea	di, TranslateTable	; we need to put the indices into the
	mov	ax, SEG_lpTransTable	; flag table
	mov	si, OFF_lpTransTable
	mov	cx, si
	or	cx, ax
	jz	SetPalTransIdentity	; make identity LUT if null pointer
	push	ds			; save ds
	push	ds
	pop	es			; now es: Data
	mov	ds, ax
	mov	cx, NUMCOLORS		; pointers and count are set up
;	 push	 cx
SetPalTransferLoop:
	lodsw				; fetch index (al) and flags (ah)
	stosb				; store the new index
;	 mov	 es:[bx], ah		 ; save the flags
	inc	bx
	loop	SetPalTransferLoop
;	 pop	 cx
	mov	cx, NUMCOLORS
	pop	ds			; restore ds
	lea	si, TranslateTable
	push	ds
	pop	es
	lea	di, ITranslateTable
	push	di
	push	cx
	sub	al, al			; make all entries in inverse color
rep	stosb				; table initially black (0)
	pop	cx
	pop	di
	sub	bh, bh
	sub	dx, dx			; dx=index
MakeInverseTableLoop:
	lodsb
	mov	bl, al
;	 mov	 al, dl
	mov	byte ptr [di+bx], dl
	inc	dx
	loop	MakeInverseTableLoop
	lea	si, PaletteFlags
	or	byte ptr [si], BITBLTACCELERATE
	jmp	short SetPalTransEnd	; indicate that we need to use tables

SetPalTransIdentity:			; make an identity LUT
	mov	ax, ds
	mov	es, ax
	sub	ax, ax
	mov	cx, NUMCOLORS
IDTYXferLoop:
	stosb
;	 mov	 [bx], ah
;	 inc	 bx
	inc	ax
;	 cmp	 ax, 256
	loop	IDTYXferLoop
	lea	si, PaletteFlags
	and	byte ptr [si], (not BITBLTACCELERATE)
					; no color Xlate needed while
SetPalTransEnd: 			; BLTing to or from the screen
	lea	si, BrushCopy		; make sure to set the flag that the
	mov	al, [si].bcFlags	; index table has changed
	or	al, NEWINDEX
	mov	[si].bcFlags, al
cEnd

assumes cs, DynamicEight
assumes ds, Data
assumes es, nothing

;*********************************Public*Routine*******************************
;This routine is used to pass the current translate table to GDI.
;******************************************************************************

cProc	GetPalTrans, <FAR, PUBLIC, WIN, PASCAL>, <si, di>
	parmD	lpTransTable		; indices in WORDS
cBegin
	assumes ds, Data
	assumes cs, DynamicEight
	cmp	seg_lpTranslate, 0
	je	GetPalTransEnd		; don't do anything with a null pointer
	lea	si, TranslateTable	; set all the pointers and then copy
	les	di, lpTransTable
	mov	es, ax
	mov	cx, NUMCOLORS
	sub	ah, ah
	push	di
GetPalTransferLoop:
	lodsb				; index into al
	stosw
	loop	GetPalTransferLoop
GetPalTransEnd:
cEnd

;*********************************Public*Routine*******************************
;GDI calls this routine to set the physical colors in the hardware palette of
;the 8514.
;******************************************************************************

cProc	SetPalette, <FAR, PUBLIC, WIN, PASCAL>, <si>
	parmW	nStartIndex		; this routine expects the palette in
	parmW	nNumEntries		; densly packed rgb format: first byte
	parmD	lpPalette		; red, then green, then blue
cBegin
	assumes cs, DynamicEight
	assumes ds, nothing
	assumes es, nothing

	cmp	seg_lpPalette, 0
	jz	EndSetPalette		; exit in case of null pointer

	lds	si, lpPalette
	mov	cx, nStartIndex
	mov	bx, nNumEntries
	add	bx, cx
SetPaletteLoop:
	mov	dx, WPALETTE
	mov	al, cl
	out	dx, al
	mov	dx, DPALETTE
	lodsb
	shr	al, 1
	shr	al, 1
	out	dx, al
	lodsb
	shr	al, 1
	shr	al, 1
	out	dx, al
	lodsb
	shr	al, 1
	shr	al, 1
	out	dx, al
	inc	si			; ignore the flags
	inc	cx
	cmp	cx, bx
	jb	SetPaletteLoop
EndSetPalette:
cEnd

;*********************************Public*Routine*******************************
;GDI calls this routine to get the physical colors in the hardware palette of
;the 8514.
;******************************************************************************

cProc	GetPalette, <FAR, PUBLIC, WIN, PASCAL>, <si, di>
	parmW	nStartIndex
	parmW	nNumEntries
	parmD	lpPalette
cBegin
	assumes ds, Data
	cmp	seg_lpPalette, 0	;is there really a palette?
	je	EndGetPalette		;no.  Just don't GP fault
	mov	cx, nStartIndex 	;CX: 1st color index to fetch RGB from
	mov	bx, nNumEntries 	;BX: # of consecutive RGBQuads to get
	les	di, lpPalette		;ES:DI-->where to put RGB quads
	assumes es, nothing
	cmp	[HardwarePaletteRead], 2    ;is it time to get the real colors?
	je	DoNormalGetPaletteRGB	    ;indded it is!
	mov	al, [SystemPaletteColors]
	shr	al, 1			;AL: # of reserved system colors/2
	cbw				;expand to word in AX
	cmp	bx, ax			;is that the # of entries to read?
	jne	DoNormalGetPaletteRGB	;no.  Keep going
	inc	[HardwarePaletteRead]	;anticipate GETSYSTEMCOLOR RGBs
	jcxz	GetLowerSystemColors	;Must retrieve lower colors from the
					; Palette table, not the hardware.
	neg	ax
	add	ax, 256 		;AX=256-(# of system colors/2)
	cmp	cx, ax			;get upper 10 system colors
	je	GetUpperSystemColors	;yes, go do it!
	dec	[HardwarePaletteRead]	;just a normal read

DoNormalGetPaletteRGB:
	add	bx, cx
GetPaletteLoop:
	mov	dx, RPALETTE		; set the index register
	mov	al, cl			; and read whatever is in the hardware
	out	dx, al			;palette and write it into the array
	mov	dx, DPALETTE		;passed by GDI.
	in	al, dx			;read actual red value
	shl	al, 2			;align to MSB of byte
	stosb				;save in array
	in	al, dx			;do the same with green
	shl	al, 2
	stosb
	in	al, dx			;and blue
	shl	al, 2
	stosb
	inc	di			;skip over flags
	inc	cx			;update loop counter
	cmp	cx, bx
	jb	GetPaletteLoop
EndGetPalette:
cEnd

GetLowerSystemColors:
	mov	si, DataOFFSET RealPalette
	mov	cx, bx			;CX: loop counter
	jmp	CopySystemPaletteColorsLoop

GetUpperSystemColors:
	mov	si, DataOFFSET RealPalette
	mov	cx, bx			;CX: loop counter
	shl	bx, 1			;multiply by 3
	add	bx, cx			;BX: offset into palette
	add	si, bx			;DS:SI-->1st RGB triple to copy
CopySystemPaletteColorsLoop:
	movsw
	movsb
	inc	di			;skip over flags
	loop	CopySystemPaletteColorsLoop
	jmp	short EndGetPalette	;we're done!

sEnd	DynamicEight
end
