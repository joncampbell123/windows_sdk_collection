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
;		         SetDeviceBitmapBits				     ;
;			 -------------------				     ;
;   Converts device independent bitmap format into a bitmap format suitable  ;
;   for EGA adapters.							     ;
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
;  NOTE: BMC_ETI.ASM IN DISP20 & DISP30 TREES ARE IDENTICAL. AFTER CHANGING  ;
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
	include lines.inc
	include	gdidefs.inc
	include	macros.inc
	include	display.inc
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


	externFP	sum_RGB_alt_far		; in ..\ROBJECT.ASM


createSeg _DIMAPS,DIMapSeg,word,public,CODE
sBegin	DIMapSeg
  	assumes	cs,DIMapSeg

	externNP	copy_i1e1		; in .\BMC_ITE.ASM

;----------------------------------------------------------------------------;
; The following table contains the addresses of the initialization routines  ;
; for each of the 4 Device Independent Bitmap formats.			     ;
;----------------------------------------------------------------------------;

anpfnPreProc	label	word
	dw	eti_1			; 1 bit per pel format
	dw	eti_4			; 4 bits per pel format
	dw	eti_8			; 8 bits per pel format
	dw	eti_24			; 24 bits per pel format



	assumes ds,nothing
	assumes es,nothing

cProc	SetDeviceBitmapBits,<NEAR,PASCAL,PUBLIC>,<si,di,ds,es>

	parmD   lp_bits_own           ; -> pointer to own internal format
	parmD	cyScans			; no of scans to copy
	parmD	iStart			; start copy at this scan
	parmD   lp_bits_ind		; -> pointer to independent format
	parmD	lpbi			; -> Pointer to info block
	parmB	fbsd			; own format mono or color

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
	localW	convert_C0		; Mono to 4 plane conversion routine
	localW	convert_C1		; Mono to 4 plane conversion routine
	localW	convert_C2		; Mono to 4 plane conversion routine

if NUMBER_PLANES eq 4
	localW	convert_C3		; Mono to 4 plane conversiona routine
endif

	localW	alignment		; Shift to align partial bytes
	localD	scan_length		; length of one buffer scanline
	localW  NextSegOff		; offset to next huge map segment
	localW	filler_bytes		; filler bytes at segment end
	localW	prev_offset		; lst scan offset in prev segment
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

	xor	cx,cx			; will be the success/failure code 
	cmp	es:[bx].biPlanes,1 ; # of color planes must be 1
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
	jmp	eti_exit		; get out

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
	mov	scans_copied,di

; the above routine does the following:
;	.   copies parameters from the info block to local storage
;	.   deceided whether the internal map is huge or small
;       .   clears carry flag if no copy is to be done
;       .   sets tha carry flag if copy is to be done and in DX:AX
;           returns the start pointer in the internal map

	jnc	eti_copy_needed		; test for error
	mov	scans_copied,0
@@:	jmp	eti_xfered_all
eti_copy_needed:
	or	di,di
	jz	@b			; zero scans copied.
	mov	es,dx
	assumes es,nothing
	mov	di,ax			; set ES:DI point to internal map

; ES:DI now points to the starting position in target (device dependent) map

	lds	si,lp_bits_ind		; Source is user's buffer 
	assumes ds,nothing		

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
	sbb	dx,0			;Back off by 1.

; SI has the start offset to the user's buffer

	add	ax,si
	adc	dx,0			; DX:AX = last address
	jz	eti_in_segment		; Won't cross a segment boundary
	or	fbsd,CAN_CROSS_SEG	; Can cross a segment boundary
	mov	NumSegments,dx		; Set number of segments in huge bitmap.

eti_next_scan:
	test	fbsd,CAN_CROSS_SEG	; If buffer might cross a segment
	jnz	eti_might_cross_seg	; then go check it out

eti_in_segment:
	push	di			; Save bitmap scan start address
	mov	cx,scan_byte_count	; Set translation procedure loop count
	jcxz	eti_see_about_partial	; No full bytes!
	call	full_byte_proc		; Process all complete bytes

eti_see_about_partial:
	mov	cx,partial_byte_flags	; Process any partial byte
	jcxz	eti_no_partial_byte
	mov	bx,alignment		; alignment for the partial bytes
	call	partial_byte_proc

eti_no_partial_byte:
	pop	di
	dec	scanline_count
	jz	eti_xfered_all		; Transfered all scans

; remember we map the first scan line in the user buffer to the last scan line
; in our internal map, so the pointer in our own format starts at the bottom 
; and then goes up

	sub	di,next_scan		; --> previous bitmap scanline
	add	si,buffer_adjust	; Align to dword boundary
	jnc	eti_chk_segment_update	; don't update segment if no wrap
	mov	ax,ds			; otherwise, increment segment
	add	ax,__NEXTSEG
	mov	ds,ax
	assumes ds,nothing

eti_chk_segment_update:
	test	fbsd,SD_HUGE		; If not a huge bitmap,
	jz	eti_next_scan		; go do next scanline

	dec	huge_scans_left 	; Any more scans in this segment?
	jnz	eti_next_scan		; yes, don't update bitmap pointer
	mov	ax,es
	sub	ax,__NEXTSEG		; go to the previous segment
	mov	es,ax
	assumes es,nothing

	mov	di,prev_offset		; start from the bottom of the segment
	mov	ax,scans_per_seg
	mov	huge_scans_left,ax	; This many scans in the segment
	jmp	eti_next_scan


eti_might_cross_seg:

;  The buffer will cross a segment boundary.  If this scanline will
;  cross the segment, then we'll process eight bits of bitmap data
;  at a time.  !!! someday we may improve upon this, but we don't
;  expect this to be critical !!!

	mov	ax,si			; If the entire scan shows, then
	xor	dx,dx			; we can use the normal code
	add	ax,scan_length.lo
	adc	dx,scan_length.hi	; scan_length may be > 64K
	jz	eti_in_segment		; It won't cross a segment boundary

;  The scan will cross a segment boundary.   We'll take the easy
;  way out and do the entire scanline one bitmap byte at a time.

	push	di			; Save bitmap scan start address
	mov	cx,scan_byte_count	; If only a last byte exists,
	jcxz	eti_see_about_partial	; use the normal code
	mov	some_temp,cx		; Save full byte count

eti_looking_to_cross:
	mov	cx,8			; Accumulate 8 dest pixels
	xor	bx,bx			; No alignment required
	call	partial_byte_proc
	dec	some_temp
	jnz	eti_looking_to_cross
	jmp	eti_see_about_partial	; Finish with partial byte code

eti_xfered_all:
	mov	ax,scans_copied 	; Return number of scans copied
					   
eti_exit:
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
; eti_1								             ;
;									     ;
;   Bitmap conversion, external format to internal 1 bit		     ; 
;									     ;
;   This routine is responsible for all preprocessing for converting	     ;
;   from external 1 bit per pixel to internal formats			     ;	
;									     ;
; Entry:								     ;	
;	ES:DI --> where color table is					     ;
; Returns:								     ;
;	None								     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

eti_1	proc	near

	xor	ax,ax
	mov	partial_byte_flags,ax	; Will never have a partial byte
	mov	scan_length.hi,ax	; Max dest scan size is < 64K

	mov	ax,es:[di][0]		; Convert color for all 0 pixels
	mov	dl,es:[di][2]
	call	rgb_to_ipc		; gets the nearest color
	mov	cl,al
	mov	ax,es:[di][4]		; Convert color for all 1 pixels
	mov	dl,es:[di][6]
	call	rgb_to_ipc		; get the color

	xor	cl,al			; 0 where colors are the same
	test	fbsd,SD_COLOR		; Monochrome destination?
	jnz	eti_1_not_mono		; No, must be 4 plane format


;	Mono to mono.  Set up to copy as is, store all 0's, or invert,
;	depending on the two passed colors.

	shiftr	cl,4			; Align bits for eti_1_get_proc
	shiftr	al,4			; the mono bit is the 4th bit 
	errnz	MONO_BIT-00010000b
	call	eti_1_get_proc
	mov	bx,DIMapSegOFFSET copy_e1i1_partial
	jmp	short eti_1_have_proc


eti_1_not_mono:
	call	eti_1_get_proc	      	
	mov	convert_C0,dx		; routine for the C0 plane
	call	eti_1_get_proc
	mov	convert_C1,dx		; routine for the C1 plane
	call	eti_1_get_proc
	mov	convert_C2,dx		; routine for the C2 plane
	call	eti_1_get_proc

if NUMBER_PLANES eq 4
	mov	convert_C3,dx		; routine for the C3 plane
endif

	mov	dx,DIMapSegOFFSET copy_e1i
	mov	bx,DIMapSegOFFSET copy_e1i_partial

eti_1_have_proc:
	mov	full_byte_proc,dx
	mov	partial_byte_proc,bx	; Might need this for seg boundaries

; Whenever we allocated a scanline (or each scan of a color
; bitmap), we allocated to a dword boundary.  If we just
; translate for the width of the scan in bytes, the required
; dword alignment will be maintained.

	mov	ax,width_scan		; Moving all bytes of the scan
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
	ret


mono_convert	label	word
	dw	copy_0s 		; Same color, color is 0
	dw	copy_1s 		; Same color, color is 1
	dw	copy_inverted		; Different colors, inverted
	dw	copy_i1e1               ; defined in ./bmc_ite.asm

eti_1_get_proc:
	shiftr	cl,1			; Short little function to return
	rcl	bx,1			; the function address of the
	shiftr	al,1			; routine to copy the mono
	rcl	bx,1
	rcl	bx,1			; source bits
	and	bx,00000110b
	mov	dx,mono_convert[bx]
	ret

eti_1	endp

	page
;--------------------------Private-Routine----------------------------------;
; eti_4									    ;
;									    ;
;   Bitmap conversion, external 4 bit to internal format		    ;
;									    ;
;   This routine is responsible for all preprocessing for converting	    ;
;   from external 4 bit format to our internal formats.			    ; 
;									    ;
; Entry:								    ;
;	ES:DI --> where color table is					    ;
; Returns:								    ;
;	None								    ;
;---------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

eti_4	proc	near

	xor	ax,ax
	mov	buffer_adjust,ax	; Everything will be dword aligned
	mov	partial_byte_flags,ax	; Will never have a partial byte
	mov	scan_length.hi,ax	; Max dest scan size is < 64K
	mov	ax,ColorsUsed		; no of colors used, 7fffh => all
	mov	cx,16			; can have max of 16 colors
	min_ax	cx			; get the minimum of the 2 values
	mov	cx,ax			; no of colors to translate
	call	create_color_table	; Create the color table to use
	mov	ax,DIMapSegOFFSET copy_e4i1
	mov	bx,DIMapSegOFFSET copy_e4i1_partial
	test	fbsd,SD_COLOR		; Monochrome destination?
	jz	eti_4_have_proc 	; It is mono
	mov	ax,DIMapSegOFFSET copy_e4i
	mov	bx,DIMapSegOFFSET copy_e4i_partial

eti_4_have_proc:
	mov	full_byte_proc,ax
	mov	partial_byte_proc,bx	; Might need this for seg boundaries
	mov	ax,bitmap_width 	; Compute the number of source bytes
	add	ax,7			; We can force dword alignment by
	shiftr	ax,3			; always xlating a multiple of 8 bits
	mov	scan_byte_count,ax
	shiftl	ax,2			; 1 source byte = 4 dest bytes
	mov	scan_length.lo,ax	; Set # bytes of user buffer required
	ret				; for one bitmap scan

eti_4	endp

	page
;--------------------------Private-Routine----------------------------------;
; eti_8									    ;
;									    ;
;   Bitmap conversion, external 8 bit to internal format		    ;
;									    ;
;   This routine is responsible for all preprocessing for converting	    ;
;   from external 8 bit format to our internal formats			    ;
;									    ;
; Entry:								    ;
;	ES:DI --> where color table is					    ;
; Returns:								    ;
;	None								    ;
;---------------------------------------------------------------------------;      

	assumes ds,nothing
	assumes es,nothing

eti_8	proc	near

	xor	ax,ax			; Will not require any buffer adjustment
	mov	buffer_adjust,ax
	mov	scan_length.hi,ax	; Max dest scan size is < 64K
	mov	ax,ColorsUsed		; no of colors used, 7fffh => all
	mov	cx,256			; can have max of 256 colors
	min_ax	cx			; get the minimum of the 2 values
	mov	cx,ax			; no of colors to translate
	call	create_color_table	; Create the color table to use
	mov	ax,DIMapSegOFFSET copy_e8i1
	mov	bx,DIMapSegOFFSET copy_e8i1_partial
	test	fbsd,SD_COLOR		; Monochrome destination?
	jz	eti_8_have_proc 	; It is mono
	mov	ax,DIMapSegOFFSET copy_e8i
	mov	bx,DIMapSegOFFSET copy_e8i_partial

eti_8_have_proc:
	mov	full_byte_proc,ax
	mov	partial_byte_proc,bx
	mov	ax,bitmap_width 	; Compute the number of source bytes
	add	ax,3			; to convert.	Each dword becomes
	mov	bx,ax			; a nibble in our bitmap
	shiftr	ax,3
	mov	scan_byte_count,ax
	and	bl,11111100b		; 4 source bytes = 1 dest nibble
	mov	scan_length.lo,bx	; Set # bytes of user buffer required
	and	bx,4			; Will either have a partial nibble or
	mov	partial_byte_flags,bx	; no extra nibble to xlate
	mov	alignment,bx		; Save alignment of last byte
	ret

eti_8	endp

	page
;--------------------------Private-Routine-----------------------------------;
; eti_24								     ;
;									     ;
;   Bitmap conversion, external 24 bit RGB to internal format		     ;
;									     ;
;   This routine is responsible for all preprocessing for converting	     ;
;   from external 24 bit RGB format to our internal formats		     ;
;									     ;
; Entry:								     ;
;	None								     ;
; Returns:								     ;
;	None								     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

eti_24	proc	near

	mov	ax,DIMapSegOFFSET copy_e24i1
	mov	bx,DIMapSegOFFSET copy_e24i1_partial
	test	fbsd,SD_COLOR		; Monochrome destination?
	jz	eti_24_have_proc	; Yes

eti_24_not_mono:
	mov	ax,DIMapSegOFFSET copy_e24i
	mov	bx,DIMapSegOFFSET copy_e24i_partial

eti_24_have_proc:
	mov	full_byte_proc,ax
	mov	partial_byte_proc,bx
	mov	cx,3
	mov	ax,bitmap_width 	; Compute the number of source bytes
	mov	bx,ax			; to convert.	Each bit becomes
	shr	ax,cl			; three bytes in the destination
	mov	scan_byte_count,ax
	mov	ax,bx			; 1 source bit = 3 dest bytes
	mul	cx
	add	ax,cx			; Need to round up to a double word
	adc	dx,0
	and	al,11111100b
	mov	scan_length.lo,ax	; Set # bytes of user buffer required
	mov	scan_length.hi,dx
	and	bx,00000111b		; Can have up to 7 bits in the last
	mov	partial_byte_flags,bx	; byte to convert
	push	bx
	neg	bx			; shift the last byte
	add	bx,8
	and	bx,00000111b		; Max is 7 bits of alignment
	mov	alignment,bx		; Save alignment of last byte
	pop	bx
	and	bl,00000011b		; Compute number of bytes to pad with
	mov	buffer_adjust,bx
	ret

eti_24	endp

	page
;--------------------------Private-Routine-----------------------------------;
; create_color_table							     ;
;									     ;
;   The color table for translating from user colors to our colors	     ;
;   is generated							     ;
;									     ;
; Entry:								     ;
;	ES:DI --> where color table is					     ;
;	CX    =   total number of color table entries			     ;
; Returns:								     ;
;	None								     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing


create_color_table proc near

	xor	si,si			; Index into saved colors

ctt_next_color:
	mov	ax,es:[di][0]
	mov	dl,es:[di][2]
	add	di, size RGBQuad
	call	rgb_to_ipc
	mov	color_xlate[si],al
	inc	si
	loop	ctt_next_color
	ret

create_color_table endp

	page
;--------------------------Private-Routine-----------------------------------;
; copy_0s								     ;
;									     ;
;   The destination is filled with 0's					     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings.	     ;
;									     ;
;   This routine is used for:						     ;
;	external mono to internal mono					     ;
;	external mono to internal four plane				     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of bytes to copy				     ;
;	Direction flag cleared						     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;	ES:DI --> next destination byte					     ;
;	Direction flag cleared						     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_0s proc	near

	xor	ax,ax
	jmp	short copy_0_or_1

copy_0s endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_1s								     ;
;									     ;
;   The destination is filled with 1's					     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings.	     ;
;									     ;
;   This routine is used for:						     ;
;	external mono to internal mono					     ;
;	external mono to internal four plane				     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of bytes to copy				     ;
;	Direction flag cleared						     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;	Direction flag cleared						     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_1s proc	near

	mov	ax,0FFFFh
	errn$	copy_0_or_1

copy_1s endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_0_or_1								     ;
;									     ;
;   The destination is filled with either 0's or 1's as indicated	     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings.	     ;
;									     ;
;   This routine is used for:						     ;
;	external mono to internal mono					     ;
;	external mono to internal four plane				     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of bytes to copy				     ;
;	AX     =  fill value (00h,FFh)					     ;
;	Direction flag cleared						     ;
; Returns:								     ;
;	DS:SI --> next destination byte					     ;
;	Direction flag cleared						     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_0_or_1	proc	near

; we shall not be accessing the source for the bytes, so update the source
; pointer. In this case scan_length = scan_byte_count

	add	si,cx			; Update source pointer
	mov	dx,cx
	xor	cx,cx
	ror	di,1			; Set 'C' if dest on odd boundary
	rcl	cx,1			; CX = 1 if on odd boundary
	sub	dx,cx			; Update count for odd byte
	rol	di,1			; Restore initial dest address
	rep	stosb			; Move possible odd aligned byte
	mov	cx,dx			; Move complete words
	shiftr	cx,1
	rep	stosw
	rcl	cx,1			; Process trailing odd byte
	rep	stosb
	ret

copy_0_or_1	endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_inverted								     ;
;									     ;
;   The destination is filled with the inverse of the source.		     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings.	     ;
;									     ;
;   This routine is used for:						     ;
;	external mono to internal mono					     ;
;	external mono to internal four plane				     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of bytes to copy				     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_inverted	proc	near

	test	si,1
	jz	ci_first_not_odd
	lodsb
	not	al
	stosb
	dec	cx

ci_first_not_odd:
	shiftr	cx,1
	jcxz	ci_last_byte

ci_words:
	lodsw
	not	ax			; NOT doesn't alter any flags
	stosw
	loop	ci_words

ci_last_byte:
	rcl	cx,1
	jcxz	ci_all_done
	lodsb
	not	al
	stosb

ci_all_done:
	ret

copy_inverted	endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_e1i1_partial							     ;
;									     ;
;   copy external 1 to internal 1					     ;
;									     ;
;   A source is copied into an internal mono bitmap.			     ;
;									     ;
;   This code handles crossing the segment boundary			     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	BX     =  mask and alignment control				     ;
;	CX     =  number of destination pixels to accumulate		     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e1i1_partial proc near

	mov	cx,1			; Load one complete source byte
	call	full_byte_proc
	dec	si			; Let load_with_seg_check deal with
	jmp	load_with_seg_check	; updating the selector if needed

copy_e1i1_partial endp
page

;--------------------------Private-Routine-----------------------------------;
; copy_e1i								     ;
;									     ;
;   copy external 1 to internal 4/3 planes				     ;
;									     ;
;   The source is copied to all four planes of the destination, with	     ;
;   color conversion via the passed color table taking place.		     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings.	     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of destination bytes to xlate			     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e1i	proc near

	push	di

	add	di,width_scan		; Start with second plane.
	push	si
	push	di
	push	cx
	call	convert_C1
	pop	cx
	pop	di
	pop	si

	add	di,width_scan	     ; third plane
	push	si
	push	di
	push	cx
	call	convert_C2
	pop	cx
	pop	di
	pop	si

if NUMBER_PLANES eq 4

	add	di,width_scan	     ; fourth plane
	push	si
	push	cx
	call	convert_C3
	pop	cx
	pop	si

endif

	pop	di			; Finish with first plane.
	call	convert_C0		; Leave SI,DI as returned.
	ret

copy_e1i	endp
page

;--------------------------Private-Routine-----------------------------------;
; copy_e1i_partial							     ;
;									     ;
;   copy external 1 to internal 4					     ;
;									     ;
;   The source is copied to all four planes of the destination, with	     ;
;   color conversion via the passed color table taking place.		     ;
;									     ;
;   This routine will handle crossing a segment boundry			     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of destination pixels to xlate			     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e1i_partial proc near

	push	di

	add	di,width_scan		; start with the second plane
	mov	cx,1			; Only want to grab a single byte
	call	convert_C1
	dec	si
	dec	di

	add	di,width_scan		; process the second plane
	mov	cx,1
	call	convert_C2
	dec	si
	dec	di

if NUMBER_PLANES eq 4

	add	di,width_scan		; process the third plane
	mov	cx,1
	call	convert_C3
	dec	si

endif

	pop	di                      ; finally the first plane
	mov	cx,1
	call	convert_C0
	dec	si			; Let load_with_seg_check deal with
	jmp	load_with_seg_check	; updating the selecotr if needed

copy_e1i_partial endp
page

;--------------------------Private-Routine-----------------------------------;
; copy_e4i								     ;
;									     ;
;   copy external 4 bit packed pixel to internal 3/4 plane		     ;
;									     ;
;   The source is copied to all four planes of the destination, with	     ;
;   color conversion via the passed color table taking place		     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings.	     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of destination bytes to xlate			     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e4i	proc near

	mov	temp_byte_count,cx

ce4i_next_dest_byte:
	mov	temp_bit_count,4	; Four source bytes per dest byte
	lea	bx,color_xlate

ce4i_next_byte:
	lodsb				; Get 2 bits worth of source data
	mov	ah,al
	shiftr	al,4			; The left nibble is the first pel
	xlat	ss:[bx] 		; Get the actual colors to use
	shiftr	al,1			; the LSB is for plane 0
	rcl	cl,1
	shiftr	al,1			; next bit for plane 1
	rcl	ch,1
	shiftr	al,1
	rcl	dl,1			; third bit for plane 2

if NUMBER_PLANES eq 4
	shiftr	al,1
	rcl	dh,1			; fourth bit for plane 3
endif

	mov	al,ah
	and	al,00001111b		; right most nibble is the next pel
	xlat	ss:[bx] 		; Get the actual colors to use
	shiftr	al,1
	rcl	cl,1			; Save C0 bit
	shiftr	al,1
	rcl	ch,1			; Save C1 bit
	shiftr	al,1
	rcl	dl,1			; Save C2 bit

if NUMBER_PLANES eq 4
	shiftr	al,1
	rcl	dh,1			; Save C3 bit
endif

	dec	temp_bit_count
	jnz	ce4i_next_byte
	mov	bx,width_scan
	mov	es:[di][bx],ch		; Save C1
	add	di,bx
	mov	es:[di][bx],dl		; Save C2

if NUMBER_PLANES eq 4
	shl	bx,1
	mov	es:[di][bx],dh		; Save C3
	shr	bx,1			; restore bx
endif

	sub	di,bx			; point back to scan line in C0 plane
	mov	al,cl
	stosb				; Save C0
	dec	temp_byte_count
	jnz	ce4i_next_dest_byte
	ret

copy_e4i	endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_e8i								     ;
;									     ;
;   copy external 8 bit packed pixel to internal 4/3 plane		     ;
;									     ;
;   The source is copied to all four planes of the destination, with	     ;
;   color conversion via the passed color table taking place		     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings.	     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of destination bytes to xlate			     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e8i	proc near

	mov	temp_byte_count,cx

ce8i_next_dest_byte:
	mov	ah,8			; Eight source bytes per dest byte
	lea	bx,color_xlate

ce8i_next_byte:
	lodsb				; Get 2 bits of source data
	xlat	ss:[bx] 		; Get the actual colors to use
	shiftr	al,1			; last bit is for C0 plane
	rcl	cl,1
	shiftr	al,1			; next bit for C1 plane
	rcl	ch,1
	shiftr	al,1			; third bit for C2 plane
	rcl	dl,1

if NUMBER_PLANES eq 4
	shiftr	al,1			; fourth bit for C3 plane
	rcl	dh,1
endif

	dec	ah
	jnz	ce8i_next_byte		; make a complete byte for the target
	mov	bx,width_scan
	mov	es:[di][bx],ch		; Save C1
	add	di,bx
	mov	es:[di][bx],dl		; Save C2

if NUMBER_PLANES eq 4
	shl	bx,1
	mov	es:[di][bx],dh		; Save C3
	shr	bx,1			; restore bx
endif

	sub	di,bx			; point back to scan line in plane 0
	mov	al,cl
	stosb				; Save C0
	dec	temp_byte_count		; complete all the target bytes
	jnz	ce8i_next_dest_byte
	ret

copy_e8i	endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_e24i								     ;
;									     ;
;   copy external 24 bit RGB to internal 4/3 plane			     ;
;									     ;
;   The source is copied to all four planes of the destination, with	     ;
;   color conversion via the actual RGB triplet.			     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings.	     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of destination bytes to xlate			     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e24i	proc near

	mov	temp_byte_count,cx

ce24i_next_dest_byte:
	mov	temp_bit_count,8	; Conversions per destination byte

ce24i_next_byte:
	lodsw
	xchg	ax,dx
	lodsb
	xchg	ax,dx
	call	rgb_to_ipc	      ; get the color for these 3 bytes
	shiftr	al,1		      ; color for C0 plane
	rcl	bl,1
	shiftr	al,1		      ; color for C1 plane
	rcl	bh,1
	shiftr	al,1		      ; color for C2 plane
	rcl	cl,1

if NUMBER_PLANES eq 4
	shiftr	al,1		      ; color for C3 plane
	rcl	ch,1
endif

	dec	temp_bit_count	      ; complete one target byte
	jnz	ce24i_next_byte
	xchg	ax,bx
	mov	bx,width_scan
	mov	es:[di][bx],ah	      ; Save C1
	add	di,bx
	mov	es:[di][bx],cl	      ; Save C2

if NUMBER_PLANES eq 4
	shl	bx,1
	mov	es:[di][bx],ch	      ; Save C3
	shr	bx,1		      ; restore
endif

	sub	di,bx		      ; get back to scan in plane 0
	stosb				; Save C0
	dec	temp_byte_count
	jnz	ce24i_next_dest_byte	; complete all the target bytes
	ret

copy_e24i	endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_e4i1								     ;
;									     ;
;   copy external 4 bit packed pixel to internal 1 plane		     ;
;									     ;
;   The source is copied to the destination, with color conversion	     ;
;   via the passed color table taking place				     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings.	     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of destination bytes to xlate			     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e4i1	proc near

	lea	bx,color_xlate

ce4i1_next_dest_byte:
	mov	dl,4			; Four source bytes per dest byte

ce4i1_next_byte:
	lodsb				; Get 2 pels of source data
	mov	ah,al
	shiftr	al,4			; right nibble is the first pel
	xlat	ss:[bx] 		; Get the actual colors to use
	shiftl	al,4			; get the mono bit
	errnz	MONO_BIT-00010000b
	rcl	dh,1			; start accumulating for a byte
	mov	al,ah
	and	al,00001111b		; left nibble is the 2nd pel
	xlat	ss:[bx]
	shiftl	al,4			; get mono bit
	errnz	MONO_BIT-00010000b
	rcl	dh,1			; Set C0 bit
	dec	dl
	jnz	ce4i1_next_byte		; complete a abyte for the target
	mov	al,dh
	stosb				; save abyte for the target
	loop	ce4i1_next_dest_byte
	ret

copy_e4i1	endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_e8i1								     ;
;									     ;
;   copy external 8 bit packed pixel to internal 1 plane		     ;
;									     ;
;   The source is copied to the destination, with color conversion	     ;
;   onversion via the passed color table taking place			     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings.	     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of destination bytes to xlate			     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e8i1	proc near

ce8i1_next_dest_byte:
	mov	ah,8			; Eight source bytes per dest byte
	lea	bx,color_xlate

ce8i1_next_byte:
	lodsb				; Get 1 bit of source data
	xlat	ss:[bx] 		; Get the actual colors to use
	shiftl	al,4			; get the mono bit
	errnz	MONO_BIT-00010000b
	rcl	dl,1			; start accumulating for a byte
	dec	ah
	jnz	ce8i1_next_byte		; complete a byte
	mov	al,dl
	stosb				; save a byte for the target
	loop	ce8i1_next_dest_byte
	ret

copy_e8i1	endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_e24i1								     ;
;									     ;
;   copy external 24 bit RGB to internal 1 plane			     ;
;									     ;
;   The source is copied to the destination, with the RGB triplet	     ;
;   being converted into black/white.					     ;
;									     ;
;   It is the caller's responsibility to handle all segment crossings.	     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CX     =  number of destination bytes to xlate			     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e24i1	proc near

ce24i1_next_dest_byte:
	mov	bh,8			; 8 conversion per destination byte

ce24i1_next_byte:
	lodsw
	xchg	ax,dx
	lodsb
	xchg	ax,dx
	call	rgb_to_ipc		; get the color for these 3 bytes
	shiftl	al,4			; Get the mono bit
	errnz	MONO_BIT-00010000b	; Mono bit must be this bit
	rcl	bl,1			; Accumulate mono bits here
	dec	bh
	jnz	ce24i1_next_byte	; complete a complete byte 
	mov	al,bl
	stosb				; store abyte for the target
	loop	ce24i1_next_dest_byte
	ret

copy_e24i1	endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_e4i_partial							     ;
;									     ;
;   copy external 4 bit packed pixel to internal 4/3 plane, partial byte     ;
;									     ;
;   The source is copied to all four planes of the destination, with	     ;
;   color conversion via the passed color table taking place		     ;
;									     ;
;   This code handles crossing the segment boundary			     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CL     =  number of destination bits to accumulate		     ;
;	BX     =  mask and alignment control				     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e4i_partial proc near

	push	bx			; Save masking and alignment control
	shiftr	cl,1			; Source is pels (nibbles)
	mov	temp_bit_count,cl	; Save number of dest bits to collect
	lea	bx,color_xlate

ce4ip_next_byte:
	call	load_with_seg_check	; Get 2 bits of source data
	mov	ah,al			; save it
	shiftr	al,4			; high nibble is the first pel
	xlat	ss:[bx] 		; Get the actual colors to use
	shiftr	al,1
	rcl	cl,1			; Set C0 bit
	shiftr	al,1
	rcl	ch,1			; Set C1 bit
	shiftr	al,1
	rcl	dl,1			; Set C2 bit

if NUMBER_PLANES eq 4
	shiftr	al,1
	rcl	dh,1			; Set C3 bit
endif

	mov	al,ah
	and	al,00001111b		; right nibble is the next pel
	xlat	ss:[bx]
	shiftr	al,1
	rcl	cl,1			; C0 next bit
	shiftr	al,1
	rcl	ch,1			; C1 next bit
	shiftr	al,1
	rcl	dl,1			; C2 next bit

if NUMBER_PLANES eq 4
	shiftr	al,1
	rcl	dh,1			; C3 next bit
endif

	dec	temp_bit_count		
	jnz	ce4ip_next_byte	; process all left over pels

ce4ip_save_byte:
	pop	bx			; Restore masking and alignment
	call	align_and_store_all
	ret

copy_e4i_partial endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_e8i_partial							     ;
;									     ;
;   copy external 8 bit packed pixel to internal 4/3 plane, partial byte     ;
;									     ;
;   The source is copied to all four planes of the destination, with	     ;
;   color conversion via the passed color table taking place		     ;
;									     ;
;   This code handles crossing the segment boundary			     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CL     =  number of destination bits to accumulate		     ;
;	BX     =  mask and alignment control				     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e8i_partial proc near

	push	bx			; Save masking and alignment control
	mov	ah,cl			; Set AH = number of dest bits needed
	lea	bx,color_xlate

ce8ip_next_byte:
	call	load_with_seg_check	; Get 1 pel of source data
	xlat	ss:[bx] 		; Get the actual colors to use
	shiftr	al,1
	rcl	cl,1			; Set C0 bit
	shiftr	al,1
	rcl	ch,1			; Set C1 bit
	shiftr	al,1
	rcl	dl,1			; Set C2 bit

if NUMBER_PLANES eq 4
	shiftr	al,1
	rcl	dh,1			; Set C3 bit
endif

	dec	ah
	jnz	ce8ip_next_byte	; process all left over bytes

ce8ip_save_byte:
	pop	bx			; restore masking and alignment
	call	align_and_store_all
	ret

copy_e8i_partial endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_e24i_partial							     ;
;									     ;
;   copy external 24 bit RGB to internal 4/3 plane, partial byte 	     ;
;									     ;
;   The source is copied to all four planes of the destination, with	     ;
;   color conversion via the actual RGB triplet.			     ;
;									     ;
;   This code handles crossing the segment boundary			     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CL     =  number of destination bits to accumulate		     ;
;	BX     =  mask and alignment control				     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e24i_partial proc near

	push	bx			; Save masking and alignment control
	xor	ch,ch
	mov	temp_byte_count,cx

ce24ip_next_byte:
	cmp	si,0FFFDh		; test for seg end (need 3 bytes)
	jae	ce24ip_next_seg
	lodsw
	xchg	ax,dx
	lodsb
	xchg	ax,dx

ce24ip_have_source:
	call	rgb_to_ipc	      ; convert the color
	shiftr	al,1
	rcl	cl,1			; Set C0 bit
	shiftr	al,1
	rcl	ch,1			; Set C1 bit
	shiftr	al,1
	rcl	bl,1			; Set C2 bit

if NUMBER_PLANES eq 4
	shiftr	al,1
	rcl	bh,1			; Set C3 bit
endif

	dec	temp_byte_count
	jnz	ce24ip_next_byte	; process all left over pels

ce24ip_save_byte:
	xchg	bx,dx
	pop	bx			; restore masking and alignment
	call	align_and_store_all
	ret

ce24ip_next_seg:
	call	load_with_seg_check	; load one byte with segment check
	xchg	al,ah
	call	load_with_seg_check	; next color byte
	xchg	al,ah
	xchg	dx,ax
	call	load_with_seg_check	; third set of 8 bits
	xchg	dx,ax
	jmp	ce24ip_have_source

copy_e24i_partial endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_e4i1_partial							     ;
;									     ;
;   copy external 4 bit packed pixel to internal 1 plane, partial byte	     ;
;									     ;
;   The source is copied to the destination, with color conversion	     ;
;   via the passed color table taking place				     ;
;									     ;
;   This code handles crossing the segment boundary			     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CL     =  number of destination bits to accumulate		     ;
;	BX     =  mask and alignment control				     ;
; Returns:								     ;
;	DS:SI --> next source byte				     	     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e4i1_partial proc near

	mov	dx,bx			; Save masking and alignment control
	lea	bx,color_xlate
	shiftr	cl,1			; Source is pels (nibbles)

ce4i1p_next_byte:
	call	load_with_seg_check	; Get 2 pels of source data
	mov	ah,al			; save
	shiftr	al,4			; right nibble is first pel
	xlat	ss:[bx] 		; Get the actual colors to use
	shiftl	al,4			; get mono bit
	errnz	MONO_BIT-00010000b
	rcl	ch,1			; Set C0 bit
	mov	al,ah
	and	al,00001111b		; left nibble is second pel
	xlat	ss:[bx]
	shiftl	al,4			; get mono bit
	rcl	ch,1			; accumulate it
	errnz	MONO_BIT-00010000b
	dec	cl
	jnz	ce4i1p_next_byte	; process all left over pels

ce4i1p_save_byte:
	mov	al,ch
	mov	bx,dx			; Restore masking and alignment control
	call	align_and_store
	ret

copy_e4i1_partial endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_e8i1_partial							     ;
;									     ;
;   copy external 8 bit packed pixel to internal 1 plane, partial byte	     ;
;									     ;
;   The source is copied to destination, with color conversion		     ;
;   via the passed color table taking place				     ;
;									     ;
;   This code handles crossing the segment boundary			     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CL     =  number of destination bits to accumulate		     ;
;	BX     =  mask and alignment control				     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e8i1_partial proc near

	mov	dx,bx			; Save masking and alignment control
	lea	bx,color_xlate

ce8i1p_next_byte:
	call	load_with_seg_check
	xlat	ss:[bx]
	shiftl	al,4			; get the mono bit
	rcl	ch,1
	errnz	MONO_BIT-00010000b
	dec	cl
	jnz	ce8i1p_next_byte	; process all left over pels

ce8i1p_save_byte:
	mov	al,ch
	mov	bx,dx			; Restore masking and alignment control
	call	align_and_store
	ret

copy_e8i1_partial endp

page
;--------------------------Private-Routine-----------------------------------;
; copy_e24i1_partial							     ;
;									     ;
;   copy external 24 bit RGB to internal 1 plane, partial byte		     ;
;									     ;
;   The source is copied to the destination, with color conversion	     ;
;   via the actual RGB triplet.						     ;
;									     ;
;   This code handles crossing the segment boundary			     ;
;									     ;
; Entry:								     ;
;	DS:SI --> source						     ;
;	ES:DI --> destination						     ;
;	CL     =  number of destination bits to accumulate		     ;
;	BX     =  mask and alignment control				     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

copy_e24i1_partial proc near

ce24i1p_next_byte:
	cmp	si,0FFFDh		; test for segment end (need 3 bytes)
	jae	ce24i1p_next_seg
	lodsw
	xchg	ax,dx
	lodsb
	xchg	ax,dx

ce24i1p_have_source:
	call	rgb_to_ipc		; get the color for this pel
	shiftl	al,4			; get the mono bit
	rcl	ch,1
	dec	cl
	jnz	ce24i1p_next_byte	; process all the left over pels

ce24i1p_save_byte:
	mov	al,ch			
	call	align_and_store
	ret

ce24i1p_next_seg:
	call	load_with_seg_check	; load a byte with segment test
	xchg	al,ah
	call	load_with_seg_check	; load the next byte
	xchg	al,ah
	xchg	dx,ax
	call	load_with_seg_check	; the last 8 of 24 bits
	xchg	dx,ax
	jmp	ce24i1p_have_source

copy_e24i1_partial endp

page
;--------------------------Private-Routine-----------------------------------;
;  load_with_seg_check							     ;
;									     ;
;   The contents of *DS:SI is loaded into AL.  If SI becomes 0, then	     ;
;   the next destination selector (if it exists) will be loaded.	     ;
;									     ;
; Entry:								     ;
;	DS:SI --> destination						     ;
; Returns:								     ;
;	DS:SI --> next source byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

load_with_seg_check proc near
	lodsb				; Get the byte
	or	si,si			; If in the next segment
	jnz	lwsc_exit		; then update to the next selector
	push	dx
	mov	dx,NumSegments
	or	dx,dx
	jz	in_last_segment
	dec	NumSegments
	mov	dx,ds			; get current segment
	add	dx,__NEXTSEG		; move on to next segment
	mov	ds,dx
	assumes ds,nothing
in_last_segment:
	pop	dx
lwsc_exit:
	ret

load_with_seg_check endp
page

;--------------------------Private-Routine-----------------------------------;
;  align_and_store_all							     ;
;									     ;
;   All four planes of the bitmap, at the current location, are set	     ;
;   to the given values							     ;
;									     ;
; Entry:								     ;
;	ES:DI --> destination						     ;
;	CL     =  C0 byte						     ;
;	CH     =  C1 byte						     ;
;	DL     =  C2 byte						     ;
;	BL     =  shift count for alignment				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

align_and_store_all proc near

	push	di
	mov	al,cl			; Save C0 bit
	call	align_and_store
	dec	di
	add	di,width_scan		; --> next plane's bytes
	mov	al,ch			; Save C1 bit
	call	align_and_store
	dec	di
	add	di,width_scan		; --> next plane's bytes
	mov	al,dl			; Save C2 bit
	call	align_and_store

if NUMBER_PLANES eq 4
	dec	di
	add	di,width_scan		; --> next plane's bytes
	mov	al,dh			; Save C3 bit
	call	align_and_store
endif

	pop	di
	inc	di
	ret

align_and_store_all endp

page
;--------------------------Private-Routine-----------------------------------;
;  align_and_store							     ;
;									     ;
;   The given byte is stored into the bitmap, being aligned to the	     ;
;   left hand side of byte						     ;
;									     ;
; Entry:								     ;
;	ES:DI --> destination						     ;
;	AL     =  byte to save						     ;
;	BL     =  shift count for alignment				     ;
; Returns:								     ;
;	ES:DI --> next destination byte					     ;
;----------------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

align_and_store proc near

	xchg	bx,cx
	shl	al,cl			; Perform any needed alignment
	stosb
	xchg	bx,cx
	ret

align_and_store endp
;----------------------------------------------------------------------------;
;   do_bitmap_computations						     ;
;									     ;
;	.   copies parameters from the info block to local storage	     ;
;	.   deceided whether the internal map is huge or small		     ;
;       .   set the carry flag if no copy is to be done			     ;
;       .   resets tha carry flag if copy is to be done and in DX:AX	     ;
;           returns the start pointer in the internal map		     ;
;									     ;
;----------------------------------------------------------------------------;

cProc	do_bitmap_computations,<NEAR,PUBLIC>

cBegin
	push	si			; save the pre processor address
	mov	ax,wptr es:[bx].biWidth
	mov	bitmap_width,ax
	mov	si,ax			; SI has cX
	mov	ax,wptr es:[bx].biHeight
	mov	bitmap_height,ax
	mov	di,ax			; DI has cY

; now we shall calculate the bytes required for ecah scan in one plane and
; for each scan in all planes as well as decide whether the map is a huge map 
; or not.
; SI has cX (will be < 32k)
; DI has cY (will be < 32k)

; Note that we are trying to find out the parameters for the target device 
; dependent bitmap.
; For the device dependent bitmap, each scan line will be alligned to a
; WORD boundary, or in other word each scan line has to be made a multiple
; of a WWORD.
; To do the WORD allignment we add 15 to the no of pels and then mask the 
; result with NOT(15).

	mov	cx,15			; to allign to double word
	add	si,cx
	not	cx
	and	si,cx			; # of bits a multiple of 32
	shiftr	si,3			; get the number of bytes to store
	mov	width_scan,si		; width of a scan
	mov	ax,1			; assume monochrome 
	test	fbsd,SD_COLOR		; our format mono chrome ?
	jz	ax_has_num_planes	; yes
	mov	ax,NUMBER_PLANES	; 4 planes
ax_has_num_planes:
	mul	si			; get the # of bytes/scan for all planes
	mov	next_scan,ax		; offset to next scan line
	mov	si,ax			; save the value in SI

; multiply by the number of scan lines to get the total store size required
; by the map.

	mul	di			; DX:AX has total number of bytes
	jno	size_obtained		; DX = 0 => small map
	dec	dx
	jnz	huge_bitmap		; DX > 1 => huge bitmap
	or	ax,ax			; DX = 1, AX = 0 => small map still
	jz	size_obtained

;----------------------------------------------------------------------------;
; We have a huge bitmap, and must calculate the additioanal parameters       ;
;----------------------------------------------------------------------------;

huge_bitmap:
	or	fbsd,SD_HUGE		; set the huge map flag
	mov	NextSegOff,__NEXTSEG; offset to next segment
	
;----------------------------------------------------------------------------;
; Let's calculate the no of scan lines which can fit in a segment and the    ;
; filler bytes at the end.						     ;
;----------------------------------------------------------------------------;

; remember, SI has the number of bytes per scan including all planes

	xor	ax,ax
	cwd
	inc	dx			; DX:AX = 64k
	div	si			; divide by bytes/scan for all planes

; DX now has the no of unused bytes at the end of the segment
; AX has the no of scans which fit in one segment

	mov	scans_per_seg,ax	; save the # of scans in each segment
	mov	filler_bytes,dx		; save the number of filler bytes
	add	dx,next_scan
	neg	dx			; gives the offset to last scan in seg
	mov	prev_offset,dx		; save it

;----------------------------------------------------------------------------;
; All the defining parameters for the target device dependent bitmap have    ;
; been obtained. Note that the memory for the target bitmap has already been ;
; allocated by a prior 'CreateDeviceBitmap' call and we have been passed a   ;
; long ptr to it. We shall now invoke the format specific initializer routine;
;----------------------------------------------------------------------------;

size_obtained:

;----------------------------------------------------------------------------;
; One of the passed parameters is the no of scan lines to be copied. We shall;
; validate this to ensure that the no of scans to copy from the starting scan;
; does not exceed the no of scans in the map.                                ;
;----------------------------------------------------------------------------;

	pop	si			; get back the preprocessor address
	push	bx			; save the information block pointer
	mov	cx,iStart.lo		; iStart.hi has to be 0
	mov	bx,bitmap_height	; no of scans in the map
	sub	bx,cx			; no of scans from start point to end
	ja	copy_needed		; not noting to copy case
	pop	bx			; balance stack
	stc				; error condition
	jmp	do_computations_ret     ; carry flag set to indicated no copy
copy_needed:
	mov	ax,cyScans.lo		; no of scans to copy
	minmax				; get minimum of ax and bx in ax
	mov	scanline_count,ax	; validated no of lines to copy
	mov	scans_copied,ax		; we will return this value
	pop	bx			; get back information block pointer
	
; ES:BX still points to the information block.

	mov	di,wptr es:[bx].biSize	; get the size of header
	add	di,bx			; add in the start offset to the table

; get the size of the color table.

	mov	ax,wptr es:[bx].biClrUsed
	or	ax,ax			;is it zero ?
	jnz	color_table_size_in_ax	;no. Non default size is in ax
	mov	ax,7fffh		;default size requested, make a big val
color_table_size_in_ax:
	mov	ColorsUsed,ax		; save it
	mov	ax, es:[bx].biHeight.lo ;AX: DIB height
	push	ax			;save it accross next call

; es:di points to the color table

	call	si	                ; format specific initializations done

;----------------------------------------------------------------------------;
; BITMAPS in windows have their origin at the top-left-corner. However the   ;
; first scan in the user buffer is actually the bottom most line of the map  ;
; and should be stored at the bottom of the map, so we will consider the     ;
; start scan number for our own format to start at the bottom.This routine is;
; called with a start scan number which may not be zero. We will thus have to;
; compute the exact starting scan line address.				     ;
;									     ;
; To get to the bottom scan line in the dependent format, we add number of   ;
; scans to blt to the start scan number					     ;
;----------------------------------------------------------------------------;
	pop	ax			;AX: DIB height
	dec	ax
	sub	ax, iStart.lo		;AX=biHeight-1-iStart: # of lines to
	xor	dx,dx			;move bmp ptr down by
	test	fbsd,SD_HUGE		; test for huge maps
	jz	eti_finish_y_calc

;----------------------------------------------------------------------------;
; This is a huge bitmap.Compute which segment the Y coordinate is in Assuming;
; that no bitmap will be bigger than 2 or 3 segments, iterative subtraction  ;
; will be faster than a divide, especially if Y is in the first segment.     ;
;----------------------------------------------------------------------------;

	mov	cx,__NEXTSEG		; offset to the next segment
	mov	bx,scans_per_seg	; number of scans in one segment

eti_huge_bitmap:
	add	dx,cx			; Show in next segment
	sub	ax,bx			; See if in this segment
	jnc	eti_huge_bitmap 	; Not in current segment, try next
	sub	dx,cx			; Show correct segment

; AX now has the no of scans from the top - the totals scans in the segment
; the no of scan left will be from the top as we will be going towards lower
; addresses in our internal map

	add	ax,bx			; restore correct Y
	inc	ax			; no of scans left includes 1st/last
	mov	huge_scans_left,ax	; save it
	dec	ax

eti_finish_y_calc:
	add	dx,lp_bits_own.sel	; Dest is our bitmap
	push	dx		        ; save segment
	mul	next_scan		; Compute offset within the segment
	pop	dx			; get back segment
	add	ax,lp_bits_own.off	; add to start offset of map
	clc                             ; reset carry, coppy is to be done
do_computations_ret:

cEnd
;----------------------------------------------------------------------------;

;----------------------------------------------------------------------------;
;   rgb_to_ipc								     ;
;              converts a RGB triplet into a a color byte for EGA adapters   ;
;									     ;
;  ENTRY:								     ;
;        DL : (R)ed value (0,255)                                            ;
;        AH : (G)reen (0,255)						     ;
;        AL : (B)lue  (0,255)						     ;
;									     ;
;  RETURN:								     ;
;        AL : the converted color byte, bit following bit significance:	     ;
;             bit 0 (LSB)   :    the red plane color                         ;
;             bit 1         :    the green plane color                       ;
;             bit 2         :    the blue plane color                        ;
;             bit 3         :    the intensity bit (4 plane only)	     ;
;             bit 4         :    (R+G+B > BW_THRESHOLD):1 ? 0 monochrome bit ;
;             bit 5         :    (if bits 0,1,2,3 are all 1 or 0) : 1 ? 0    ;
;             bits 6,7(MSB) :    0                                           ;
;									     ;
;  CALLS:								     ;
;        sum_RGB_alt_far  defined in ..\ROBJECT.ASM                          ;
;        this routine actually returns the result in DH, with AL,AH,DL having;
;        the physical color triplet				             ;
;									     ;
;        we will ignore AL,AH,DL and return the result in AL.		     ;
;----------------------------------------------------------------------------;

	assumes	ds,nothing
	assumes	es,nothing

rgb_to_ipc	proc	near
 
	xchg	al,dl		; get red into al and blue into dl
	push	bx
	push	cx
	push	dx		; save the registers which are to be affected
	call	sum_RGB_alt_far	; do the conversion
	mov	al,dh		; return result of intereset in AL
	pop	dx
	pop	cx
	pop	bx		; restore the values
	ret

rgb_to_ipc	endp
  
sEnd	DIMapSeg
end
