        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	SCANLR.ASM
;
;   This module contains the ScanLR routine.
;
; Created: 22-Feb-1987
; Author:  Walt Moore [waltm]
;
; Copyright (c) 1984-1987 Microsoft Corporation
;
; Exported Functions:	ScanLR
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
;   ScanLR is used to search a scanline for a pixel of the given
;   color or one which isn't of the given color.  This is usually
;   used by the floodfill simulation.
;
; Restrictions:
;
;-----------------------------------------------------------------------;


incDrawMode	= 1			;Include control for gdidefs.inc

	.xlist
	include cmacros.inc
	include gdidefs.inc
	include display.inc
	include macros.mac
	.list


;	Link time constants describing the size that the device
;	will be running in.

	externA ScreenSelector		;Selector to the screen
	externA SCREEN_WIDTH		;Screen width in pixels
	externA SCREEN_W_BYTES		;Screen width in bytes
	externA SCREEN_HEIGHT		;Screen height in scans


ifdef	EXCLUSION
	externFP exclude_far		;Exclude area from screen
	externFP unexclude_far		;Clear excluded area
endif


;	Define the flag values which control the direction
;	and type of the scan.

STEP_LEFT	equ	00000010b	;Flag values for DirStyle
STEP_RIGHT	equ	00000000b
FIND_COLOR	equ	00000001b
FIND_NOT_COLOR	equ	00000000b



;	Define the error conditions which will be returned

ERROR_CLIPPED	equ	8000h		;Cooridnate was clipped
ERROR_NOT_FOUND equ	-1		;Stop condition not reached


sBegin	Data

	externB enabled_flag		;Non-zero if output allowed

sEnd	Data


createSeg _BLUEMOON,BlueMoonSeg,word,public,CODE
sBegin	BlueMoonSeg
assumes cs,BlueMoonSeg

rot_bit_tbl	label	byte
		db	10000000b	;Table to map bit index into
		db	01000000b	;  a bit mask
		db	00100000b
		db	00010000b
		db	00001000b
		db	00000100b
		db	00000010b
		db	00000001b
page

;--------------------------Exported-Routine-----------------------------;
; ScanLR
;
;   ScanLR - Scan left or right
;
;   Starting at the given pixel and proceeding in the choosen direction,
;   the pixels are examined for the given color until one is found that
;   matches (or doesn't match depending on the style).  The X coordinate
;   is returned for the pixel that matched (or didn't match).
;
;   The physical device may be the screen or a monochrome bitmap.
;
; Entry:
; Returns:
;	AX = x location of sought pixel
; Error Returns:
;	AX = -1 if nothing found
;	AX = 8000h if clipped
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	exclude_far
;	unexclude_far
; History:
;	Tue 18-Aug-1987 14:50:37 -by-  Walt Moore [waltm]
;	Added test of the disabled flag.
;
;	Sun 22-Feb-1987 16:29:09 -by-  Walt Moore [waltm]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,Data
assumes es,nothing


cProc	ScanLR,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	parmD	lp_device		;ptr to a physical device
	parmW	x			;x coordinate of search start
	parmW	y			;y coordinate of search start
	parmD	color			;color for the search
	parmW	dir_style		;control and search style

	localW	width_bits		;actual width of scan in bits
	localB	is_device		;set non-zero if the device

cBegin
	mov	al,enabled_flag 	;Load this before trashing DS
	lds	si,lp_device		;--> physical device
	assumes ds,nothing

	mov	cx,[si].bmType		;Get bitmap type
	jcxz	scan_30 		;Device is a memory bitmap
	mov	is_device,al		;If not enabled, will skip unexclude
	or	al,al			;  since AL will be 0!
	jz	scan_20 		;Disabled, show clipped


;	This is the EGA.  Compute and set the exclusion rectangle
;	based on the direction of the search.

	ifdef	EXCLUSION
	mov	dx,y			;Set top of exclude area
	mov	di,dx			;Set bottom of exclude area
	mov	si,SCREEN_WIDTH-1	;Set right
	mov	cx,x			;Assume scanning left to right
	test	bptr dir_style,STEP_LEFT
	jz	scan_10 		;Scanning left to right
	mov	si,cx
	xor	cx,cx			;Scanning right to left

scan_10:
	call	exclude_far		;Exclude the scan from the screen
	endif


	mov	ax,ScreenSelector	;Set DS to VRAM
	mov	ds,ax
	assumes ds,nothing


;	The scanning code doesn't scan outside the bounds of the
;	surface, however the starting coordinate must be clipped.
;

	mov	ax,y			;Get starting Y coordinate
	cmp	ax,SCREEN_HEIGHT	;Within the surface of the device?
	jae	scan_20 		;  No, return error
	mov	di,SCREEN_W_BYTES	;Need screen width in bytes
	mul	di			;Compute Y starting address
	mov	si,ax
	mov	bx,x			;Will need X later
	mov	dx,SCREEN_WIDTH
	mov	width_bits,dx		;Save width for final bounds test
	cmp	bx,dx			;Within the surface of the device?
	jb	scan_80 		;  Yes


;	The coordinate is clipped.  Return the clipped error code.

scan_20:
	mov	ax,ERROR_CLIPPED	;Set error code
	jmp	scan_280		;  and return it



;	The scanning will be for a memory bitmap.  The scanning code
;	doesn't scan outside the bounds of the surface,  however the
;	starting coordinate must be clipped.

scan_30:
	mov	is_device,cl		;Show this is a bitmap
	mov	ax,y			;Get starting Y coordinate
	cmp	ax,[si].bmHeight	;Within the surface of the device?
	jae	scan_20 		;  No, return error
	mov	cx,[si].bmWidth 	;Get width in pixels
	cmp	x,cx			;Within the surface of the device?
	jae	scan_20 		;  No, return error
	mov	width_bits,cx		;  Yes, save width in pixels

	xor	dx,dx			;Set segment bias to 0
	mov	cx,[si].bmSegmentIndex	;Is this a huge bitmap?
	jcxz	scan_50 		;  No



;	This is a huge bitmap.	Compute which segment the Y coordinate
;	is in.	Assuming that no huge bitmap will be bigger than two
;	or three segments, iteratively computing the value would be
;	faster than a divide, especially if Y is in the first segment
;	(which would always be the case for a huge color bitmap that
;	didn't have planes >64K).


	mov	bx,[si].bmScanSegment	;Get # scans per segment

scan_40:
	add	dx,cx			;Show in next segment
	sub	ax,bx			;See if in this segment
	jnc	scan_40 		;Not in current segment, try next
	add	ax,bx			;Restore correct Y
	sub	dx,cx			;Show correct segment


;	This is a memory DC.  
;
;	Currently:
;		AX     =  Y coordinate
;		DX     =  Segment bias for huge bitmaps
;		DS:SI --> PDevice

scan_50:
	mov	di,[si].bmWidthBytes	;Get width of scan
	add	dx,wptr [si].bmBits[2]	;Compute segment of the bits
	mov	si,wptr [si].bmBits[0]	;Get offset of the bits
	mov	ds,dx			;Set DS:SI --> to the bits
	assumes ds,nothing

	mul	di			;Compute start of scan
	add	si,ax			;DS:SI --> start of scanline byte is in
	mov	bx,x



;	Currently:
;
;		DS:SI --> start of scan
;		BX     =  X coordinate
;		DI     =  Scan width

scan_80:
	mov	cx,bx
	shiftr	cx,3			;Compute byte offset in scan
	add	si,cx			;DS:SI --> byte with start pixel
	mov	ax,ds			;Will be working off both DS: and ES:
	mov	es,ax
	assumes es,nothing


;	Set cx to be the byte count for searching left.  Must adjust it
;	to include the byte pixel is in.

	inc	cx			;Adjust for partial byte



;	Compute the mask for the first byte (the partial byte).  Since
;	the defaults being set up are for searching left, this can be done
;	by getting the rotating bitmask for the pixel and decrementing it,
;	then using the logical NOT of the mask.  The mask will be used
;	for masking the bits to test in the partial (first) byte.
;
;		Bitmask 	  Mask		NotMask
;
;		10000000	01111111	10000000
;		01000000	00111111	11000000
;		00100000	00011111	11100000
;		00010000	00001111	11110000
;		00001000	00000111	11111000
;		00000100	00000011	11111100
;		00000010	00000001	11111110
;		00000001	00000000	11111111


	and	bx,00000111B		;Get bit mask for bit
	mov	bl,rot_bit_tbl[bx]	;Assume we're going left.
	dec	bl			;Create mask




;	The assumption has been made that the scan will be right to left.
;	If the scan is left to right, then the first byte mask and the
;	byte count must be adjusted.
;
;	Also set up the correct bias for getting back to the interesting
;	byte for the rep scasb instruction (DI is always updated by one
;	byte too many).


	std				;Assume search left
	mov	dx,1			;(to counter post decrement)
	test	bptr dir_style,STEP_LEFT
	jnz	scan_100		;It is left




;	Compute the first byte mask for the first byte for stepping right.
;
;		Current 	  SHL		  INC		  NOT
;
;		01111111	11111110	11111111	00000000
;		00111111	01111110	01111111	10000000
;		00011111	00111110	00111111	11000000
;		00001111	00011110	00011111	11100000
;		00000111	00001110	00001111	11110000
;		00000011	00000110	00000111	11111000
;		00000001	00000010	00000011	11111100
;		00000000	00000000	00000001	11111110


	cld				;Going right, fix up dir flag
	shl	bl,1			;Fix up first bit mask per above
	inc	bl
	not	bl


;	Compute the number of bytes from current position to end of scanline
;	and set adjustment to counter the rep's post increment

	sub	cx,di			;Fix up byte count
	neg	cx
	inc	cx
	neg	dx			;(to counter post increment)




;	Set the pixel count for the entire scan.  The scanning will actually
;	continue until the end of the scan as given in bmWidthBytes, and
;	the result clipped to bmWidth.

scan_100:
	not	bl			;Need inverse of the first byte mask
	shiftl	di,3			;Set di = pixel count of entire scan



;	The desired action of the scan is to be able to do a rep scasb
;	over the scanline until either the color is found or not found.
;	Once the stopping condition is found, it has to be possible to
;	determine which bit was the bit that stopped the scan.
;
;	Monochrome notes:
;
;	    The color will be used as an XOR mask.  If the result of
;	    the XOR is zero, then the byte did not contain any bits of
;	    importance, otherwise we made a hit and need to return the
;	    location of it.
;
;	    If searching for the color, the color must be complemented
;	    so that the XOR will set all bits not of the color to zero,
;	    and leave all bits of the color 1's.  If searching for NOT
;	    the color, then the color can be left as is so that all bits
;	    of the color will be set to zero.  The complement also gives
;	    the compare value for the scasb instruction.
;
;
;	Currently:	DS:SI --> bitmap or display
;			ES:SI --> bitmap or display
;			BL = first byte mask
;			CX = byte count
;			DX = direction bias
;			DI = bits/scanline


	mov	ah,bptr dir_style	;If searching for the color,
	shr	ah,1			;  want a mask of 1's to be
	sbb	ah,ah			;  able to invert the result
	errnz	FIND_NOT_COLOR		;  of the search
	errnz	FIND_COLOR-1

	mov	al,color.SPECIAL	;Get mono search color
	shiftl	al,4
	errnz	MONO_BIT-00010000b
	sbb	al,al
	xor	ah,al			;Invert search color if needed


;	Check the first byte for a hit or miss.

scan_210:
	lodsb				;Get the first byte
	xor	al,ah			;Adjust the color
	and	al,bl			;Mask out the bits that don't count
	jnz	scan_230		;  Hit.  Check it out

	mov	al,ah			;Otherwise restore register for scan
	dec	cx			;Any bytes left to check?
	jz	scan_300		;  No, show not found

	xchg	si,di			;scasb uses es:di
	repe	scasb			;Try for a hit or miss
	jz	scan_300		;Scanned off the end, it's a miss
	inc	cx			;Decremented one time too many
	xchg	si,di
	add	si,dx			;Adjust from post increment/decrement
	lodsb				;Get the byte which we hit on
	xor	al,ah			;Adjust to look for a set bit



;	Had a hit.  Find which pixel it was really in.
;
;	Currently:	CX = byte index pixel is in
;			DI = # pixels in the scan line
;			AL = byte hit was in

scan_230:
	shiftl	cx,3			;Convert byte index to pixel index
	test	bptr dir_style,STEP_LEFT;Scanning Right to left?
	jnz	scan_260		;  yes

scan_240:
	sub	cx,di			;Compute index of first pixel in byte
;	neg	cx			;  where hit occured
;	dec	cx			;Prepare for loop
	not	cx

scan_250:
	inc	cx			;Show next pixel
	shl	al,1			;Was this the hit?
	jnc	scan_250		;  No, try next
	cmp	cx,width_bits		;Is final x value in range?
	jge	scan_300		;  No, show not found
	jmp	short scan_270		;  Yes, return it

scan_260:
	dec	cx			;Show next pixel
	shr	ax,1			;Was this the hit?
	jnc	scan_260		;  No, try next

scan_270:
	mov	ax,cx			;Return position to caller

scan_280:
	cld

	test	is_device,0FFh		;Is this the device?
	jz	scan_290		;  No, skip this stuff

	ifdef	EXCLUSION		;If exclusion
	call	unexclude_far		;Clear any exclude rectangle
	endif


scan_290:

cEnd


scan_300:
	mov	ax,ERROR_NOT_FOUND
	jmp	scan_280

sEnd	BlueMoonSeg

	ifdef	PUBDEFS
	include scanlr.pub
	endif

end

