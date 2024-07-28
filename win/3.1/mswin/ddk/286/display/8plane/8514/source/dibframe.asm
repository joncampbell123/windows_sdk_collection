;******************************Module*Header***********************************
;
; Device Independent Bitmap support routines for the 8514 driver.
; A bitmap used by the device is translated to/from the DIBmp format.
;
; The 8514 is a single plane device. At this point 8 bit per pixel or one
; bit per pixel are implemented in the driver. (See Readscrn.asm or Blkwri.asm
; for examples for the actual use of Bitmaps)
;
; Unless specified otherwise the following color format is used for the 8514:
; BBGGGRRR where RRR red bits, GGG green bits, and BB blue bits
;
; Created   10-27-88
; by	    GunterZ	[Gunter Zieber]
; History:  10-29-88	SetDIBmp 4 bits/pixel to 8bits/pixel works (tested)
;			1 or 8 b/p -> 8b/p are also implemented
;	    10-30-88	Fixed bugs in huge DIBmp segment fumble code
;	    10-31-88	SetDIBmp 24 bit/pixel implemented
;	    11-01-88	Fixed and improved color stuff
;	    11-02-88	GetDIBmp 4 and 8 bits/pixel implemented--works fine
;	    11-03-88	GetDIBmp 24 bit/pixel implemented+debugged--works
;	    11-04-88	Rebuilt driver with Fred's latest stuff.
;	    12-0?-88	Adapted to palette manager (set 1, 4, 8 Bits/Pixel
;			stuff only!)
;******************************************************************************

.286
.xlist
include     cmacros.inc
include     gditype.inc
include     gdibitm.inc
include     dibdefs.inc
.list

tagDWord    struc
    lo	dw  0
    hi	dw  0
tagDWord    ends

public	    MakeColorTable8
public	    szGDI

sBegin	Data
externW Palette
externB WriteEnable
szGDI	db  'GDI', 0
sEnd

createSeg   _DIB, DIBSeg, word, public, CODE
sBegin	    DIBSeg

	assumes cs, DIBSeg
	assumes ds, Data
	assumes es, nothing

extrn	__AHIncr:ABS			; imported from kernel for prot. mode
externFP    GetModuleHandle
externFP    GetProcAddress

externNP    OneFourEightToEight8
externNP    TwentyfourTo88
externNP    OneFourEightToOne8
externNP    TwentyfourTo18
externNP    EightToOneFourEight8
externNP    OneToOneFourEight8
externNP    EightTo248
externNP    OneTo248
externNP    OneFourEightToFour
externNP    TwentyFourToFour
externNP    FourToOneFourEight
externNP    FourToTwentyFour
externNP    BuildColorTable_4
externNP    BuildColorInfo_4
externNP    DsEsSwap			;in DIBSTUFF.ASM; all below as well
externNP    Partial24To8
externNP    Partial24To1
externNP    Partial148To8
externNP    Partial148To1
externNP    Partial8To24
externNP    Partial1To24
externNP    Partial8To148
externNP    Partial1To148
externNP    Partial4To24		;in DIBSTUF4.ASM all below as well
externNP    Partial24To4
externNP    Partial4To148
externNP    Partial148To4

ModuleFixupTable    label   word
	dw  TwentyFourTo88		; 0000	This are the function numbers.
	dw  TwentyFourTo18		; 0001	See explaination below for the
	dw  OneFourEightToEight8	; 0010	meaning of individual bits
	dw  OneFourEightToOne8		; 0011
	dw  EightTo248			; 0100
	dw  OneTo248			; 0101
	dw  EightToOneFourEight8	; 0110
	dw  OneToOneFourEight8		; 0111
	dw  TwentyFourToFour		; 1000
	dw  TwentyFourTo18		; 1001
	dw  OneFourEightToFour		; 1010
	dw  OneFourEightToOne8		; 1011
	dw  FourToTwentyFour		; 1100
	dw  OneTo248			; 1101
	dw  FourToOneFourEight		; 1110
	dw  OneToOneFourEight8		; 1111

PartialTransferTable	label	word
	dw  Partial24To8
	dw  Partial24To1
	dw  Partial148To8
	dw  Partial148To1
	dw  Partial8To24
	dw  Partial1To24
	dw  Partial8To148
	dw  Partial1To148
	dw  Partial24To4	    ;partial 24 to 4
	dw  Partial24To1
	dw  Partial148To4	    ;partial 1, 4, 8 to 4
	dw  Partial148To1
	dw  Partial4To24	    ;partial 4 to 24
	dw  Partial1To24
	dw  Partial4To148	    ;partial 4 to 1, 4, 8
	dw  Partial1To148

ColorTableFunctionTable label	word	;having the following bits set means:
	dw  MakeColorTable8	;set8	;D1: Get DI bits
	dw  BuildColorInfo	;get8	;D2: four plane device
	dw  MakeColorTable4	;set4	;D0 = 0 (always)
	dw  BuildColorInfo_4	;get4

cProc	DeviceBitmapBits, <NEAR, PUBLIC, WIN, PASCAL>, <si>

	include dibframe.inc		;stack frame definitions
cBegin
	lea	ax, Palette		;get a far pointer to our hardware
	mov	seg_lpPalette, ds	;palette. (only used for 4 plane mode)
	mov	off_lpPalette, ax
	mov	al, [WriteEnable]
	inc	al
	mov	bLast, al
	push	ds
	les	di, lpPdevice
	xor	ax, ax
	lds	si, lpInfoBlock 	; ds:si -> lpInfoBlock
	cmp	[si].biPlanes, 1	; Must be 1, else error exit ax=0
	jne	SetDIBmpErrorNode
	mov	cx, [si].biBitCount	; Must be in {1, 4, 8, 24}
	mov	nDIBitsPixel, cx	; keep it on local stack
	mov	dx, 1
	cmp	cx, dx
	jne	NextTry4
	jmp	short OkayCount
NextTry4:
	cmp	cx, 4
	jne	NextTry8
	jmp	short OkayCount
NextTry8:
	cmp	cx, 8
	jne	NextTry24
	jmp	short OkayCount
SetDIBmpErrorNode:
	xor	ax, ax
	jmp	SetDIBmpError
NextTry24:
	cmp	cx, 24
	jne	SetDIBmpErrorNode
	sub	cx, cx			; make cx=0 in this case so that we can
OkayCount:				; still use the build color table code.
	shl	dx, cl			; if cx = 1, 4, 8 then dx = 2**cx
	push	cx			; i.e., dx = # of colors
	mov	cx, nDIBitsPixel
	mov	nColors, dx
	mov	ax, word ptr [si].biWidth ;compute the number of bytes/scanline
	mul	cx			; dx:ax -> #of bits per scanline
	add	ax, 01fh		; allign # of bits/pixel to DWORD size
	adc	dx, 0
	and	ax, 0ffe0h
	shr	dx, 1			; now divide dx:ax by 8 (ByteSize)
	rcr	ax, 1
	shr	dx, 1
	rcr	ax, 1
	shr	dx, 1
	rcr	ax, 1			; now one word can hold bytes/scanline
	mov	wDIByteScanline, ax	; save on local stack
	mov	ax, es:[di].bmWidthBytes
	mov	wBmByteScanline, ax	; ditto for our bitmap
	mov	al, es:[di].bmBitsPixel
	mov	bMask, al		;store bmp bits/pixel temporarily
	pop	cx			; if 24 bits/pixel cx=0
public	DIBDoFixup
DIBDoFixup:
	sub	bx, bx			;use BX as flag accumulator (init. 0)
	cmp	bx, cx			;now its time to determine which
	pushf				;transfer function to use.  Make BX an
	adc	bx, bx			;index into a transfer function table.
	shl	bx, 1			;If the following bits are set then:
	popf				;D0 -> monochrome bitmap
	cmc				;D1 -> not a 24 bit/pixel DIB
	adc	dh, dh			;D2 -> Get DI bits to Bitmap
	mov	bType24, dh		;D3 -> 4 plane device
	and	al, 01h 		;The bType24 flag is set to one if 24
	or	bl, al			;bit/pixel DIB, else its zero.
	mov	al, bLast		;[WriteEnable]+1 was stored in bLast
	shr	al, 1
	or	bl, al
	mov	cx, wSetGet
	sub	dx, dx
	cmp	dx, cx
	adc	dx, dx
	shl	dx, 2
	or	bx, dx
	shl	bx, 1
	mov	ax, cs:[bx].ModuleFixupTable
	mov	wTranslate, ax
	mov	ax, cs:[bx].PartialTransferTable
	mov	wTranslatePartial, ax
	shr	bx, 3
	shl	bx, 1
	mov	ax, cs:[bx].ColorTableFunctionTable
	mov	ColorTableFunction, ax

; All the nitty-gritty stuff has been taken care of. Now its time to compute
; segment:offset of the corresponding starting line in the PDevice buffer. If
; the number of lines to be tranlated is greater than the height of our bitmap
; only as many lines as there is room for will be translated.  The number of
; lines successfully translated will be returned by this function.
; At this point es:di -> lpDevice, ds:si -> lpInfoBlock

	mov	ax, [si].biHeight.lo
	mov	wTmpSrc, ax

	mov	ax, es:[di].bmHeight	; set nNumScans to min(bmHeight,
	cmp	nNumScans, ax		; nNumScns) and store the result if
	jb	NumScansOkay		; neccessary
	mov	nNumScans, ax		; nNumScans will be our line counter
NumScansOkay:
	mov	ax,nNumScans
	mov	nLinesDone, ax
	mov	ax, [si].biWidth.lo	; now do the same for the width
	cmp	es:[di].bmWidth, ax	; store the min. width of the two bmps
	jg	WidthOkay		; on the local stack for later use
	mov	ax, es:[di].bmWidth
WidthOkay:
	mov	nWidth, ax
	mov	ax, es:[di].bmFillBytes
	mov	nFill, ax		; save for later use

;In case of a Get DIBits we need to return the size of the image.

	cmp	wSetGet, 0
	jz	DIBDoColorTableStuff
	mov	ax, nNumScans
	mul	wDIByteScanline
	mov	[si].biSizeImage.lo, ax
	mov	[si].biSizeImage.hi, dx

; now is a good time to build the color table.	Note, it is not needed in the
; case of 24 bits/pixel bmps. At this point ds:si -> lpInfoBlock

DIBDoColorTableStuff:
	cmp	nNumScans,0		; Exit if zero scans are to be 
	jnz	short @f		; copied.
	jmp	TransferDone
@@:	add	si, word ptr [si].biSize; put pointer to color table in ds:si
	mov	cx, nColors		; get the number of actual colors
	call	ColorTableFunction
public	AlignDestBitmap
AlignDestBitmap:
	mov	bx, wTmpSrc		;BX: biHeight.lo
	lds	si, es:[di].bmBits	; get pointer to our bitmap
	sub	bx, iStart		; bx: # of lines to move down in our
	dec	bx			; bmp to allign to DIBmp
	mov	cx, es:[di].bmSegmentIndex
	mov	nSegIndex, cx		; save for later
	jcxz	NotHuge
	xor	dx, dx
	mov	ax, bx			; keep in ax for possible divide instr
	mov	bx, es:[di].bmScanSegment
	div	bx			; ax: # of segs to advance
	mov	bx, dx			; #of lines to move down in new segment
	xor	dx, dx
	mul	cx
	mov	dx, ax			; now advanve ds by the product of # of
	mov	ax, ds			; segments to move down times HugeIncr.
	add	ax, dx
	mov	ds, ax
NotHuge:
	mov	ax, bx			; ax: # of lines to move down in segm
	mul	es:[di].bmWidthBytes	; # of lines times bytes per line
	add	si, ax			; and add that to the offset
	mov	OFF_pDest, si		; store the result for later use
	mov	SEG_pDest, ds

; at this point ds:si points to the location corresponding to lpBits.  Now,
; depending on the number of bits per pixel and number of planes, get the
; pixels from the DIBmp, translate them using the colortable (which has to be
; translated to a color format that the 8514 can understand), and write them
; into the destination bitmap.	DANGER, DANGER, DANGER!!! The DIBmp is so
; densely packed that segment crossings may occur within a scanline.  Now let's
; set up the parameters for the transfer call.

public	ComputeParameters		;wrt DI bitmap
ComputeParameters:
	mov	wStackTop, sp		;keep current SP, we may change SP
	cmp	bType24, 0
	jnz	Type24Param
	mov	ax, nDIBitsPixel
	mov	bx, ax			; keep in bx for further use
	and	al, 07h
	mov	bShiftCount, al
	mov	ax, bx
;	 xor	 dx, dx
	mul	nWidth
	mov	cx, ax
	and	ax, 07h
	div	bl			;bl is never zero at this point
	mov	bRem, al
	shr	cx, 3
	mov	ax, cx
	cmp	bMask, 1		;bMask=bmp bits/pixel
	jne	DFStoreOCount
	cmp	wSetGet, 0		;compute oCount and bRem differently
	jne	DFStoreOCount		;in case of SetDIBits

	mov	ax, nWidth
	mov	cx, 8
	div	cx
;	 div	 nDIBitsPixel
	mov	bRem, dl
DFStoreOCount:
	mov	oCount, ax
	mov	ax, 8			;AH must be zero for divide ahead
	div	bl
	mov	iCount, al
	mov	ax, nColors
	dec	ax
	mov	bMask, al
	jmp	Short Bypass0
Type24Param:
	cmp	bLast, 0		;8 bit/pixel mode?
	jne	Type24Param_20		;no.  Bypass the stuff that follows
	cmp	bMask, 1
	je	Type24Param_10
	cmp	wSetGet, 1
	jne	Type24Param_10
	sub	sp, 1024		;allocate 1k on the stack
	jc	SetDIBmpErrorNode2	;exit if we fail
	mov	di, sp
	mov	pdwIndexToRGB, sp	;keep ptr to that array
	push	ss
	pop	es
	lds	si, lpConversionInfo
	mov	cx, 512
rep	movsw				;copy conversion info into local stack

Type24Param_10:
	mov	ax, DataBASE
	mov	ds, ax
	mov	ax, DataOFFSET szGDI	;name string must be in fixed segment
	cCall	GetModuleHandle, <ds, ax>
	sub	dx, dx
	mov	bx, 449 		;ord. # of DeviceColorMatch in GDI
	cCall	GetProcAddress, <ax, dx, bx>
	mov	seg_lpDeviceColorMatch, dx
	mov	off_lpDeviceColorMatch, ax
Type24Param_20:
	mov	ax, nWidth		; get the number of pixels/scanline
	mov	oCount, ax		; this will be our outer loop count
	sub	al, al			; The bMask field is used as a flag
	mov	bRem, al		; whether an incomplete pixel is to be
	mov	bMask, al		; or not.  Usually bMask=0 (complete)
Bypass0:

; at this point all parameters for a normal transfer call are set up.  Only if
; a segment boundary of the source pointer is about to be crossed in the next
; scanline transfer, bRem and oCount will be different and two subsequent calls
; to the transfer routine will be made instead of just one.

	cmp	seg_lpBits, 0
	jne	short DIBSetupPointers
	jmp	TransferDone

SetDIBmpErrorNode2:
	jmp	SetDIBmpErrorNode
DIBSetupPointers:
	lds	ax, lpBits		; initialize pointers
	mov	wTmpSrc, ax
	les	ax, pDest
	mov	wTmpDst, ax
TransferLoop:
	mov	si, wTmpSrc		;DS:SI-->next line in DIB
	mov	di, wTmpDst		;ES:DI-->next line in BMP
	mov	bl, bRem
	mov	cl, bShiftCount
	mov	ch, iCount
	mov	dx, oCount
	mov	ax, si			; get current source offset
	add	ax, wDIByteScanline	; are we about to cross a seg. boundary
	mov	wTmpSrc, ax		; pre-update source pointer
	mov	ah, bMask
	jz	UpdateAfterTransfer
	jnc	SourceOkay		;no carry? go transfer scanline

	cCall	wTranslatePartial
	or	si, si			;has the segment been updated?
	jns	CheckLineCount		;yes. Keep going
GoToNextSegment:
	cmp	nNumScans, 1		;was it the last scan??
	je	TransferDone		;yes-that means don't touch any sel.
	mov	si, ds			;no!  Better do it now!
	add	si, __AHIncr
	mov	ds, si
	jmp	short CheckLineCount

UpdateAfterTransfer:
	call	wTranslate		;transfer DIB<-->Bmp
	jmp	short GoToNextSegment

SourceOkay:
	call	wTranslate		; transfer yet another line
CheckLineCount:

	dec	nNumScans		; check if we're done yet
	jz	TransferDone

	mov	di, wTmpDst		; if not done, update dest. ptr.
	sub	di, wBmByteScanline	; go UP to the next line
	jnc	BusinessAsUsual 	; no carry = no segment overrun
	sub	di, di
	sub	di, nFill		; skip the filler bytes
	sub	di, wBmByteScanline	; go to beginning of next scanline
	mov	ax, es			; now update the selector
	sub	ax, nSegIndex
	mov	es, ax
BusinessAsUsual:
	mov	wTmpDst, di		; write the updated offset back!
	jmp	short TransferLoop	; do next line

TransferDone:
	mov	ax, nLinesDone		; get return value
SetDIBmpError:
	mov	sp, wStackTop
	pop	ds
cEnd

;*****************************Private*Routine**********************************
;This routine is used to build the color definition table in the InfoBlock.
;In case of 1 bit per pixel: 0->black, 1->white;
;in case of 4 bit per pixel: 0..15 -> default color pallet;
;in case of 8 bit per pixel: invert the 8514's color definition, i.e.
;RRR00000b, GGG00000b, BB000000b will be written into the color def. table
;On entry:  cx	    -> #of entries into color table (1, 2, 16, 256)
;	    ds:si   -> lpColorTable (to be consistent with BuildColorTable)
;This routine sets only the MSBs of the respective RGB intensities
;******************************************************************************

cProc	BuildColorInfo, <NEAR, PUBLIC>, <es, ds, si, di, bx>
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing
cBegin
	call	DsEsSwap
	cmp	cx, 1
	je	BuildColorInfoEnd
	cmp	cx, 256
	je	Next256
	lds	si, lpConversionInfo	;in case of 2 and 16 color DIBs GDI
	assumes ds, nothing		;passes us a conversion table to map
	push	ss			;the 256 colors in the source BMP
	pop	es
	lea	di, bColorTable
	mov	cx, 128
	rep	movsw
	jmp	short BuildColorInfoEnd
Next256:
	sub	bx, bx
	lea	si, bColorTable 	;in cse of 256 colors build an
GetInfoLoop:				;identity table on the stack since
	mov	ss:[si][bx], bl 	;no color mapping will be needed
	inc	bx
	loop	GetInfoLoop
BuildColorInfoEnd:
cEnd

;*****************************Public*Routine***********************************
; copies translate table provided by GDI into local stack frame
; on entry:
; ds:si -> color table
; ss:bp+?? -> bColorTable	just don't change the stack segment
; cx -> #of entries in the color table
; modifies cx
;******************************************************************************

MakeColorTable8 proc near
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	cmp	cx, 1			; if cx=1, don't translate anything
	je	MakeColorTable8End
	push	es
	push	di
	push	ss
	pop	es
	lea	di, bColorTable
CopyIndexLoop:				; The color table that is passed
	lodsw				; is an array of physical color indices
	stosb				; Each index is passed as a word, but
	loop	CopyIndexLoop		; only the lower byte is used here
	pop	di
	pop	es
MakeColorTable8End:
	ret
MakeColorTable8 endp

MakeColorTable4 proc near
	assumes cs, DIBSeg

	cmp	cx, 1			;this routine is invoked upon SetDIBits
	je	MakeColorTable4End	;in 4 plane configuration
	push	es			;already: DS:SI-->DIB's RGBQuad table
	push	di			;make ES:DI-->index translate table
	push	ss			;pass 8514's 16 color palette
	pop	es			; (RGBTripple) as argument
	lea	di, bColorTable
	cCall	BuildColorTable_4, <lpPalette>
	pop	di
	pop	es
MakeColorTable4End:
	ret
MakeColorTable4 endp


sEnd	DIBSeg
end
