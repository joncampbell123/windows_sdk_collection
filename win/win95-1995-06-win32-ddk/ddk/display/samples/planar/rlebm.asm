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

	page,132
;----------------------------------------------------------------------------;
;		RUN LENGTH ENCODED (RLE) BITMAPS			     ;
;		--------------------------------			     ;
;									     ;
; A RLE bitmap consists of two parts - a information header block and a coded;
; buffer which we will refer to as the RLE bitmap.			     ;
;        								     ;
; Unlike normal bitmaps the RLE bitmap has the encoding of the information in;
; a bitmap. The incoding consists of a series of records of variable lengths.;
; With the first byte of every record designating the type of the record.    ;
;									     ;
; The RLEs can be recorded in 8 bit mode or in 4 bit mode. In 8 bit mode, all;
; indices are byte valued, whereas, in 4 bit mode the indices are specified  ;
; in 4 bits and a byte will hold 2 of these indices.		             ;
;									     ;
; The various types of records are classified below:			     ;
;									     ;
; Encoding of a Runlength:                                                   ;
; ----------------------------						     ;
; FIRSTBYTE:  COUNT which is >= 1.				             ;
; SECONDBYTE: INDEX.							     ;
;        	  							     ;
;            (8 BIT FORMAT)						     ;
;	     This is just a 2 byte record and encodes COUNT number of pels   ;
;	     in the bitmap having the same color index. All indexes in the   ;
;	     RLE records are with respect to the color table in the info hea-;
;	     -der.							     ;
;									     ;
;            (4 BIT FORMAT)						     ;
;            The second byte contains 2 color indices in 4 bits each and the ;
;            first byte spcifies the number of pels of the alternating seq.  ;
;            The color for the pels are obtained by alternately choosing the ;
;            high and the low nibble, till COUNT number of pels are done.
;									     ;
; Unencoded run:                                                             ;
; ------------------							     ;
; FIRSTBYTE: = 0.					                     ;
; SECONDBYTE: COUNT >= 3. Followed by,					     ;
;									     ;
; (8 BIT FORMAT)							     ;
; a string of BYTEs specifying the color indices, this string must be even, &;
; be padded by a zero if the COUNT is ODD.			             ;
;									     ;
; (4 BIT FORMAT)							     ;
; a string of BYTES, each byte providing two 4 bit indices. The string must  ;
; be even. There may be a filler byte and there maybe a byte with an index   ;
; in just the left nibble.                                                   ;
;									     ;
;  Delta:                                                                    ;
;  ----------								     ;
;  FIRSTBYTE:   = 0							     ;
;  SECONDBYTE:  = 2							     ;
;  THIRDBYTE:	= DELTA_X (unsigned), >= 0				     ;
;  FOURTHBYTE:  = DELTA_Y (unsigned), >= 0, but not both 0		     ;
;									     ;
;	      This is relative-jump record and implies that the decoding of  ;
; 	      the next RLE record is to start at (DELTA_X,DELTA_Y) wrt to the;
;	      current position in the bitmap.			             ;
;									     ;
;  EOL (end-of-line):                                                        ;
;  FIRSTBYTE:	= 0						             ;
;  SECONDBYTE:  = 0							     ;
;									     ;
;		Signifies the end of the current line in the bitmap. The dec-;
;	        -oding of the next record is to be done at the start of the  ;
;	 	next line.						     ;
;									     ;
;  EORLE (end-of-RLE):                                                       ;
;  FIRSTBYTE:	= 0						             ;
;  SECONDBYTE:  = 1							     ;
;									     ;
;		Signifies the end of the encoding.			     ;
;								     	     ;
;									     ;
;               	  RLE Related API functions			     ;
;		  	  -------------------------			     ;
;									     ;
; RLEs are not treated separately from DIBs, the biCompression field is used ;
; to determine a RLE DIB from a normal DIB, it will contain BI_RLE4 or       ;
; BI_RLE8 if bits are RLE,                                                   ;
;								             ;
; A point to note here is that the direction of Y progress on the RLE is     ;
; reverse from windows convention (just as in PM). This means that the first ;
; record of the RLE is for the bottom left corner of the source or destinati-;
; -on rectangle. We traverse the rectangle from top to bottom on the device  ;
; or bitmap but bottom to top on the RLE.			             ;
;----------------------------------------------------------------------------;

FIREWALLS = 0
ifdef	HERCULES
	CGAHERC = 1
endif
ifdef	IBM_CGA
	CGAHERC = 1
endif


ifdef	EGA_MONO
	MONO = 1
endif

ifdef	VGA_MONO
	MONO = 1
endif

ifdef	EGA_HIBW
	MONO = 1
endif

; for the MONO family of drivers we will not be using any of the device
; registers (as the mouse code assumes that no one's going to touch the
; device registers. That means that to do clipped bytes we will have to
; do it the way it is done for CGAHERC drivers. So we will create a special
; symbol for CGAHERC and MONO drivers called CGAHERCMONO. If this label is
; set, we will not touch any of the registers and do clipping by reading
; in the byte, changing the unmasked pels and writing the byte back.

ifdef	CGAHERC
	CGAHERCMONO = 1
endif

ifdef	MONO
	CGAHERCMONO = 1
endif

incDrawMode	= 1			;include GDI DRAWMODE definition

	.xlist
	include	cmacros.inc
	include	gdidefs.inc
	include	njmp.inc
	include	macros.inc
	include display.inc
  	include mflags.inc

IFNDEF	CGAHERC
	include	ega.inc
	include	egamem.inc
ELSE
	EXCLUSION = 1
	NUMBER_PLANES = 1
ENDIF
	.list

; The following structure should be used to access high and low
; words of a DWORD.  This means that "word ptr foo[2]" -> "foo.hi".

LONG    struc
lo      dw      ?
hi      dw      ?
LONG    ends

FARPOINTER      struc
off     dw      ?
sel     dw      ?
FARPOINTER      ends

;----------------------------------------------------------------------------;

	externA	__NEXTSEG		;segment offset to next segment
	externA	ScreenSelector		;display memory selector
	externA SCREEN_W_BYTES		;width of a screen scan

IFDEF	CGAHERC
	externA	Y_SHIFT_COUNT		; level of interleaving factor
ENDIF

	externNP IntersectRects		;in .\discreen.asm

IFDEF	CGAHERC
	externNP device_get_dest_scan	;in .\discreen.asm
ENDIF
        externFP sum_RGB_alt_far        ;get nearest color

ifdef EXCLUSION
	externFP exclude_far		;exclude the cursor from the blt area
        externFP unexclude_far          ;allow cursor to be drawn
endif

if NUMBER_PLANES eq 4
BITS_IN_PCOLOR	equ	00001111b	;resolution of display device.
else
BITS_IN_PCOLOR	equ	00001111b	;resolution of display device.
endif
OPAQUE		equ	2		;code for opaque background mode

	public	surface_is_memory
	public	start_work
	public	set_RLE_bits
	public	xlate_next_color
	public	set_have_addresses
	public	have_mem_set_pel_addresses
	public	have_mem_offset_calc_address
	public	set_initializations_done
	public	set_decode_RLE
	public	set_multipel_update_X_Y
	public	set_encoded_run
	public	set_onepel_update_X_Y
	public	set_EOL
	public	set_EORLE
	public	RLEset_end
	public	get_RLE_bits
	public	get_table_for_color
	public	table_for_color
	public	color_table_done
	public	get_RLE_init_done
	public	get_find_correct_segment
	public	get_found_correct_segment
	public	get_do_calc_for_small
	public	get_rect_code_loop
	public	get_scan_code_loop
	public	initial_state
	public	initial_to_encoded
	public	encode_encode
	public	encode_absolute
	public	encode_overflow
	public	initial_to_absolute
	public	absolute_absolute
	public	absolute_encode
	public	absolute_overflow
	public	encode_to_scan_end
	public	absolute_to_scan_end
	public	solitary_pel_at_end
	public	scan_epilogue
	public	same_display_surface_seg
	public	get_end_of_RLE
	public	RLEget_end
	public	RLEerror
	public	RLEend
	public	set_start_on_lft_edge
	public	set_clip_type_2_or_3
	public	set_get_segment_end
	public	set_absolute_mode
	public	set_4bpp_onepel_segment
	public	set_encoded_run
	public	set_multipel_loop
	public	set_onepel_first
	public	set_onepel_loop
	public	set_onepel_last
	public	set_get_offset_in_segment
	public	set_get_huge_segment_loop
	public	set_got_huge_segment
	public	obtain_pel_from_buffer
	public	fill_get_buffer
	public	set_get_segment	
	public	set_multipel_segment	
	public	set_onepel_segment	
	public	dev_set_pels_partial 
	public	dev_set_pels_full 
	public	mem_set_pels_partial 
	public	monomem_set_pels_partial 
	public	mem_set_pels_full 
	public	monomem_set_pels_full 
	public	set_get_bit_mask 
	public	set_get_byte_mask 
	public	set_small_start_offset 
	public	set_huge_start_offset 
	public	get_next_pel	
	public	dev_color_get_pels_masked 
	public	dev_color_get_pels 
	public	dev_mono_get_pels_masked 
	public	dev_mono_get_pels 
	public	mem_color_get_pels_masked 
	public	mem_color_get_pels 
	public	mem_mono_get_pels_masked 
	public	mem_mono_get_pels 
	public	color_get_pel_from_buffer 
	public	mono_get_pel_from_buffer 

IFNDEF	CGAHERCMONO
	public	dev_init_write_2 
	public	dev_reinit
ENDIF

	public	update_set_src_ptr_8_bit
	public	update_set_src_ptr_4_bit
	public	get_next_abs_mode_index_4_bit
	public	decide_4_bit_strategy_4_bit
	public	same_color_strategy
	public	get_encoded_index_4_bit
;	 public  paint_background
;	 public  paint_background_ret
	public	mono_munge
        public  mono_munge_ret
        public  ReadRLE
        public  StoreRLE
        public  StoreRLEIncCount

sBegin	Data

	externB	enabled_flag		;screen enabled or not

sEnd	Data

createSeg  _DIMAPS,DIMapSeg,word,public,code
sBegin	DIMapSeg
	assumes	cs,DIMapSeg
	assumes	ds,Data
	assumes	es,nothing

externB DIMapSeg_color_table

;----------------------------------------------------------------------------;
; below we have a couple of tables holding address of routines used for      ;
; get_RLE_bits.						 		     ;
;----------------------------------------------------------------------------;

FetchFromBuffer	label	word

	dw	DIMapSegOFFSET color_get_pel_from_buffer
	dw	DIMapSegOFFSET mono_get_pel_from_buffer

FetchIntoBuffer	label	word

	dw	DIMapSegOFFSET mem_color_get_pels_masked
	dw	DIMapSegOFFSET mem_mono_get_pels_masked
	dw	DIMapSegOFFSET dev_color_get_pels_masked
	dw	DIMapSegOFFSET dev_mono_get_pels_masked


;----------------------------------------------------------------------------;


cProc	 RLEBitBlt,<FAR,PUBLIC,WIN,PASCAL>,<si,di,es>

	parmD	lpPDevice		;pointer to display surface descriptor
	parmW	DstX			;X origin on display surface
	parmW	DstY			;Y origin on display surface
	parmW	DstXE			;X extent of display rectangle
	parmW	DstYE			;Y extent on display rectangle
	parmW	StartScan		;start of the band of RLE wrt whole
	parmW	NumScans		;no of scans in RLE band
	parmW	SetGet			;set or get code
	parmD	lpClipRect		;the clipping rectangle 
	parmD	lpDrawMode		;ptr to drawmode structure
	parmD	lpRLEInfo		;the RLE info block
	parmD	lpRLEbits		;pointer to RLE encoded buffer
	

	localD	lpSurface		;pointer to start of surface
	localW	ClipLft			;left of clip rectangle
	localW	ClipRgt			;right of clip rectangle
	localW	ClipTop			;top of clip rectangle
	localW	ClipBot			;bottom of the clip rectangle
	localW	ClipXE			;X extent of clip rect
	localW	ClipYE			;Y extent of clip rect
	localW	X			;current X on display surface
	localW	Y			;current Y on display surface
	localB	num_planes		;number of planes
	localV	WorkRect,8		;a work rectangle structure

	localB	SurfaceFlags		;defines the following flags

RLE_MONO	equ	00000001b	;monochrome display surface
RLE_DEVICE	equ	00000010b	;the display surface is the screen
RLE_HUGE	equ	00000100b	;the display surface size is > 64k
RLE_CURSOR_OFF  equ     00001000b	;cursor has been disabled
RLE_8_BIT	equ	00010000b	;8 bit encoded RLE
RLE_CLIPED      equ     00100000b       ;clipping is required

	localW	width_scan		;the width of a scan
	localW	next_scan		;total bytes in a scan
	localW	prev_offset		;offset of last scan in a segment
	localW	ScansPerSegment		;number of scans in huge segment
	localW	NibbleToggle		;decides which nibble to use.
	localB	special_4bpp_mask	;mask for encoded 4bit rle runs
	localV	color_xlate,256		;color translation table

	localW	set_partial_pels	;addr of fn to set < 8 pels in a byte
	localW	set_full_pels		;addr of fn to set 8 pels in a byte
	localW	set_get_start_offset	;addr of fn to get offset from x,y
	localW	get_pel_from_buffer     ;addr of fn to access pel buffer
	localW	fill_pel_buffer		;addr of fn to fillup pel buffer
	localW	saved_get_pel_addr	;a copy of the above address
	localW	PelsPerPaintScan	;x extent for background paint
	localW	munge_proc		;color replication for mono driver
	localW  update_set_src_ptr	;addr of fn. to update src pointer 
	localW	get_next_abs_mode_index	;addr of fn. to get next abs mode index
	localW	get_encoded_index	;addr of fn. to get encode index value
	localW	decide_4_bit_strategy	;decides strategy to adopt
	localW	get_next_index_byte	;fn to get next byte
	localW	update_get_dst_ptr	;fn to update dst ptr for filler byte
        localW  fnStoreRLE               ;function to write bytes to RLE buffer
        localW  fnUpdateAbsCount         ;function to update abs count field

	localB	BackColor		;background color saved here
	localB	PelPlane0		;buffer for byte in plane 0
	localB	PelPlane1		;buffer for byte in plane 1
	localB	PelPlane2		;buffer for byte in plane 2

if NUMBER_PLANES eq 4
	localB	PelPlane3		;buffer for byte in plane 3
endif
        localD  dwSizeImage             ;count of bytes writen to RLE buffer

        localB  BufferedPelCount        ;number of planes in buffer

        localD  ptrCount                ;pointer to count fld of unencoded run
	localB	LastPel			;the index of the last pel
	localB	count			;no of pels of same color
	localB	expected_return		;no of pels expected from 1 get_pel call

cBegin

	
; retrieve the clip rectangle parameters into local variables

	les	di,lpClipRect		;the clipping rectangle
	assumes	es,nothing

	mov	ax,es:[di].left		;get left x
	mov	bx,es:[di].right	;get right x
	mov	ClipLft,ax
	mov	ClipRgt,bx
	sub	bx,ax			;get the extent
	mov	ClipXE,bx 
	mov	ax,es:[di].top		;get top y
	mov	bx,es:[di].bottom	;get bottom y
	mov	ClipTop,ax
	mov	ClipBot,bx
	sub	bx,ax			;get y extent
        mov     ClipYE,bx

        xor     bl,bl                   ;will hold surface flags

; test for a totaly visible dest rectangle

        or      bl,RLE_CLIPED           ;assume clipping

        mov     ax,DstX
        cmp     ax,ClipLft
        jl      short @f
        add     ax,DstXE
        cmp     ax,ClipRgt
        jg      short @f

        mov     ax,DstY
        cmp     ax,ClipTop
        jl      short @f
        add     ax,DstYE
        cmp     ax,ClipBot
        jg      short @f

        and     bl,not RLE_CLIPED

@@:

; find out information about the surface

	les	di,lpPDevice		;display surface descriptor
	mov	cx,es:[di].bmType	;see the type of device
	jcxz	surface_is_memory	;display surface is memory

; the display surface is the screen

; if the enabled flag is not set exit with error

	mov	al,enabled_flag
	or	al,al			;is creen enabled
	njz	RLEerror		; no

	or	bl,RLE_DEVICE		;indicates surface is device, not memory

; set up number of planes and width of a scan for the device

	mov	al,NUMBER_PLANES	;get number of planes for surface
	mov	num_planes,al
	xor	ah,ah
	mov	width_scan,SCREEN_W_BYTES;width of screem in bytes
	mov	next_scan,SCREEN_W_BYTES;total width including all planes
	dec	al			;will be 0 for monochrome driver
	cmp	al,1			;carry will be set for monochrome
	sbb	al,al			;al 0 for color
	and	al,RLE_MONO		;mono bit in al set for monochrome
	or	bl,al			;set mnochrome flag

; set up the address to the first byte of the screen

	mov	ax,ScreenSelector	;segment address of video memory
	mov	seg_lpSurface,ax
	mov	off_lpSurface,0		;starts at offset 0
	jmp	short start_work

surface_is_memory:

; set up the color/monochrome flag

	mov	al,es:[di].bmPlanes	;get the number of planes
	mov	num_planes,al		;save it
	dec	al			;will be 0 if numplanes = 1
	cmp	al,1			;carry set for monochrome
	sbb	al,al			;al = 0 for color, 0ffh for mono
	and	al,RLE_MONO		;mono bit set for monochrome only
	or	bl,al			;gather the monochrome flag

; get the address to the start of the memory surface

        mov     ax,es:[di].bmBits.off
        mov     off_lpSurface,ax        ;save segment
        mov     ax,es:[di].bmBits.sel
        mov     seg_lpSurface,ax        ;save segment

; get the width of a scan

	mov	ax,es:[di].bmWidthBytes
	mov	width_scan,ax		;save
	mov	cl,num_planes		;get the number of planes
	xor	ch,ch
	mul	cx			;ignore dx
	mov	next_scan,ax

; test for huge bitmaps

	mov	cx,es:[di].bmSegmentIndex
	jcxz	start_work		;non zero for huge maps

; we have a huge bitmap.

	or	bl,RLE_HUGE		;set HUGE bitmap flag
	mov	ax,es:[di].bmScanSegment
	mov	ScansPerSegment,ax	;save the number of scans in a segment

; set up the offset to the last scan in a segment. Will be needed by get_RLE

	mov	ax,es:[di].bmFillBytes	;filler bytes at the end
	add	ax,next_scan		;width of last scan
	neg	ax			

; if we treat ax as an unsigned quantity we have the offset to the start of the
; last scan in a segment

	mov	prev_offset,ax		;save it

start_work:

; now set flag bit to indicate 8 bit RLE if it is so.

	les	di,lpRLEInfo		;es:di points to rle info block
	cmp	bptr es:[di].biCompression, BI_RLE8
        jne     @f                      ;will treat as 4 bit RLE
	or	bl,RLE_8_BIT		;set flag to indicate 8 bit RLE
@@:
	mov	SurfaceFlags,bl		;save the surface type flags
	
;----------------------------------------------------------------------------;
; The Y orientation of an RLE is inverse of the windows convention.This thus ;
; means that we start setting bits to/geting bits from the display surface   ;
; at (DstX,DstY+DstYE-1). Set up the start X and Y coordinates.		     ;
;----------------------------------------------------------------------------;

	mov	ax,DstX			;start X coordinate
	mov	X,ax			;save it
	mov	ax,DstY
	add	ax,DstYE		;add in Y extent
	dec	ax			;start at the bottom most scan
	sub	ax,StartScan		;the place where this band goes
	mov	Y,ax

; to us the yExt of the BLT is jut NumScans

	mov	ax,NumScans		;no of cans in the RLE band
	mov	DstYE,ax		;save it


;----------------------------------------------------------------------------;
; if 'SetGet' is 0 we do a set bits from an RLE encoded buffer to the rect   ;
; on the display surface. If it is 1 we do a Get bits from the surface and   ;
; RLE encode them into the buffer.				             ;
;----------------------------------------------------------------------------;

	cmp	SetGet,0		;is it a set bits call
	jz	set_RLE_bits		;yes
	jmp	get_RLE_bits		;we already had ensured it is 0 or 1


;----------------------------------------------------------------------------;
;			SET_RLE_BITS					     ;
;----------------------------------------------------------------------------;
set_RLE_bits:

; depending on 4 bit or 8 bit encoding set up function address variables

	mov	al,SurfaceFlags		;load the flag byte
	test	al,RLE_8_BIT		;8 bit encoding
	jnz	@f			;yes

; set up function addresses for 4 bit encoding.

        mov     ax,DIMapSegOFFSET get_encoded_index_4_bit
	mov	[get_encoded_index],ax	;function to get index for encoded run
	mov	ax,DIMapSegOFFSET decide_4_bit_strategy_4_bit
	mov	[decide_4_bit_strategy],ax
	mov	ax,DIMapSegOFFSET update_set_src_ptr_4_bit
	mov	[update_set_src_ptr],ax
	mov	ax,DIMapSegOFFSET get_next_abs_mode_index_4_bit
	mov	[get_next_abs_mode_index],ax
	mov	ax,16			;full size of color table
	jmp	short color_table

@@:

; set up function addresses for 8 bit encoding.

        mov     ax,DIMapSegOFFSET get_encoded_index_8_bit
	mov	[get_encoded_index],ax
	mov	ax,DIMapSegOFFSET same_color_strategy
	mov	[decide_4_bit_strategy],ax
	mov	ax,DIMapSegOFFSET update_set_src_ptr_8_bit
	mov	[update_set_src_ptr],ax
        mov     ax,DIMapSegOFFSET ReadRLE
	mov	[get_next_abs_mode_index],ax
	mov	ax,256			;full size of color table

color_table:

; create a color translate table, converting the logical color specified in 
; the RLE info block into color indices in the device format.
; ES:DI point to info block

	mov	cx,wptr es:[di].biClrUsed;get the number of colors used
	jcxz	create_color_table		;ax has the ize of the table

; cx .ne. 0 thus color table size is not the default of 156

	min_ax	cx			;get 256 or CX whichever is lower

create_color_table:
	mov	cx,ax			;the size of the color table passed tous

; to reach to the color table simply add the size of the header block.

	
	add	di,wptr es:[di].biSize	;ignore hiword of size
	xor	si,si			;index into translate table

; for monochrome destination we will use only the mono bit as the pel value,
; but we will replicate this bit in all 8 bits.

	mov	ax,DIMapSegOFFSET mono_munge_ret    
	test	SurfaceFlags,RLE_MONO	;is it mono
	jz	save_munge_addr		;we have a color blt
	mov	ax,DIMapSegOFFSET mono_munge	      

save_munge_addr:
	mov	[munge_proc],ax		;save address, NOP for color

xlate_next_color:
	mov	ax,es:[di][0]		;get blue and green
	mov	dl,es:[di][2]		;get red
	add	di,size RGBQuad 	;update to next entry
	xchg	al,dl			;get red into al, blue into dl
	push	cx
	push	bx			;save count and mask
	call	sum_RGB_alt_far		;returns index in DH
	pop	bx
	pop	cx			;restore

	call	[munge_proc]		;do transformation for mono only

; DH has either the mono bit in the lsb position or the color bits depending
; on the type of the display surface.

	mov	color_xlate[si],dh	;save translated value
	inc	si			;point to next entry
	loop	xlate_next_color

;----------------------------------------------------------------------------;
; set up the EGA registers for write mode 2 and for mono chrome drivers just ;
; enable plane 0.							     ;
;----------------------------------------------------------------------------;

	test	SurfaceFlags,RLE_DEVICE	;is surface the screen
	jz	set_have_addresses	;get addresses of the relevant funtion

; disable the cursor from the clipping rectangle and set cursor disabled flag on

ifdef	EXCLUSION

	mov	cx,ClipLft		;X1
	mov	si,ClipRgt		;X2
	mov	dx,ClipTop		;Y1
	mov	di,ClipBot		;Y2
	call	exclude_far		;exclude cursor from clip rectangle
	or	SurfaceFlags,RLE_CURSOR_OFF

endif

; set up the routine addresses for the case where the display surface is a
; device.

	mov	ax,DIMapSegOFFSET dev_set_pels_partial
	mov	set_partial_pels,ax	;set bits in partial bytes
	mov	ax,DIMapSegOFFSET dev_set_pels_full
	mov	set_full_pels,ax	;set bits in complete bytes

IFNDEF  CGAHERC
	mov	ax,DIMapSegOFFSET set_small_start_offset
ELSE
	mov	ax,DIMapSegOFFSET set_device_start_offset
ENDIF

	mov	set_get_start_offset,ax	;calculate start offset

IFNDEF	CGAHERCMONO

	les	di,lpSurface		;we need screen selector in ES
	assumes	es,EGAMem		;to access shadow memory
	mov	bl,SurfaceFlags		;the flag byte
	and	bl,RLE_MONO		;bl will be 0 for color
	call	dev_init_write_2	;initialize device to write mode 2

ENDIF

	jmp	short set_initializations_done

set_have_addresses:

; set up the calling function addresses for memory maps

; assume color surfacw which isn't huge

	mov	ax,DIMapSegOFFSET mem_set_pels_partial
	mov	bx,DIMapSegOFFSET mem_set_pels_full
	test	SurfaceFlags,RLE_MONO	;is it monochrome
	jz	have_mem_set_pel_addresses

; we have monochrome memory surface, get relevant function addresses

	mov	ax,DIMapSegOFFSET monomem_set_pels_partial
	mov	bx,DIMapSegOFFSET monomem_set_pels_full

have_mem_set_pel_addresses:

	mov	set_partial_pels,ax	;set pels in partial byte
	mov	set_full_pels,bx	;set pels in complete byte

; set correct offset calculation routine address

	mov	ax,DIMapSegOFFSET set_small_start_offset
	test	SurfaceFlags,RLE_HUGE	;is it a huge map
	jz	have_mem_offset_calc_address
	mov	ax,DIMapSegOFFSET set_huge_start_offset

have_mem_offset_calc_address:
	
	mov	set_get_start_offset,ax	;save address of function

set_initializations_done:

; if the display mode in the DRAWMODE structure is OPAQUE, then the intersection
; of the ClipRectangle and the draw rectangle should be flooded with the      
; background color.


;	call	paint_background	;paint background for opaque modes

	lds	si,lpRLEbits		;DS:SI points to rle buffer
	mov	es,seg_lpSurface	;load surface selector into es

set_decode_RLE:

; The current position in the surface is at (X,Y). The clipping rectangle is
; from (XClipLft,YClipTop) to (XClipRgt,YClipBot)

	mov	NibbleToggle,0		;used for 4 bit RLEs

	xor	ah,ah			;count is 1 byte
        call    ReadRLE                 ;get the next byte
	or	al,al			;is it 0
	jnz	set_encoded_run		;the next byte has the color index.

;----------------------------------------------------------------------------;
; Possibilities are: we may have,					     ;
;	. End of Line   	 -  If next byte is 0,	    	             ;
;       . End of RLE             -  If next byte is 1,                       ;
;       . Delta                  -  If next byte is 2                        ;
;       . Unencoded run		 -  If next byte is 3 or more		     ;
;----------------------------------------------------------------------------;

        call    ReadRLE                 ;get the next code byte
	cmp	al,3			;absolute mode ?
	jae	set_absolute_mode	;absolute mode of encoding
	cmp	al,2			;delta or segment cross
        je      set_delta               ;yes
	or	al,al			;end of line?
	jz	set_EOL			;yes
        jmp     short set_EORLE         ;end of encoding

set_absolute_mode:

;----------------------------------------------------------------------------;
; we have an unencoded run, COUNT INDEX1 INDEX2 ...,with no of indexes in the;
; run being in ax.							     ;
;----------------------------------------------------------------------------;

        push    ax                      ;save the count of RLE

        test    SurfaceFlags,RLE_CLIPED
        jz      set_multipel_not_cliped

; calculate the portion of the above segment that is within the clipping rect.

        call    set_get_segment         ;set up params for multipel segment

        ;   ax is pixels cliped on left
        ;   cx is pixels visible
        ;   bx is pixels cliped on right

        call    [update_set_src_ptr]    ;advance over left clipped pels
        jcxz    set_multipel_update_X_Y ;segment is clipped

;----------------------------------------------------------------------------;
; Atleast a part of the segment is visible. (DI,Y) has the start coordinate, ;
; CX has the extent and DS:SI points to the index of the first pel in the    ;
; visible part								     ;
;----------------------------------------------------------------------------;

        push    bx                      ;save right clipped count
        call    set_multipel_segment    ;draws the segment, pels of diff color
        pop     ax                      ;restore right clip count and
        call    [update_set_src_ptr]    ;   advance over clipped pels
        jmp     short set_multipel_update_X_Y

set_multipel_not_cliped:
        mov     di,X
        mov     cx,ax
        call    set_multipel_segment    ;draws the segment, pels of diff color

set_multipel_update_X_Y:

        mov     ax,NibbleToggle
        call    [update_set_src_ptr]

; now update (X,Y) to account for the coding in the present RLE record.

        pop     ax                      ;get back count of indices in record
        add     X,ax                    ;update the abscissa.

        test    si,1                    ;WORD align source pointer
        jz      @f
        call    ReadRLE
@@:
	jmp	short set_decode_RLE	;continue with next record

set_encoded_run:

;----------------------------------------------------------------------------;
; The record is a COUNT INDEX type of record and all pels in the segment are ;
; of the same color. If we are in 4 bit encoding mode, two things could happ-;
; -en:								             ;
;      . If the two nibbles are the same, we will treat this to be a truly   ;
;        encoded record just as we do in the case of 8 bit mode.	     ;
;									     ;
;      . If the two nibbels do not have the same value then we will do the   ;
;        decoding in two passes, the first one will set all the pels in the  ;
;        run to the first color and the next one will alternating pels to the;
;        second color.
;									     ;
; We call a function to decide on the display strategy.			     ;
;----------------------------------------------------------------------------;

        push    ax                      ;save count

; obtain the start coordinate in the segment and extent wrt the clip rectangle

	call	set_get_segment 	;get start (x,y) in segment and count
	jcxz	set_onepel_update_X_Y	;the whole segment is clipped

	call	[decide_4_bit_strategy]	;decide how to treat 4 bit encoding
	jnc	treat_as_encoded	;stretch of same color

; we have a stretch of alternating colors, so we will call a routine to do this
; strtech in two passes.

	call	set_4bpp_onepel_segment	;draw visible part of segment
	jmp	short set_onepel_update_X_Y

treat_as_encoded:
;----------------------------------------------------------------------------;
; at least a part is visible. (AX,Y) has start coordinate, CX has extent,    ;
; DS:SI points to the pel index to be used for each pel in the segment	     ;
;----------------------------------------------------------------------------;

	call	set_onepel_segment	;draw the visible part of the segment

; update (X,Y) for start of next RLE record.

set_onepel_update_X_Y:

        call    ReadRLE                 ;advance source pointer

	pop	ax			;get back count of pels in RLE record
	add	X,ax			;move X, Y stays the same, SI is OK
        jmp     set_decode_RLE          ;decode next RLE record.

set_delta:

; we have reached a delta record

        call    ReadRLE                 ;get next byte
	xor	ah,ah			;zeroize
	mov	bx,ax			;save in bx
        call    ReadRLE                 ;fetch the next byte

;----------------------------------------------------------------------------;
; we have a DELTA encoded record, DELTA_X is in BX and DELTA_Y in AX. Update ;
; the current position to be (X+DELTA_X,Y+DELTA_Y)			     ;
;----------------------------------------------------------------------------;

	add	X,bx			;update X
	sub	Y,ax			;update Y
        jmp     set_decode_RLE          ;decode next  record

set_EOL:
;----------------------------------------------------------------------------;
; we have an end-of-line record. The action is to update Y and have the X    ;
; same as the left edge of the destination rectangle. Since we are traversing;
; the surface in the reverse direction, we will have to decrement Y. 	     ;
;----------------------------------------------------------------------------;

	mov	ax,DstX			;left edge of rectangle
	mov	X,ax			;is new current abscissa
	dec	Y			;point to previos scan
	jmp	set_decode_RLE		;decode the next record.

set_EORLE:
; we are done with the blitting. Restore device programming if necessary

	test	SurfaceFlags,RLE_DEVICE	;was the surfac a device
	jz	RLEset_end		;no
	mov	bl,SurfaceFlags		;the flag byte
	and	bl,RLE_MONO		;bl will be 0 for color

IFNDEF	CGAHERCMONO
	call	dev_reinit		;reinitialize the device
ENDIF

	test	SurfaceFlags,RLE_CURSOR_OFF
	jz	RLEset_end		;cursor was not disabled

; enable the cursor

ifdef EXCLUSION
	call	unexclude_far		;allow cursor to be drawn
endif

RLEset_end:
	mov	ax,1			;success code
	jmp	RLEend			;return to caller

;----------------------------------------------------------------------------;
;			GET_RLE_BITS					     ;
;----------------------------------------------------------------------------;
get_RLE_bits:

; Initialize count of bytes to zero

        xor     ax,ax
        mov     dwSizeImage.lo,ax
        mov     dwSizeImage.hi,ax

; load address of correct memory variables for 4 and 8 bit modes

	mov	al,SurfaceFlags		;get the flag byte
	test	al,RLE_8_BIT		;8 bit encoding requested ?
	jnz	@f			;yes

; set up addresses for 4 bit mode

	mov	ax,DIMapSegOFFSET get_next_index_byte_4_bit
	mov	[get_next_index_byte],ax
	mov	ax,DIMapSegOFFSET update_get_dst_ptr_4_bit
	mov	[update_get_dst_ptr],ax
	mov	expected_return,2	
	jmp	short fill_color_table

@@:

; set up addresses for 8 bit mode

	mov	ax,DIMapSegOFFSET get_next_index_byte_8_bit
	mov	[get_next_index_byte],ax
	mov	ax,DIMapSegOFFSET update_get_dst_ptr_8_bit
	mov	[update_get_dst_ptr],ax
	mov	expected_return,1

fill_color_table:

; fill up the color table with the color which the driver supports. For
; color surfaces load the color from the own color table, for monochrome
; surfaces, there will just be two entries one for black and the other for white

	les	di,lpRLEInfo		;pointer to the info block

; to get to the color table add in the size of the header block to the start 
; offset of the info block

	push	di			;save pointer to the info block
	add	di,wptr es:[di].biSize	;ignore the high word of the size
	test	SurfaceFlags,RLE_MONO	;is it a monochrome surface
	jz	get_table_for_color	;no

; the surface is a monochrome surface and the table will have two colors only

	xor	ax,ax			;color byte for black
	stosw				;fill in blue and green component
	stosw				;fill in red component
	dec	ax			;color byte for white
	stosw				;fill in blue and green component
	stosb				;fill in red component
	sub	al, al
	stosb				;fill in newly defined flags

; the size of the color table that we fill in is jut 2 in this case

	mov	ax,2			;value to be put back into the infoblock
	jmp 	short color_table_done

get_table_for_color:

	lea	si,DIMapSeg_color_table	;ds:si points to oem color table

if NUMBER_PLANES eq 4
	mov	cx,16			;16 entries in table
else
	mov	cx,8			;8 entries in table
endif

table_for_color:
	mov	ax,cs:[si]		;get blue and green
	stosw				;xfer them
	add	si,2
	mov	al,cs:[si]		;get red
	add	si,2
	stosw				;save red discard dummy
	.errnz	(size RGBQuad)-4
	loop	table_for_color

; the size of the color table is thus 16  or 8

if NUMBER_PLANES eq 4
	mov	ax,16 			;16 colors for 4 plane driver
else
	mov	ax,8			;8 colors for 3 plane driver
endif

color_table_done:

; fill in the size of the color table that we have just made into the info block

	pop	di			;get back the pointer to the info block
        mov     es:[di].biClrUsed.lo,ax
        mov     es:[di].biClrUsed.hi,0

; depending on the type of the display surface load the address of appropriate
; functions into the calling memory variables.

	mov	dx,width_scan		;needed for memory surfaces
	test	SurfaceFlags,RLE_DEVICE ;is it a device ?
	jz	get_RLE_from_memory	;no

IFNDEF	CGAHERCMONO

; load the address of the MAP SELECT register into the CONTROLLER register
; and leave the address of the data register in DX

	mov	dx,EGA_BASE+GRAF_ADDR	;the address register

; disable the cursor from the bounds of the rectangle on the surface

ENDIF

ifdef	EXCLUSION
	push	dx			;save address of MAP SEL REG
	mov	cx,DstX			;X1
	mov	si,cx
	add	si,DstXE		;X2
	mov	dx,DstY			;Y1
	mov	di,dx
	add	di,DstYE		;Y2
	call	exclude_far		;exclude cursor from clip rectangle
	pop	dx
	or	SurfaceFlags,RLE_CURSOR_OFF
endif

get_RLE_from_memory:

; load the address of routine to retrieve buffered pels. We have one routine 
; for color surfaces and one for monochrome surfaces. For mono surfaces the
; LSB bit in SurfaceFlags is set on.

	mov	bl,SurfaceFlags		;load the flag byte
	and	bl,1			;just have the last bit
	xor	bh,bh
	shl	bx,1			;we index into a word table
	mov	ax,FetchFromBuffer[bx]	;get the appropriate address
	mov	get_pel_from_buffer,ax	;save in call memory var

; now get the address of the function which loads the pel buffers. We have
; 4 different routines - different for color and monochrome and differnt for
; device and memory.

	mov	bl,SurfaceFlags		;load the flag byte, bh is 0
	and	bl,00000011b		;have last two bits
	shl	bx,1			;index into word table
	mov	ax,FetchIntoBuffer[bx]	;get the address
	mov	saved_get_pel_addr,ax	;save it

;
; set up a pointer to the RLE buffer, and set fnStoreRLE to the proper
; function to use when outputing bytes to the RLE buffer.
; if the RLE buffer is NULL just accumulate size of RLE buffer
;
        les     di,lpRLEbits                    ;es:di points to RLE buffer
        mov     ax,es
        or      ax,di

        mov     ax,DIMapSegOFFSET StoreRLE      ;assume pointer is not NULL
        mov     bx,DIMapSegOFFSET UpdateAbsCount
        jnz     get_RLE_ptr_not_null
        mov     ax, DIMapSegOFFSET StoreRLEIncCount
        mov     bx, DIMapSegOFFSET UpdateAbsCountNop
get_RLE_ptr_not_null:
        mov     fnStoreRLE,ax
        mov     fnUpdateAbsCount,bx

get_RLE_init_done:

        push    dx                      ;save EGA addr or width_scan

; calculate  offset to the bottom left corner of the rectangle on the display
; surface.

	mov	ds,seg_lpSurface	;get the surfaces segment
	mov	ax,Y			;the start Y
	mov	si,X			;the start X

IFDEF	CGAHERC
	test	SurfaceFlags,RLE_DEVICE	;is the surface a device
	jz	@f
	call	device_get_dest_scan	;get the offset for the scan
	jmp	short common_calculation
@@:
ENDIF

	test	SurfaceFlags,RLE_HUGE	;do we have a huge surface
	jz	get_do_calc_for_small	; no

; find out the correct segment which has (X,Y)

        xor     dx,dx                   ;will have the segment offset here

get_find_correct_segment:
	sub	ax,ScansPerSegment	;is it less than no of scans in seg
	jc	get_found_correct_segment;yes
	add	dx,__NEXTSEG		;try next segment
	jmp	short get_find_correct_segment

get_found_correct_segment:
	add	ax,ScansPerSegment		;correct ordinate in this segment
	mov	bx,ds			;get start segment
	add	bx,dx			;addin offset to target segment
        mov     ds,bx                   ;ds has correct surface segment now


get_do_calc_for_small:

; now calculate the start offset in bytes and the start bitmask

	mul	next_scan		;multiply y by width of a scan

common_calculation:

	mov	cx,si			;get x
	shiftr	si,3			;no of complete bytes contributed by x
	add	si,ax			;DS:SI points to start byte rel to beg.

; now add in the start offset of the surface which we had ignored

        add     si,off_lpSurface        ;DS:SI points to the abs. start byte

	and	cl,00000111b		;get bit position in first byte
	mov	bl,80h			;count start from left end
	shr	bl,cl			;bl has correct start mask

; we will be needing the start mask only for geting the pels from the first
; byte in the mask. But we will not tamper with bl so that it retains its 
; value for the next scan too.

        pop     dx                      ;restore EGA addr or width_scan

get_rect_code_loop:

; initialize count and the first pel.

	push	si			;save start of scan
	mov	BufferedPelCount,0	;number of pels in buffer

; load the  address of the routine to get the pel from the display surface

	mov	ax,saved_get_pel_addr	;this is the masked get routine address
	mov	fill_pel_buffer,ax
	mov	cx,DstXE		;no pels in one scan
	call	[get_next_index_byte]	;index byte in AL, count of pels in AH
	mov	LastPel,al		;save it
	mov	count,ah		;count of last pel (1 or 2)


get_scan_code_loop:

initial_state:

	njcxz	solitary_pel_at_end	;one byte at end
	call	[get_next_index_byte]	;get the next byte
	cmp	ah,expected_return	;expected no of pels ?
	jnz	initial_to_absolute	;treat as a no-match
	cmp	LastPel,al		;same as buffered pel
	jnz	initial_to_absolute	;absolute mode starts

initial_to_encoded:

encode_encode:
	add	count,ah		;last pels accounted
	jc	encode_overflow		;255 encoded pels reached
	njcxz	encode_to_scan_end	;last pel in buffer
	call	[get_next_index_byte]	;get another index byte
	cmp	ah,expected_return	;expected no of pels ?
	jnz	encode_absolute		;treat as a no-match
	cmp	al,LastPel		;still in encoded run
	jz	encode_encode		;yes

; we switch from encoded to absolute mode. Save the record.

encode_absolute:

	push	ax			;save new pel
	mov	al,count
        call    [fnStoreRLE]            ;save count
	pop	ax			;get back last pel
	xchg	al,LastPel		;save it and get previous pel
        call    [fnStoreRLE]            ;save the pel
	mov	count,ah		;back to intial state
	jmp	short initial_state

encode_overflow:

	mov	al,count		;value we reached at overflow time
	sub	al,ah			;value before overflow
        call    [fnStoreRLE]            ;save the count
	mov	al,LastPel		;get the saved pel
        call    [fnStoreRLE]            ;save the pel
	mov	count,ah		;number of ples in buffer
	jmp	short initial_state

initial_to_absolute:

;----------------------------------------------------------------------------;
;----------------------------------------------------------------------------;

;
;   Make sure there are at least 3 pixels left on the scan line, if
;   not we cant encode a absolute mode.
;
        cmp     cx,3                    ;are there at least three pixels?
        jl      encode_absolute         ;no write as encoded

        push    ax
        xor     ax,ax
        call    [fnStoreRLE]
        mov     ptrCount.off,di         ;save pointer to count field
        mov     ptrCount.sel,es         ;  so we can backpatch count later
        call    [fnStoreRLE]
        pop     ax

absolute_absolute:

	xchg	al,LastPel		;buffer new pel ang get last one
        call    [fnStoreRLE]            ;save last pel
	add	count,ah		;add number of pels in last fetch
	jc	absolute_overflow
        jcxz    absolute_to_scan_end    ;one byte in buffer, the last one

	call	[get_next_index_byte]	;get the next index byte
	cmp	ah,expected_return	;expected no of pels ?
	jnz	absolute_absolute	;treat as a no-match
	cmp	al,LastPel		;different from last
	jnz	absolute_absolute	;yes, continue absolute run.

;----------------------------------------------------------------------------;
; an absolute mode of just 2 pels, can't be recorded as an absolute run as   ;
; it will clash with the delta encoding, so never try to record a abs run <3 ;
;----------------------------------------------------------------------------;

absolute_encode:
        mov     bh,count                ;count of pels in buffer
        sub     bh,ah                   ;the last fetch is not part of run

        cmp     bh,3                    ;keep going if count < 3
        jl      absolute_absolute

        mov     al,bh                   ;count of pels in buffer

;----------------------------------------------------------------------------;
; a regular absolute mode record, with count (now in AL) >= 3		     ;
;----------------------------------------------------------------------------;

        call    [fnUpdateAbsCount]

; if the count was odd, skip over filler byte
        call    [update_get_dst_ptr]    ;update the pointer with filler

out_from_absolute:
	mov	count,ah		;initialize
	mov	al,LastPel		;get the last pel same as previous to it
	jmp	initial_to_encoded      ;start another encoded run

absolute_overflow:

	mov	al,count		;count before overflow
        sub     al,ah                   ;value before overflow
        call    [fnUpdateAbsCount]
        call    [update_get_dst_ptr]    ;update ptr for odd strings
        mov     count,ah                ;initialize
	jmp	initial_state		;a pel in buffer, a DEC CX pending

encode_to_scan_end:

	mov	al,count		
        call    [fnStoreRLE]            ;save count of last record
	mov	al,LastPel		;get the lst pel
        call    [fnStoreRLE]            ;save count of last record
	jmp	short scan_epilogue	;the end of scan record

absolute_to_scan_end:

	mov	al,LastPel		;get the saved pel
        call    [fnStoreRLE]            ;the last pel in the run
	mov	al,count
        call    [fnUpdateAbsCount]

; if the count was odd, skip over filler byte

        call    [update_get_dst_ptr]
	jmp	short scan_epilogue

solitary_pel_at_end:

	mov	al,ah			;encoded run, 1 byte in it
        call    [fnStoreRLE]
	mov	al,LastPel		;only pel in run
        call    [fnStoreRLE]

scan_epilogue:

; save 0,0 as an EOL record

	xor	ax,ax			;code for EOL is 0,0
        call    [fnStoreRLE]
        call    [fnStoreRLE]

	pop	si			;get back to strt of scan on surface
	dec	DstYE			;one more scan encoded
	jz	get_end_of_RLE		;all the scans have been encoded

; update the pointer on the display surface to the next scan (actually prev,
; as we go up on the display surface). We may go to a another segment

IFDEF	CGAHERC

	test	SurfaceFlags,RLE_DEVICE
	jz	previous_memory_scan	;do calculations for memory

ifdef HERCULES

	sub	si,2000h		;get to previous bank
	jnb	same_display_surface_seg	
	sub	si,next_scan		;get to previos scan
	xor	si,8000h		;and start at last bank

endif

ifdef IBM_CGA

	sub	si,2000h		;get to previous bank
	jnb	same_display_surface_seg	
	sub	si,next_scan		;get to previos scan
	and	si,3fffh		;and start at last bank

endif

	jmp	short same_display_surface_seg

previous_memory_scan:

ENDIF

	sub	si,next_scan		;go to the preivios_scan
	jnc	same_display_surface_seg;we have not crossed a segment

; we cross a segment on the display surface, update DS:SI

	mov	ax,ds
	sub	ax,__NEXTSEG		;go to the previous segment
	mov	ds,ax
	mov	si,prev_offset		;offset of the last scan in the segment

same_display_surface_seg:

	jmp	get_rect_code_loop	;encode the other scans

get_end_of_RLE:

; output a EORLE record [0,1]

        xor     al,al
        call    [fnStoreRLE]
        inc     al
        call    [fnStoreRLE]

; save the size of the map in the info table

        les     di,lpRLEInfo            ;load a pointer to the info block

        mov     ax,dwSizeImage.lo
        mov     dx,dwSizeImage.hi
        mov     es:[di].biSizeImage.lo,ax
        mov     es:[di].biSizeImage.hi,dx

; we are done with the encoding. If the display surface was the screen do
; the reinitializations

	test	SurfaceFlags,RLE_DEVICE	;is it a device
	jz	RLEget_end		;no reinitializations

IFNDEF	CGAHERCMONO

; reinitialize the device registers

	mov	es,seg_lpSurface
	assumes	es,EGAMem
	mov	bl,SurfaceFlags		;the flag byte
	and	bl,RLE_MONO		;bl will be 0 for color
	call	dev_reinit

ENDIF

; enable the display of the cursor

ifdef EXCLUSION
	call	unexclude_far		;allow cursor to be drawn
endif

RLEget_end:
	mov	ax,1
	jmp	short RLEend

RLEerror:
	xor	ax,ax			;error
RLEend:
cEnd

;----------------------------------------------------------------------------;
; ReadRLE   Reads a byte from a encoded RLE buffer.  This function will      ;
;           take care of segment crossing.                                   ;
;									     ;
; Entry:                                                                     ;
;       DS:SI   pointer into RLE buffer                                      ;
; Exit:                                                                      ;
;       AL      byte from buffer                                             ;
;       DS:SI   incremented                                                  ;
; Destroys:								     ;
;       flags                                                                ;
;----------------------------------------------------------------------------;
ReadRLE proc near
	lodsb
	or	si, si
        jz      ReadRLENextSeg
	ret
ReadRLENextSeg:
	mov	si, ds
        add     si, __NEXTSEG
	mov	ds, si
	sub	si, si
	ret
ReadRLE endp

;----------------------------------------------------------------------------;
; StoreRLE  Writes a byte to a encoded RLE buffer.  This function will       ;
;           take care of segment crossing and update dwSizeImage to contain  ;
;           number of total bytes writen                                     ;
; Entry:                                                                     ;
;       AL          byte to store                                            ;
;       ES:DI       pointer into RLE buffer                                  ;
; Exit:                                                                      ;
;       ES:DI       incremented                                              ;
;       dwSizeImage incremented                                              ;
; Destroys:								     ;
;       flags                                                                ;
;----------------------------------------------------------------------------;
StoreRLE proc near
        stosb
        or      di, di
        jnz     StoreRLEIncCount

        mov     di, es
        add     di, __NEXTSEG
        mov     es, di
        sub     di, di
StoreRLEIncCount:
        add     dwSizeImage.lo,1
        adc     dwSizeImage.hi,0
	ret
StoreRLE endp

;----------------------------------------------------------------------------;
; UpdateAbsCount back patches count field of a absolute run                  ;
;                                                                            ;
; Entry:                                                                     ;
;       ptrCount    pointer to count field of absolute run                   ;
;       AL          count value to store                                     ;
; Exit:                                                                      ;
;       count value stored, current es:di preserved                          ;
; Destroys:								     ;
;       flags                                                                ;
;----------------------------------------------------------------------------;
UpdateAbsCount proc near
        push    es
        push    di
        mov     di,ptrCount.sel
        mov     es,di
        mov     di,ptrCount.off
        stosb
        pop     di
        pop     es
UpdateAbsCountNop:
        ret
UpdateAbsCount endp

;----------------------------------------------------------------------------;
; set_get_segment:						             ;
;									     ;
;         gets a portion of the current segment from (X,Y) to (X+AX,Y)       ;
;         which fits within the clip rectangle.			             ;
;									     ;
; State On Entry:					                     ;
;	          (X,Y)  -  start of segment for current RLE record	     ;
;                   AX   -  number of pels in the segment		     ;
;     (ClipLft,ClipTop)	 -  top left of the clip rectangle		     ;
;     (ClipRgt,ClipBot)  -  bottom right of the clip rectangle	             ;
;								             ;
; State On Exit:							     ;
;                   CX   -  no of pels within clip rectangle, 0 => total clip;
;                           (if CX=0, rest is not valid)                     ;
;                   AX   -  no of pels clipped on left                       ;
;                   BX   -  no of pels clipped on right                      ;
;		    DI	 -  absisca of first visible pel in the segment      ;
;									     ;
; Destroys:								     ;
;		  AX,BX,DX						     ;
;----------------------------------------------------------------------------;

comment ~

  we will divide the region surrounding the clip rectangle into 4 areas as
  shown below. The segment is toattly clipped if it lies toatly in any of
  the four regions. If it is not totall cliped, there can be 4 ways that
  the segment mwy reside. These are all shown below.

				      region-1
		<---------------------------------------------------->
		      |					     |
		      |					     |
		      |					     |
	 region-4     |		 The Clip Rectangle	     |	region-2
		      |					     |
		type_1|		     type_2	       type_3|
		------|------	   ----------	       ------|-----
		------|--------------------------------------|----- type_4
		<---------------------------------------------------->
				      region-3


end comment ~


set_get_segment proc    near

        test    SurfaceFlags,RLE_CLIPED
        jz      set_get_segment_not_cliped
	       
	mov	bx,X			;get left abscissa of segment
	mov	di,Y			;get ordinate
	xor	cx,cx			;assume total clip

; if Y is > ClipBot (region-3) or < ClipTop (region_1) it is totall clipped

	cmp	di,ClipTop
        jl      set_get_segment_end     ;segment is totaly clipped
	cmp	di,ClipBot		
        jge     set_get_segment_end     ;segment is totaly clipped

; Y ordinate is OK, but we may still be in region-2 or 4.

	cmp	bx,ClipRgt		;are we in region-2
        jge     set_get_segment_end     ;yes. so nothing is visible
	push	ax			;save extent
	add	ax,bx			;ax has coord just past rt edge of seg
	cmp	ax,ClipLft		;are we in region-4
	pop	ax			;get back extent in ax
        jle     set_get_segment_end     ;another total clip case

; atleast a part of the segment is visible. Find out the type of clip

        mov     cx,ax                   ;assume all is visible
        xor     ax,ax                   ;   and nothing cliped on left

	cmp	bx,ClipLft		;do we start on the left edge
	je	set_start_on_lft_edge	;yes
        jg      set_clip_type_2_or_3    ;cant be clip type 1 or 4

; we have clip type 1 or 4, so a portion is clipped off the left end

	sub	bx,ClipLft
        neg     bx                      ;no of pels clipped from left end

        sub     cx,bx                   ;bx less pixels to draw
        mov     ax,bx                   ;remeber left cliped pixels

set_start_on_lft_edge:
        mov     di,ClipLft              ;start on left edge
        mov     bx,ClipXE               ;get extent not excceding clip rect
        jmp     short set_get_segment_right_clip

set_clip_type_2_or_3:

        mov     di,bx                   ;start sbscissa of visible segment
	sub	bx,ClipRgt		;bx has left x of segment
	neg	bx

set_get_segment_right_clip:
;
;   cx is wanted extent
;   bx is max alowed extent
;
        cmp     cx,bx                   ;Are we clipped on the right?
        jg      set_get_segment_right_cliped
        xor     bx,bx                   ;No, set right cliped to zero
        jmp     short set_get_segment_end

set_get_segment_right_cliped:
        sub     cx,bx                   ;We are right cliped, set...
        xchg    cx,bx

;   bx is amount left clipped
;   cx is clipped extent
;   bx is amount right clipped

set_get_segment_end:
	ret

set_get_segment_not_cliped:
        mov     di,X
        mov     cx,ax
        xor     ax,ax
        xor     bx,bx
	ret

set_get_segment	endp

;----------------------------------------------------------------------------;
;									     ;
; set_multipel_segment:				         		     ;
;									     ;
;	 draws a segment onto the display surface, where the pels in the     ;
;	 segment are of different colors. 			             ;
;									     ;
; State On Entry:						             ;
;									     ;
;        (DI,Y)	   -   start coordinate of the segment.			     ;
;	     CX	   -   no pels in the segment				     ;
;	  DS:SI    -   pointer to the index for the first pel in the segment ;
;   color_xlate    -   index to 4bit color translate table.		     ;
;									     ;
;----------------------------------------------------------------------------;

set_multipel_segment	proc	near
	
; calculate the start offset in the surface and the the bit mask and start 
; byte mask

	call	set_get_bit_mask

; ES:DI	points to byte on destination surface to hold the first bit. AH has
; the bit mask.

	lea	bx,color_xlate		;address of the translation table

IFNDEF	CGAHERCMONO

	mov	dx,EGA_BASE+GRAF_DATA   ;address of bitmask register
	test	SurfaceFlags,RLE_DEVICE	;is surface a device ?
	jnz	set_multipel_loop	;yes dx has correct value

ENDIF
	mov	dx,width_scan		;for memory get the width of a scan

set_multipel_loop:
	
	call	[get_next_abs_mode_index];get next pel index
	xlat	ss:[bx]			;get the color value for the index

; AL has color and AH has mask byte

	call	[set_partial_pels]	;set the pels
	ror	ah,1			;update mask to next pel
	adc	di,0			;update byte if wrap around

	loop	set_multipel_loop

	ret

set_multipel_segment	endp

;----------------------------------------------------------------------------;
; set_onepel_segment:				         		     ;
;									     ;
;	 draws a segment onto the display surface, where the pels in the     ;
;	 segment are all of the same color.				     ;
;									     ;
; State On Entry:						             ;
;									     ;
;        (DI,Y)	   -   start coordinate of the segment.			     ;
;	     CX	   -   no pels in the segment				     ;
;	  DS:SI    -   pointer to the index for the pel color		     ;
;   color_xlate    -   index to 4bit color translate table.		     ;
;									     ;
;----------------------------------------------------------------------------;

set_onepel_segment	proc	near
	
	
; calculate the start offset in the surface and the the bit mask and start 
; byte mask

	call	set_get_byte_mask

; ES:DI	points to byte on destination surface to hold the first bit. 
; BL has the first byte mask, BH has the last byte mask and CX has the inner
; byte count.

	mov	dx,bx			;save masks
	lea	bx,color_xlate		;address of the translation table
	call	[get_encoded_index]	;get the index value in al
	xlat	ss:[bx]			;get the color value for the index
	mov	bx,dx			;get back masks

IFNDEF	CGAHERCMONO
	mov	dx,EGA_BASE+GRAF_DATA   ;address of bitmask register
	test	SurfaceFlags,RLE_DEVICE	;is surface a device ?
	jnz	set_onepel_first    	;yes dx has correct value
ENDIF

	mov	dx,width_scan		;for memory get the width of a scan

set_onepel_first:

; if cx is 0, the two byte masks are to be combined and only one byte to be
; done.

	mov	ah,bl			;get first byte mask
	jcxz	set_onepel_last		;do the last byte only.

; do the first byte
; ES:DI-> destination byte. AH has byte mask. AL has pel color

	call	[set_partial_pels]	;set the pels for partial byte
	inc	di			;point to next byte
	dec	cx			;first pel was incuded in count

; now do the intermediate bytes and 8 bits will be set in a call

	mov	ah,0ffh			;all bits enabled
	jcxz	set_onepel_last		;no intermediate bytes

IFNDEF	CGAHERCMONO

	test	SurfaceFlags,RLE_DEVICE	;is it a device
	jz	set_onepel_loop
	xchg	ah,al			;get mask value
	out	dx,al			;set up mask
	xchg	ah,al			;restore original values

ENDIF

set_onepel_loop:
	call	[set_full_pels]		;do a complete byte
	inc	di			;do next byte
	loop	set_onepel_loop		;complete the intermediate runs

; now do the last byte in the run.

set_onepel_last:
	and	ah,bh			;combine with previous mask
	call	[set_partial_pels]	;set pels in the last byte

; we are done.

	ret

set_onepel_segment	endp
;----------------------------------------------------------------------------;
; set_4bpp_onepel_segment:     			         		     ;
;									     ;
;	 draws a segment onto the display surface, where the pels in the     ;
;	 segment are of alternating colors. This is used for paying back     ;
;        encoded pels in 4 bit RLE. We basically will make two passes, the   ;
;        first pass will set all the pels of the first color and the next    ;
;        will set all the pels of the other color.			     ;
;       								     ;
; State On Entry:						             ;
;									     ;
;        (DI,Y)	   -   start coordinate of the segment.			     ;
;	     CX	   -   no pels in the segment				     ;
;	  DS:SI    -   pointer to the index for the 2 pel colors	     ;
;   color_xlate    -   index to 4bit color translate table.		     ;
;									     ;
;----------------------------------------------------------------------------;

set_4bpp_onepel_segment	proc	near

	pushem	di,cx,si		;save these  values
	call	set_onepel_segment	;do the entire thing with one color
	popem	di,cx,si		;gat back saved values
	mov	ax,[set_full_pels]	;get the full pel address 
	push	ax			;save it
	mov	ax,[set_partial_pels]	;get the partial pel address
	mov	[set_full_pels],ax	;will need masking all through

	
; calculate the start offset in the surface and the the bit mask and start 
; byte mask

	call	set_get_byte_mask

; get the position of the first bit in the byte and decide whether we use
; 10101010 or 01010101 as the running mask.

	pushem	bx,cx			;save
	mov	cx,X			;get starting coordinate
	and	cl,7			;get the starting bit number
	mov	bl,80h			;initial position
	shr	bl,cl			;get the starting bit position
	mov	cl,10101010b		;first type of mask
	mov	ch,10101010b		;mask for second pel if cl not good
	and	cl,bl			;did it match ?
	jz	@f			;ch has runing mask for 2nd pel
	mov	ch,01010101b		;mask for second bit
@@:
	mov	al,ch			;get the mask
	mov	ah,ch			;mask in al & ah
	popem	bx,cx			;get back actual masks
	and	bx,ax			;update masks
	mov	special_4bpp_mask,al	;save it

; ES:DI	points to byte on destination surface to hold the first bit. 
; BL has the first byte mask, BH has the last byte mask and CX has the inner
; byte count.

	mov	dx,bx			;save masks
	lea	bx,color_xlate		;address of the translation table
	mov	al,[si]			;get the index
	and	al,0fh			;interested in second nibble
	xlat	ss:[bx]			;get the color value for the index
	mov	bx,dx			;get back masks

IFNDEF	CGAHERCMONO
	mov	dx,EGA_BASE+GRAF_DATA   ;address of bitmask register
	test	SurfaceFlags,RLE_DEVICE	;is surface a device ?
	jnz	set_4bpp_onepel_first   	;yes dx has correct value
ENDIF

	mov	dx,width_scan		;for memory get the width of a scan

set_4bpp_onepel_first:

; if cx is 0, the two byte masks are to be combined and only one byte to be
; done.

	mov	ah,bl			;get first byte mask
	jcxz	set_4bpp_onepel_last	;do the last byte only.

; do the first byte
; ES:DI-> destination byte. AH has byte mask. AL has pel color

	call	[set_partial_pels]	;set the pels for partial byte
	inc	di			;point to next byte
	dec	cx			;first pel was incuded in count

; now do the intermediate bytes and 8 bits will be set in a call

	mov	ah,special_4bpp_mask	;all bits enabled
	jcxz	set_4bpp_onepel_last	;no intermediate bytes

IFNDEF	CGAHERCMONO

	test	SurfaceFlags,RLE_DEVICE	;is it a device
	jz	set_4bpp_onepel_loop
	xchg	ah,al			;get mask value
	out	dx,al			;set up mask
	xchg	ah,al			;restore original values

ENDIF

set_4bpp_onepel_loop:
	call	[set_full_pels]		;do a complete byte
	inc	di			;do next byte
	loop	set_4bpp_onepel_loop	;complete the intermediate runs

; now do the last byte in the run.

set_4bpp_onepel_last:
	and	ah,bh			;combine with previous mask
	call	[set_partial_pels]	;set pels in the last byte

	pop	ax			;gat back the full pel address
	mov	[set_full_pels],ax	;restore it

; we are done.

	ret

set_4bpp_onepel_segment	endp
;----------------------------------------------------------------------------;
;									     ;
; The following two routines sets the pels in byte masked by a specified     ;
; mask to a sepcified color.					             ;
;									     ;
; Inputs:								     ;
;	    ES:DI  -  destination byte where run of pels start.	 	     ;
;	       AL  -  color values of the pels				     ;
;	       AH  -  bitmask for the byte			 	     ;
;              DX  -  address of bitmask register for screen, width of a scan;
;	              for memory maps.					     ;
;									     ;
;----------------------------------------------------------------------------;
; we define a macro which takes three parameters (one implicit):  	     ;
;	 (1)  A Screen Byte pointer in ES:DI				     ;
;	 (2)  reg1 -- has a color value in byte (00 0r 0ffh) 	             ;
;        (3)  reg2 -- has a bit mask for the above byte			     ;
;									     ;
; and the macro sets the specified bit to the specified color bit. It uses   ;
; DX as a work register and does not preserve it.			     ;
;----------------------------------------------------------------------------;
set_bit_in_surface_byte macro	reg1, reg2

	mov	dl,reg2			;; get the bit mask
	not	dl			;; zero only the bit required
	and	es:[di],dl		;; mask out the required bit
	mov	dl,reg1			;; get the color value 
	and	dl,reg2			;; and in the bitmask
	or	es:[di],dl		;; set in the color bit

	endm
;----------------------------------------------------------------------------;
; routine to output a partial byte to a screen device.		     	     ;
;----------------------------------------------------------------------------;

dev_set_pels_partial proc	near

; set up the mask value.

IFNDEF	CGAHERCMONO
	push	ax			;save
	xchg	ah,al			;get mask byte into AL
	out	dx,al			;the mask is set
	xchg	es:[di],ah		;set the pel colors
	pop	ax			;restore
ELSE
	set_bit_in_surface_byte	al,ah
ENDIF
	ret

dev_set_pels_partial endp

;----------------------------------------------------------------------------;
; routine for device full byte output					     ;
;----------------------------------------------------------------------------;

dev_set_pels_full proc	near

	push	ax			;save
	xchg	es:[di],al		;set 
	pop	ax			;restore
	ret

dev_set_pels_full endp

;----------------------------------------------------------------------------;
; define a mcro to be used for updating a partial byte in one plane for a    ;
; memory surface.							     ;
;----------------------------------------------------------------------------;

set_do_one_plane macro

;; CH has the original byte

	shr	al,1			;;get the next color value into carry
	sbb	cl,cl			;;all bits set to plane bit
	and	cl,dh			;;mask with provided mask
	and	ch,dl			;;mask with inverted  mask
	or	ch,cl			;;combine the two part
	endm

;----------------------------------------------------------------------------;
; the routine to do a partial byte for colored memory surfaces.		     ;
;----------------------------------------------------------------------------;

mem_set_pels_partial proc near

	push	ax
	push	cx
	push	bx
	xchg	bx,dx			;bx has width scan
	mov	dl,ah			
	not	dl			;the inverse mask
	mov	dh,ah			;the actual mask
	push	di			;save original pointer
	mov	ch,es:[di]		;get byte from plane 0
	set_do_one_plane		;set bits for plane 0
	mov	es:[di],ch		;save updated byte
	add	di,bx			;point to next plane
	mov	ch,es:[di]		;get byte from the plane
	set_do_one_plane		;combine new value for masked bits
	mov	es:[di],ch		;set the new byte
	add	di,bx			;point to plane 3
	mov	ch,es:[di]		;get original byte
	set_do_one_plane		;combine new value for masked bits
	mov	es:[di],ch		;save new byte

IF NUMBER_PLANES eq 4
	mov	ch,es:[di+bx]		;get the byte for the fourth plane
	set_do_one_plane		;combine new value for masked bits
	mov	es:[di+bx],ch		;save new byte
ENDIF

	pop	di			;get back original pointer
	mov	dx,bx			;restore dx
	pop	bx			;restore bx
	pop	cx
	pop	ax			;restore the other saved register

	ret
mem_set_pels_partial endp

;----------------------------------------------------------------------------;
; the routine to do a partial byte for monochrome memory surfaces	     ;
;----------------------------------------------------------------------------;

monomem_set_pels_partial proc near

	push	ax
	push	cx
	push	dx
	mov	dl,ah			
	not	dl			;the inverse mask
	mov	dh,ah			;the actual mask
	mov	ch,es:[di]		;get byte from plane 0
	set_do_one_plane		;set bits for plane 0
	mov	es:[di],ch		;save updated byte
	pop	dx			;restore bx
	pop	cx
	pop	ax			;restore the other saved register

	ret
monomem_set_pels_partial endp


;----------------------------------------------------------------------------;
; The routine for outputing pels to a complete byte on a color memory surface;
;----------------------------------------------------------------------------;

mem_set_pels_full  proc  near

	push	ax			;save
	xchg	bx,dx			;get width of a scan into bx
	push	di			;save pointer
	shr	al,1			;get the bit for plane 0
	sbb	ah,ah			;replicate into all bits
	mov	es:[di],ah		;set byte for plane 0
	add	di,bx			;point to plane 1
	shr	al,1			;get next bit
	sbb	ah,ah			;replicate into a byte
	mov	es:[di],ah		;set plane 1
	add	di,bx			;point to plane 2
	shr	al,1			;get next color bit
	sbb	ah,ah			;replicate into a byte
	mov	es:[di],ah		;set plane 2

IF NUMBER_PLANES eq 4
	shr	al,1			;get the last color pel
	sbb	ah,ah			;replicate into a byte
	mov	es:[di+bx],ah		;set plane 3
ENDIF

	xchg	bx,dx			;get back original values
	pop	di			;restore original pointer
	pop	ax			;restore
	ret

mem_set_pels_full  endp

;----------------------------------------------------------------------------;
; The routine for outputing pels to a complete byte on a mono memory surface ;
;----------------------------------------------------------------------------;

monomem_set_pels_full  proc  near

	push	ax			;save
	shr	al,1			;get the bit for plane 0
	sbb	ah,ah			;replicate into all bits
	mov	es:[di],ah		;set byte for plane 0
	pop	ax			;restore
	ret

monomem_set_pels_full  endp

;----------------------------------------------------------------------------;
; set_get_bit_mask:						             ;
;									     ;
;	computes pointer to start byte on destination surface and bit mask   ;
;	for the first bit in the mask.					     ;
;									     ;
; State On Entry:						             ;
;	          (DI,Y)  - start point on surface		             ;
;		      CX  - no of pels in the segment			     ;
;									     ;
; State On Exit:						             ;
;		  ES:DI   - strat byte on the surface.			     ;
;		     AH   - bitmask					     ;
;									     ;
; Registers destroyed:						             ;
;		  AL,BX,DX						     ;
;----------------------------------------------------------------------------;

set_get_bit_mask proc	near
	
	call	[set_get_start_offset]	
	
;returns start offset in ES:DI, StartX & 00000111b in dl

	push	cx			;save count
	mov	cl,dl			;get no of bits to left of start
	mov	ah,80h			;set mask to left end
	shr	ah,cl			;set mask to the correct bit 
	pop	cx			;restore
	ret

set_get_bit_mask endp
;----------------------------------------------------------------------------;
; set_get_byte_mask:						             ;
;									     ;
;	computes pointer to start byte on destination surface and byte masks ;
;	for the first and last bytes and the number of intermediate bytes.   ;
;									     ;
; State On Entry:						             ;
;	          (DI,Y)  - start point on surface		             ;
;		      CX  - no of pels in the segment			     ;
;									     ;
; State On Exit:						             ;
;		  ES:DI   - start byte on the surface.			     ;
;		     BL	  - first byte mask				     ;
;                    BH   - last byte mask				     ;
;		     CX   - number of intermediate bytes (may be 0)	     ;
; Registers destroyed:						             ;
;		  AX,DX							     ;
;----------------------------------------------------------------------------;

set_get_byte_mask proc	near

	call	[set_get_start_offset]	
	
; rets ES:DI pointing to start offset and StrtX & 00000111b in dl

	mov	ax,cx			;save extent of segment

; get the start bitmask in bl

	mov	cl,dl			;get number of bits before start bit
	mov	bl,80h			;initialize bitmask at left end
	shr	bl,cl			;set correct start mask
	mov	cx,ax			;get back pel count
	dec	cx			;exclude last pel
	mov	ax,cx			
	shiftr	ax,3			;get number of complete byte in run
	and	cx,00000111b		;and the number of bits left
	mov	bh,bl			;save start mask
	ror	bl,cl			;get end mask
	mov	cx,ax			;number of bytes
	cmp	bh,bl			;test for wrap around
	adc	cx,0			;one more byte if so
	neg	bl			;ending byte mask
	add	bh,bh
	dec	bh			;starting byte mask
	xchg	bh,bl			;bl has start mask and bh has end mask
	ret

set_get_byte_mask  endp
;----------------------------------------------------------------------------;
; The following routines are for obtaining the start offset on a surface     ;
; given the start coordinates. One of the routines is for small bitmap and   ;
; screen devices which do not exceed 64k in size and the other is for huge   ;
; bitmaps.								     ;
;									     ;
; State On Entry:							     ;
;		  (DI,Y)  -  start coordinate on display surface	     ;
;                    ES   -  display segment for small maps or screen        ;
;									     ;
; State On Exit:							     ;
;		  (ES:DI) - start byte on display surface		     ;
;		      DL  - number of bits in 1st byte to left of start bit  ;
;----------------------------------------------------------------------------;

;----------------------------------------------------------------------------;
; first find the offset in surfaces that will not exceed 64k in size         ;
;----------------------------------------------------------------------------;

set_small_start_offset proc near

; ES points to correct display segment and Y has current scan offset in it
        mov     ax,Y                    ;get start ordinate
        mov     bx,off_lpSurface
set_get_offset_in_segment:
        mul     next_scan               ;get width of scan including all planes
        add     ax,bx
	
; DX will be 0.

	mov	dx,di			;get start x
	shiftr	di,3			;get no of complete bytes
	add	di,ax			;di has correct offset
	and	dl,00000111b		;get no of bits before start bit
	ret

set_small_start_offset endp

;----------------------------------------------------------------------------;
; now find the offset in surfaces that will exceed 64k and the start Y may be;
; in differnt segment from the one in ES				     ;
;----------------------------------------------------------------------------;

set_huge_start_offset proc near

; find the correct segment and the start scan in the segment

	xor	dx,dx			;will hold offset to target segment
	mov	es,seg_lpSurface	;get segment of surface
	mov	ax,Y			;start ordinate
set_get_huge_segment_loop:
	sub	ax,ScansPerSegment
	jc	set_got_huge_segment
	add	dx,__NEXTSEG		;update to next segment
	jmp	short set_get_huge_segment_loop
set_got_huge_segment:
	add	ax,ScansPerSegment	;now has the correct Y from start of seg
	mov	bx,es			;get start segment
	add	bx,dx			;offset to the target segment
        mov     es,bx                   ;setup es

; now calculate the start offset in the segment

        xor     bx,bx                   ;offset into segment is zero
        jz      set_get_offset_in_segment

set_huge_start_offset endp

IFDEF	CGAHERC
;----------------------------------------------------------------------------;
; finally have the routine to get the start offset for a device (CGA/HERC)   ;
;----------------------------------------------------------------------------;

set_device_start_offset	proc	near

	mov	ax,Y			;get the start scan no
	call	device_get_dest_scan	;get start offset of the scan
	mov	dx,di			;get start x
	shiftr	di,3			;get no of complete bytes
	add	di,ax			;di has correct offset
	and	dl,00000111b		;get no of bits before start bit
	ret

set_device_start_offset	endp

;----------------------------------------------------------------------------;

ENDIF

;----------------------------------------------------------------------------;
; The support routines for GET_RLE_BITS follow now.			     ;
;----------------------------------------------------------------------------;

;----------------------------------------------------------------------------;
; get_next_index_byte_4/8_bit:							     ;
;									     ;
; At entry CX ha the number of pels left in the scan line. This routine      ;
; returns the next index byte in AL and the count of the number of pels in   ;
; AH. CX is updated depending on the number of pels obtained. The first      ;
; routine id for the 8 bit mode where one pel is saved in one byte, the      ;
; second is for the 4 bit mode where 2 pels can be saved in a byte.	     ;
;----------------------------------------------------------------------------;

; routine for 8 bit mode.

get_next_index_byte_8_bit proc near

	call	get_next_pel		;get the next pel in al.
	mov	ah,1			;1 pel in al
	dec	cx			;one more pel obtained
	ret

get_next_index_byte_8_bit endp

; routine for 4 bit mode

get_next_index_byte_4_bit proc near

	call	get_next_pel		;get a pel in AL
	shiftl	al,4			;get it to the high nibble
	or	al,00001111b		;dummy for the low end
	mov	ah,1
	dec	cx			;one pel in al
	jcxz	get_next_index_byte_4_bit_ret;return with one pel at scan end
	push	bx			;save
	push	ax			;save partial byte
	call	get_next_pel		;get the next pel
	mov	bl,al			;save it
	pop	ax			;get back aved pel,count
	and	al,11110000b		;mak of low nibble
	or	al,bl			;jam in the second one
	pop	bx			;restore
	inc	ah			;one more pel
	dec	cx			;one more pel obtained
get_next_index_byte_4_bit_ret:
	ret

get_next_index_byte_4_bit endp

;----------------------------------------------------------------------------;
; The following two routines are used to bump the code save pointer if the   ;
; run of pels in the absolute mode is odd. The first routine is for 8 bit    ;
; mode and the second for 4 bit mode. The number of pels is in AL.	     ;
;----------------------------------------------------------------------------;

update_get_dst_ptr_8_bit proc near

        shr     al,1                    ;carry set for odd count
        jnc     @f
        xor     al,al                   ;output a zero filler byte
        call    [fnStoreRLE]
@@:
	ret

update_get_dst_ptr_8_bit endp

; for 4 bit mode 2 pels come in a byte.

update_get_dst_ptr_4_bit proc near

; if the count is odd, add 1 to make it even

	test	al,1			;is it odd ?
	jz	@f			;no.
	inc	al			;make it even
@@:

; halve the result now, this is the no of bytes that we take for the run

	shr	al,1			; no of bytes

; now if the no of bytes is odd, we need a filler.

	shr	al,1			;is it odd ?
	jnc	@f			;no.

; put in the filler.

        xor     al,al                   ;output a zero filler byte
        call    [fnStoreRLE]
@@:
	ret

update_get_dst_ptr_4_bit endp

;----------------------------------------------------------------------------;
; get_next_pel:								     ;
;								 	     ;
;	 returns the index of the next pel in the surface rectangle in AL    ;
;									     ;
;	We generally try to read in 8 pels at a time from the display surface;
;	and buffer them. This routine returns pels from the buffer. Once the ;
;	buffer is exhausted, it calls lower level routines to fill the buffer;
;									     ;
;	There are 4 lower level routines, one each for monochrome device,    ;
;	color device, monochrome memory and color memory. The address of the ;
;       appropriate routine would have been saved in the calling memory var. ;
;									     ;
;	The lower level routines maintain a pointer to the current byte on   ;
;	the display surface in ES:DI and a bitmask in BL. These registers    ;
;	will not be touched by any other routines during the encoding loop.  ;
;									     ;
;----------------------------------------------------------------------------;

get_next_pel	proc	near

	cmp	BufferedPelCount,0	;is there any more pels in buffer
	jz	fill_get_buffer		;no

; at least one pel is in the buffer.

obtain_pel_from_buffer:

	dec	BufferedPelCount	;the pel is being taken out
	call	[get_pel_from_buffer]	;routine different for mono/color surf.
	ret

fill_get_buffer:

	call	[fill_pel_buffer]	;fill buffer, return first pel in AL
	ret

get_next_pel	endp

;----------------------------------------------------------------------------;
;	The routines which fill the pel buffer follow now.		     ;
;----------------------------------------------------------------------------;

;----------------------------------------------------------------------------;
;  Fill pel buffer routine for color screen surfaces (EGA/VGA)		     ;
;----------------------------------------------------------------------------;

;----------------------------------------------------------------------------;

; given a bit mask in BL, the following macro returns the number of pels we
; can retrieve from the byte in CH and the number of shls to get the masked
; bit into the MSB position in CL


get_count_of_pels macro
	local	get_mask_position
	local	got_mask_position

	mov	cl,bl			;;get the mask
	mov	ch,8			;;at best 8 bits can be obtained
get_mask_position:
	shl	cl,1			;;is mask bit in carry ?
	jc	got_mask_position	;;count of bits in byte is in cl
	dec	ch			;;one less bit in byte
        jnz     get_mask_position
got_mask_position:
	mov	cl,ch			;;no of valid bits in byte
	neg	cl
	add	cl,8			;;no of shifts to get masked bit
	inc	cl			;;into carry
	endm

;----------------------------------------------------------------------------;

dev_color_get_pels_masked  proc  near

; ES:DI has the byte in memory and BL has bit mask. DX has the address of the
; map mask register.

	push	cx			;save
	get_count_of_pels		;ch has pel count, cl pel count
	dec	ch			;one pel will be got now
        mov     BufferedPelCount,ch     ;number of buffered pels
	xor	bh,bh			;will hold the first pel

IFNDEF	CGAHERCMONO

	mov	al,GRAF_READ_MAP	;map select register
	mov	ah,3			;mask for plane 3

if NUMBER_PLANES eq 4

; read plane 3

	out	dx,ax			;enable plane 3

	mov	ch,ds:[si]		;get the byte
	shl	ch,cl			;get next(masked) color bit into carry
	rcl	bh,1			;gather in cl
	mov	PelPlane3,ch		;buffer the other pels

endif

; read plane 2

	dec	ah			;ah has 2
	out	dx,ax			;select plane 2
	mov	ch,ds:[si]		;get byte from plane 2
	shl	ch,cl			;get masked bit into carry
	rcl	bh,1			;gather bit into bh
	mov	PelPlane2,ch		;buffer rest of the bits

; read plane 1

	dec	ah			;al has 1
	out	dx,ax			;select plane 1
	mov	ch,ds:[si]		;get byte from plane 1
	shl	ch,cl			;get masked bit into carry
	rcl	bh,1			;gather bit into bh
	mov	PelPlane1,ch		;buffer rest of the bits

; read plane 0

	dec	ah			;al has 0
	out	dx,ax			;select plane 0

ENDIF

	mov	ch,ds:[si]		;get byte from plane 0
	shl	ch,cl			;get masked bit into carry
	rcl	bh,1			;gather bit into bh
	mov	PelPlane0,ch		;buffer rest of the bits
	pop	cx			;restore

; the pels are buffered and the next pel index is in BH. From the next
; call on we will be doing complete bytes without masks, so modify the
; calling memory variables address to the unmasked dev-get routine

	mov	ax,DIMapSegOFFSET dev_color_get_pels
	mov	fill_pel_buffer,ax
	mov	al,bh			;the return pel value
	inc	si			;point to next byte
	ret

dev_color_get_pels_masked endp
;----------------------------------------------------------------------------;
; The version of the above routine which gets complete bytes follows now     ;
;----------------------------------------------------------------------------;

; a macro to mask a plane, get the byte and shift the first pel to carry 
; follows

get_dev_byte	macro

IFNDEF	CGAHERCMONO
	out	dx,ax			;;select the proper mask register
ENDIF

	mov	ah,ds:[si]		;;get the byte from the plane
	shl	ah,1			;;shift the left most bit to carry
	rcl	bh,1			;;gather it in bh
	endm


dev_color_get_pels proc near

	mov	BufferedPelCount,7	;we return 1 pel, buffer the other 7

IFNDEF	CGAHERCMONO

	mov	al,GRAF_READ_MAP	;map select register

if NUMBER_PLANES eq 4
	mov	ah,3			;start with plane 3
	xor	bh,bh			;will hold first pel value
	get_dev_byte			;get byte from plane 3
	mov	PelPlane3,ah		;buffer the byte
endif

	mov	ah,2			;now plane 2
	get_dev_byte			;get byte from plane 2
	mov	PelPlane2,ah		;buffer the remaining pels
	mov	ah,1			;then plane 1
	get_dev_byte			;get byte from the plane
	mov	PelPlane1,ah		;save remaining pels

ENDIF

	xor	ah,ah			;finally plane 0
	get_dev_byte			;get the byte from the plane
	mov	PelPlane0,ah		;save the remaining pels
	inc	si			;point to next byte on the surface
	mov	al,bh			;the return pel value
	ret

dev_color_get_pels endp
;----------------------------------------------------------------------------;
; The version of the above two routines for a monochrome screen follows now  ;
;----------------------------------------------------------------------------;
dev_mono_get_pels_masked  proc  near

; ES:DI has the byte in memory and BL has bit mask. DX has the address of the
; map mask register.

	push	cx			;save
	get_count_of_pels		;ch has pel count, cl pel count
	dec	ch			;one pel will be got now
	mov	BufferedPelCount,ch	;number of buffered pels
	xor	bh,bh			;will hold the first pel

; read plane 0

IFNDEF	CGAHERCMONO

	xor	ah,ah
	mov	al,GRAF_READ_MAP
	out	dx,ax			;select plane 0

ENDIF

	mov	ah,ds:[si]		;get byte from plane 0
	shl	ah,cl			;get masked bit into carry
	rcl	bh,1			;gather bit into bh
	mov	PelPlane0,ah		;buffer rest of the bits
	pop	cx			;restore

; the pels are buffered and the next pel index is in BH. From the next
; call on we will be doing complete bytes without masks, so modify the
; calling memory variables address to the unmasked dev-get routine

	mov	ax,DIMapSegOFFSET dev_mono_get_pels
	mov	fill_pel_buffer,ax
	mov	al,bh			;the return pel value
	inc	si			;point to next byte
	ret

dev_mono_get_pels_masked endp
;----------------------------------------------------------------------------;
; The version of the above routine which gets complete bytes follows now     ;
;----------------------------------------------------------------------------;

dev_mono_get_pels proc near

	mov	BufferedPelCount,7	;we return 1 pel, buffer the other 7

IFNDEF	CGAHERCMONO
	xor	ah,ah			;only plane 0 to read
	mov	al,GRAF_READ_MAP
ENDIF

	xor	bh,bh			;will hold first pel value
	get_dev_byte			;get byte from plane 3
	mov	PelPlane0,ah		;buffer the byte
	inc	si			;point to next byte on the surface
	mov	al,bh			;the return pel value
	ret

dev_mono_get_pels endp

;----------------------------------------------------------------------------;
; Next are two routines for color memory surfaces, the first two get the pels;
; when a mask is there for the byte and the next when all 8 pels have to be  ;
; obtained. The next two are for monochrome memory surfaces.		     ;
;	    Here DX has the width of a scan in bytes.			     ;
;----------------------------------------------------------------------------;
mem_color_get_pels_masked  proc  near

; ES:DI has the byte in memory and BL has bit mask. DX has the address of the
; map mask register.

	push	cx			;save
	get_count_of_pels		;ch has pel count, cl pel count
	dec	ch			;one pel will be got now
	mov	BufferedPelCount,ch	;number of buffered pels
	xor	bh,bh			;will hold the first pel
	add	si,next_scan		;point to next scan

if NUMBER_PLANES eq 4
; read plane 3
	sub	si,dx			;point to plane 3 of current scan
	mov	ah,ds:[si]		;get the byte
	shl	ah,cl			;get next(masked) color bit into carry
	rcl	bh,1			;gather in cl
	mov	PelPlane3,ah		;buffer the other pels
endif

; read plane 2

	sub	si,dx			;point to plane 2
	mov	ah,ds:[si]		;get byte from plane 2
	shl	ah,cl			;get masked bit into carry
	rcl	bh,1			;gather bit into bh
	mov	PelPlane2,ah		;buffer rest of the bits

; read plane 1
	
      	sub	si,dx			;point to plane 1
	mov	ah,ds:[si]		;get byte from plane 1
	shl	ah,cl			;get masked bit into carry
	rcl	bh,1			;gather bit into bh
	mov	PelPlane1,ah		;buffer rest of the bits

; read plane 0

	sub	si,dx			;back to plane 
	mov	ah,ds:[si]		;get byte from plane 0
	shl	ah,cl			;get masked bit into carry
	rcl	bh,1			;gather bit into bh
	mov	PelPlane0,ah		;buffer rest of the bits
	pop	cx			;restore

; the pels are buffered and the next pel index is in BH. From the next
; call on we will be doing complete bytes without masks, so modify the
; calling memory variables address to the unmasked mem-get routine

	mov	ax,DIMapSegOFFSET mem_color_get_pels
	mov	fill_pel_buffer,ax
	mov	al,bh			;the return pel value
	inc	si			;point to next byte
	ret

mem_color_get_pels_masked endp
;----------------------------------------------------------------------------;
; The version of the above routine which gets complete bytes follows now     ;
;----------------------------------------------------------------------------;

; a macro to mask a plane, get the byte and shift the first pel to carry 
; follows

get_mem_byte	macro

	sub	si,dx			;;point to previous scan
	mov	ah,ds:[si]		;;get the byte from the plane
	shl	ah,1			;;shift the left most bit to carry
	rcl	al,1			;;gather it in al
	endm


mem_color_get_pels proc near

	mov	BufferedPelCount,7	;we return 1 pel, buffer the other 7
	xor	al,al			;will hold first pel value
	add	si,next_scan		;update to next scan

if NUMBER_PLANES eq 4
	get_mem_byte			;get byte from plane 3
	mov	PelPlane3,ah		;buffer the byte
endif

	get_mem_byte			;get byte from plane 2
	mov	PelPlane2,ah		;buffer the remaining pels
	get_mem_byte			;get byte from the plane
	mov	PelPlane1,ah		;save remaining pels
	get_mem_byte			;get the byte from the plane
	mov	PelPlane0,ah		;save the remaining pels
	inc	si			;point to next byte on the surface
	ret

mem_color_get_pels endp
;----------------------------------------------------------------------------;
; The version of the above two routines for a monochrome screen follows now  ;
;----------------------------------------------------------------------------;
mem_mono_get_pels_masked  proc  near

; ES:DI has the byte in memory and BL has bit mask. DX has the address of the
; map mask register.

	push	cx			;save
	get_count_of_pels		;ch has pel count, cl pel count
	dec	ch			;one pel will be got now
	mov	BufferedPelCount,ch	;number of buffered pels
	xor	bh,bh			;will hold the first pel

; read plane 0

	mov	ah,ds:[si]		;get byte from plane 0
	shl	ah,cl			;get masked bit into carry
	rcl	bh,1			;gather bit into bh
	mov	PelPlane0,ah		;buffer rest of the bits
	pop	cx			;restore

; the pels are buffered and the next pel index is in BH. From the next
; call on we will be doing complete bytes without masks, so modify the
; calling memory variables address to the unmasked mem-get routine

	mov	ax,DIMapSegOFFSET mem_mono_get_pels
	mov	fill_pel_buffer,ax
	mov	al,bh			;the return pel value
	inc	si			;point to next byte
	ret

mem_mono_get_pels_masked endp
;----------------------------------------------------------------------------;
; The version of the above routine which gets complete bytes follows now     ;
;----------------------------------------------------------------------------;

mem_mono_get_pels proc near

	mov	BufferedPelCount,7	;we return 1 pel, buffer the other 7
	xor	al,al			;will hold first pel value
	mov	ah,ds:[si]		;get the byte
	shl	ah,1			;shift the left pel into carry
	rcl	al,1			;gather the pel
	mov	PelPlane0,ah		;buffer the byte
	inc	si			;point to next byte on the surface
	ret

mem_mono_get_pels endp

;----------------------------------------------------------------------------;
; The next two routines obtain the next buffered pels. The first is for color;
; surfaces and the next for monochrome surfaces.			     ;
;----------------------------------------------------------------------------;

color_get_pel_from_buffer proc near

	xor	al,al			;will gather the pel bits here

if NUMBER_PLANES eq 4
	shl	PelPlane3,1		;shift the next pel bit in plane 3
	rcl	al,1			;gather the pel into al
endif

	shl	PelPlane2,1		;shift the next pel bit in plane 2
	rcl	al,1			;gather the pel into al
	shl	PelPlane1,1		;shift the next pel bit in plane 1
	rcl	al,1			;gather the pel into al
	shl	PelPlane0,1		;shift the next pel bit in plane 0
	rcl	al,1			;gather the pel into al
	ret

color_get_pel_from_buffer endp

mono_get_pel_from_buffer proc near

	xor	al,al			;will gather the pel bit here
	shl	PelPlane0,1		;shift off the next pel
	rcl	al,1			;gather it in al
	ret

mono_get_pel_from_buffer  endp

;----------------------------------------------------------------------------;
; The following two routines are for updating the source pointer after doing ;
; an absolute run of pels. The first routine is for 8 bit encoding and the   ;
; next is for 4 bit encoding.				                     ;
;									     ;
; Entry:							             ;
;        SI  -- pointer to start of last record.			     ;
;        AX  -- number of pels encoded in last record.			     ;
; Returns:							             ;
;	 SI  -- pointer to start of next record.			     ;
;----------------------------------------------------------------------------;

; routine to update source pointer for 8 bit encoding. (Each index is a byte)

update_set_src_ptr_4_bit:

        add     ax,NibbleToggle         ;handle odd nibble dudes
        mov     NibbleToggle,ax         ;save low bit in NibbleToggle
        and     NibbleToggle,1
        shr     ax,1                    ;convert nibble count to bytes

update_set_src_ptr_8_bit  proc near

        add     si,ax                   ;point to next RLE record
        jnc     @f                      ;advance segment if overflow
        mov     ax, ds
        add     ax, __NEXTSEG
        mov     ds, ax
@@:
	ret

update_set_src_ptr_8_bit  endp

;----------------------------------------------------------------------------;
; The following routine are used for obtaining the next index for a run of   ;
; pels in absolute mode. The first one is for 8 bit encoding, where each     ;
; index is in a byte and the second one is for 4 bit encoding where 2 indices;
; come in a byte.							     ;
;									     ;
; The third routine is for 4 bit mode too, but this is used when an encoded  ;
; record is reached in the 4 bit mode and the two nibbles of the color byte  ;
; are not the same.							     ;
;----------------------------------------------------------------------------;

; routine for 4 bit encoding. BitToggle if 0, return the left nibble of the 
; current byte, else return the right nibble and bump the pointer.

get_next_abs_mode_index_4_bit proc near

	xor	NibbleToggle,1		;update the toggle switch
	test	NibbleToggle,1		;test state
	jnz	left_nibble_4_bit	;was zero, so return left nibble

; the state was 1, return the right nibble and bump the pointer.

        call    ReadRLE
	and	al,00001111b		;retain the left nibble.
	ret

; the state was 0, retun the left nibble rotated into the right nibble.
left_nibble_4_bit:

	mov	al,[si]			;get the current byte
	shiftr	al,4			;get the left nibble into the right
	ret

get_next_abs_mode_index_4_bit endp

; routine to get the next encoded pel index in 4 bit mode.

get_next_encoded_4_bit_index proc near

; routine for 4 bit encoding. BitToggle if 0, return the left nibble of the 
; current byte, else return the right nibble.

	xor	NibbleToggle,1		;update the toggle switch
	mov	al,[si]			;get the current byte
	test	NibbleToggle,1		;test state
	jnz	encoded_left_nibble	;was zero, so return left nibble

; the state was 1, return the right nibble and bump the pointer.

	and	al,00001111b		;retain the left nibble.
	ret

; the state was 0, retun the left nibble rotated into the right nibble.
encoded_left_nibble:

	shiftr	al,4			;get the left nibble into the right
	ret

get_next_encoded_4_bit_index endp

;----------------------------------------------------------------------------;
; The following routine decides what strategy is to be adopted for handling  ;
; encoded run in 4 bit mode. For strtches of the same color we will reset the;
; carry flag and return, but for alternate color stretches, we will set carry;
; and return. For 8 bit mode, the address of the part that clears the carry  ;
; should be saved in the calling memory variable.			     ;
;									     ;
; Entry:    SI --- points to encode recors index byte.			     ;
;           AX --- to be preserved.				             ;
; Returns:							             ;
;         CARRY -- set if the run is of alternate colors		     ;
;         CARRY -- cleared if the run is of the same color		     ;
;----------------------------------------------------------------------------;

decide_4_bit_strategy_4_bit proc near
	
	push	ax			;save ax
	mov	al,[si]			;get the index
	mov	ah,al			;have a copy
	and	al,00001111b		;get the lower index
	shiftr	ah,4			;have the shifted upper index
	cmp	al,ah			;compare the two
	pop	ax			;restore ax
	je	same_color_strategy	;stretch is of same color
	stc				;set carry flag, different color
	ret
same_color_strategy:
	clc				;reset carry
	ret

decide_4_bit_strategy_4_bit endp

;----------------------------------------------------------------------------;
; The following routine gets the index value in an encoded record. DS:SI     ;
; points to the index byte. The first routine is for the 8 bit mode, where   ;
; all 8 bits in the byte constitute the index. The second routine for 4 bit  ;
; mode and will be used only when the stretch is of the same color so we can ;
; only look at the left nibble and shift it into the right nibble.	     ;
;----------------------------------------------------------------------------;

; routine for 4 bit encoding, each byte has two 4 bit indices, this routine
; will return the left nibble index shifted into the low nibble.

get_encoded_index_8_bit proc near

        mov     al,[si]
	ret

get_encoded_index_8_bit endp

; routine for 4 bit encoding, each byte has two 4 bit indices, this routine
; will return the left nibble index shifted into the low nibble.

get_encoded_index_4_bit proc near

        mov     al,[si]
	shiftr	al,4			;left nibble of interest
	ret

get_encoded_index_4_bit endp

if 0
;----------------------------------------------------------------------------;
; paint_background:							     ;
;									     ;
; for opaque draw modes, this routine paints the intersection of the clip &  ;
; the draw rectangle with the background color. For transparent draw modes   ;
; this routine is a NOP.						     ;
;----------------------------------------------------------------------------;

paint_background proc near

	les	di,lpDrawMode		;load a pointer to the drawmode struct
	mov	ax,es
        or      ax,di
        njz     paint_background_ret    ;is lpDrawMode NULL?
	cmp	es:[di].bkMode,OPAQUE	;is the draw mode opaque ?
	njnz	paint_background_ret	;this routine is a NOP for transparent
	mov	dh,bptr es:[di].bkColor.SPECIAL
	call	[munge_proc]		;translate the color for monochrome
	mov	BackColor,dh		;save the background color

; load the workrectangle with the points of the draw rectangle.

	smov	ds,ss
	lea	si,WorkRect		;load a pointer to the work rectangle
	mov	ax,DstX			;the top left x
	mov	ds:[si].left,ax		;save it
	add	ax,DstXE		;the X extent
	mov	ds:[si].right,ax	;save the right
	mov	ax,DstY			;the start Y
	add	ax,StartScan		;leave the part not in the band
	mov	ds:[si].top,ax		;this is the top left Y
	add	ax,NumScans		;the no of scans in the band
	mov	ds:[si].bottom,ax	;the bottom right y
	les	di,lpClipRect		;the clip rectangle

; DS:SI points to the display rectangle and ES:DI points to the clip ractangle
; Find out the intersection of the two and leave the result in the rect pointed
; to by DS:SI

	cCall	IntersectRects

; now dump the intersected rectangle one scan at a time with the background
; color.


	mov	cx,ds:[si].bottom
	sub	cx,ds:[si].top		;get the extent of the paint
	mov	ax,ds:[si].right
	sub	ax,ds:[si].left		;the no of pels in each scan
	mov	PelsPerPaintScan,ax	;save it
	jcxz	paint_background_ret	;nothing to piant
	push	Y			;save this variable
	mov	ax,ds:[si].top		;the first scan
	mov	Y,ax			;save in Y
	mov	di,ds:[si].left		;the left edge
	mov	es,seg_lpSurface	;load surface selector into es

paint_rectangle:
	push	cx			;save the no of scans to paint
	push	di			;save the start X
	

; calculate the start offset in the surface and the the bit mask and start 
; byte mask

	mov	cx,PelsPerPaintScan	;load the length of the segment
	call	set_get_byte_mask

; ES:DI	points to byte on destination surface to hold the first bit. 
; BL has the first byte mask, BH has the last byte mask and CX has the inner
; byte count.

	mov	al,BackColor		;load the back ground color

IFNDEF	CGAHERCMONO
	mov	dx,EGA_BASE+GRAF_DATA   ;address of bitmask register
	test	SurfaceFlags,RLE_DEVICE	;is surface a device ?
	jnz	set_paint_first    	;yes dx has correct value
ENDIF
	mov	dx,width_scan		;for memory get the width of a scan

set_paint_first:

; if cx is 0, the two byte masks are to be combined and only one byte to be
; done.

	mov	ah,bl			;get first byte mask
	jcxz	set_paint_last		;do the last byte only.

; do the first byte
; ES:DI-> destination byte. AH has byte mask. AL has pel color

	call	[set_partial_pels]	;set the pels for partial byte
	inc	di			;point to next byte
	dec	cx			;first pel was incuded in count

; now do the intermediate bytes and 8 bits will be set in a call

	mov	ah,0ffh			;all bits enabled
	jcxz	set_paint_last		;no intermediate bytes

IFNDEF	CGAHERCMONO
	test	SurfaceFlags,RLE_DEVICE	;is it a device
	jz	set_paint_loop
	xchg	ah,al			;get mask value
	out	dx,al			;set up mask
	xchg	ah,al			;restore original values
ENDIF

set_paint_loop:
	call	[set_full_pels]		;do a complete byte
	inc	di			;do next byte
	loop	set_paint_loop		;complete the intermediate runs

; now do the last byte in the run.

set_paint_last:
	and	ah,bh			;combine with previous mask
	call	[set_partial_pels]	;set pels in the last byte

; we are done with one scan.

	pop	di			;get back the left edge
	pop	cx			;scan remaining to be done
	inc	Y			;the next scan
	loop	paint_rectangle		;complete it
	pop	Y			
paint_background_ret:
	ret

paint_background endp
endif

;----------------------------------------------------------------------------;
; The following routine is used only for monochrome drivers. We want to use  ;
; Write mode 2 so that we can program a no of pels with the same value. But  ;
; unfortunately the cursor code for mono driver does not set its own mode. To;
; alleviate this we can fake WRITE MODE 2 by still having WRITE MODE 0, but  ;
; using a color value of 00h or FFh instead of 0 and 1.			     ;
;  									     ;
; This routine replicates the MONO_BIT in DH and creates a 00 or 0ffh in it  ;
; depending on whether it is a 0 or a 1.				     ;
;									     ;
; Entry:								     ;
;	 DH -- color index returned by get nearest color		     ;
; Return:							             ;
;	 DH = 00h if MONO_BIT was reset					     ;
;	    = 0FFH if MONO_BIT was set					     ;
;----------------------------------------------------------------------------;

mono_munge proc near

	and	dh,MONO_BIT		;test just the mono color bit
	cmp	dh,MONO_BIT		;carry set if color = 0
	cmc				;carry set if color was 1
	sbb	dh,dh			;desired return value
mono_munge_ret:
	ret

mono_munge endp

IFNDEF	CGAHERCMONO
;----------------------------------------------------------------------------;
; dev_init_write_2:							     ;
;									     ;
;	. Initializes EGA/VGA registers to following values.		     ;
;		. READ MODE 0, WRITE MODE 2			             ;
;		. NO SHIFT, REPLACE					     ;
;		. enable all planes for color driver, plane 0 for mono.	     ;
;		. save address of bit mask register in GRX address register  ;
;									     ;
; ENTRY:								     ;
;	ES	-   screen segment selector				     ;
;	BL	-   non zero for mono chrome driver			      ;
;----------------------------------------------------------------------------;

dev_init_write_2 proc near

	assumes	es,EGAMem

	mov	dx,EGA_BASE+GRAF_ADDR	;the GRX controller address

; for mono chrome drivers, we will not mess with the mode and s/r registers

	or	bl,bl			;is it a mono chrome driver
	jnz	skip_mode_sr_programming

; set up for write mode 2

	mov	al,GRAF_MODE		;the mode register
	mov	ah,M_COLOR_WRITE	;set up for READ 0/WRITE 2
if MASMFLAGS and EGA
	mov	es:shadowed_mode,ah	;save for cursor interrupt
endif
	out	dx,ax
	jmp	$+2			;I/O delay

; set up the combine and function register to be replace

	mov	al,GRAF_DATA_ROT	; the combine function register
	mov	ah,DR_SET		; use replace and no shifts
	out	dx,ax
	jmp	$+2			; I/O delay


skip_mode_sr_programming:

; set up the sequencer map mask register to enable all planes for color drivers and
; just plane 0 for monochrome drivers

	mov	ah,MM_ALL		;assume all planes are being enabled
	or	bl,bl			;is it a color driver
	jz	set_plane_masks		; yes
	mov	ah,00000001b		; select just plane 0
set_plane_masks:
	mov	dx,EGA_BASE + SEQ_ADDR	; the sequencer address register
	mov	al,SEQ_MAP_MASK		; the map mask register
	out	dx,ax			; set up proper mask
	jmp	$+2			; I/O delay

; finally set up the GRX address register with the index for the bit mask 
; register.

	mov	dx,EGA_BASE + GRAF_ADDR
	mov	al,GRAF_BIT_MASK	; the BIT MASK register
 	out	dx,al		
	jmp	$+2			; I/O delay


; the EGA/VGA registers are programmed as we will be needing them
		
	ret
dev_init_write_2 endp
;----------------------------------------------------------------------------;
; dev_reinit:								     ;
;		reinitializes EGA/VGA registers to default state	     ;
;									     ;
; Entry:								     ;
;		ES - screen segment selector				     ;
;               BL - non zero only for mono chrome driver		     ;
;----------------------------------------------------------------------------;

dev_reinit proc	near

	assumes	es,EGAMem

; disable all planes for Set/Rest

	mov	dx,EGA_BASE+GRAF_ADDR	; GRX select register
	mov	al,GRAF_ENAB_SR		; the Set/Reset register
	xor	ah,ah			; disable all planes
	out	dx,ax			

; enable all bits in bit mask register

	mov	al,GRAF_BIT_MASK	; the bit mask register
	mov	ah,0ffh			; select all bits
	out	dx,ax

; set the data rotate function to DR_SET

	mov	al,GRAF_DATA_ROT
	mov	ah,DR_SET
	out	dx,ax

; set map mask register in the sequencer to select required planes

	mov	ah,MM_ALL		;assume all planes are being enabled
	or	bl,bl			;is it a color driver
	jz	reset_plane_masks      	; yes
	mov	ah,00000001b		; select just plane 0
reset_plane_masks:
	mov	dl,SEQ_ADDR		; the select register
	mov	al,SEQ_MAP_MASK		; the map mask register
	out	dx,ax

; for monochrome drivers skip mode register programming

	or	bl,bl			;is it mono chrome
	jnz	skip_mode_reinit	;yes

; set read and write modes to 0

	mov	dl,GRAF_ADDR
	mov	al,GRAF_MODE		; the mode select register
	mov	ah,M_DATA_READ + M_PROC_WRITE
if MASMFLAGS and EGA
	mov	es:shadowed_mode,ah	; shadow the mode
endif
	out	dx,ax

skip_mode_reinit:

; leave the address of the bitmask register in the GRX address register

	mov	al,GRAF_BIT_MASK
	out	dx,al
	ret

dev_reinit endp

ENDIF
;----------------------------------------------------------------------------;


sEnd	DIMapSeg
end



