        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	PIXEL.ASM
;
; This module contains the Set/Get Pixel routine.
;
; Created: 22-Feb-1987
; Author:  **** ***** [*****]
;
; Copyright (c) 1984-1987 Microsoft Corporation
;
; Exported Functions:	Pixel
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
;   Pixel is used to either set a pixel to a given color with the
;   current binary raster operation, or to return the color of the
;   the pixel at the given location.
;
; Restrictions:
;
;-----------------------------------------------------------------------;


	??_out	Pixel

incDrawMode	= 1			;Include control for gdidefs.inc

	.xlist
	include cmacros.inc
	include gdidefs.inc
	include display.inc
	include macros.mac
	.list


	externA ScreenSelector		;Selector to the screen
	externA SCREEN_W_BYTES		;Screen width in bytes
	externA	Y_SHIFT_COUNT


ifdef	EXCLUSION
	externFP exclude_far		;Exclude area from screen
	externFP unexclude_far		;Clear excluded area
endif


sBegin	Data

	externB enabled_flag		;Non-zero if output allowed

sEnd	Data


createSeg _PIXEL,PixelSeg,word,public,CODE
sBegin	PixelSeg
assumes cs,PixelSeg

	externW PixelSeg_interlace_adjust

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
; Pixel
;
;   Set or Get a Given Pixel
;
;   The given pixel is set to the given color or the given pixel's
;   physical color is returned.
;
;   The physical device may be the screen or a monochrome bitmap.
;
;   If lp_draw_mode is NULL, then the physical color of the pixel is
;   returned.  If lp_draw_mode isn't NULL, then the pixel will be set
;   to the physical color passed in, combined with the pixel already
;   at that location according to the raster-op in lp_draw_mode.  Pixel
;   doesn't pay attention to the background mode.
;
;   No clipping of the input value is required.  GDI clips the
;   coordinate before it is passed in, for both Set and Get.
;
; Entry:
; Returns:
;	DX:AX = physical color if get pixel
;	DX:AX = positive if no error and set was OK.
; Error Returns:
;	DX:AX = 8000:0000H if error occured
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	exclude_far
;	unexclude_far
; History:
;	Sat 31-Oct-1987 00:21:06 -by-  **** ***** [*****]
;	Added clipping of the (X,Y)
;
;	Tue 18-Aug-1987 14:50:37 -by-  **** ***** [*****]
;	Added test of the disabled flag.
;
;	Tue 03-Mar-1987 20:42:07 -by-  **** ****** [******]
;	Moved a mov instruction in EGA ROP handling code.
;
;	Sun 22-Feb-1987 16:29:09 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


cProc	Pixel,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	parmD	lp_device		;Pointer to device
	parmW	x			;X coordinate of pixel
	parmW	y			;Y coordinate of pixel
	parmD	p_color 		;Physical color to set pixel to
	parmD	lp_draw_mode		;Drawing mode to use, or null if Get

	localB	is_device		;set non-zero if the device

cBegin
	mov	al,enabled_flag 	;Load this before trashing DS
	lds	si,lp_device		;--> physical device
	assumes ds,nothing


;	If the X or Y coordinate is outside the surface of the bitmap
;	or device, return an error code of 8000:0000.  The test will
;	be performed as an unsigned compare to test for the range of
;	0:n-1.

	mov	cx,x
	cmp	cx,[si].bmWidth
	jae	pixel_clipped
	mov	cx,y
	cmp	cx,[si].bmHeight
	jae	pixel_clipped

	mov	cx,[si].bmType		;Get device type
	jcxz	pixel_20		;Memory bitmap, skip cursor exclusion
	or	al,al
	jz	pixel_disabled		;Disabled, return 8000:0000
	mov	is_device,al		;Non-zero show this is the device


;	This is the device.  The cursor must be excluded from the pixel
;	that will be processed.

ifdef	EXCLUSION			;If exclusion required
	mov	cx,x			;Set left
	mov	dx,y			;    top
	mov	si,cx			;    right
	mov	di,dx			;    bottom
	call	exclude_far 		;Exclude the area
endif

	mov	ax,y			;Compute base address of the scan
	mov	bx,ax
	and	bx,00000011b
	shl	bx,1
	mov	si,PixelSeg_interlace_adjust[bx]
	mov	cl,Y_SHIFT_COUNT	;bias y coordinate for interlace
	shr	ax,cl
	mov	bx,SCREEN_W_BYTES
	mul	bx
	add	si,ax	

	mov	ax,ScreenSelector	;Set screen segment
	mov	ds,ax
	assumes ds,nothing		;Don't allow addressing off DS
	jmp	short pixel_70

pixel_clipped:
pixel_disabled:
	mov	dx,8000h
	xor	ax,ax
	jmp	pixel_exit


;	The device is a memory bitmap.	 If it is a huge bitmap,
;	special processing must be performed to compute the Y address.

pixel_20:
	mov	is_device,cl		;Show this is a bitmap
	mov	ax,y			;Need Y coordinate a few times
	xor	dx,dx			;Set segment bias to 0
	mov	cx,[si].bmSegmentIndex	;Is this a huge bitmap?
	jcxz	pixel_40		;  No



;	This is a huge bitmap.	Compute which segment the Y coordinate
;	is in.	Assuming that no huge bitmap will be bigger than two
;	or three segments, iteratively computing the value would be
;	faster than a divide, especially if Y is in the first segment
;	(which would always be the case for a huge color bitmap that
;	didn't have planes >64K).


	mov	bx,[si].bmScanSegment	;Get # scans per segment

pixel_30:
	add	dx,cx			;Show in next segment
	sub	ax,bx			;See if in this segment
	jnc	pixel_30		 ;Not in current segment, try next
	add	ax,bx			;Restore correct Y
	sub	dx,cx			;Show correct segment


;	This is a memory DC.  If this is a monochrome memory DC, set up
;	the inner loop so that it will terminate after one time through
;	and set the color to be equal to the mono bit in the physical
;	color.
;
;	Currently:
;		AX     =  Y coordinate
;		DX     =  Segment bias for huge bitmaps
;		DS:SI --> PDevice

pixel_40:
	mov	bx,[si].bmWidthBytes	;Get Y multiplier
	add	dx,wptr [si].bmBits[2]	;Compute segment of the bits
	mov	si,wptr [si].bmBits[0]	;Get offset of the bits
	mov	ds,dx			;Set DS:SI --> to the bits
	assumes ds,nothing

	mul	bx			;Compute start of scan
	add	si,ax			;DS:SI --> start of scanline byte is in



;	Currently:
;		DS:SI --> the bitmap, start of the correct scan

pixel_70:				;Display code enters here
	mov	ax,x			;Get X coordinate
	mov	bx,ax
	shiftr	ax,3			;Compute byte offset from start of scan
	add	si,ax			;ds:si --> byte of pixel

	and	bx,00000111B		;Get bit mask for bit
	mov	ch,rot_bit_tbl[bx]

	mov	di,off_lp_draw_mode	;If a drawmode was given
	mov	bx,seg_lp_draw_mode	;  then set the pixel, else
	mov	ax,bx			;  return it's physical color
	or	ax,di
	jnz	pixel_100		 ;Given, operation is set pixel
	jmp	pixel_200		 ;Not given, return pixel color
page

;	The operation to be performed is SetPixel.  Currently:
;
;		CH    =   bit mask
;		DS:SI --> byte bit is to be set in
;		BX:DI --> physical drawing mode


pixel_100:
	mov	es,bx			;es:di --> drawmode
	assumes es,nothing

	mov	bx,es:[di].Rop2 	;Get drawing mode to use
	dec	bx			;Make it zero based
	and	bx,000Fh		;Keep it valid
page

;	The SetPixel operation is to be performed on a memory bitmap.
;
;	The loop will use the following registers:
;
;	    AH	Inverse of the bitmask
;	    AL	The bitmask
;	    BX	Work
;	    CH	Pen Color
;	    CL	mask for getting pixel color
;	 DS:SI	destination pointer
;	    DI	ROP table address for the rop


	shl	bx,1			;Set rop_indexes table address
	lea	di,cs:rop_indexes[bx]	;  for the given rop
	mov	ah,ch			;Get mask for bit being altered
	mov	al,ah			;  and also create a mask for
	not	ah			;  ANDing
	mov	cl,MONO_BIT

	mov	bl,p_color.SPECIAL	;Get color for the pixel
	and	bl,cl			;Mask color
	cmp	bl,cl			;'C' if pen is a 0
	sbb	bx,bx			;BX = -1 if black, 0 if white
	inc	bx			;BX = 0  if black, 1 if white
	mov	bl,bptr cs:[di][bx]	;Get delta to the drawing function
	add	bx,PixelSegOFFSET pixel_base_address
	jmp	bx			;Invoke the function

pixel_base_address:			;Deltas are computed from here
pixel_is_0:
	and	[si],ah 		;Set pixel to a 0
	jmpnext

pixel_is_inverted:
	xor	[si],al 		;Invert destination
	jmpnext

pixel_is_1:
	or	[si],al 		;Set pixel to a 1
	jmpnext stop

pixel_is_dest:
	xor	ax,ax			;Set dx:ax = 0:0
	cwd
	jmp	short pixel_300 	;Return 0:0 to show success
page

;	The operation to be performed is get pixel.  The color of the
;	pixel will be returned (black or white).
;
;	Currently:
;
;		CH    =   bit mask
;		DS:SI --> byte bit is to be set in


WHITE	equ	C0_BIT+C1_BIT+C2_BIT+C3_BIT+MONO_BIT+ONES_OR_ZEROS
BLACK	equ	ONES_OR_ZEROS


pixel_200:
	xor	ax,ax			;Assume pixel is black
	mov	dx,BLACK*256+000h
	test	[si],ch 		;Is pixel black?
	jz	pixel_300		;  It is, return color in AX:DX
	dec	ax			;  No, set AX:DX for white
	mov	dx,WHITE*256+0FFh


pixel_300:
	test	is_device,0FFh
	jz	pixel_exit

ifdef	EXCLUSION			;If exclusion
	call	unexclude_far		;Clear any exclude rectangle
endif

pixel_exit:

cEnd



;	make_rop - Make a ROP For Pixel
;
;	MakeROP makes a raster operation for the pixel routine.
;	The raster operation generated is based on the following
;	table which shows the ROP broken down into the boolean
;	result for each plane based on what the pen color is for
;	the plane.
;
;
;
;		Color	  Result			Color	  Result
;
;	DDx	  0	    0			DPa	  0	    0
;		  1	    0				  1	   dest
;
;	DPon	  0	  ~dest 		DPxn	  0	  ~dest
;		  1	    0				  1	   dest
;
;	DPna	  0	   dest 		D	  0	   dest
;		  1	    0				  1	   dest
;
;	Pn	  0	    1			DPno	  0	    1
;		  1	    0				  1	   dest
;
;	PDna	  0	    0			P	  0	    0
;		  1	  ~dest 			  1	    1
;
;	Dn	  0	  ~dest 		PDno	  0	  ~dest
;		  1	  ~dest 			  1	    1
;
;	DPx	  0	   dest 		DPo	  0	   dest
;		  1	  ~dest 			  1	    1
;
;	DPan	  0	    1			DDxn	  0	    1
;		  1	  ~dest 			  1	    1


make_rop macro	l,ops

ifdef	PUBDEFS
pixel_&l&:				;;& keeps grep from matching
endif
irp x,<ops>
ifidn	<&&x>,<0>
	db	pixel_is_0-pixel_base_address
endif
ifidn	<&&x>,<1>
	db	pixel_is_1-pixel_base_address
endif
ifidn	<&&x>,<~dest>
	db	pixel_is_inverted-pixel_base_address
endif
ifidn	<&&x>,<dest>
	db	pixel_is_dest-pixel_base_address
endif
endm
endm


rop_indexes	label	byte
make_rop DDx,<0,0>
make_rop DPon,<~dest,0>
make_rop DPna,<dest,0>
make_rop Pn,<1,0>
make_rop PDna,<0,~dest>
make_rop Dn,<~dest,~dest>
make_rop DPx,<dest,~dest>
make_rop DPan,<1,~dest>
make_rop DPa,<0,dest>
make_rop DPxn,<~dest,dest>
make_rop D,<dest,dest>
make_rop DPno,<1,dest>
make_rop P,<0,1>
make_rop PDno,<~dest,1>
make_rop DPo,<dest,1>
make_rop DDxn,<1,1>


ifdef	PUBDEFS
	include pixel.pub
endif

sEnd	PixelSeg
end
