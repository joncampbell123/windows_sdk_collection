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
; Module Name:	PIXEL.ASM
;
; This module contains the Set/Get Pixel routine.
;
;
; Modified to support:
;		. 4 Plane EGA/VGA devices
;		. interleaved scan format for bitmaps
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

	.286

incDrawMode	= 1			;Include control for gdidefs.inc

	.xlist
	include cmacros.inc
	include gdidefs.inc
	include display.inc
	include	mflags.inc
	include ega.inc
	include egamem.inc
	include macros.inc
	.list

	??_out	Pixel


	externA ScreenSelector		;Selector to the screen
	externA SCREEN_W_BYTES		;Screen width in bytes
	externA COLOR_FORMAT		;Color format (will now be 0104h)

	externFP sum_rgb_alt_far	;Color mapping routine


ifdef	EXCLUSION
	externFP exclude_far		;Exclude area from screen
	externFP unexclude_far		;Clear excluded area
endif


;	Define the type flags used to determine which type
;	of scan needs to be performed (color or mono).

COLOR_OP	equ	00000001b
MONO_OP 	equ	MONO_BIT
END_OP		equ	MONO_BIT shl 1 + (1 shl (NUMBER_PLANES))


sBegin	Data

	externB enabled_flag		;Non-zero if output allowed

sEnd	Data
page

createSeg _PIXEL,PixelSeg,word,public,CODE
sBegin	PixelSeg
assumes cs,PixelSeg

	externD  PixelSeg_color_table	;Index --> RGB mapping

	.xlist
	include drawmod2.inc		;Define special ega drawing mode tables
	.list

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
;   The physical device may be the screen, a monochrome bitmap, or a
;   bitmap in our color format.
;
;   There will not be error checking to see if the bitmap is in our
;   color format.  If it isn't, then we'll treat the bitmap as monochrome.
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
;	EGA registers in default state
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
;	exclude_far	(far version of exclude)
;	unexclude_far	(far version of unexclude)
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
	jcxz	pixel_20		;Memory bitmap, skip EGA specific code
	or	al,al
	jz	pixel_disabled		;Disabled, return 8000:0000
	mov	is_device,al		;Non-zero show this is the device


;	This is the device.  The cursor must be excluded from the pixel
;	that will be processed.  The innerloop mask for the device will
;	be set for color.  The width of a plane will be set to 1 to flag
;	to the loop code that this is the EGA (odd widths are illegal).


ifdef	EXCLUSION			;If exclusion required
	mov	cx,x			;Set left
	mov	dx,y			;    top
	mov	si,cx			;    right
	mov	di,dx			;    bottom
	call	exclude_far 		;Exclude the area
endif

	mov	ax,SCREEN_W_BYTES	;Compute starting address of scan
	mul	y			;Better on 286 than explicit shifts
	xchg	ax,si			;SI is offset of the scan
	mov	ax,ScreenSelector	;Set screen segment
	mov	ds,ax
	assumes ds,nothing		;Don't allow addressing off DS

	mov	cl,COLOR_OP		;Show color in case this is GetPixel
	mov	di,1			;Next scan index = 1 indicates display
	jmp	short pixel_70		;  (odd width is illegal)

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
	jnc	pixel_30		;Not in current segment, try next
	add	ax,bx			;Restore correct Y
	sub	dx,cx			;Show correct segment

;----------------------------------------------------------------------------;
; even small bit maps have the interleaved scan line format, so the code for ;
; small and huge bitmaps will be the same henceforth, except that segment    ;
; checking will not be necessary for small maps. 			     ;
;----------------------------------------------------------------------------;



;	This is a memory DC.  If this is a monochrome memory DC, set up
;	the inner loop so that it will terminate after one time through
;	and set the color to be equal to the mono bit in the physical
;	color.	If it is color, set up the inner loop for all planes,
;	same as for the display.
;
;	Also handle modifying Y for huge bitmaps is necessary.
;
;
;	Currently:
;		AX     =  Y coordinate
;		DX     =  Segment bias for huge bitmaps
;		DS:SI --> PDevice

pixel_40:
	mov	di,[si].bmWidthBytes	;Get index to next plane
	mov	bx,di			;  (and Y multiplier)
	mov	cl,MONO_OP		;Assume mono loop
	cmp	wptr [si].bmPlanes,COLOR_FORMAT
	jne	pixel_60		;Not our color format, treat as mono
	errnz	bmBitsPixel-bmPlanes-1

;----------------------------------------------------------------------------;
; we now have 4 plane for the device, so the scan no has to be multed by 4   ;
;----------------------------------------------------------------------------;

	add	ax,ax			;Compute Y to use for offset calculation
	add	ax,ax

pixel_50:
	mov	cl,COLOR_OP		;Show color loop

pixel_60:
	add	dx,wptr [si].bmBits[2]	;Compute segment of the bits
	mov	si,wptr [si].bmBits[0]	;Get offset of the bits
	mov	ds,dx			;Set DS:SI --> to the bits
	assumes ds,nothing

	mul	bx			;Compute start of scan
	add	si,ax			;DS:SI --> start of scanline byte is in



;	Currently:
;		DS:SI --> the bitmap, start of the correct scan
;		DI = Index to next plane of the scan
;		     1 if the display (odd width illegal)
;		CL = looping flag

pixel_70:				;Display code enters here
	mov	ax,x			;Get X coordinate
	mov	bx,ax
	shiftr	ax,3			;Compute byte offset from start of scan
	add	si,ax			;ds:si --> byte of pixel

	and	bx,00000111B		;Get bit mask for bit
	mov	ch,rot_bit_tbl[bx]

	mov	dx,di			;Need index to next plane here
	mov	di,off_lp_draw_mode	;If a drawmode was given
	mov	bx,seg_lp_draw_mode	;  then set the pixel, else
	mov	ax,bx			;  return it's physical color
	or	ax,di
	jnz	pixel_100		;Given, operation is set pixel
	jmp	pixel_200		;Not given, return pixel color

pixel_90:
	jmp	pixel_130		;Just a vector




;	The operation to be performed is SetPixel.  Currently:
;
;		CH    =   bit mask
;		DS:SI --> byte bit is to be set in
;		BX:DI --> physical drawing mode
;		CL    =   loop mask
;		DX    =   Index to next plane
;			  1 if the display


pixel_100:
	mov	es,bx			;ES:DI --> drawmode
	assumes es,nothing

	mov	bx,es:[di].Rop2 	;Get drawing mode to use
	dec	bx			;Make it zero based
	and	bx,000Fh		;Keep it valid
	test	dl,00000001b		;Memory device?
	jz	pixel_90		;  Yes, can't use special drawing modes
page

;	This is the EGA.  Regardless of the raster operation being
;	performed in one or two operations, set up the following
;	registers which will be the same:
;
;	BitMask Register     - To the bit to alter


	mov	dx,EGA_BASE+GRAF_ADDR	;Set bitmask register to bit to
	mov	al,GRAF_BIT_MASK	;  be altered
	mov	ah,ch
	out16	dx,ax

	mov	cl,p_color.SPECIAL	;Need color here
	mov	ah,MM_ALL		;Plane enable / color mask
	test	dm_flags[bx],SINGLE_OK	;Can the ROP occur in one operation?
	jz	pixel_110		;Must do two operations


;	A special drawing mode can be used to set the pixel in one pass.

	mov	al,GRAF_ENAB_SR 	;Enable all planes for set/reset
	out16	dx,ax

	and	cl,dm_pen_and[bx]	;Set correct color for the ROP
	xor	cl,dm_pen_xor[bx]
	and	ah,cl			;Only leave bits of interest
	mov	al,GRAF_SET_RESET
	out16	dx,ax

	mov	al,GRAF_DATA_ROT	;Set Data Rotate reg value
	mov	ah,dm_data_r[bx]
	out16	dx,ax

	xchg	cl,[si] 		;Load latches/write pen into memory

	errnz	MM_C0-C0_BIT
	errnz	MM_C1-C1_BIT
	errnz	MM_C2-C2_BIT
	errnz	MM_C3-C3_BIT


;	Restore the EGA back to the defaults

	mov	ax,GRAF_ENAB_SR 	;Disable all planes for set/reset
	out16	dx,ax
	jmp	short pixel_restore_ega_more





;	The ROP cannot be performed in one pass using the
;	EGA hardware.  It can be performed in two seperate
;	operations to the EGA.	This will be done in the
;	following manner:
;
;	    The color will be used for the Write Plane Enable Mask
;	    after possibly being inverted to sync for the output
;	    mode being used (xor or set)
;
;	    A write with 0's or 1's in drSet mode will occur.
;	    This will set bits to either 1's or 0's as needed.
;
;	    After this write occurs, the color will be inverted
;	    for use as the Write Plane Enable Mask for those
;	    planes which must be XORed to get ~dest.  An XOR
;	    will occur to toggle those bits which must be toggled.
;
;		  Color Destination
;
;	    DPon    0	 ~dest	 for color bits which are 0, xor with 1
;		    1	   0	 for color bits which are 1, set to   0
;
;	    PDna    0	   0	 for color bits which are 0, set to   0
;		    1	 ~dest	 for color bits which are 1, xor with 1
;
;	    DPan    0	   1	 for color bits which are 0, set to   1
;		    1	 ~dest	 for color bits which are 1, xor with 1
;
;	    PDno    0	 ~dest	 for color bits which are 0, xor with 1
;		    1	   1	 for color bits which are 1, set to   1


pixel_110:
	mov	dl,SEQ_DATA		;Will be playing with the sequencer
	xor	cl,dm_pen_xor[bx]	;Invert color if necessary
	and	cl,ah
	mov	al,cl			;3/3/87 kfs  used to be after jz.
	jz	pixel_120		;No bits to SET to 0's or 1's
	out	dx,al			;Enable planes which need enabling


;	The dm_pen_and table is not set correctly for using DR_SET
;	mode.  It is a color instead of a byte consisting entirely
;	of 0's or 1's, for use with Write Mode 2 or the Set/Reset
;	register.  This color will have to be mapped for use with
;	DM_SET mode.


	mov	bl,dm_pen_and[bx]	;Set will use this color, but
	shr	bl,1			;  it must be mapped first
	sbb	bl,bl			;00h if black, FFh if white
	xchg	bl,[si]

pixel_120:
	xor	al,ah			;Now write to the other planes
	jz	pixel_restore_ega	;No bits to XOR
	out	dx,al			;Enable these planes for write

	mov	dl,GRAF_ADDR		;Set up for inversion
	mov	ax,DR_XOR shl 8 + GRAF_DATA_ROT
	out16	dx,ax

	xchg	ch,[si] 		;Invert bit if needed
	errnz	MM_C0-C0_BIT		;This assumption has been made
	errnz	MM_C1-C1_BIT		;  a lot in this sequence
	errnz	MM_C2-C2_BIT
	errnz	MM_C3-C3_BIT

pixel_restore_ega:
	mov	al,MM_ALL		;Enable all planes for writting
	mov	dl,SEQ_DATA
	out	dx,al

pixel_restore_ega_more:
	mov	dl,GRAF_ADDR		;Set DR_SET mode
	mov	ax,DR_SET shl 8 + GRAF_DATA_ROT
	out16	dx,ax

	mov	ax,0FF00h+GRAF_BIT_MASK ;Enable all bits for alteration
	out16	dx,ax
	jmp	short pixel_160 	;All done
page

;	The SetPixel operation is to be performed on a memory bitmap.
;
;	The loop will use the following registers:
;
;	    AH	Inverse of the bitmask
;	    AL	The bitmask
;	    BX	Work
;	    CH	Pen Color
;	    CL	Loop mask
;	    DX	Index to next plane, or 1 if EGA
;	 DS:SI	destination pointer
;	    DI	ROP table address for the rop


pixel_130:
	shl	bx,1			;Set rop_indexes table address
	lea	di,cs:rop_indexes[bx]	;  for the given rop
	mov	ax,cx			;Get mask for bit being altered
	mov	al,ah			;  and also create a mask for
	not	ah			;  ANDing
	mov	ch,p_color.SPECIAL	;Get color for the pixel

pixel_140:
	mov	bl,ch			;Get pen color
	and	bl,cl			;Mask color for current plane
	cmp	bl,1			;'C' if pen is a 0 for this plane
	sbb	bx,bx			;BX = -1 if black, 0 if white
	inc	bx			;BX = 0  if black, 1 if white
	mov	bl,bptr cs:[di][bx]	;Get delta to the drawing function
	add	bx,PixelSegOFFSET pixel_base_address
	jmp	bx			;Invoke the function

pixel_base_address:			;Deltas are computed from here
pixel_is_0:
	and	[si],ah 		;Set pixel to 0
	jmpnext

pixel_is_inverted:
	xor	[si],al 		;Invert destination
	jmpnext

pixel_is_1:
	or	[si],al 		;Set pixel to 1
	jmpnext stop

pixel_is_dest:
	add	si,dx			;--> byte in next plane
	rol	cl,1			;Select next plane
	test	cl,END_OP		;Done with all planes?
	jz	pixel_140		;  Not yet

pixel_160:
	xor	ax,ax			;Set DX:AX = 0:0
	cwd
	jmp	short pixel_300 	;Return 0:0 to show success
page

;	The operation to be performed is get pixel.  The color of the
;	pixel will be returned.  The color of the pixel will be composed
;	from all planes if a color bitmap and the result "AND"ed to
;	produce the mono bit in the physical color (this is the same
;	as bitblt when blting from color to monochrome).
;
;	If this is a monochrome bitmap, then the color will simply be
;	black or white.
;
;	Currently:
;
;		CH     =  bit mask
;		CL     =  loop mask
;		DX     =  index to next plane
;			  1 if physical device
;		DS:SI --> byte bit is to be set in


WHITE	equ	C0_BIT+C1_BIT+C2_BIT+C3_BIT+MONO_BIT+ONES_OR_ZEROS
BLACK	equ	ONES_OR_ZEROS


pixel_200:
	cmp	cl,MONO_OP		;Is this for a mono bitmap?
	jne	pixel_210		;  No, its for color
	xor	ax,ax			;Assume pixel is black
	mov	dx,BLACK*256+000h
	test	[si],ch 		;Is pixel black?
	jz	pixel_300		;  It is, return color in AX:DX
	dec	ax			;  No, set AX:DX for white
	mov	dx,WHITE*256+0FFh
	jmp	short pixel_300



;	Counter the first DEC SI which the EGA will throw in
;	if this is for the EGA.  DX will be 1 if it is.  DX
;	cannot be 0 at this point.


pixel_210:
	xor	bx,bx			;Accumulate the color into BL
	cmp	dx,2			;If EGA, counter the
	adc	si,bx			;  first dec SI

pixel_220:
	test	dl,00000001b		;Is this the physical device?
	jz	pixel_240		;  No, don't play with EGA registers

	dec	si			;Adjust for adding in phoney width
	mov	dx,EGA_BASE + GRAF_ADDR ;--> EGA graphics addr register
	mov	al,GRAF_READ_MAP	;--> Read Map Select Register
	mov	ah,cl			;Set read plane mask
;----------------------------------------------------------------------------;
; we have to convert the plane index 0001,0010,0100 or 1000 to a binary value;
; 00,01,10 or 11.							     ;
;----------------------------------------------------------------------------;

	shr	ah,1
	cmp	ah,100b 		;Set 'C' if not C3
	adc	ah,-1			;Sub -1 only if C3
;----------------------------------------------------------------------------;

	out16	dx,ax
	mov	dx,1			;Show device

pixel_240:
	mov	al,[si] 		;Get the byte
	and	al,ch			;Mask bit of interest
	cmp	al,1			;Set 'C' if bit is 0
	cmc				;Set 'C' if bit is 1
	rcr	bl,1			;Propagate the color
	add	si,dx			;--> next plane
	rol	cl,1			;Select next plane
	test	cl,END_OP		;Done with all planes?
	jz	pixel_220		;  Not yet

	mov	cl,8-NUMBER_PLANES-2	;Make the color index into a
	shr	bl,cl			;  dword index
	errnz	C0_BIT-00000001b	;Must be this for alignment

	mov	ax,wptr PixelSeg_color_table[bx][0]
	mov	dl,bptr PixelSeg_color_table[bx][2]
	call	sum_rgb_alt_far 	;Get the real physical color into DX:AX

pixel_300:
	test	is_device,0FFh
	jz	pixel_exit

ifdef	EXCLUSION
	call	unexclude_far		;Clear any exclude rectangle
endif

pixel_exit:

cEnd
page

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

if	MASMFLAGS and PUBDEFS
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


sEnd	PixelSeg
if	MASMFLAGS and PUBDEFS
	public	pixel_20
	public	pixel_30
	public	pixel_40
	public	pixel_50
	public	pixel_60
	public	pixel_70
	public	pixel_90
	public	pixel_100
	public	pixel_110
	public	pixel_120
	public	pixel_restore_ega
	public	pixel_restore_ega_more
	public	pixel_130
	public	pixel_140
	public	pixel_is_0
	public	pixel_is_inverted
	public	pixel_is_1
	public	pixel_is_dest
	public	pixel_160
	public	pixel_200
	public	pixel_210
	public	pixel_220
	public	pixel_240
	public	pixel_300
	public	rop_indexes
endif
	end
