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
;			     DIScreeBlt					     ;
;		             ---------- 				     ;
;  Blts a portion of a Device Independent Bitmap directly onto the screen,or ;
;  viceversa.								     ;
;								             ;
;		. The Device Independent Bitmap can have 1/4/8/24 bits/pel   ;
;                 but only one plane.				             ;
;		. length in byte of 1 scan does not exceed 64k		     ;
;               . The target device can be a mono chrome or 3/4 plane EGA/VGA;
;                 display.						     ;
;		. No RasterOperations are supported and direct copy is done. ;
;		. Bound checking of the source rectangle is done against the ;
;                 screen extents.					     ;
;               . Returns 1 in AX to indicate success (0 => failure)         ;
;									     ;
;----------------------------------------------------------------------------;

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


	.xlist
	include	cmacros.inc
	include	gdidefs.inc
	include	display.inc
	include mflags.inc


IFNDEF	CGAHERC
	include	ega.inc
	include	egamem.inc
ENDIF

	include	macros.inc
	.list


IFDEF	CGAHERC
	EXCLUSION = 1
ENDIF

;----------------------------------------------------------------------------;
; define the equates and externAs here.					             ;
;----------------------------------------------------------------------------;

	externA		__NEXTSEG	; offset to next segment

MAP_IS_HUGE	equ	10000000b	; source maps spans segments
COLOR_BLT	equ	01000000b	; the screen is a color device

;----------------------------------------------------------------------------;

	externA	SCREEN_WIDTH		; width of screen in pels
	externA	SCREEN_HEIGHT		; height of screen in scans
	externA	SCREEN_W_BYTES		; width of screen in bytes
	externA	ScreenSelector		; video segment address
        externA COLOR_FORMAT            ; own color format

IFDEF	CGAHERC
	externA	Y_SHIFT_COUNT		; level of interleaving
ENDIF

ifdef EXCLUSION
        externFP exclude_far            ; xeclude cursor from blt area
        externFP unexclude_far          ; redraw cursor
endif

	externFP sum_RGB_alt_far	; get nearest color 
        externFP RLEBitBlt              ; in .\rlebm.asm

	externNP mono_munge		; in .\rlebm.asm
	externNP mono_munge_ret	        ; in .\rlebm.asm

	public	normal_DIBScreenBlt
	public	monochrome_screen
	public	save_num_colors_used
	public	parameter_error_relay
	public	bit_count_ok
	public	parameter_error
	public	valid_extents
	public	map_segment_update
	public	map_start_updated
	public	blt_area_in_1_segment
	public	init_1_bits_per_pel
	public	init_4_bits_per_pel
	public	init_8_bits_per_pel
	public	init_24_bits_per_pel
	public	init_done
	public	blt_next_scan
	public	blt_next_scan_continue
	public	new_scan_in_segment
	public	copy_1_bp_loop
	public	copy_1_bp_same_byte
	public	copy_1_bp_done
	public	copy_4_bp_loop
	public	copy_4_bp_next_nibble
	public	copy_4_bp_low_nibble
	public	copy_4_bp_next_byte
	public	copy_4_bp_done
	public	copy_8_bp_loop
	public	copy_24_bp_loop
	public	get_byte_with_test_ret
	public	DIScreenBlt_Ret
	public	create_color_table	
	public	copy_1_bp_full	
	public	copy_4_bp_full	
	public	copy_8_bp_full	
	public	copy_24_bp_full	
	public	get_byte_wo_test 
	public	get_byte_with_test 


sBegin	Data

	externB	enabled_flag

sEnd	Data

createSeg   _DIMAPS,DIMapSeg,word,public,code
sBegin	DIMapSeg
	assumes	cs,DIMapSeg
	assumes	ds,Data
	assumes	es,nothing

IFDEF	CGAHERC
	externW DIMapSeg_interlace_adjust
ENDIF

InitProc	equ	this word	; has the init routine addresses
	dw	DIMapSegOFFSET init_1_bits_per_pel
	dw	DIMapSegOFFSET init_4_bits_per_pel
	dw	DIMapSegOFFSET init_8_bits_per_pel
	dw	DIMapSegOFFSET init_24_bits_per_pel


cProc	DIBScreenBlt,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	parmD	lp_pdevice		; pointer to device structure
	parmW	ScrXorg			; Screen origin X coordinate
	parmW	ScrYorg			; Screen origin Y coordinate
	parmW	StartScan		; in the DIB
	parmW	NumScan			; number of scans to blt
	parmD	lpClipRect		; the clipping rectangle on the surface
	parmD	lpDrawMode		; pointer to draw mode structure
	parmD	lp_bits			; pointer to DI bits
        parmD   lp_bi                   ; pointer to bitmap info block
        parmD   dwColorInfo             ; Extra stuff passed by GDI

	localB	fbFlags			; flag bytes
	localW	MapXOrg			; X origin on map (after clip)
	localW	MapYOrg			; Y origin on map (after clip)
	localW	xExt			; pels in x-ext of blt (after clip)
	localW	yExt			; pels in y-ext of blt (after clip)
	localW	MapWidth		; width of map in pels
	localW  MapHeight		; height of map in scans
	localW	MapBitCount		; no of bits per map pel
	localB	MapBitSrl		; 1->0,4->1,8->2,24->3
	localW	ColorsUsed		; number of colors used
	localW	next_map_scan		; offset to next map scan
	localW	next_screen_scan	; offset to next screen scan
	localB	BitMask			; the mask for the current bit
	localD	lp_screen		; pointer to start byte of screen
	localV	color_xlate,256		; the local color xlate table
	localW	full_proc		; the routine to call to blt a byte/scan
	localW	get_byte		; holds address of fetch proc
	localW	SourceBytesPerScanBlt	; no of bytes in map for 1 scan blt
	localV	actual_rect,8		; storage for the actual rectangle
  	localB	UnusedPels		; ued for first byte DIB mask for 1,4bpp
	localW	PelsAtATime		; used for 1 bit per pel case
	localW  munge_proc		; munge color for mono driver

cBegin

;----------------------------------------------------------------------------;
; if the call is for an RLE, set up the parameters for RLEBitBlt and call it ;
; off.								 	     ;
;----------------------------------------------------------------------------;

	les	di,lp_bi		; load the pointer to the info block
	cmp	bptr es:[di].biCompression, 0
	jz	normal_DIBScreenBlt	; not a RLE call
	mov	bx, wptr es:[di].biWidth
	mov	cx, wptr es:[di].biHeight

	xor	ax,ax			; will need 0 for parameter passing
	arg	lp_pdevice		; pointer to screen descriptor
	arg	ScrXorg			; x origin on screen
	arg	ScrYorg			; y origin on screen
	arg	bx			; width of DIB is x extent
	arg	cx			; height of DIB is y extent
	arg	StartScan		; start scan wrt complete DIB
	arg	NumScan			; number of scans in RLE band
	arg	ax			; 0 for SetRle call
	arg	lpClipRect		; the clipping rectangle
	arg	lpDrawMode		; pointer to the draw mode structure
	arg	lp_bi			; long pointer to info block
	arg	lp_bits			; RLE buffer area
	cCall	RLEBitBlt		; call off
	jmp	DIScreenBlt_Ret		; ax ha return code

;----------------------------------------------------------------------------;

normal_DIBScreenBlt:

	cld
	mov	al,enabled_flag		; error if screen is not enabled
	or	al,al
	jz	parameter_error_relay	; will not blt now

;----------------------------------------------------------------------------;
; do a few validations at this point:				             ;
;               .  pointer to bits must not be NULL			     ;
;               .  destination must be a device and not a bitmap	     ;
;               .  calculate the actual blt rectangle on the screen, this    ;
;                  has to be clipped against the clip rectangle and can't    ;
;                  be NULL						     ;
;----------------------------------------------------------------------------;

	les	di,lp_bits		; get the pointers to the bits
	assumes	es,nothing

; test for souce pointer to be non NULL

	mov	ax,es
	or	ax,di			; NULL implies error
	jz	parameter_error_relay	; can't support NULL pointer

	les	di,lp_pdevice		; get the pdevice structure
	assumes	es,nothing

; test for target to be a screen

	mov	cx,es:[di].bmType	; test the type of the structure
	jcxz	parameter_error_relay	; can't support memory bitmaps here

; set mono chrome or color destination flag

	xor	al,al			
	mov	fbFlags,al		; reset the flag byte
	mov	ax,word ptr es:[di].bmPlanes
	cmp	ax,101h			; 1 plane, 1 bit per plane ?
	jz	monochrome_screen	; its a mono chrome screen
	mov	fbFlags,COLOR_BLT	; the screen is colored
monochrome_screen:

; load the height, width and color particulars of the bitmap

	lds	si,lp_bi		; pointer to the bitmap header
	mov	ax,wptr [si].biWidth	; the horixontal extent of map
	mov	MapWidth,ax		; save it
	mov	ax,wptr[si].biHeight	; the height of the map
	mov	MapHeight,ax		; save it

; get the number of colors used.

	mov	ax,wptr [si].biClrUsed	; just use the low word
        or	ax,ax			; 0 mean ue default number of colors
	jnz	save_num_colors_used
	mov	ax,7fffh		; make it a large poitive number
save_num_colors_used:
	mov	ColorsUsed,ax

	mov	ax,[si].biBitCount	; get the bits per pel
	mov	MapBitCount,ax		; save it
	cmp	[si].biPlanes,1		; no of color planes has to be 1
	jnz	parameter_error		; its an error

;----------------------------------------------------------------------------;
; validate the bits/pel count at this point. It has to be 1/4/8 or 24        ;
;----------------------------------------------------------------------------;
	xor	cx,cx			; will build up the serial no
	IRP	x,<1,4,8,24>
	cmp	ax,x
	je	bit_count_ok		; valid count
	inc	cl
	ENDM
parameter_error_relay:
	jmp	short parameter_error
bit_count_ok:
	mov	MapBitSrl,cl

;----------------------------------------------------------------------------;
; lets do the clipping at this point.  Let the start coordinate on the displ-;
; -ay surface be at (x,y). Let the height of the DIB be h, the width be w.Let;
; the start scan be s and the number of scans in the copy be n.		     ;
; 								             ;
; So the picture looks like:					             ;
comment ~

   (0,0)           (w-1,0)
  -|-----------------|-
   |		     |
   |(0,s)	     |(w-1,s)	   This shows the position of the BLT rect on
  -|-----------------|		   the DIB
   |    BLT RECT     |
   |(0,s+n-1)	     |(w-1,s+n-1)
  -|-----------------|-
   |		     |
  -|-----------------|-
  (0,h-1)	     (w-1,h-1)


  (x,y)		    (x+w-1,y)
  -|-----------------|-
   |		     |
   |(x,y+h-s-n)	     |(x+w-1,y+h-s-n) 
  -|-----------------|		   	  This shows the mapped rectangle on
   |    BLT RECT     |			  the screen. (note that the top left
   |(x,y+h-1-s)	     |(x+w-1,y+h-1-s)     of the DIB maps to bottom left of the
  -|-----------------|-			  screen
   |		     |
  -|-----------------|-
  (x,y+h-1)	     (x+w-1,y+h-1)


Over this we will have to impose the clipping rectangle	and end up with a 
clipping mask even on the DIB!

end comment ~

;----------------------------------------------------------------------------;

	les	di,lpClipRect		; es:[di] points to the clip rectangle
	lea	si,actual_rect		; save the actual rectangle here
	smov	ds,ss

; fill in the actual rectangle coordinates

	mov	ax,ScrYorg		; starting Y on the surface
	add	ax,MapHeight		; bottom scan is exclusive
	sub	ax,StartScan		; last scan on the BLT RECT on surface
	mov	[si].bottom,ax
	sub	ax,NumScan		; the number of scans in the blt area
	mov	[si].top,ax
	mov	ax,ScrXorg		; the left edge of the rectangle
	mov	[si].left,ax
	add	ax,MapWidth		; the width of the dib,right is excl.
	mov	[si].right,ax

; the following routine will do the clipping and return the clipped rectangle
; coordinates in an area pointed by DS:SI.
; Also, AX --  will have number if scans clipped from the bottom
;       BX --  no of pels clipped from the left

	cCall	IntersectRects		; find out intersection

	mov	MapXOrg,bx		; the number of pels clipped off left
	mov	MapYOrg,ax		; the number of scan clipped off the DIB

	mov	ax,[si].right
	sub	ax,[si].left
	mov	xExt,ax
	or	ax,ax			; is the X extent 0
	jz	parameter_error		; nothing to blt

	mov	ax,[si].bottom
	sub	ax,[si].top
	mov	yExt,ax
	or	ax,ax			; is the Y extent 0
	jnz	valid_extents

parameter_error:
	xor	ax,ax
	jmp	DIScreenBlt_Ret

valid_extents:

	mov	ax,[si].left		; the X origin on the surface
	mov	ScrXorg,ax
	mov	ax,[si].bottom		; the start scan on the surface 
	dec	ax			; the bottom can is exclusive
	mov	ScrYorg,ax

;----------------------------------------------------------------------------;
; we shall also calculate the no of byte in the source map that correspond to;
; xExt bits, as we shall use this information to test whther we cross a segm-;
; -ent on the source during the blt of xExt pels on a scan.		     ;
;----------------------------------------------------------------------------;

	mov	ax,xExt			; get the no of bits in 1 scan blt
	mul	MapBitCount		; multiply be no of bits per pel

; our assumption is that one scan does not cross a segnent, so ignore dx

	shiftr	ax,3			; get the number of bytes
	add	ax,1			; take the cealing
	mov	SourceBytesPerScanBlt,ax; save it for use later


;----------------------------------------------------------------------------;
; exclude the cursor from the blt area of the screen.			     ;
;----------------------------------------------------------------------------;

ifdef	EXCLUSION

	mov	cx,[si].left		; X1
	mov	dx,[si].top		; Y1
	mov	di,[si].bottom		; Y2
	mov	si,[si].right		; X2
	call	exclude_far		; exclude cursor from blt area

endif

;----------------------------------------------------------------------------;
; calculate the offset to the next scan in the map (scans there are DWORD    ;
; alligned ).								     ;
;----------------------------------------------------------------------------;

	mov	ax,MapWidth		; get the no of pels in a scan
	mul	MapBitCount		; bits per pel
	add	ax,31
	and	ax,not 31		; ax has multiple of 32 bits

; assume that the offset to the next scan fits in  a word, ignore DX

	shiftr	ax,3			; get the no of bytes
	mov	next_map_scan,ax	; save it
	mov	next_screen_scan,SCREEN_W_BYTES

;----------------------------------------------------------------------------;
; calculate the offset to the start byte in the map and the screen and the   ;
; position of the first pel in the screen byte and on the map.	             ;
;----------------------------------------------------------------------------;

	mov	ax,MapYOrg		; get the Y origin of the map
	mul	next_map_scan		; the result is in DX:AX
	mov	bx,ax
	mov	cx,dx			; result in CX:BX
	mov	ax,MapXOrg		; get the starting X pel no
	mul	MapBitCount		; no of bits per pel

; we will ignore DX 

	shiftr	ax,3			; get the no of bytes
	xor	dx,dx
	
; add in the contribution on CX:BX

	add	ax,bx
	adc	dx,cx			; DX:AX has the offset

; add in the contribution of the original start offset of the DIB.

	add	off_lp_bits,ax		; update the offset
	adc	dx,0			; incase this causes another wrap
	jz	map_start_updated	; done with updation.

; DX is not zero we have a huge map on our hands, update the start seg too.

	mov	bx,seg_lp_bits		; the segment part of lp_bits

map_segment_update:

	add	bx,__NEXTSEG		; make it go to next segment
	dec	dx			; do it until dx turns 0
	jnz	map_segment_update
	mov	seg_lp_bits,bx		; update the segment part of ptr

map_start_updated:

; lp_bits now points to the first byte of the map in the blt area

;----------------------------------------------------------------------------;
;  the starting scan no might not have crossed a segment but the blt area    ;
;  might still cross a segment, test for that.				     ;
;----------------------------------------------------------------------------;
		
	mov	ax,yExt			; get the Y ext of the blt
	mul	next_map_scan		; bytes per scan on the map
	add	ax,off_lp_bits		; add the first byte offset
	adc	dx,0			; do a 32 bit addition
	cmp	dx,0			; if dx is 0 we donot cross a segment
	jz	blt_area_in_1_segment
	or	fbFlags,MAP_IS_HUGE	; will have to test for segment update
blt_area_in_1_segment:

;----------------------------------------------------------------------------;
; now calculate the address of the start byte in the screen blt area. 	     ;
;----------------------------------------------------------------------------;

 	mov	ax,ScrYorg		; get the Y origin (bottom scan)

IFNDEF	CGAHERC
	mul	next_screen_scan	; no of bytes in a scan
ELSE
	call	device_get_dest_scan
ENDIF

; dx will definitely be zero, so ignore it

	mov	bx,ScrXorg		; get the x origin
	mov	cx,bx			; save it
	shiftr	bx,3			; get the no of bytes
	add	ax,bx			; this is the start offset 
	mov	off_lp_screen,ax	; save it
	mov	ax,ScreenSelector	; the screen segment
	mov	seg_lp_screen,ax	; save the segment

; lp_screen now has the pointer to the first byte in the screen blt area

;----------------------------------------------------------------------------;
; calculate the position of the first bit in the first byte of the screen    ;
; blt area.								     ;
;----------------------------------------------------------------------------;

	and	cl,07h			; get the bit no in the starting byte
	mov	bl,80h			; starting mask
	shr	bl,cl			; bl has the correct bit set
	mov	BitMask,bl		; save it

;----------------------------------------------------------------------------;
; now get the address of the full and partial byte proces that we will be    ;
; using depending on the bits per pel.				             ;
;----------------------------------------------------------------------------;

	les	di,lp_bi 		; point to the info block
	mov	si,wptr es:[di]		; get the size till start of color table
	add	di,si			; es:di points to color table
	xor	bx,bx
	mov	bl,MapBitSrl		; get the bits per pel serial num
	shl	bx,1			; look up a word table
	mov	bx,InitProc[bx]		; get the appropriate init procs addr
	jmp	bx			; do the format specific inits

;----------------------------------------------------------------------------;
;  the format specific initialization routines follow here.		     ;
;  ES:DI poits to the color table.				             ;
;----------------------------------------------------------------------------;

init_1_bits_per_pel:

	mov	ax,DIMapSegOFFSET copy_1_bp_full
	mov	full_proc,ax 

; we need to know how many bits in the first DIB byte (from the left) are
; unused.

	mov	ax,MapXOrg		; the start pel number in a scan
	and	ax,7			; the number of pel unused in 1st byte
	mov	UnusedPels,al		; save it

 
; fill in the color table with the two color values

	mov	ax,2
	min_ax	ColorsUsed		; color table may not have full set
	mov	cx,ax
	call	create_color_table	; translates the color into indices
	jmp	init_done
	   
init_4_bits_per_pel:
	
	mov	ax,DIMapSegOFFSET copy_4_bp_full
	mov	full_proc,ax		

; we need to know wheter we start with the high nibble or the low nibble

	mov	ax,MapXOrg		; start DIB pel number in a byte
	shr	ax,1			; if odd we start with low nibble
	rcl	UnusedPels,1		; LSB bit for low nibble usage

; fill in the color table with the 16 color values

	mov	ax,16
	min_ax	ColorsUsed		; color table need not have full set
	mov	cx,ax
	call	create_color_table	; translates the color into indices
	jmp	init_done
	   
init_8_bits_per_pel:

	mov	ax,DIMapSegOFFSET copy_8_bp_full
	mov	full_proc,ax		
 
; fill in the color table with the 256 color values

	mov	ax,256
	min_ax	ColorsUsed		; color table may not have full set
	mov	cx,ax
	call	create_color_table	; translates the color into indices
	jmp	init_done

init_24_bits_per_pel:

	mov	ax,DIMapSegOFFSET copy_24_bp_full
	mov	full_proc,ax		
	jmp	init_done


;----------------------------------------------------------------------------;
; the color convert routine reads a color triplet from the map specific color;
; table - converts it into an index and stores it in a table on the stack. It;
; does the conversion for the count of triplets passed in CX		     ;
;----------------------------------------------------------------------------;

create_color_table	proc	near

; CX     ---  count of triplets to converts
; ES:DI  ---  place where the triplets are stored

	xor	si,si

; for mono chrome drivers we will use only the mono bit as the pel value

IFNDEF	CGAHERCMONO
	mov	bh,MM_ALL		; mask for the index bits
ELSE
	mov	bh,00000001b
ENDIF
	xor	bl,bl			; assume a color blt, so no shift
	mov	ax,DIMapSegOFFSET mono_munge_ret
	test	fbFlags,COLOR_BLT	; is it a color blt
	jnz	save_munge_addr		; yes
	mov	ax,DIMapSegOFFSET mono_munge

save_munge_addr:
	mov	munge_proc,ax		; save color munger address

xlate_next_color:
	mov	ax,es:[di][0]		; get blue and green
	mov	dl,es:[di][2]		; get red
	add	di,size RGBQuad 	; point to next triplet
	xchg	al,dl			; get red into al, blue into dl
	push	cx			; save cx
	push	bx			; save the mask
	call	sum_RGB_alt_far		; returns index in DH
	pop	bx
	pop	cx			; restore

	call	[munge_proc]		; replicate bits for mono driver

	mov	color_xlate[si],dh	; save the translated color
	inc	si			; next entry
	loop	xlate_next_color	; do the next color
	ret

create_color_table	endp

;----------------------------------------------------------------------------;

init_done:

IFNDEF CGAHERCMONO

;----------------------------------------------------------------------------;
; we will program the EGA/VGA GRX registers to be in write mode 2, where the ;
; CPU value can be used to write masked pixels. 			     ;
; For Monochrome driver, we will have only plane 0 enabled where as for the  ;
; other drivers we will have all the planes enabled.			     ;
;----------------------------------------------------------------------------;
	les	di,lp_screen		; we need screen selector in es
	assumes	es,EGAMem		; to acces shadowed_mode

	mov	dx,EGA_BASE + GRAF_ADDR	; the GRX controller address

; for mono chrome driver skip programming mode and s/r

	test	fbFlags,COLOR_BLT	; is it a color driver ?
	jz	skip_mode_sr_programming

; set up for write mode 2

	mov	al,GRAF_MODE		; the mode register address
	mov	ah,M_COLOR_WRITE	; set up for READ 0/WRITE 2

if MASMFLAGS and EGA
	mov	es:shadowed_mode,ah	; for mouse cursor interrupt
endif
	out	dx,ax
	jmp	$+2			; I/O delay

; set up the combine and function register to be replace

	mov	al,GRAF_DATA_ROT	; the combine function register
	mov	ah,DR_SET		; use replace and no shifts
	out	dx,ax
	jmp	$+2			; I/O delay

skip_mode_sr_programming:


; set up the sequencer map mask register to enable all planes for color drivers and
; just plane 0 for monochrome drivers

	mov	ah,MM_ALL		; assume all plane are being enabled
	test	fbFlags,COLOR_BLT	; is it a color driver ?
	jnz	set_plane_masks		; yes
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

ENDIF  ;IFNDEF CGAHERCMONO

;----------------------------------------------------------------------------;
; The sorce bytes in a scan may cross a segment boundary only if the flag -  ;
; MAP_IS_HUGE is set.							     ;
;									     ;
; If the flag is set a test will be made at the start of every scan line to  ;
; see whether that scan crosses the segment boundary or not. If it does cross;
; a segment boundary, every byte will be fetched with a test to detect the   ;
; segment cross if not then 'LODSB' will be used to get the byte.	     ;
;									     ;
; We assume that a scan will not cross two segment boundaries.	             ;
;----------------------------------------------------------------------------;

	lds	si,lp_bits		; the strting point of the bits
	assumes	ds,nothing
	les	di,lp_screen		; the start on the screen
	assumes	es,nothing
	lea	bx,color_xlate		; address of the xlate table

	mov	cx,yExt			; the number of scans to copy

IFNDEF	CGAHERCMONO
	mov	dx,EGA_BASE + GRAF_DATA ; will alaways have this value
ENDIF


blt_next_scan:

; assume that segment crossings will not be required
	mov	get_byte,DIMapSegOFFSET get_byte_wo_test

	test	fbFlags,MAP_IS_HUGE	; will the blt cross a segment
	jz	blt_next_scan_continue	; segment crossing check not required

; test to see if the current scan will span a segment 

	push	dx
	mov	ax,si			; set current offset
	xor	dx,dx
	add	ax,SourceBytesPerScanBlt; no of bytes involved in 1 scan blt
	adc	dx,0			; collect the carry if any
	pop	dx			; restore
	jz	blt_next_scan_continue	; this scan will be in segment
        or      ax,ax                   ; dx:ax == 1:0 => scan's going to finish at end of seg
	jz	blt_next_scan_continue	; in this case, we will not actually span a segment

; the scan will cross a segment somewhere, so use the get_byte proc which tests
; for segment crossings

	mov	get_byte,DIMapSegOFFSET get_byte_with_test

blt_next_scan_continue:
	push	cx			; save scan loop count
	push	ds			; save map segment
	push	si
	push	di			; save the source and target pointers
	mov	ah,BitMask		; the starting mask
	mov	cx,xExt			; the no of pels to blt
	call	full_proc		; do the blt of a scan
	pop	di
	pop	si			; get back the pointers
	pop	ds

IFNDEF  CGAHERC

	sub	di,next_screen_scan	; we map top to bottom

ENDIF	;NOT CGAHERC

IFDEF	CGAHERC

IFDEF	HERCULES

	sub	di,2000h		; go up a bank
	jnb	device_scan_updated	; di has correct scan line offset
	sub	di,next_screen_scan	; go up a scan
	xor	di,8000h		; start at the bottom most bank

ENDIF	;HERCULES
IFDEF	IBM_CGA

	sub	di,2000h		; go up a bank
	jnb	device_scan_updated	; di has correct scan line offset
	sub	di,next_screen_scan	; go up a scan
	and	di,3fffh		; convert to interleave bank range

ENDIF	;IBM_CGA

device_scan_updated:

ENDIF	;CGAHERC

	pop	cx			; get back the no of scans left to blt
	add	si,next_map_scan	; update to the next scan
	jnc	new_scan_in_segment	; the new scan is in segment

; the new scan crosses a segment, has to be in the next segment, so take DS
; into the next segment

        cmp     cx,1                    ; are we actually done ?
        jz      new_scan_in_segment     ; yes, don't mess with segments
	mov	ax,ds			; get current DS
	add	ax,__NEXTSEG		; update to the next segment
	mov	ds,ax

new_scan_in_segment:
	loop	blt_next_scan		; blt all the scans

IFNDEF	CGAHERCMONO
	call	reset_registers		; reset the register
ELSE
	call	undo_cursor_exclusion
ENDIF
	mov	ax,yExt			; Return number of scans copied.
	jmp	DIScreenBlt_Ret		; exit


IFDEF	CGAHERCMONO

;----------------------------------------------------------------------------;
; we define a macro which takes three parameters (one implicit):  	     ;
;	 (1)  A Screen Byte pointer in ES:DI				     ;
;	 (2)  reg1 -- has a color value in byte (00 0r 0ffh) 	             ;
;        (3)  reg2 -- has a bit mask for the above byte			     ;
;									     ;
; and the macro sets the specified bit to the specified color bit. It uses   ;
; DX as a work register and does not preserve it.			     ;
;----------------------------------------------------------------------------;

set_bit_in_screen_byte macro	reg1, reg2

	mov	dl,reg2			;; get the bit mask
	not	dl			;; zero only the bit required
	and	es:[di],dl		;; mask out the required bit
	mov	dl,reg1			;; get the color value 
	and	dl,reg2			;; and in the bitmask
	or	es:[di],dl		;; set in the color bit

	endm

;----------------------------------------------------------------------------;

ENDIF	;CGAHERCMONO

;----------------------------------------------------------------------------;
;			copy_1_bp_full					     ;
;                       --------------					     ;
;  handles the blt of a scan or a part of it for 1 bits per pixel case       ;
;									     ;
;  Entry:							             ;
;		DS:SI	  --	  current byte in the map		     ;
;		ES:DI     --	  current byte in the screen		     ;
;		   AH	  --      current bit position in the first byte     ;
;	           BX	  --	  address of color_xlate table on stack      ;
;		   CX	  --      number of pels to convert		     ;
;                  DX	  --      BIT MASK register address		     ;
;								             ;
;  Returns:								     ;
;		DS:SI,ES:DI --	  next bytes in respective areas	     ;
;                        AH --    updated bit mask			     ;
;		      BX,DX --	  unchanged				     ;
;	              AL,CX --    destroyed				     ;
;----------------------------------------------------------------------------;
	

copy_1_bp_full	proc	near

; the first byte fetched from the DIB may yield partial number of pels
		
	call	get_byte		; get the first byte

; shift left the contents till the start pel is on left edge

	push	cx			; save
	mov	cl,UnusedPels		; no of pels not used in 1st byte
	shl	al,cl			; the first pel is on the edge
	neg	cl	
	add	cl,8			; the number of pels left in byte
	xor	ch,ch
	mov	PelsAtATime,cx		;save it
	pop	cx
copy_1_bp_loop:
	xchg	al,ah			; get the current mask
	push	cx			; save # to convert

; PelsATATime has the number of pels in the byte, usually 8 except for first
; and last

	cmp	cx,PelsAtATime		; 8 or more bits left
	jbe	copy_1_bp_same_byte	; no partial byte
	mov	cx,PelsAtATime		; convert the 8 bits in the byte
copy_1_bp_same_byte:
	push	cx			; save mask and byte

IFNDEF	CGAHERCMONO 
	out	dx,al			; set up the mask register
ENDIF	;not CGAHERCMONO

	shl	ah,1			; get the next oel into carry
	rcl	cl,1			; get the pel value
	and	cl,1			; only 1 bit significant
	xchg	al,cl			; save mask in cl, get pel in al
	xlat	ss:[bx]			; get the converted value

IFNDEF	CGAHERCMONO
	mov	ch,es:[di]
	mov	es:[di],al		; write the pel
ELSE
;----------------------------------------------------------------------------;
; AL     -- has the pel to write (0 or 1)	       		             ;
; CL     -- has the bit mask					   	     ;
; ES:DI  -- byte on screen					             ;
;									     ;
; the following macro call will set the bit in the screen byte to the specif-;
; -ied value (0 or 1)							     ;
;----------------------------------------------------------------------------;

	set_bit_in_screen_byte  al,cl		

;----------------------------------------------------------------------------;
ENDIF	
	mov	al,cl			; get the mask in al
	ror	al,1			; next bit position
	adc	di,0			; update screen pointer if needed
	pop	cx			; get back byte and mask
	loop	copy_1_bp_same_byte	; convert the byte
	pop	cx			; get back remaining bits
	mov	ah,al			; get back mask in ah
	sub	cx,PelsAtATime		; 8 or less were just converted
	jbe	copy_1_bp_done		; CX <= 0 means we have done all
	call	get_byte		; get the next byte
	mov	PelsAtATime,8		; do 8 bits in it
	jmp	short copy_1_bp_loop	; continue in loop

; we are done 

copy_1_bp_done:
	ret

copy_1_bp_full	endp

;----------------------------------------------------------------------------;
;			copy_4_bp_full					     ;
;                       --------------					     ;
;  handles the blt of a scan or a part of it for 4 bits per pixel case       ;
;									     ;
;  Entry:							             ;
;		DS:SI	  --	  current byte in the map		     ;
;		ES:DI     --	  current byte in the screen		     ;
;		   AH	  --      current bit position in the first byte     ;
;	           BX	  --	  address of color_xlate table on stack      ;
;		   CX	  --      number of pels to convert		     ;
;                  DX	  --      BIT MASK register address		     ;
;								             ;
;  Returns:								     ;
;		DS:SI,ES:DI --	  next bytes in respective areas	     ;
;                        AH --    updated bit mask			     ;
;		      BX,DX --	  unchanged				     ;
;	              AL,CX --    destroyed				     ;
;----------------------------------------------------------------------------;

copy_4_bp_full	proc	near
	test	UnusedPels,1		; is just the low nibble to be used
	jnz	copy_4_bp_first_byte	; for the first time? 

copy_4_bp_loop:
	call	get_byte		; get the next source byte

IFNDEF	CGAHERCMONO
	xchg	al,ah			; get the mask into al
	out	dx,al			; set up the pel mask for 1 pel
	xchg	al,ah			; get back byte in al
ENDIF

	push	ax			; save the byte
	shiftr	al,4			; get the high nibble into low pos
	xlat	ss:[bx]			; get the mapping index

IFNDEF	CGAHERCMONO
	mov	ah,es:[di]
	mov	es:[di],al		; write the pel
ELSE
;----------------------------------------------------------------------------;
; AL     -- has the pel to write (0 or 1)	       		             ;
; CL     -- has the bit mask					   	     ;
; ES:DI  -- byte on screen					             ;
;									     ;
; the following macro call will set the bit in the screen byte to the specif-;
; -ied value (0 or 1)							     ;
;----------------------------------------------------------------------------;

	set_bit_in_screen_byte  al,ah		

;----------------------------------------------------------------------------;
ENDIF	
	pop	ax			; get back pel
	ror	ah,1			; rotate mask
	jnc	copy_4_bp_next_nibble	; process low nibble of source
	inc	di			; position to next target byte
copy_4_bp_next_nibble:
	dec	cx			; one more pel done
	jcxz	copy_4_bp_done
copy_4_bp_low_nibble:

IFNDEF	CGAHERCMONO
	xchg	al,ah			; get the mask into al
	out	dx,al			; set up the mask
	xchg	al,ah			; get back the pel in al
ENDIF

	and	al,0fh			; get the lower 4 bits
	xlat	ss:[bx]			; get the mapping index
IFNDEF	CGAHERCMONO
	mov	dl,es:[di]
	mov	es:[di],al		; write the pel
	mov	dx,EGA_BASE + GRAF_DATA ; dx = mask register
ELSE
;----------------------------------------------------------------------------;
; AL     -- has the pel to write (0 or 1)	       		             ;
; CL     -- has the bit mask					   	     ;
; ES:DI  -- byte on screen					             ;
;									     ;
; the following macro call will set the bit in the screen byte to the specif-;
; -ied value (0 or 1)							     ;
;----------------------------------------------------------------------------;

	set_bit_in_screen_byte  al,ah		

;----------------------------------------------------------------------------;
ENDIF	
	ror	ah,1			; the next bit position in the mask
	jnc	copy_4_bp_next_byte	; continue xfer
	inc	di			; next target  byte
copy_4_bp_next_byte:
	dec	cx
	jnz	copy_4_bp_loop		; process all the remaining pels
copy_4_bp_done:
	ret
	
copy_4_bp_first_byte:
	call	get_byte		; process first byte separately
	jmp	copy_4_bp_low_nibble    ; Jump into the middle of the loop.


copy_4_bp_full	endp
;----------------------------------------------------------------------------;
;			copy_8_bp_full					     ;
;                       --------------					     ;
;  handles the blt of a scan or a part of it for 8 bits per pixel case       ;
;									     ;
;  Entry:							             ;
;		DS:SI	  --	  current byte in the map		     ;
;		ES:DI     --	  current byte in the screen		     ;
;		   AH	  --      current bit position in the first byte     ;
;	           BX	  --	  address of color_xlate table on stack      ;
;		   CX	  --      number of pels to convert		     ;
;                  DX	  --      BIT MASK register address		     ;
;								             ;
;  Returns:								     ;
;		DS:SI,ES:DI --	  next bytes in respective areas	     ;
;                        AH --    updated bit mask			     ;
;		      BX,DX --	  unchanged				     ;
;	              AL,CX --    destroyed				     ;
;----------------------------------------------------------------------------;

copy_8_bp_full	proc	near

; here every source byte contributes one pel to the destination

	call	get_byte		; get the next source byte

IFNDEF	CGAHERCMONO
	xchg	al,ah			; get the mask into al
	out	dx,al			; set up the mask for 1 pel
	xchg	al,ah			; get back the byte in al
ENDIF

	xlat	ss:[bx]			; get the mapping index value
IFNDEF	CGAHERCMONO
	mov	dl,es:[di]
	mov	es:[di],al		; write the pel
	mov	dx,EGA_BASE + GRAF_DATA ; dx = mask register
ELSE
;----------------------------------------------------------------------------;
; AL     -- has the pel to write (0 or 1)	       		             ;
; CL     -- has the bit mask					   	     ;
; ES:DI  -- byte on screen					             ;
;									     ;
; the following macro call will set the bit in the screen byte to the specif-;
; -ied value (0 or 1)							     ;
;----------------------------------------------------------------------------;

	set_bit_in_screen_byte  al,ah		

;----------------------------------------------------------------------------;
ENDIF	
	ror	ah,1			; rotate the mask for the next bit
	jnc     copy_8_bp_loop		; continue with same target byte
	inc	di			; the next screen byte
copy_8_bp_loop:
	loop	copy_8_bp_full		; repeat till all bytes processed
	ret

copy_8_bp_full	endp
;----------------------------------------------------------------------------;
;			copy_24_bp_full					     ;
;                       ---------------					     ;
;  handles the blt of a scan or a part of it for 24 bits per pixel case      ;
;									     ;
;  Entry:							             ;
;		DS:SI	  --	  current byte in the map		     ;
;		ES:DI     --	  current byte in the screen		     ;
;		   AH	  --      current bit position in the first byte     ;
;		   CX	  --      number of pels to convert		     ;
;                  DX	  --      BIT MASK register address		     ;
;								             ;
;  Returns:								     ;
;		DS:SI,ES:DI --	  next bytes in respective areas	     ;
;                        AH --    updated bit mask			     ;
;		         DX --	  unchanged				     ;
;	           AL,BX,CX --    destroyed				     ;
;----------------------------------------------------------------------------;


copy_24_bp_full	proc	near

; here every 3 source bytes contribute one pel for the screen

	push	dx
	push	cx
	push	ax			; save the bytes to be destroyed
	call	get_byte		; get the blue pel				; get blue and green
	mov	dl,al			; have blue in dl
	call	get_byte		; get green
	mov	ah,al			; have it in ah
	call	get_byte		; get red in al
	call	sum_RGB_alt_far		; do the mapping
	pop	ax

IFDEF	CGAHERCMONO
	call	mono_munge		; replicate the color pel
ENDIF

	mov	al,dh			; get the index in al (low 4)
	pop	cx
	pop	dx			; get the other values back

IFNDEF	CGAHERCMONO
	and	al,MM_ALL		; have just the index bits
	xchg	al,ah			; get the mask into al
	out	dx,al			; set up the mask for the bit
	xchg	al,ah			; get the mask back into ah
	mov	dl,es:[di]		; load the latches.
	mov	es:[di],al		; write the pel
	mov	dx,EGA_BASE + GRAF_DATA ; dx = mask register
ELSE
;----------------------------------------------------------------------------;
; AL     -- has the pel to write (0 or 1)	       		             ;
; CL     -- has the bit mask					   	     ;
; ES:DI  -- byte on screen					             ;
;									     ;
; the following macro call will set the bit in the screen byte to the specif-;
; -ied value (0 or 1)							     ;
;----------------------------------------------------------------------------;

	set_bit_in_screen_byte  al,ah		

;----------------------------------------------------------------------------;
ENDIF	

	ror	ah,1			; select the next bit for mask
	jnc	copy_24_bp_loop		; screen will have same byte
	inc	di			; go the next screen byte
copy_24_bp_loop:
	loop   copy_24_bp_full		; process all the other bytes
	ret

copy_24_bp_full	endp
;----------------------------------------------------------------------------;
;  The following two routines will be used to fetch the next source byte     ;
;  The first one is to be used when no segment crossing is to be tested and  ;
;  the next one when the scan at some point will cross a segment. The address;
;  of one of these two routinbes will be put in 'get_byte'		     ;
;----------------------------------------------------------------------------;

get_byte_wo_test  proc	near
	lodsb				; get the next byte 
	ret
get_byte_wo_test  endp


get_byte_with_test  proc near
	lodsb				; get the current byte
	or	si,si			; si wraps to 0 ?
	jnz	get_byte_with_test_ret	; no, we are in same segment
	push	dx			; save
	mov	dx,ds			; get current segment
	add	dx,__NEXTSEG		; go to the next segment
	mov	ds,dx			; update segment

; here we assume that the rest of the current scan will not cross a segment
; boundary again, so will use the shorter get byte proc

	mov	get_byte,DIMapSegOFFSET get_byte_wo_test
	pop	dx			; restore
get_byte_with_test_ret:
	ret				; get back

get_byte_with_test endp

IFNDEF	CGAHERCMONO
;----------------------------------------------------------------------------;
; finally reset the EGA/VGA registers to the their original state and return ;
; to caller wih success/failure.			                     ;
;----------------------------------------------------------------------------;

reset_registers proc near

; note that es still has the screen selector

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

; set map mask register in the sequencer to select all planes

	mov	dl,SEQ_ADDR		; the select register
	mov	al,SEQ_MAP_MASK		; the map mask register
	mov	ah,MM_ALL		; select all the planes
	out	dx,ax

; set read and write modes to 0

	mov	dl,GRAF_ADDR
	mov	al,GRAF_MODE		; the mode select register
	mov	ah,M_DATA_READ + M_PROC_WRITE
	out	dx,ax
if MASMFLAGS and EGA
	mov	es:shadowed_mode,ah	; shadow the mode
endif

; leave the address of the bitmask register in the GRX address register

	mov	al,GRAF_BIT_MASK
	out	dx,al

; now bring back the cursor

ifdef	EXCLUSION
	call	unexclude_far		; re-draw the cursor
endif
		
	ret

reset_registers endp

ENDIF

IFDEF	CGAHERCMONO

;----------------------------------------------------------------------------;
; bring back the cursor						 	     ;
;----------------------------------------------------------------------------;

undo_cursor_exclusion proc near

ifdef	EXCLUSION
	call	unexclude_far		; re-draw the cursor
endif
		
	ret

undo_cursor_exclusion endp

ENDIF	;CGAHERCMONO

IFDEF	CGAHERC
;----------------------------------------------------------------------------;
; device_get_dest_scan							     ;
;									     ;
; Compute the address offset of the start of the destination-scanline	     ;
; based on the type of device and the Y coordinate.			     ;
;									     ;
; Entry:								     ;
;	AX = Y coordinate						     ;
; Returns:								     ;
;	AX = offset of first byte of scan line of destination byte	     ;
; Registers Preserved:							     ;
;	BX,CX,DX,SI,DI,BP,DS,ES						     ;
; Registers Destroyed:							     ;
;	None								     ;
; Calls:								     ;
;	None								     ;
;----------------------------------------------------------------------------;

	public	device_get_dest_scan

device_get_dest_scan	proc	near

	push	dx
	push	bx
	mov	bx,ax
	and	bx,00000011b
	shl	bx,1
	mov	bx,DIMapSeg_interlace_adjust[bx] ;add interlace adjust to base address
	xchg	cx,dx
	mov	cl,Y_SHIFT_COUNT	;bias y coordinate for interlace
	shr	ax,cl
	xchg	cx,dx
	mov	dx,SCREEN_W_BYTES
	mul	dx
	add	ax,bx
	pop	bx
	pop	dx
	ret

device_get_dest_scan	endp


ENDIF	;CGAHERC
;----------------------------------------------------------------------------;
DIScreenBlt_Ret:
	       
cEnd


;----------------------------------------------------------------------------;
; The following routine find the interection of two rectangles.		     ;
;									     ;
; ES:DI   ---   points to a clipping rectangle				     ;
; DS:SI   ---   points to a source rectangle			             ;
;									     ;
; Returns:								     ;
;		rectangle pointed by ES:DI is not touched	  	     ;
;		DS:SI points to intersected rectangle			     ;
;		AX has no of scans clipped off the bottom (if any clip)	     ;
;		BX has no of pels clipped off the left edge (if any)         ;
;----------------------------------------------------------------------------;

cProc	IntersectRects,<NEAR,PUBLIC>

cBegin

; clip the top

	mov	ax,[si].top
	mov	bx,es:[di].top
	max_ax	bx			; the interected top is the max of the 2
	mov	[si].top,ax		; save it

; clip the right

	mov	ax,[si].right
	mov	bx,es:[di].right
	min_ax	bx			; the interected rt is the min of 2
	mov	[si].right,ax

; now  clip the bottom taking care to remember amount clipped

	mov	ax,[si].bottom		
	mov	bx,es:[di].bottom
	sub	ax,bx			; if we do clip |ax| has amount of clip
	mov	cx,ax			; save amount of clip
	cwd				; DX = 0, if no clipping
	and	ax,dx
	add	ax,bx			; ax has minimum
	not	dx
	and	cx,dx			; cx has amount of clip
	mov	[si].bottom,ax		; save

	push	cx			; save amount of clip at bottom

; now clip the left,

	mov	ax,[si].left
	mov	bx,es:[di].left
	sub	ax,bx
	mov	cx,ax			; save the amount of clip
	cwd
	not	dx
	and	ax,dx
	add	ax,bx			; get the max of the two
	not	dx
	and	cx,dx
	neg	cx			; make it positive
	mov	bx,cx			; have for return in bx
	mov	[si].left,ax		; save new left
	pop	ax			; get back amount clipped on bottom 


; coud be possible that left > right or top > bottom this means that the
; rectangle was clipped in totality
	
	mov	cx,[si].left
	cmp	cx,[si].right
	ja	total_clip_in_x
check_total_clip_in_y:
	mov	cx,[si].top
	cmp	cx,[si].bottom
	ja	total_clip_in_y
	jmp	IntersectRectsRet
total_clip_in_x:
	mov	[si].right,cx
	jmp	short check_total_clip_in_y
total_clip_in_y:
	mov	[si].bottom,cx

IntersectRectsRet:
cEnd

;----------------------------------------------------------------------------;


sEnd	DIMapSeg

	end


