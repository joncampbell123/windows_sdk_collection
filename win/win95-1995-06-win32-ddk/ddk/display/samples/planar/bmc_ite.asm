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
;----------------------------------------------------------------------------;
;		         GetDeviceBitmapBits				     ;
;			 -------------------				     ;
;   Converts device EGA dependent bitmap format into a device independent    ;
;   bitmap format							     ;
;									     ;
; General Description:							     ;
;									     ;
;	There are four standard bitmap formats.  All device drivers	     ;
;	are required to be able to translate between any of these	     ;
;	formats and their own internal formats. The standard formats	     ;
;	are as follows:							     ;
;									     ;
;			      Bitcount	      Planes			     ;
;			      --------	      ------			     ;
;				  1		 1			     ;
;				  4		 1			     ;
;				  8		 1			     ;
;				 24		 1			     ;
;									     ;
;	These formats are chosen because they are identical or similar	     ;
;	to all formats commonly used by raster devices.  Only single	     ;
;	plane formats are standard, but it is very easy to convert these     ;
;	to any multiple plane format used internally by a device.	     ;
;									     ;
;	For both the device independent format and the device specific format;
;       the origin of the bitmap is at the top left hand corner of the bitmap;
;       X axis extends to the right and Y towards the bottom, both moving to-;
;       -wards higher memory addresses.					     ;
;									     ;
;  ************************************************************************  ;
;  NOTE: BMC_ITE.ASM IN DISP20 & DISP30 TREES ARE IDENTICAL. AFTER CHANGING  ;
;  ONE. PLEASE MAKE SURE THAT YOU COPY THE FILE OVER TO THE OTHER TREE AND   ;
;  CHECK IT IN.								     ;
;  ************************************************************************  ;
;									     ;
;----------------------------------------------------------------------------;

SD_COLOR		equ	01000000b
SD_HUGE			equ	00100000b
CAN_CROSS_SEG           equ     00010000b
MONO_BIT	        equ     00010000b

;----------------------------------------------------------------------------;
 
	.xlist
	include	cmacros.inc
	include	gdidefs.inc
	include lines.inc
	include	macros.inc
	include display.inc
	include mflags.inc
	.list

	externA		__NEXTSEG	; offset to next segment


LOHI	STRUC
lo			dw	0	; lo word of a double word
hi			dw	0	; hi word of a double word
LOHI	ENDS

lpDECODE STRUC
off			dw	0	; offset part of a long pointer
sel			dw	0	; selector or segment part
lpDECODE ENDS


createSeg _DIMAPS,DIMapSeg,word,public,CODE
sBegin	DIMapSeg
	assumes cs,DIMapSeg

	externB	    DIMapSeg_color_table	; in vga|ega.asm
	externNP    do_bitmap_computations	; in bmc_eti.asm	

;----------------------------------------------------------------------------;
; The following table contains the addresses of the initialization routines  ;
; for each of the 4 Device Independent Bitmap formats.			     ;
;----------------------------------------------------------------------------;

anpfnPreProc	label	word
	dw	ite_1
	dw	ite_4
	dw	ite_8
	dw	ite_24

;----------------------------------------------------------------------------;
;ColorToMono table is used to convert a color bitmap into a monochrome DIB.  ;
;This table contains the thresholds determining which colors will be white   ;
;and which ones will be black.	The thresholds are the same as in rgb_to_ipc ;
;----------------------------------------------------------------------------;

ColorToMono label byte

if	MASMFLAGS and EGA

	db  0		;map black to 0
	db  0		;map low red to 0
	db  0		;map low green to 1
	db  0		;map low yellow to 1
	db  0		;map low blue to 0
	db  0		;map low magenta to 1
	db  0		;map low cyan to 1
	db  0		;map low gray to 0
	db  0		;map black to 0
	db  0		;map high red to 0
	db  1		;map high green to 1
	db  1		;map high yellow to 1
	db  0		;map high blue to 0
	db  1		;map high magenta to 1
	db  1		;map high cyan to 1
	db  1		;map high gray to 0

endif
if	MASMFLAGS and VGA

	db  0		;map black to 0
	db  0		;map low red to 0
	db  0		;map low green to 1
	db  0		;map low yellow to 1
	db  0		;map low blue to 0
	db  0		;map low magenta to 1
	db  0		;map low cyan to 1
	db  0		;map low gray to 0
	db  1		;map high grey to 1
	db  0		;map high red to 0
	db  1		;map high green to 1
	db  1		;map high yellow to 1
	db  0		;map high blue to 0
	db  1		;map high magenta to 1
	db  1		;map high cyan to 1
	db  1		;map high gray to 0

endif

	.errnz	(size RGBQuad)-4

	assumes ds,nothing
	assumes es,nothing

cProc	GetDeviceBitmapBits,<NEAR,PASCAL,PUBLIC>,<si,di,ds,es>
	parmD   lp_bits_own             ; -> pointer to own internal format
	parmD	cyScans			; no of scans to copy
	parmD	iStart			; start copy at this scan
	parmD   lp_bits_ind		; -> pointer to independent format
	parmD	lpbi			; -> Pointer to info block
	parmB	fbsd			; own format color or mono

	localW	scans_copied		; Number of scanlines copied
	localW	scanline_count		; Number of scanlines to copy
	localW	bitmap_width		; Width in pixels of the bitmap
	localW	bitmap_height		; Height of the bitmap in scans
	localW	width_scan		; Width of one scanline in bytes
	localW	next_scan		; Index to next scan line
	localW	scans_per_seg		; Scanlines per segment
	localW	scan_byte_count 	; # of full bitmap bits per scanline
	localW	full_byte_proc		; Routine which xlates full bytes
	localW	partial_byte_flags	; Flags for a partial byte
	localW	partial_byte_proc	; Routine which xlates partial bytes
	localW	huge_scans_left 	; Count of scans left in huge segment
	localW	some_temp		; temp variable to save counts
	localW	buffer_adjust		; Alignment for each scan of buffer
	localW	temp_byte_count 	; Work area for conversion routines
	localW	convert_C0		; Mono to 3/4 plane conversion routine
	localW	convert_C1		; Mono to 3/4 plane conversion routine
	localW	convert_C2		; Mono to 3/4 plane conversion routine

if NUMBER_PLANES eq 4
	localW	convert_C3		; Mono to 4 plane conversion routine
endif

	localW	alignment		; Shift to align partial bytes
	localD	scan_length		; length of one buffer scanline
	localW  NextSegOff		; offset to next huge map segment
	localW	filler_bytes		; filler bytes at segment end
	localW	prev_offset		; last scan offset in prev segment
	localB	temp_bit_count		; Work area for conversion routines

	localV	color_xlate,256 	; Color translation table
	localW	ColorsUsed		; no of colors used, 7fffh => all
	localW	NumSegments		; Number of segments in hugh bitmap.

cBegin
	cld

;----------------------------------------------------------------------------;
; Test to see that the format of the Device Independent Bitmaps is one that  ;
; we support. 								     ;
; The number of color planes must be 1, and the bits per pel can be 1/4/8/24 ;
;----------------------------------------------------------------------------;

	les	bx,lpbi 		; far pointer to Bitmap Header
	assumes es,nothing

; initialize the colors used and size field of the info block

	mov	wptr es:[bx].biClrUsed,0; we will return default size
	mov	wptr es:[bx].biClrUsed+2,0
	mov	wptr es:[bx].biSizeImage,0; initialize
	mov	wptr es:[bx].biSizeImage+2,0	

	xor	cx,cx			; will be the success/failure code 
        cmp     es:[bx].biPlanes,1      ; # of color planes must be 1
	jne	cbmi_invalid_format	; not supported
	mov	ax,es:[bx].biBitCount 
	cmp	ax,1			; test the bits per pel value
	je	cbmi_have_format	; CX has the index
	inc	cx			; try next index
	cmp	ax,4			; 4 bits per pel ?
	je	cbmi_have_format	; index = 1
	inc	cx			; try next index
	cmp	ax,8			; 8 bits per pel ?
	je	cbmi_have_format	; index = 2
	inc	cx			; try next index
	cmp	ax,24			; 4 bits per pel ?
	je	cbmi_have_format	; index = 3
cbmi_invalid_format:
	mov	cx,0ffffh		; want CX to be made zero below
	xor	ax,ax			; error code
	jmp	ite_exit		; get out

cbmi_have_format:
	mov	ax,cx			; get the index
	add	ax,ax			; double it for word table access

;----------------------------------------------------------------------------;
; perform the format specific initialization now			     ;
;----------------------------------------------------------------------------;
	push	bx			; save offset to info block
	xchg	ax,bx
	mov	si,anpfnPreProc[bx]	; Save preprocessor's address
	pop	bx			; get back offset to info block

; SI has the address of the format specific pre-processor routine
; Copy the bitmap data as necessary to local frame storage

	cCall	do_bitmap_computations  
	mov	di,scanline_count
	mov	scans_copied,di		; Set up return value.

; the above routine does the following:
;	.   copies parameters from the info block to local storage
;	.   deceided whether the internal map is huge or small
;       .   clears carry flag if no copy is to be done
;       .   sets tha carry flag if copy is to be done and in DX:AX
;           returns the start pointer in the internal map

	jnc	ite_copy_needed		; test for error
	mov	scans_copied,0
@@:	jmp	ite_xfered_all		; successful return
ite_copy_needed:
	or	di,di
	jz	@b			; 0 scans copied.
	mov	ds,dx
	assumes ds,nothing
	mov	si,ax			; set DS:SI point to internal map

; DS:SI now points to the starting position in source (device dependent) map

	les	di,lp_bits_ind		; target is user's buffer 
        assumes es,nothing

; if lpBits is NULL, just calc size and return

        mov     ax,es
        or      ax,di
        jnz     @f
        jmp     ite_calc_size
@@:
;----------------------------------------------------------------------------;
; A point to remember. The sorce of the transfer is just an array of bits in ;
; the specified format. It is not a complete bitmap, but just a portion that ;
; is to be copied. We, thus, do not have to do any further calculations for  ;
; obtaining the starting address in the source buffer, it is the passed long ;
; pointer. Also note that the user buffer may arbitarily cross a segment     ;
; boundary (even a single scan line could span the boundary). However even in;
; source the scan lines will be alligned to a DWORD (they could still cross a;
; segment if the bytes per scan is large enough).                            ;
;----------------------------------------------------------------------------;

; test to see whether users buffer cross a segment or not, if it does special 
; code will be needed.
	mov	cx,scanline_count	; no of scans to copy
	mov	ax,scan_length.hi	; do full 32 bit multiplication
	mul	cx			; multiply the HIWORD

; assuming that scan_lenght.hi is not so large to overflow result into DX

	xchg	ax,bx			; save in bx
	mov	ax,scan_length.lo	; LOWORD multiplication
	mul	cx
; BX has the HIWORD mul result

	add	dx,bx			; DX:AX = total bytes of buffer needed
	sub	ax,1
	sbb	dx,0			; Back off my one.

; DI has the start offset to the user's buffer

	add	ax,di
	adc	dx,0			; DX:AX = last address
	jz	ite_in_segment		; Won't cross a segment boundary
	or	fbsd,CAN_CROSS_SEG	; Can cross a segment boundary
	mov	NumSegments,dx		; Store number of segments in user's buffer.

ite_next_scan:
	test	fbsd,CAN_CROSS_SEG	; If buffer might cross a segment
	jnz	ite_might_cross_seg	; then go check it out

ite_in_segment:
	push	si			;Save bitmap scan start address
	mov	cx,scan_byte_count	;Set translation procedure loop count
	jcxz	ite_see_about_partial	;No full bytes!
	call	full_byte_proc		;Process all complete bytes

ite_see_about_partial:
	mov	cx,partial_byte_flags	;Process any partial byte
	jcxz	ite_no_partial_byte
	call	partial_byte_proc

ite_no_partial_byte:
        pop     si
	dec	scanline_count
	jz	ite_xfered_all		;Transfered all scans

; we pointer for the own bitmap starts at the bottom and then goes up

	sub	si,next_scan		;--> previous bitmap scanline
	add	di,buffer_adjust	;Align to dword boundary
	jnc	ite_chk_segment_update	;don't update segment if no wrap
	mov	ax,es			; otherwise, increment segment
	add	ax,__NEXTSEG
	mov	es,ax
	assumes es,nothing

ite_chk_segment_update:
	test	fbsd,SD_HUGE		; If not a huge bitmap,
	jz	ite_next_scan		; go do next scanline

	dec	huge_scans_left 	; Any more scans in this segment?
	jnz	ite_next_scan		; yes, don't update bitmap pointer
	mov	ax,ds
	sub	ax,__NEXTSEG		; go to the previous segment
	mov	ds,ax
	assumes ds,nothing

	mov	si,prev_offset		; start from the bottom of the segment
	mov	ax,scans_per_seg
	mov	huge_scans_left,ax	; This many scans in the segment
	jmp	ite_next_scan

ite_might_cross_seg:

;	The buffer will cross a segment boundary.  If this scanline will
;	cross the segment, then we'll process eight bits of bitmap data
;	at a time.  !!! someday we may improve upon this, but we don't
;	expect this to be critical !!!

	mov	ax,di			;If the entire scan shows, then
	xor	dx,dx			;  we can use the normal code
	add	ax,scan_length.lo
	adc	dx,scan_length.hi	;scan_length may be > 64K
	jz	ite_in_segment		;It won't cross a segment boundary

;	The scan will cross a segment boundary.   We'll take the easy
;	way out and do the entire scanline one bitmap byte at a time.

	push	si			;Save bitmap scan start address
	mov	cx,scan_byte_count	;If only a last byte exists,
	jcxz	ite_see_about_partial	;  use the normal code
	mov	some_temp,cx		;Save full byte count

ite_looking_to_cross:
	mov	cx,8			;Always xlate 8 pixels of data
	call	partial_byte_proc
	dec	some_temp
	jnz	ite_looking_to_cross
	jmp	ite_see_about_partial	;Finish with partial byte code

ite_xfered_all:

; compute and save the size of the DIB created

ite_calc_size:
        les     di,lpbi                 ;load a pointer to the info block
        assumes es,nothing

        mov     ax,wptr es:[di].biBitCount
        mov     bx,wptr es:[di].biWidth
        mul     bx
        add     ax,001Fh                ; Dont forget 32 bit aligned
        and     ax,0FFE0h
        shiftr  ax,3
        mov     bx,wptr es:[di].biHeight
        mul     bx

	mov	wptr es:[di].biSizeImage,ax
        mov     wptr es:[di].biSizeImage+2,dx

	mov	ax,scans_copied 	;Return number of scans copied
ite_exit:
	cwd
cEnd

;----------------------------------------------------------------------------;
comment ~
	    	METHOD OF TRANSFERRING A SINGLE SCAN LINE
		-----------------------------------------

 We know that:
 		. The source may have 1,4,8 or 24 bits per pel, and
		. the size of each scan in the source buffer is a multiple
		  of 4 bytes (DWORD)

 This means that towards the end of the scan line some of the bits will be filler
 bits and not information bits. We have the option of skipping over the filler bits
 or convert them too - and would choose which ever gives a simpler algorithm.

 Device dependent bitmaps also store scan lines as a multiple of DWORDS and hence
 we will also have filler bits in our internal formats.

 Consider, the first two formats: (1 or 4 bits per pel)
 1 or 4 bits from the source will be mapped into 1 bit (for each plane) in the
 target, and we know that the no of bits in the source are a multiple of 32 
 (including the fiiler bits ofcourse). As 32/4 = 8, so if we converts all the 
 bits we will always have an integral number of bytes in the destination. Also
 since we shall be converting all the bits, we will not have to skip over the 
 filler bits in the source buffer to move ober to the next scan line.

 Consider the 8 bits per pel format:
 8 bits from the source will be converted into one bit in the target, and the 
 source scan has a multiple of 32 bits. Here again we will try to convert the 
 whole of the source scan line including the filler bits, thus here again at 
 the end of the copy of one scan line, the source pointer will not have to be
 advanced over the filler  bytes. However, 32/8 is 4, and hence if the length 
 of the source scan line is an odd multiple of a DWORD size, then we will have 
 an incomplete byte at the end for the destination.

 How do we generate the target bits ? we process them a byte at a time. We 
 extract the required number of source bits, convert them into one destination
 bit (based on the color mapping) and start building a target byte by stuffing
 it into a byte variable from the right. Once 8 targer bits have been compiled
 we store the target byte.

 With this approch, for the 8 bits/pel case, if we do have a leftover nibble,
 it will be at the right end of the work byte. Thus it has to be alligned 
 against the left end and then stored. So we will also prepare an allignment
 mask for this case.

 Finally consider the 24 bits/pel case. Here the 24 bits to a pel themselves
 give the color value and we will always process the source 24 bits at a time.
 However as 32 is not a multiple of 24, we cannot always include the filler bits
 in the conversion. So in this case the source pointer will have to be carried
 over the filler bytes and we will calculate the number of bytes of sdjustment
 rewuired.

 In the last case 24 bits get mapped into 1 target bit so for the target we 
 may then get an incomplete byte again, which has to be correctly alligned
 before storing.

 We will thus calculate the following parameters for each of the formats:

 	scan_byte_count     ----   the number of complete bytes for target
	scan_length	    ----   no of bytes for source which yield the above
	partial_byte_flags  ----   the last few source pels which do not yield
	                           a complete target byte. 
        allignment          ----   no of positions by which the incomplete last
	                           target byte is to be shifted before storing
        buffer_adjust       ----   no of bytes to be added to the the source 
	                           ptr at the end of the copy to move over to
				   the start of the next scan line in src buf

 Let's see how we calculate these values:

 1 bits per pel:
 ---------------
 scan_byte_count = scan_length = width_scan (no of bytes in one scan)
 rest of the parameters are all zero.


 4 bits per pel:
 ---------------
 We have to first calculate the no of bits in the source buffer	after the 
 DWORD allignment.

 To do this we have to multiply the no of pels by 4, add 31 and AND with
 NOT(31). This then has to be divided by 8*4 to give the no of bytes. The 
 process can be simplified if we observe what exactly happens in binary.

 let n a string of 0's and 1's representing the no of pels in binary.

    n*4	 -----                   n00      (shiftl n,2)
add 31                  +      11111
AND NOT(31)             &   ..100000
divide by 8*4    	    shift right by 5 bits

This is equivelent to:
				 
			        n
			+     111   (add n,7)
		    shift right 5 3 bits

 The above result will be scan_byte_count, and this multiplied by 4
 will give scan_length.

 Here again we will not have partial bytes neither any source buffer 
 adjustment byte.

 8 bits per pel:
 ---------------

 This case is very similar to the 4 bits per pel case. Following similar
 logic,
 			mov	ax,n	--  no of pels
			add	ax,3    --  last 2 bits of value 31
			mov	bx,ax
           this value /8 gives no of bytes in target
	   		shiftr	ax,3	--- scan_byte_count
	   to get the no of bytes in source we will have to AND NOT(31)
	   and divide by 8, but since last 3 bits are always zero, so
	   		and	bl,0f4h -- scan_length in bx

 also note since we pack 8 bits into 1 bit, the remainder on division by
 64 gives the last few bits whcich can't be packed into a byte, then divided
 by 8 gives the last few pels that cant be packed in a byte.
 This can be obtained simply by ANDind BX with 4 and note that the result will
 be 0 or 4 (as at best one nibble can be left over) and if it is 4, then that
 is also the alignment factor. 

 24 bits per pel:
 ----------------

 Here we will not be converting any of the filler bits. So if n is the no.	
 of pels, n/8 gives the no. of complete target bytes and rem(n,8) gives the 	
 number of pels in the incomplete byte, the differnece of this from 8 will
 give us the allignment(ie, the number of left shifts).			       
									       
 mod(allignment,4) will give us the number of bytes by which the source pointer
 has to be advanced at the end of the copy.				       
									       
 enc comment ~								       
;----------------------------------------------------------------------------;


page
;--------------------------Private-Routine-----------------------------------;
; ite_1									     ;
;									     ;
;   Bitmap conversion, internal format to external 1 bit		     ;
;									     ;
;   This routine is responsible for all preprocessing for converting	     ;
;   from our internal formats to external 1 bit format			     ;
;									     ;
; Entry:								     ;
;	ES:DI --> where color table would go				     ;
; Returns:								     ;
;	None								     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

ite_1	proc	near

	xor	ax,ax
	mov	buffer_adjust,ax	;Everything will be dword aligned
	mov	partial_byte_flags,ax	;Will never have a partial byte
	mov	scan_length.hi,ax	;Max dest scan size is < 64K
	mov	ax,DIMapSegOFFSET copy_i1e1 ;Assume monochrome bitmap
	mov	bx,DIMapSegOFFSET copy_i1e1_partial
	test	fbsd,SD_COLOR		;Monochrome source?
	jz	ite_1_have_proc 	;  Yes

	mov	ax,DIMapSegOFFSET copy_ie1 ;Color bitmap
	mov	bx,DIMapSegOFFSET copy_ie1_partial

ite_1_have_proc:
	mov	full_byte_proc,ax
	mov	partial_byte_proc,bx

;	Whenever we allocated a scanline (or each scan of a color
;	bitmap), we allocated to a dword boundary.  If we just
;	translate for the width of the scan in bytes, the required
;	dword alignment will be maintained.

	mov	ax,width_scan		;Moving all bytes of the scan
	mov	scan_byte_count,ax
	mov	bx,ax			; save

; independent map is alligned on DWORDS and own map on WORDS
; so may have left over bytes at the end
	
	mov	ax,bitmap_width		; get no of pels
	mov	cx,31
	add	ax,cx			; try to allign to dword
	not	cx
	and	ax,cx			; ax is alligned to dwords
	shiftr	ax,3			; get no of bytes
	mov	scan_length.lo,ax	; Set # bytes of user buffer required
	sub	ax,bx			; subtract no of bytes in internal
	mov	buffer_adjust,ax	; filler bytes

	mov	cx,2*4			;Two color table entries
	call	create_mono_color_table	;each 3 bytes
	ret

ite_1	endp

page
;--------------------------Private-Routine-----------------------------------;
; ite_4									     ;
;									     ;
;   Bitmap conversion, internal format to external 4 bit		     ;
;									     ;
;   This routine is responsible for all preprocessing for converting	     ;
;   from our internal formats to external 4 bit format			     ;
;									     ;
; Entry:								     ;
;	ES:DI --> where color table would go				     ;
; Returns:								     ;
;	None								     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

ite_4	proc	near

	xor	ax,ax
	mov	buffer_adjust,ax	;Everything will be dword aligned
	mov	partial_byte_flags,ax	;Will never have a partial byte
	mov	scan_length.hi,ax	;Max dest scan size is < 64K
	mov	cx,16*(size RGBQuad)	;There will be 16 color table entries
	test	fbsd,SD_COLOR		;Monochrome source?
	jnz	ite_4_not_mono		;Not mono, must be color
	call	create_mono_color_table
	mov	ax,DIMapSegOFFSET copy_i1e4
	mov	bx,DIMapSegOFFSET copy_i1e4_partial
	jmp	short ite_4_have_proc

ite_4_not_mono:
	call	create_color_color_table
	mov	ax,DIMapSegOFFSET copy_ie4
	mov	bx,DIMapSegOFFSET copy_ie4_partial

ite_4_have_proc:
	mov	full_byte_proc,ax
	mov	partial_byte_proc,bx	;Might need this for seg boundaries
	mov	ax,bitmap_width 	;Compute the number of source bytes
	add	ax,7			;We can force dword alignment by
	shiftr	ax,3			;  always xlating a multiple of 8 bits
	mov	scan_byte_count,ax
	shiftl	ax,2			;1 source byte = 4 dest bytes
	mov	scan_length.lo,ax	;Set # bytes of user buffer required
	ret				;  for one bitmap scan

ite_4	endp
page

;--------------------------Private-Routine-----------------------------------;
; ite_8									     ;
;									     ;
;   Bitmap conversion, internal format to external 8 bit		     ;
;									     ;
;   This routine is responsible for all preprocessing for converting	     ;
;   from our internal formats to external 8 bit format			     ;
;									     ;
; Entry:								     ;
;	ES:DI --> where color table would go				     ;
; Returns:								     ;
;	None								     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

ite_8	proc	near

	xor	ax,ax			;Will not require any buffer adjustment
	mov	buffer_adjust,ax
	mov	scan_length.hi,ax	;Max dest scan size is < 64K
	mov	cx,256*(size RGBQuad)	;There will be 256 color table entries
	test	fbsd,SD_COLOR		;Monochrome source?
	jnz	ite_8_not_mono		;Not mono, must be color
	call	create_mono_color_table
	mov	ax,DIMapSegOFFSET copy_i1e8
	mov	bx,DIMapSegOFFSET copy_i1e8_partial
	jmp	short ite_8_have_proc

ite_8_not_mono:
	call	create_color_color_table
	mov	ax,DIMapSegOFFSET copy_ie8
	mov	bx,DIMapSegOFFSET copy_ie8_partial

ite_8_have_proc:
	mov	full_byte_proc,ax
	mov	partial_byte_proc,bx
	mov	ax,bitmap_width 	;Compute the number of source bytes
	add	ax,3			;  to convert.	Each nibble becomes
	mov	bx,ax			;  a double word in the destination
	shiftr	ax,3
	mov	scan_byte_count,ax
	and	bl,11111100b		;1 source nibble = 4 dest bytes
	mov	scan_length.lo,bx	;Set # bytes of user buffer required
	and	bx,4			;Will either have a partial nibble or
	mov	partial_byte_flags,bx	;  no extra nibble to xlate
	ret

ite_8	endp

page
;--------------------------Private-Routine-----------------------------------;
; ite_24								     ;
;									     ;
;   Bitmap conversion, internal format to external 24 bit RGB		     ;
;									     ;
;   This routine is responsible for all preprocessing for converting	     ;
;   from our internal formats to external 24 bit RGB format		     ;
;									     ;
; Entry:								     ;
;	ES:DI --> where color table would go				     ;
; Returns:								     ;
;	None								     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

ite_24	proc	near

	mov	ax,DIMapSegOFFSET copy_i1e24
	mov	bx,DIMapSegOFFSET copy_i1e24_partial
	test	fbsd,SD_COLOR		;Monochrome source?
	jz	ite_24_have_proc	;  Yes

ite_24_not_mono:
	mov	ax,DIMapSegOFFSET copy_ie24
	mov	bx,DIMapSegOFFSET copy_ie24_partial

ite_24_have_proc:
	mov	full_byte_proc,ax
	mov	partial_byte_proc,bx
	mov	cx,3
	mov	ax,bitmap_width 	;Compute the number of source bytes
	mov	bx,ax			;  to convert.	Each bit becomes
	shr	ax,cl			;  three bytes in the destination
	mov	scan_byte_count,ax
	mov	ax,bx			;1 source bit = 3 dest bytes
	mul	cx
	add	ax,cx			;Need to round up to a double word
	adc	dx,0
	and	al,11111100b
	mov	scan_length.lo,ax	;Set # bytes of user buffer required
	mov	scan_length.hi,dx
	and	bx,00000111b		;Can have up to 7 bits in the last
	mov	partial_byte_flags,bx	;  byte to convert
	and	bl,00000011b		;Compute number of bytes to pad with
	mov	buffer_adjust,bx
	ret

ite_24	endp

page
;--------------------------Private-Routine-----------------------------------;
; create_mono_color_table						     ;
;									     ;
;   The color table for monochrome source bitmaps is created.  The	     ;
;   table will consists of all black except for the last entry which	     ;
;   will be white.							     ;
;									     ;
; Entry:								     ;
;	ES:DI --> where color table would go				     ;
;	CX    =   (total number of color table entries to generate)*size RGBQuad;
; Returns:								     ;
;	None								     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing


create_mono_color_table proc near

	sub	cx,size RGBQuad
	xor	ax,ax
	shr	cx,1			;CX better be even!
	rep	stosw
;	rcl	 cx,1
;	rep	 stosb
	dec	ax
	stosw
	stosb
	inc	ax
	stosb
	ret

create_mono_color_table endp

page
;--------------------------Private-Routine-----------------------------------;
; create_color_color_table						     ;
;									     ;
;   The color table for color source bitmaps is created.  The table	     ;
;   will consists of the 16 fixed colors for the driver.  The remaining	     ;
;   entries will be set to black, even though we will never generate	     ;
;   those indicies.							     ;
;									     ;
; Entry:								     ;
;	ES:DI --> where color table would go				     ;
;	CX    =   total number of color table entries to generate, * 4	     ;
; Returns:								     ;
;	None								     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

create_color_color_table proc near

	mov	bx,cx			;Save total table count
	xchg	ax,si			;Don't destroy si

	lea	si,DIMapSeg_color_table
	mov	cx,32			;16 entries, 4 bytes each, word movs.
	rep	movs word ptr es:[di], word ptr cs:[si]

	xchg	ax,si			;Restore caller's SI
	sub	bx,16*(size RGBQuad)
	.errnz	(size RGBQuad)-4	;make sure RGBQuad is the correct size!
	jle	ccct_exit		;All done
	mov	cx,bx
	xor	ax,ax			;Fill rest of table with black
	shr	cx,1
	rep	stosw
	rcl	cx,1
	rep	stosb

ccct_exit:
	ret

create_color_color_table endp

page

xlate_1_to_4	label	word

	dw	0000000000000000b	;0000b
	dw	0000000000001111b	;0001b
	dw	0000000011110000b	;0010b
	dw	0000000011111111b	;0011b
	dw	0000111100000000b	;0100b
	dw	0000111100001111b	;0101b
	dw	0000111111110000b	;0110b
	dw	0000111111111111b	;0111b
	dw	1111000000000000b	;1000b
	dw	1111000000001111b	;1001b
	dw	1111000011110000b	;1010b
	dw	1111000011111111b	;1011b
	dw	1111111100000000b	;1100b
	dw	1111111100001111b	;1101b
	dw	1111111111110000b	;1110b
	dw	1111111111111111b	;1111b

page
;--------------------------Private-Routine-----------------------------------;
; copy_i1e1								     ;
;									     ;
;   The given number of bytes are copied from source to destination.	     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings.	     ;
;									     ;
;   This routine is used for:						     ;
;	internal mono to external mono					     ;
;	external mono to internal mono					     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of bytes to copy				     ;
;	Direction flag cleared						     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;	Direction flag cleared						     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

	public	copy_i1e1

copy_i1e1	proc	near

	xchg	ax,cx			;Save count of bytes to mov in AX
	xor	cx,cx
	ror	si,1			;Set 'C' if source on odd boundary
	rcl	cx,1			;CX = 1 if on odd boundary
	sub	ax,cx
	rol	si,1			;Restore initial source address
	rep	movsb			;Move possible odd aligned byte
	xchg	ax,cx			;Move complete words
	shr	cx,1
	rep	movsw
	rcl	cx,1			;Process trailing odd byte
	rep	movsb
	ret

copy_i1e1	endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_i1e4								     ;
;									     ;
;   copy internal 1 bit to external 4 bit packed pixel			     ;
;									     ;
;   The destination is filled with the source data, with each source	     ;
;   bit expanded to a nibble in the destination.			     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings,	     ;
;   and to ensure that there are enough bytes of destination available	     ;
;   to hold the data.							     ;
;									     ;
;   This routine is used for:						     ;
;	internal mono to external four-bit packed pixels		     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of bytes to xlate				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_i1e4	proc	near

	mov	dx,00011110b		;Mask for bits to convert

ci1e4_next_byte:
	lodsb
	mov	bx,ax			; get the left nibble to lsb and then
	shiftr	bl,3		        ; shl by 3 is equivalent to shr by 3
	and	bx,dx			; we thus convert 4 bits in one go
	mov	bx,xlate_1_to_4[bx]
	xchg	ax,bx
	xchg	ah,al
	stosw				; now convert the right nibble
	shl	bx,1
	and	bx,dx
	mov	ax,xlate_1_to_4[bx]
	xchg	ah,al
	stosw
	loop	ci1e4_next_byte
	ret

copy_i1e4	endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_i1e4_partial							     ;
;									     ;
;   copy internal 1 bit to external 4 bit packed pixel, partial byte	     ;
;									     ;
;   The destination is filled with the source data, with each source	     ;
;   bit expanded to a nibble in the destination.  This routine handles	     ;
;   partial bytes where some portion of the first or last byte needs	     ;
;   to be saved.							     ;
;									     ;
;   This routine will handle the destination (user's buffer) crossing	     ;
;   a segment boundary during the translation of the source byte.	     ;
;   The source will never cross a segment boundary since it is an	     ;
;   internal bitmap and all scans are allocated to fit within a		     ;
;   segment.								     ;
;									     ;
;   Since this routine always writes nibbles, it is guaranteed that	     ;
;   the shift count passed in is even.					     ;
;									     ;
;   This routine shouldn't be called very often, so a short loop will	     ;
;   be sufficient for our needs.					     ;
;									     ;
;   This routine is used for:						     ;
;	internal mono to external four-bit packed pixels		     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CL     =  number of pixels to xlate				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_i1e4_partial proc near

	mov	bx,DIMapSegOFFSET xlate_1_to_4 
	lodsb
	mov	ah,al
	shr	cx,1			;/2 for number of iterations
	xor	ch,ch

ci1e4p_next_byte:
	rol	ax,1
	rol	ax,1			; get two of the bits off ah
	and	al,00000011b
	push	ax			; save ah
	shl	ax,1			;Fetching bytes from a word table
	xlat	cs:[bx]
	call	store_with_seg_check
	pop	ax			; restore ah
	loop	ci1e4p_next_byte
	ret

copy_i1e4_partial endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_i1e8								     ;
;									     ;
;   copy internal 1 bit to external 8 bit packed pixel			     ;
;									     ;
;   The destination is filled with the source data, with each source	     ;
;   bit expanded to a byte in the destination.				     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings,	     ;
;   and to ensure that there are eight bytes of destination available	     ;
;   to hold each byte of source data.					     ;
;									     ;
;   This routine is used for:						     ;
;	internal mono to external eight-bit packed pixels		     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of bytes to xlate				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_i1e8	proc	near

ci1e8_next_byte:    			; convert a 0 bit to a byte full of
	lodsb				; 0's and a 1 bit to a byte full of 1's 
	xchg	ax,bx
	shl	bl,1
	sbb	al,al
	shl	bl,1
	sbb	ah,ah
	stosw
	shl	bl,1
	sbb	al,al
	shl	bl,1
	sbb	ah,ah
	stosw
	shl	bl,1
	sbb	al,al
	shl	bl,1
	sbb	ah,ah
	stosw
	shl	bl,1
	sbb	al,al
	shl	bl,1
	sbb	ah,ah
	stosw
	loop	ci1e8_next_byte
	ret

copy_i1e8	endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_i1e8_partial							     ;
;									     ;
;   copy internal 1 bit to external 8 bit packed pixel, partial byte	     ;
;									     ;
;   The destination is filled with the source data, with each source	     ;
;   bit expanded to a byte in the destination.	This routine handles	     ;
;   partial bytes where some portion of the first or last byte needs	     ;
;   to be saved.							     ;
;									     ;
;   This routine will handle the destination (user's buffer) crossing	     ;
;   a segment boundary during the translation of the source byte.	     ;
;   The source will never cross a segment boundary since it is an	     ;
;   internal bitmap and all scans are allocated to fit within a		     ;
;   segment.								     ;
;									     ;
;   This routine shouldn't be called very often, so a short loop will	     ;
;   be sufficient for our needs.					     ;
;									     ;
;   This routine is used for:						     ;
;	internal mono to external eight-bit packed pixels		     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CL     =  number of pixels to xlate				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
; Error Returns:							     ;
;	None								     ;
; Registers Preserved:							     ;
;	BP,ES,DS,BP							     ;
; Registers Destroyed:							     ;
;	AX,BX,DX,SI,FLAGS						     ;
; Calls:								     ;
;	None								     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_i1e8_partial proc near

	lodsb
	xchg	ax,bx
	xor	ch,ch

ci1e8p_next_byte:
	shl	bl,1
	sbb	al,al
	call	store_with_seg_check
	loop	ci1e8p_next_byte
	ret

copy_i1e8_partial endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_i1e24								     ;
;									     ;
;   copy internal 1 bit to external 24 bit RGB				     ;
;									     ;
;   The destination is filled with the source data, with each source	     ;
;   bit expanded to a full RGB triplet in the destination.  The colors	     ;
;   being translated are black and white, so no color lookup is required.    ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings,	     ;
;   and to ensure that there are 24 bytes of destination available to	     ;
;   hold each source byte of data.					     ;
;									     ;
;   This routine is used for:						     ;
;	internal mono to external RGB (24-bit packed pixels)		     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of bytes to xlate				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_i1e24	proc	near

ci1e24_next_byte:
	lodsb
	xchg	ax,bx
	shl	bl,1
	sbb	ax,ax
	stosw
	shl	bl,1
	sbb	ah,ah
	stosw
	mov	al,ah
	stosw
	shl	bl,1
	sbb	ax,ax
	stosw
	shl	bl,1
	sbb	ah,ah
	stosw
	mov	al,ah
	stosw
	shl	bl,1
	sbb	ax,ax
	stosw
	shl	bl,1
	sbb	ah,ah
	stosw
	mov	al,ah
	stosw
	shl	bl,1
	sbb	ax,ax
	stosw
	shl	bl,1
	sbb	ah,ah
	stosw
	mov	al,ah
	stosw
	loop	ci1e24_next_byte
	ret

copy_i1e24	endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_i1e24_partial							     ;
;									     ;
;   copy internal 1 bit to external 24 bit RGB, partial byte		     ;
;									     ;
;   The destination is filled with the source data, with each source	     ;
;   bit expanded to a full RGB triplet in the destination.  The colors	     ;
;   being translated are black and white, so no color lookup is required.    ;
;									     ;
;   This routine handles partial bytes where some portion of the first	     ;
;   or last byte needs to be saved.					     ;
;									     ;
;   This routine will handle the destination (user's buffer) crossing	     ;
;   a segment boundary during the translation of the source byte.	     ;
;   The source will never cross a segment boundary since it is an	     ;
;   internal bitmap and all scans are allocated to fit within a		     ;
;   segment.								     ;
;									     ;
;   This routine shouldn't be called very often, so a short loop will	     ;
;   be sufficient for our needs.					     ;
;									     ;
;   This routine is used for:						     ;
;	internal mono to external RGB (24-bit packed pixels)		     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CL     =  number of pixels to xlate				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes	ds,nothing
	assumes es,nothing

copy_i1e24_partial proc near

	lodsb
	xchg	ax,dx
	xor	ch,ch

ci1e24p_next_byte:
	shl	dl,1
	sbb	ax,ax
	cmp	di,0FFFDh		;If within 3 bytes, then will have
	jae	ci1e24p_next_sel	;  to update the selector
	stosw
	stosb

ci1e24p_loop_next:
	loop	ci1e24p_next_byte

ci1e24p_no_more_sels:
	ret

ci1e24p_next_sel:
	call	store_with_seg_check
	call	store_with_seg_check
	call	store_with_seg_check
	jmp	ci1e24p_loop_next

copy_i1e24_partial endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_i1e1_partial							     ;
;									     ;
;   A Byte is copied from the source to destination.			     ;
;									     ;
;   This routine will handle the destination (user's buffer) crossing	     ;
;   a segment boundary during the translation of the source byte.	     ;
;   The source will never cross a segment boundary since it is an	     ;
;   internal bitmap and all scans are allocated to fit within a		     ;
;   segment.								     ;
;									     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CL     =  number of pixels to translate (Will always be 8)	     ;
;	Direction flag cleared						     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;	Direction flag cleared						     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_i1e1_partial proc near

	lodsb
	errn$	store_with_seg_check

copy_i1e1_partial endp

page
;--------------------------Private-Routine-----------------------------------;
;   store_with_seg_check						     ;
;									     ;
;   The contents of AL is stored at ES:DI.  If DI becomes 0, then	     ;
;   the next destination selector (if it exists) will be loaded.	     ;
;									     ;
; Entry:								     ;
;	ES:DI --> destination						     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

store_with_seg_check proc near

	stosb				;Save the byte
	or	di,di			;If not in the next segment
	jnz	swsc_exit		;  then skip selector update
	cmp	NumSegments,0
	jz	short swsc_exit
	dec	NumSegments
	push	ax
	push	dx
	mov	dx,es
	add	dx,__NEXTSEG
	mov	es,dx
	assumes es,nothing
	pop	dx
	pop	ax
swsc_exit:
	ret

store_with_seg_check endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_ie1								     ;
;									     ;
;   copy internal 3/4 plane to external 1 bit				     ;
;									     ;
;   The destination is filled with the source data.  All four planes	     ;
;   of the source byte are combined to form the single destination byte.     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings,	     ;
;   and to ensure that there are enough bytes of destination available	     ;
;   to hold the data.							     ;
;									     ;
;   Since there will be information loss in going from four color to	     ;
;   one color, all colors which match the !!!??? background color	     ;
;   will be mapped to 1, and all other colors will be mapped to the	     ;
;   !!!??? forground color.  This is the same strategy used in color	     ;
;   to mono bitblts.							     ;
;									     ;
;   This routine is used for:						     ;
;	internal four-plane to external mono				     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of bytes to xlate				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

public	copy_ie1
copy_ie1	proc  near

;!!!	Until color tables are in and working, maybe I should just map
;!!!	anything which isn't white to black??

	mov	bx,width_scan
	push	bp

cie1_next_byte:
	add	si,bx

if NUMBER_PLANES eq 4
	shl	bx,1
	mov	dh,[si][bx]		;Get C3 color into DH
	shr	bx,1			;restore bx
else
	xor	dh,dh			;for table look up
endif

	mov	dl,[si][bx]		;Get C2 color into DL
	mov	ah,[si] 		;Get C1 color into AH
	sub	si,bx
	lodsb				;Get C0 -> AL, update source pointer
	push	cx
	mov	cx, 8
cie1_xlate_loop:
	sub	bp, bp
	shl	dh, 1
	rcl	bp, 1
	shl	dl, 1
	rcl	bp, 1
	shl	ah, 1
	rcl	bp, 1
	shl	al, 1
	rcl	bp, 1
	shl	ch, 1
	or	ch, cs:[bp].ColorToMono
	dec	cl
	jnz	cie1_xlate_loop
	mov	al, ch
	pop	cx
;       and	 ax,dx			 ;!!!For now, just favor black
;       and	 al,ah
	stosb
	loop	cie1_next_byte
	pop	bp
	ret

copy_ie1	endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_ie1_partial							     ;
;									     ;
;   copy internal 3/4 plane to external 1 bit, partial byte		     ;
;									     ;
;   The destination is filled with the source data.  All four planes	     ;
;   of the source byte are combined to form the single destination byte.     ;
;									     ;
;   This routine will handle the destination (user's buffer) crossing	     ;
;   a segment boundary during the translation of the source byte.	     ;
;   The source will never cross a segment boundary since it is an	     ;
;   internal bitmap and all scans are allocated to fit within a		     ;
;   segment.								     ;
;									     ;
;   Since there will be information loss in going from four color to	     ;
;   one color, all colors which match the !!!??? background color	     ;
;   will be mapped to 1, and all other colors will be mapped to the	     ;
;   !!!??? forground color.  This is the same strategy used in color	     ;
;   to mono bitblts.							     ;
;									     ;
;   This routine is used for:						     ;
;	internal four-plane to external mono				     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of bits to xlate (will always be 8)		     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_ie1_partial proc near
	mov	bx,width_scan
	add	si,bx

if NUMBER_PLANES eq 4
	shl	bx,1
	mov	dh,[si][bx]		;Get C3 color
	shr	bx,1
else
	xor	dh,dh			;for table look up
endif

	mov	dl,[si][bx]		;Get C2 color
	mov	ah,[si] 		;Get C1 color
	sub	si,bx
	lodsb				;Get C0 color, update source pointer
	push	cx
	mov	cx, 8
cie1_partial_xlate_loop:
	sub	bx, bx
	shl	dh, 1
	rcl	bl, 1
	shl	dl, 1
	rcl	bl, 1
	shl	ah, 1
	rcl	bl, 1
	shl	al, 1
	rcl	bl, 1
	shl	ch, 1
	or	ch, cs:[bx].ColorToMono
	dec	cl
	jnz	cie1_partial_xlate_loop
	mov	al, ch
	pop	cx
;       and	 ax,dx			 ;!!!For now, just favor black
;       and	 al,ah
	jmp	store_with_seg_check

copy_ie1_partial endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_ie4								     ;
;									     ;
;   copy internal 3/4 plane to external 4 bit packed pixel		     ;
;									     ;
;   The destination is filled with the source data.  All four planes	     ;
;   of the source byte are used to form the nibble for the destination	     ;
;   byte.								     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings,	     ;
;   and to ensure that there are enough bytes of destination available	     ;
;   to hold the data.							     ;
;									     ;
;   Since there will be information loss in going from four color to	     ;
;   one color, all colors which match the !!!??? background color	     ;
;   will be mapped to 1, and all other colors will be mapped to the	     ;
;   !!!??? forground color.  This is the same strategy used in color	     ;
;   to mono bitblts.							     ;
;									     ;
;   This routine is used for:						     ;
;	internal four-plane to external 4-bit packed pixels		     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of bytes to xlate				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_ie4	proc near

;!!!	This is very inefficient and will have to change
;!!!	i.e. table lookup

	mov	bx,width_scan
	push	bp
	mov	bp,cx			;Use BP for loop counter

cie4_next_byte:
	add	si,bx

if NUMBER_PLANES eq 4
	shl	bx,1
	mov	dh,[si][bx]		;Get C3 color
	shr	bx,1	
else
	xor	dh,dh			;MSB of each 4 bit pel will be 0
endif

	mov	dl,[si][bx]		;Get C2 color
	mov	ch,[si] 		;Get C1 color
	sub	si,bx
	mov	cl,[si] 		;Get C0 color, update source pointer
	inc	si

	mov	ah,4			;Set # bytes to write

cie4_next_dest_byte:
	shl	dh,1			;Collect 1st nibble
	rcl	al,1
	shl	dl,1
	rcl	al,1
	shl	ch,1
	rcl	al,1
	shl	cl,1
	rcl	al,1
	shl	dh,1			;Collect 2nd nibble
	rcl	al,1
	shl	dl,1
	rcl	al,1
	shl	ch,1
	rcl	al,1
	shl	cl,1
	rcl	al,1
	stosb
	dec	ah
	jnz	cie4_next_dest_byte
	dec	bp
	jnz	cie4_next_byte
	pop	bp
	ret

copy_ie4	endp

page
;--------------------------Private-Routine-----------------------------------;
;									     ;
; copy_ie4_partial							     ;
;									     ;
;   copy internal 3/4 plane to external 4 bit packed pixel, partial byte     ;
;									     ;
;   The destination is filled with the source data.  All four planes	     ;
;   of the source byte are used to form the nibble for the destination	     ;
;   byte.								     ;
;									     ;
;   This routine handles partial bytes where some portion of the first	     ;
;   or last byte needs to be saved.					     ;
;									     ;
;   This routine will handle the destination (user's buffer) crossing	     ;
;   a segment boundary during the translation of the source byte.	     ;
;   The source will never cross a segment boundary since it is an	     ;
;   internal bitmap and all scans are allocated to fit within a		     ;
;   segment.								     ;
;									     ;
;   Since this routine always writes nibbles, it is guaranteed that	     ;
;   the shift count passed in is even.					     ;
;									     ;
;   This routine is used for:						     ;
;	internal four-plane to external 4-bit packed pixels		     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	BX     =  index to next plane					     ;
;	CL     =  number of pixels to xlate				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_ie4_partial proc near
	mov	bx,width_scan
	add	si,bx

if NUMBER_PLANES eq 4
	shl	bx,1
	mov	dh,[si][bx]		;Get C3 color
	shr	bx,1	
else
	xor	dh,dh			;MSB of 4 bit nibble will be 0
endif

	mov	dl,[si][bx]		;Get C2 color
	mov	ah,[si] 		;Get C1 color
	sub	si,bx
	lodsb				;Get C0 color, update source pointer
	xchg	ax,cx
	mov	ah,al
	shr	ah,1			;/2 for correct number of iterations

cie4p_next_byte:
	shl	dh,1			;Collect 1st nibble
	rcl	al,1
	shl	dl,1
	rcl	al,1
	shl	ch,1
	rcl	al,1
	shl	cl,1
	rcl	al,1
	shl	dh,1			;Collect 2nd nibble
	rcl	al,1
	shl	dl,1
	rcl	al,1
	shl	ch,1
	rcl	al,1
	shl	cl,1
	rcl	al,1
	call	store_with_seg_check
	dec	ah
	jnz	cie4p_next_byte
	ret

copy_ie4_partial endp

page
;--------------------------Private-Routine-----------------------------------;
;									     ;
; copy_ie8								     ;
;									     ;
;   copy internal 3/4 plane to external 8 bit packed pixel		     ;
;									     ;
;   The destination is filled with the source data.  All four planes	     ;
;   of the source byte are used to form the byte for the destination.	     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings,	     ;
;   and to ensure that there are enough bytes of destination available	     ;
;   to hold the data.							     ;
;									     ;
;   Since there will be information loss in going from four color to	     ;
;   one color, all colors which match the !!!??? background color	     ;
;   will be mapped to 1, and all other colors will be mapped to the	     ;
;   !!!??? forground color.  This is the same strategy used in color	     ;
;   to mono bitblts.							     ;
;									     ;
;   This routine is used for:						     ;
;	internal four-plane to external 8-bit packed pixels		     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of bytes to xlate				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
; Error Returns:							     ;
;	None								     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_ie8	proc near

	mov	bx,width_scan
	push	bp
	mov	bp,cx			;Use BP for loop counter

cie8_next_byte:
	add	si,bx

if NUMBER_PLANES eq 4
	shl	bx,1
	mov	dh,[si][bx]		;Get C3 color
	shr	bx,1	
else
	xor	dh,dh			;MSB of 8 bpp wil be 0
endif

	mov	dl,[si][bx]		;Get C2 color
	mov	ch,[si] 		;Get C1 color
	sub	si,bx
	mov	cl,[si] 		;Get C0 color, update source pointer
	inc	si
	mov	ah,08h			;There will be 8 bytes of destination

cie8_next_dest_byte:
	xor	al,al			;Collect and store a nibble
	shl	dh,1
	rcl	al,1
	shl	dl,1
	rcl	al,1
	shl	ch,1
	rcl	al,1
	shl	cl,1
	rcl	al,1
	stosb
	dec	ah
	jnz	cie8_next_dest_byte
	dec	bp
	jnz	cie8_next_byte
	pop	bp
	ret

copy_ie8	endp

page
;--------------------------Private-Routine-----------------------------------;
;									     ;
; copy_ie8_partial							     ;
;									     ;
;   copy internal 3/4 plane to external 8 bit packed pixel, partial byte     ;
;									     ;
;   The destination is filled with the source data.  All four planes	     ;
;   of the source byte are used to form the byte for the destination.	     ;
;									     ;
;   This routine handles partial bytes where some portion of the first	     ;
;   or last byte needs to be saved.					     ;
;									     ;
;   This routine will handle the destination (user's buffer) crossing	     ;
;   a segment boundary during the translation of the source byte.	     ;
;   The source will never cross a segment boundary since it is an	     ;
;   internal bitmap and all scans are allocated to fit within a		     ;
;   segment.								     ;
;									     ;
;   This routine is used for:						     ;
;	internal four-plane to external 8-bit packed pixels		     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CL     =  number of pixels to xlate				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_ie8_partial proc near

	mov	bx,width_scan
	add	si,bx

if NUMBER_PLANES eq 4
	shl	bx,1
	mov	dh,[si][bx]		;Get C3 color
	shr	bx,1	
else
	xor	dh,dh			;MSB will be 0
endif

	mov	dl,[si][bx]		;Get C2 color
	mov	ah,[si] 		;Get C1 color
	sub	si,bx
	lodsb				;Get C0 color, update source pointer
	xchg	ax,cx
	mov	ah,al

cie8p_next_byte:
	xor	al,al			;Collect and store a nibble
	shl	dh,1
	rcl	al,1
	shl	dl,1
	rcl	al,1
	shl	ch,1
	rcl	al,1
	shl	cl,1
	rcl	al,1
	call	store_with_seg_check
	dec	ah
	jnz	cie8p_next_byte
	ret

copy_ie8_partial endp

page
;--------------------------Private-Routine-----------------------------------;
;									     ;
; copy_ie24								     ;
;									     ;
;   copy internal 3/4 plane to external 24 bit RGB			     ;
;									     ;
;   The destination is filled with the source data.  All four planes	     ;
;   of the source byte are used to form the RGB triplet for the		     ;
;   destination.							     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings,	     ;
;   and to ensure that there are enough bytes of destination available	     ;
;   to hold the data.							     ;
;									     ;
;   Since there will be information loss in going from four color to	     ;
;   one color, all colors which match the !!!??? background color	     ;
;   will be mapped to 1, and all other colors will be mapped to the	     ;
;   !!!??? forground color.  This is the same strategy used in color	     ;
;   to mono bitblts.							     ;
;									     ;
;   This routine is used for:						     ;
;	internal four-plane to external RGB (24-bit packed)		     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of bytes to xlate				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_ie24	proc near

	mov	temp_byte_count,cx	;Save # bytes to xlate

cie24_next_byte:
	mov	bx,width_scan
	add	si,bx

if NUMBER_PLANES eq 4
	shl	bx,1
	mov	dh,[si][bx]		;Get C3 color
	shr	bx,1	
else
	xor	dh,dh			;MSB will be 0
endif

	mov	dl,[si][bx]		;Get C2 color
	mov	ch,[si] 		;Get C1 color
	sub	si,bx
	mov	cl,[si] 		;Get C0 color, update source pointer
	inc	si
	xchg	si,bx
	mov	ah,08h			;There will be 8 triplets stored

cie24_next_dest_byte:
	xor	si,si			;Collect and store a triplet
	shl	dh,1
	rcl	si,1
	shl	dl,1
	rcl	si,1
	shl	ch,1
	rcl	si,1
	shl	cl,1
	rcl	si,1
	shiftl	si,2			;Indexing into dwords


	mov	al,DIMapSeg_color_table[si][2]
	mov	si,word ptr DIMapSeg_color_table[si][0]
	xchg	ax,si
	stosw
	xchg	ax,si
	stosb
	dec	ah
	jnz	cie24_next_dest_byte
	xchg	si,bx
	dec	temp_byte_count
	jnz	cie24_next_byte
	ret

copy_ie24	endp

page
;--------------------------Private-Routine-----------------------------------;
;									     ;
; copy_ie24_partial							     ;
;									     ;
;   copy internal 3/4 plane to external 24 bit RGB, partial byte             ;
;									     ;
;   The destination is filled with the source data.  All four planes	     ;
;   of the source byte are used to form the RGB triplet for the		     ;
;   destination.							     ;
;									     ;
;   This routine handles partial bytes where some portion of the first	     ;
;   or last byte needs to be saved.					     ;
;									     ;
;   This routine will handle the destination (user's buffer) crossing	     ;
;   a segment boundary during the translation of the source byte.	     ;
;   The source will never cross a segment boundary since it is an	     ;
;   internal bitmap and all scans are allocated to fit within a		     ;
;   segment.								     ;
;									     ;
;   This routine is used for:						     ;
;	internal four-plane to external RGB (24-bit packed)		     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CL     =  number of pixels to xlate				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_ie24_partial proc near

	mov	bx,width_scan
	add	si,bx

if NUMBER_PLANES eq 4
	shl	bx,1
	mov	dh,[si][bx]		;Get C3 color
	shr	bx,1	
else
	xor	dh,dh			;MSB of pels will be 0
endif

	mov	dl,[si][bx]		;Get C2 color
	mov	ah,[si] 		;Get C1 color
	sub	si,bx
	lodsb				;Get C0 color, update source pointer
	xchg	ax,cx
	mov	ah,al			;Set number of pixels to xlate
	xchg	si,bx

cie24p_next_byte:
	xor	si,si			;Collect and store a triplet
	shl	dh,1
	rcl	si,1
	shl	dl,1
	rcl	si,1
	shl	ch,1
	rcl	si,1
	shl	cl,1
	rcl	si,1
	shiftl	si,2			;Indexing into dwords

	mov	al,DIMapSeg_color_table[si][2]
	mov	si,word ptr DIMapSeg_color_table[si][0]

	cmp	di,0FFFDh
	jae	cie24p_next_sel
	xchg	ax,si
	stosw
	xchg	ax,si
	stosb

cie24p_loop_next:
	dec	ah
	jnz	cie24p_next_byte
	xchg	si,bx

cie24p_no_more_sels:
	ret

cie24p_next_sel:
	xchg	ax,si
	call	store_with_seg_check
	xchg	al,ah
	call	store_with_seg_check
	xchg	al,ah
	xchg	ax,si
	call	store_with_seg_check
	jmp	cie24p_loop_next

copy_ie24_partial endp

sEnd	code
end
