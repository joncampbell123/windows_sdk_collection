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

	.xlist
;----------------------------Module-Header------------------------------;
; Module Name: COMPUTEY.BLT
;
; Subroutine to compute Y-related parameters of BLT.
;
; This file is part of a set that makes up the Windows BitBLT function
; at driver-level.
;-----------------------------------------------------------------------;
	.list

	.xlist
if	MASMFLAGS and PUBDEFS
	public	compute_y
ifdef	GEN_COLOR_BLT
endif
	public	compute_y_100
	public	compute_y_120
ifdef	GEN_COLOR_BLT
	public	compute_y_140
endif
	public	compute_y_160
	public	compute_y_180
	public	compute_y_200
ifdef	GEN_COLOR_BLT
	public	compute_y_220
endif
	public	compute_y_240
	public	compute_y_260
endif
	.list

;----------------------------Private-Routine----------------------------;
; compute_y
;
; Compute y-related parameters.
;
; The parameters related to the Y coordinate and BLT direction
; are computed.  The parameters include:
;
;	a) Index to next scan line
;
;	b) Starting Y address calculation
;
;	c) Huge bitmap update parameters
;
;		1) overflow conditional jump opcode
;
;		2) overflow address range
;
;		3) bmFillBytes correct value
;
;		4) bmSegmentIndex correct value
;
;	d) Index to next plane
;
; Entry:
;	BP --> DEV structure to use (src or dest)
;	AX  =  Y coordinate
;	CX  =  BLT direction
;	       0000 = Y+
;	       FFFF = Y-
;	BX  =  inclusive Y extent
; Returns:
;	CX  =  BLT direction
;	BX  =  inclusive count
; Registers Preserved:
;	BP,DS,ES
; Registers Destroyed:
;	AX,DX,SI,DI,flags
; Calls:
;	None
;
; ************************************************************************
; modifications for bug fix:
;
; A bug was found when the blit area from a huge mono bitmap came from
; the second or subsequent segments but was contained in a segment. The
; original code special cased this out. However the special case code 
; does not check for huge maps and would work only if the blt area is totaly
; contained in the first segment.
;
; To resolve this bug I shall set the SPANS_SEG flag once the blt area
; goes past the first segment even if the blt area is contained in one segment
;
; ************************************************************************
;
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

compute_y	proc near

	mov	si,seg_index[bp] 	;Is this a huge bitmap?
	or	si,si
	jne	compute_y_100		;  Yes, lots of special processing
;----------------------------------------------------------------------------;
; Updates for small maps for next scan line and next plane will now be       ;
; exactly the same as what we have for huge maps, as the formats will now be ;
; the same. However as the map is still contained in one segment, we will not;
; need to do the segment calculation. Instead of having separate code for the;
; small maps, we will merge into the code for huge maps just after the segme-;
; -nt calculation has been done, and the scan position in the correct segment;
; has been found. The code there will also calculate the overflow check      ;
; this will be meaningless for small maps, however we shall do it but will   ;
; not be needing it for the sctual transfer.			             ;
;----------------------------------------------------------------------------;

; This is either a small bitmap or a display. For small maps we will merge
; into the code for huge maps, for EGA display the update to the next scan 
; line is just:
;	+/- bmWidthBytes

	test	dev_flags[bp],IS_DEVICE	; is it the physical display
	jz	merge_for_small		; merge into huge map code

; this is for the physical display only.

	
	mov	si,width_b[bp]			;Need bmWidthBytes a couple of times
	mul	si				;Compute Y address
	add	word ptr lp_bits[bp][0],ax	;  and add it into lp_bits
	xor	si,cx				; 1's complement if Y-
	sub	si,cx				; 2's complement if Y-


; SI is now +/- bmWidthBytes.

	mov	next_scan[bp],si 	;Set index to next scan line
	ret				;All done with device, small bitmaps



	subttl	Y Computations For Huge Bitmaps
	page

; This is a huge bitmap.	Compute which segment the Y coordinate
; is in, and update the lp_bits pointer to that segment.  This will
; be done iteratively since it is expected that only two or three
; segments will ever be involved, so it will be faster in most
; cases.



compute_y_100:
;	mov	si,seg_index[bp] 	;Get segment index
	mov	di,scans_seg[bp] 	;Get number of scans per segment
	xor	dx,dx			;Sum segments here

compute_y_120:
	add	dx,si			;Compute bias to next segment selector
	sub	ax,di			;See if Y is within this 64K
	jae	compute_y_120		;Y isn't in the 64K, try next
	sub	dx,si			;Added one index too many
	add	wptr lp_bits[bp][2],dx	;Update selector



; The selector has been updated if needed.  Now see if the BLT
; will span a segment boundary.  If it will span a segment boundary,
; then set the SPANS_SEG flag.  Optimizations can be made if the BLT
; doesn't cross a segment boundary.
;
; AX is currently the Y offset within a segment minus the number of
; scans in a segment, so it must be negative.
;
; Currently:
; 	AX = offset within segment - # scans per segment.
; 	DI - # scans per segment
; 	CX = BLT direction
; 	BX = inclusive Y extent


ifdef	GEN_COLOR_BLT
compute_y_140:
endif
	mov	si,ax			;Need a copy of Y
	jcxz	compute_y_160		;This is a Y+ BLT


; Y- BLT
;
; SI is the Y offset within the segment minus the number of scans
; in the segment (it is negative).  Add in the number of scans,
; then subtract the inclusive extent.  If the result stays positive,
; then the result fits within the segment, else it spans more than
; one segment.
;
; Also take the 2's complement of both the bmSegmentIndex and the
; bmFillBytes values.  This will allow the Y update code to always
; generate adds.	Also save the conditional jump opcode for the
; scan line update generation.


	mov	comp_test[bp],LOW I_JC_P12H
	neg	fill_bytes[bp]		;Set 2's complement for >64K
	neg	seg_index[bp]		;  huge update generation
	add	si,di			;Get Y offset within segment
	sub	si,bx			;Subtract inclusive Y extent
	jns	compute_y_200		;BLT fits within one segment
	jmp	short compute_y_180	;BLT spans a segment boundary




; Y+ BLT
;
; SI is the Y offset within the segment minus the number of scans
; in the segment (it is negative).  Add in the inclusive extent.
; If the result is still negative, then the BLT is contained
; within the segment, else it spans more than one segment.
;
; Also save the conditional jump opcode for the scan line update
; generation.


compute_y_160:
	mov	comp_test[bp],LOW I_JNC_P12H
	add	si,bx			;Is last scan line in another segment?
	js	compute_y_200		;  No, BLT will fit in the segment

compute_y_180:
	or	dev_flags[bp],SPANS_SEG	;Show source spans a segment



; The correct segment and the SPANS_SEG flag have been computed.
; Compute the Y address and update lp_bits.


compute_y_200:

;----------------------------------------------------------------------------;
; to fix a bug (see modification legend in header) the SPANS_SEG flag will   ;
; be set irrespective of whether the blt area spans a segment or not.        ;
;----------------------------------------------------------------------------;

	or	dev_flags[bp],SPANS_SEG	;Show source spans a segment
	add	ax,di			;Get Y offset within segment
merge_for_small:			;code for small map starts here too
	mov	si,width_b[bp]		;Need bmWidthBytes a couple of times
	mul	si			;Compute Y address
	add	wptr lp_bits[bp][0],ax	;  and add it into lp_bits



; Compute and save the increment to the next scan line.  This
; increment will be:
;
ifdef	GEN_COLOR_BLT
;     If a monochrome bitmap, regardless of color conversion:
;
endif
; 	+/- bmWidthBytes
;
ifdef	GEN_COLOR_BLT
;     If a color bitmap, and it's the source for a color==>mono BLT:
;
; 	+/- bmWidthBytes * NUMBER_PLANES
;
;     For a color bitmap, either mono==>color as the destination,
;     or color==>color:
;
; 	if Y+ blt, then 0
; 	if Y-, then -2 * NUMBER_PLANES * bmWidthBytes
;
;
; These increments take into account any additions to the offset
; made in the plane selection logic.
;
; In all cases, set plane_w to be bmWidthBytes.
endif
;
;
; Also compute the address range used to test for overflow.
; This value will be  (bmFillBytes + bmWidthBytes)  for Y+ BLTS.
; For Y- BLTs, it will be  -(planes * bmWidthBytes).


	mov	dx,si			;Set DX = Y+ range check value
	add	dx,fill_bytes[bp]	;  of bmFillBytes + bmWidthBytes
	mov	di,si			;Set DI = Y- mono range check
	neg	di
	mov	plane_w[bp],si		;Plane width will always be scan width
ifdef	GEN_COLOR_BLT
	test	dev_flags[bp],IS_COLOR	;Is this a mono device?
	jz	compute_y_220		;  Yes, just conditionally complement


; Have a color device. The increment will depend on color
; conversion or not, using the values given above.
;
; Also have to correct the offset portion of the pointer to account
; for this being a color bitmap.  The pointer points to where the scan
; would start if we were using the old small format (each plane separate).
; Because the scans here are interleaved we must add
; (NUMBER_PLANES - 1) * (the size of a plane) to get to where the scan
; actually starts in this interleaved format.
;                      (old format should die soon !!!) ALL_HUGE



if NUMBER_PLANES eq 4
	add	wptr lp_bits[bp][0],ax
endif
	add	ax,ax			;(NUMBER_PLANES - 1) * offset within plane
	add	wptr lp_bits[bp][0],ax	;  and add it into lp_bits
if NUMBER_PLANES eq 4
	shiftl	si,2			;Compute  NUMBER_PLANES * bmWidthBytes
else
	add	si,si			;Compute  NUMBER_PLANES * bmWidthBytes
	sub	si,di			; DI == -SI(before * 2) == -bmWidthBytes
endif
	mov	di,si			;Set Y- range check
	neg	di
	test	dev_flags[bp],COLOR_UP	;Color ==> mono update in use?
	jz	compute_y_220		;  Yes, just conditionally complement


; The BLT is color to color.  Want either 0 or -2*NUMBER_PLANES * bmWidthBytes.

; !!! What's going on with AX below?  Seems pointless.
	xor	ax,ax			;Assume Y+ BLT
	xchg	ax,si			;Huge color Y+ increment is 0
	jcxz	compute_y_240		;This is a Y+ BLT
	xchg	ax,si
	add	si,si			;Y-, need -2*NUMBER_PLANES*bmWidthBytes

compute_y_220:
endif
	xor	si,cx			;Conditionally negate increment
	sub	si,cx

compute_y_240:
	mov	next_scan[bp],si 	;Save update to next scan
	jcxz	compute_y_260		;Y+, DX has range check value
	mov	dx,di

compute_y_260:
	mov	comp_value[bp],dx	;Save range check value
	ret				;All done

compute_y	endp
