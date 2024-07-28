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
.286c

incDrawMode	= 1			;Include control for gdidefs.inc

	.xlist
	include cmacros.inc
	include gdidefs.inc
	include display.inc
        include macros.mac
        include 8514.inc
	.list



;	Define the flag values which control the direction
;	and type of the scan.

STEP_LEFT	equ	00000010b	;Flag values for DirStyle
STEP_RIGHT	equ	00000000b
FIND_COLOR	equ	00000001b
FIND_NOT_COLOR	equ	00000000b



;	Define the error conditions which will be returned

ERROR_CLIPPED   equ     8000h           ;Coordinate was clipped
ERROR_NOT_FOUND equ	-1		;Stop condition not reached

sBegin	Data
externB WriteEnable
externB ShadowMemoryTrashed
externB BigFontInfoTable
sEnd	Data

subttl          Code Segment Definitions
page +
sBegin  code
assumes cs,code
externFP        <CursorExclude,CursorUnExclude> ;in ROUTINES.ASM
sEnd    code

externFP    AllocCSToDSAlias

createSeg _SCANLR,ScanLRSeg,word,public,CODE
sBegin  ScanLRSeg
assumes cs,ScanLRSeg
assumes ds,Data
assumes es,nothing


rot_bit_tbl	label	byte
		db	10000000b	;Table to map bit index into
		db	01000000b	;  a bit mask
		db	00100000b
		db	00010000b
		db	00001000b
		db	00000100b
		db	00000010b
                db      00000001b

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

cProc	ScanLR,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

        parmD   lpDevice                ;ptr to a physical device
	parmW	x			;x coordinate of search start
	parmW	y			;y coordinate of search start
	parmD	color			;color for the search
        parmW   DirStyle                ;control and search style

	localW	width_bits		;actual width of scan in bits
	localB	PlaneBitMask

cBegin
	mov	al, [WriteEnable]
	mov	PlaneBitMask, al
	mov	ax, ds
	mov	es, ax
	assumes es, Data
        lds     si,lpDevice             ;--> physical device
        cmp     word ptr [si+0],0       ;is it a memory bitmap?
        je      scan_30                 ;yes, go do a memory ScanLR
        jmp     ScanLR_COLOUR           ;otherwise, do it on the device
;
scan_20:
        mov     ax,ERROR_CLIPPED        ;set error code
        jmp     scan_280                ;and return it

;	The scanning will be for a memory bitmap.  The scanning code
;	doesn't scan outside the bounds of the surface,  however the
;	starting coordinate must be clipped.

scan_30:
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
	mov	bx,x

	mov	cl, [si].bmBitsPixel
	shr	cl, 1			;monochrome?
	jc	scan_800		;yes
	cmp	cl, 2			;4 plane mode?
	je	scan_4_plane		;yes
	cmp	cl, 4			;8 plane mode?
	jne	scan_20 		;if neither, exit with error

;if we are at this point we are dealing with an 8 bit/pixel bitmap.

	mov	es, dx
	assumes es, nothing
	mul	di
	mov	di, ax
	add	di, wptr [si].bmBits[0] ;ES:DI-->correct scanline
	add	di, bx			;ES:DI-->starting point

	lea	si, [di][1]		;keep offset to starting point plus 1
	mov	dx, DirStyle
	mov	al, bptr color		;the low byte contains the color index
	cld				;assume we're stepping right
	mov	cx, width_bits
	sub	cx, bx
	test	dl, STEP_LEFT
	jz	scan_color8_10
	std				;oh, no! we're stepping left.
	dec	si
	dec	si			;SI: offset to starting point minus 1
	mov	cx, bx
scan_color8_10:
	test	dl, 1			;do we look for 1st match?
	jnz	scan_color8_20		;yes, go there and do it!
repe	scasb				;now find the first non-match
	jcxz	scan_color8_not_found	;no mismatch found?
	jmp	short scan_color8_30	;go compute X of mismatch
scan_color8_20:
repne	scasb				;find the first match
	jcxz	scan_color8_not_found	;no match found?
scan_color8_30:
	lea	ax, [di][bx]		;X coordinate=BX+DI-SI
	sub	ax, si
	jmp	scan_280

scan_color8_not_found:
	jmp	scan_300

scan_800:
	jmp	scan_80

scan_4_plane:
	mov	ds, dx
	assumes ds, nothing
	mul	di
	mov	si, wptr [si].bmBits[0]
	add	si, ax			;ES:DI-->correct scanline

	mov	dl, bptr color		;put the color index into both nibbles
	mov	dh, dl			;of DL
	shl	dl, 4
	and	dh, 0fh
	or	dl, dh
	mov	dh, 0f0h		;DH: nibble mask
	mov	cx, bx
	shr	cx, 1
	jnc	scan_color4_10
	not	dh
scan_color4_10:
	add	si, cx			;ES:DI-->starting point
	mov	ax, DirStyle
	cld				;assume we're stepping right
	mov	cx, 1			;this is our X pos increment then
	mov	di, width_bits		;this is the max possible X pos allowed
	push	bp			;save the frame pointer
	mov	bp, bx			;BP: current X position
	test	al, STEP_LEFT
	jz	scan_color4_20
	std				;we're stepping left!
	neg	cx			;now CX has the correct pos increment
	sub	di, di			;this is the min possible X pos allowed
scan_color4_20:
	add	di, cx			;we'll compare after updating X pos
	test	al, 1			;do we look for the first match?
	jnz	scan_color4_50		;yes! go there and do it!
scan_color4_30: 			;okay, here we look for non-matches
	lodsb				;fetch 2 pixels, update SI
scan_color4_40:
	mov	ah, al
	mov	bl, dl
	and	ah, dh			;AH pixel of interest
	and	bl, dh			;color portion of interest
	cmp	ah, bl			;have we found a match
	jne	scan_color4_60		;yes, we're done!
	add	bp, cx			;update X position
	cmp	bp, di			;have we reached the end?
	je	scan_color4_70		;if so, exit appropriately
	not	dh			;update nibble mask
	mov	bh, dh
	xor	bh, ch			;if sign bit set fetch 2 new pixels
	js	scan_color4_30
	jmp	short scan_color4_40	;else continue with the other pixel

scan_color4_50: 			;okay, here we look for matches
	lodsb				;fetch 2 pixels, update SI
scan_color4_80:
	mov	ah, al
	mov	bl, dl
	and	ah, dh			;AH pixel of interest
	and	bl, dh			;color portion of interest
	cmp	ah, bl			;have we found a match
	je	scan_color4_60		;yes, we're done!
	add	bp, cx			;update X position
	cmp	bp, di			;have we reached the end?
	je	scan_color4_70		;if so, exit appropriately
	not	dh			;update nibble mask
	mov	bh, dh
	xor	bh, ch			;if sign bit set fetch 2 new pixels
	js	scan_color4_50
	jmp	short scan_color4_80	;else continue with the other pixel

scan_color4_60:
	mov	ax, bp			;AX: X pos where condition was met
	pop	bp			;restore frame pointer
	jmp	scan_280

scan_color4_70: 			;at this point we couldn't meet cond.
	pop	bp			;restore frame pointer
	jmp	scan_300



scan_80:
	mov	si,wptr [si].bmBits[0]	;Get offset of the bits
	mov	ds,dx			;Set DS:SI --> to the bits
	assumes ds,nothing

	mul	di			;Compute start of scan
	add	si,ax			;DS:SI --> start of scanline byte is in

;	Currently:
;
;		DS:SI --> start of scan
;		BX     =  X coordinate
;		DI     =  Scan width

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
        test    bptr DirStyle,STEP_LEFT
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
        shl     di,3                    ;Set di = pixel count of entire scan



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


        mov     ax,off_color            ;Get mono search color in AL
        mov     ah,bptr DirStyle        ;If searching for the color,
	shr	ah,1			;  want a mask of 1's to be
	sbb	ah,ah			;  able to invert the result
	errnz	FIND_NOT_COLOR		;  of the search
	errnz	FIND_COLOR-1

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
        shl     cx,3                    ;Convert byte index to pixel index
        test    bptr DirStyle,STEP_LEFT;Scanning Right to left?
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
cEnd


scan_300:
	mov	ax,ERROR_NOT_FOUND
        jmp     scan_280


subttl          IBM 8514 Board ScanLR Code
page +                                                        
assumes ds,data
public  ScanLR_COLOUR
ScanLR_COLOUR:

;       We are going to read in a line on the screen into a buffer in our
;data segment.  By carefully investigating the direction of the scan in 
;relation to the starting X coordinate, we can probably get away with 
;reading in only part of a scanline, thus decreasing significantly the time
;it takes to do a flood fill (since the reading-in process is what takes all
;of the time).

	assumes es, Data
	cmp	y,Y_SIZE			;is Y-coordinate on the device?
	jb	SCLR_0				;yes, continue

SCLR_CLIP_ERROR:
	jmp	scan_20 			;No, return error

SCLR_0:         
	cmp	x,X_SIZE			;is X-coordinate on the device?
	jae	SCLR_CLIP_ERROR 		;nope, give an error

;Next, determine the X-coordinates that we must read in:

	test	DirStyle,STEP_LEFT		;are we going right to left?
	jnz	SCLR_GET_X_HEBREW		;yes, do it Hebrew style

SCLR_GET_X_ENGLISH:
	mov	bx,X_SIZE-1			;get the high X on board
	mov	cx,bx				;copy to CX for extent calc
	sub	cx,X				;now CX has the xExt-1
	jmp	short SCLR_EXCLUDE_CURSOR	;and continue

SCLR_GET_X_HEBREW:                                                   
	mov	bx,X				;get the high X as ending X
	mov	X,0				;replace starting X with 0
	mov	cx,bx				;this is now our xExt-1

SCLR_EXCLUDE_CURSOR:

;Next, exclude the cursor from the read area:
;At this point:
;       X has the low X
;       BX has the high X

	Arg	X			;send exclusion rectangle down
	Arg	Y
	Arg	bx
	Arg	Y
	cCall	CursorExclude		;go exclude the cursor

;Now, read a scanline into our local buffer:

public	SetupPointers
SetupPointers:
	mov	di, DataOFFSET BigFontInfoTable
	and	es:[ShadowMemoryTrashed], 0fdh
	CheckFIFOSpace	SEVEN_WORDS

	mov	al, PlaneBitMask	;enable all installed planes for
	rol	al, 1			;reading
	mov	dx,READ_ENABLE_PORT
	out	dx,ax
	mov	ax,0a000h		;set the mode to normal colour read
	mov	dx,MODE_PORT
	out	dx,ax

;Set the extents on board:

	mov	ax,cx			;set X-extent
	mov	dx,RECT_WIDTH_PORT	;
	inc	cx			;make CX have a true extent
	out	dx,ax			;
	xor	ax,ax			;set Y-extent = 0
	mov	dx,RECT_HEIGHT_PORT	;
	out	dx,ax			;

;Now set the X-origin and Y-coordinate

	mov	ax,X			;set the X-origin
	mov	dx,Srcx_PORT		;
	out	dx,ax			;
	mov	ax,Y			;set the Y-coordinate
	mov	dx,Srcy_PORT		;
	out	dx,ax			;
	mov	ax,3318h		;get command to send
	mov	dx,COMMAND_FLAG_PORT	;
	out	dx,ax			;

SCLR_2:
	mov	dx,9ae8h		;get status to see if there's data to
	in	ax,dx			;read from the board
	and	ah,1			;is there data available?
	jz	SCLR_2			;nope, keep waiting
	mov	dx,PATTERN_DEFINE_PORT	;set DX to variable data port

;At this point we must make our INSW loop counter.  CX has the X-extent of
;our read (in bytes).  Since we operate in words, we must divide CX by 2.
;If there's a remainder (ie: carry set), we must add on another byte and
;correct for it later:

	push	cx			;save our X-extent
	shr	cx,1			;make # of bytes into # of words
	push	di			;save our pointer to start of buffer
rep	insw				;slam 'em in there
	jnc	SCLR_UNEXCLUDE_CURSOR	;if no extra byte, continue
	in	ax,dx			;otherwise, get odd byte
	stosb				;and put it into destinaion

SCLR_UNEXCLUDE_CURSOR:
	pop	di			;restore pointer to start of buffer
	pop	cx			;restore saved X-extent
	cCall	CursorUnExclude 	;go allow the cursor again

;Now our buffer has the correct scanline in it.  ES:DI points at the first
;pixel to start scanning at.  Perform the scan:

	mov	dx,DirStyle		;get the style of scan

;Now determine the style and direction that we want to do the scan for!

public  SC_GET_DIRECTION
SC_GET_DIRECTION:                                              
	cld				;assume we're scanning left to right
	test	dl,STEP_LEFT		;are we scanning right to left?
	jz	SC_DO_SCAN		;nope, scan left to right
	std				;yes, set direction backward
	add	di,bx			;and set ourselves to the end
					;of the scanline segment

public  SC_DO_SCAN
SC_DO_SCAN:
	mov	ax,OFF_Color		;AL has colour to scan for
	and	al, PlaneBitMask	;strip off any accelerator bits
	test	dl,FIND_COLOR		;are we searching for colour?
	jnz	SC_SCAN_FOR_COLOUR	;yes

SC_SCAN_FOR_NOT_COLOUR:
repe	scasb				;while equal, keep scanning
	jmp	short SC_COMMON 	;go do common stuff

SC_SCAN_FOR_COLOUR:                                                       
repne	scasb				;while not equal, keep scanning

SC_COMMON:
	jcxz	SC_NOT_FOUND		;if not found, it's an error
	mov	ax,di			;otherwise we've found it
	sub	ax,DataOFFSET BigFontInfoTable	;subtract off offset of buffer
	inc	ax			;assume we scanned Hebrew style

;Now AX has the byte offset of the desired pixel offset from where we started
;our scan process.  If we were scanning right to left (Hebrew style), AX has
;the correct value in it.  If we were scanning left to right, we need to
;subtract 2 from AX (one to correct for the extra inc of DI in 
;scasb and one to correct for the increment we just did).  We also need to 
;add on the starting X-coordinate to give the true pixel address within the
;scanline.

	test	dl,STEP_LEFT			;are we stepping left?
	jnz	SCLR_DONE			;yes, we're done!
	dec	ax				;subtract 2
	dec	ax				;
	add	ax,X				;add on starting X

SCLR_DONE:
	jmp	scan_280			;and we're done

SC_NOT_FOUND:
	mov	ax,ERROR_NOT_FOUND		;flag the error
	jmp	scan_280			;and we're done

sEnd    ScanLRSeg
end
