
		page	,132
		%out	Get/SetMode
		name	GSMODE
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	GSMODE.ASM
;
; DESCRIPTION
;	This file contains the code for the internal GetMode and SetMode
;	routines.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;
;	1.10	071588	vvr	Added InfoTables for new VGA modes;changed
;				widthbytes from byte to word.
;
;       1.11    082589  amitc   forced mode 10h to save 0a000h (as opposed to
;				08000h so that LOTUS 1-2-3's offscreen areas
;			        could be saved and restored

_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT

		.xlist
		include vgaabs0.inc
		include vgaic.inc
		include dc.inc
		include vga.inc
		include grabber.inc
		.list


		extrn	IC:byte
		extrn	DC:byte
		extrn	InHiLow:near
		extrn	OutHiLow:near

		public	GetMode
		public	SetMode

		public	InternalSetModeCall
		public	saveFlags


FALSE		=	0
TRUE		=	1

;
; INFOTABLE STRUCTURE
;
;	InfoTable structure is a subset of the InfoContext structure which we
;	use here to address portions of the data which we will simply rep mov
;	into the IC for the given mode.  See InfoContext structure for details.
;
InfoTable	struc
  itScrType	db	?
  itPixelsX	dw	?
  itPixelsY	dw	?
  itWidthBytes	dw	?		;changed 6/6/88 for mode 13h ~vvr
  itBitsPixel	db	?
  itPlanes	db	?
  itInterlaceS	db	?
  itInterlaceM	db	?
  itScrSeg	dd	?
InfoTable	ends


;
; Define InfoTable Parms for all modes supported
;
IT		label	byte
IT0		InfoTable	<ST_TEXT,320,200,080,1,1,0,0,0B800h>
IT1		InfoTable	<ST_TEXT,320,200,080,1,1,0,0,0B800h>
IT2		InfoTable	<ST_TEXT,640,200,160,1,1,0,0,0B800h>
IT3		InfoTable	<ST_TEXT,640,200,160,1,1,0,0,0B800h>
IT4		InfoTable	<ST_GRPH,320,200,080,2,1,1,1,0B800h>
IT5		InfoTable	<ST_GRPH,320,200,080,2,1,1,1,0B800h>
IT6		InfoTable	<ST_GRPH,640,200,080,1,1,1,1,0B800h>
IT7		InfoTable	<ST_TEXT,720,350,160,1,1,0,0,0B000h>

ITD		InfoTable	<ST_GRPH + ST_LARGE,320,200,080,1,4,0,0,0A000h>
ITE		InfoTable	<ST_GRPH + ST_LARGE,640,200,080,1,4,0,0,0A000h>
ITF		InfoTable	<ST_GRPH	   ,640,350,080,1,1,0,0,0A000h>
IT10		InfoTable	<ST_GRPH + ST_LARGE,640,350,080,1,4,0,0,0A000h>
IT11		InfoTable	<ST_GRPH + ST_LARGE,640,480,080,1,1,0,0,0A000h>
IT12		InfoTable	<ST_GRPH + ST_LARGE,640,480,080,1,4,0,0,0A000h>
IT13		InfoTable	<ST_GRPH + ST_LARGE,320,200,320,8,4,0,0,0A000h>

MAX_KNOWN_MODE	equ	13h

InternalSetModeCall  db	0	;will be 0ffh for internal setmode calls
SaveFlags	     db	10h	;will be non 10 if set differently.


		subttl	Getmode
		page


;
; GetMode - get display mode and update DC and IC data structures
;
; ENTRY
;	none
; EXIT
;	carry clear: successful
;	carry set  : unknown mode
; USES
;	flags
;
GetMode 	proc	near
		cld
		push	ax
		push	cx
		push	dx
		push	si
		push	di
		push	ds
		push	es

		xor	ax,ax
		mov	ds,ax			;setup ABS0 segment address
		assume	ds:Abs0
		mov	si,offset CrtMode	;point to BIOS video data area

		lodsb				;get CrtMode
		mov	dl,al			;save it, we need later
		mov	cs:[DC.dcScrMode],al
		cmp	al,MAX_KNOWN_MODE	;known modes
		jbe	@F
		stc				;unknown mode.
		jmp	gsModeRet		
@@:
		lodsw				;get CrtCols
		mov	cs:[IC.icCharsX],al	;use only low byte

		lodsw				;get CrtLen

;----------------------------------------------------------------------------;
; Special Code put in for support of APPS like LOTUS 1-2-3 version 3. These  ;
; apps save some data outside the displayable part of the screen for loading ;
; latches etc. We found that lotus configured for EGA runs in mode 10h and   ;
; for it we need to save about 9700h bytes rather than 8000h bytes that is   ;
; the size of the screen for mode 10h. So we put in a hack. If mode is 10h   ;
; we force the size of the buffer to be 0a000h.				     ;
;----------------------------------------------------------------------------;

		cmp	dl,10h			;special mode ?
		jnz	@f			;no, 
		mov	ax,0a000h		;force to save extra space
@@:
		mov	cs:[IC.icScrLen],ax

		lodsw				;get CrtStart
		mov	word ptr cs:[IC.iclpScr],ax

		mov	si,offset CursorMode	;skip the 8 cursor positions
		lodsw				;get CursorMode
		mov	cs:[DC.dcCursorMode],ax

		inc	si			;skip ActivePage
		lodsb				;get Addr6845 from video bios
		and	al,11110000b		;isolate D or B nybble
		mov	cs:[DC.dcAddrPatch],al

		mov	si,offset Rows
		lodsb				;get Rows
		inc	al			;make 1-based
		mov	cs:[IC.icCharsY],al

		mov	ax,cs
		mov	ds,ax
		mov	es,ax			;ds = es = cs
		assume	ds:_TEXT

		mov	dx,CRTC_ADDR AND 0FF0Fh
		or	dl,[DC.dcAddrPatch]	;get proper CRTC address

		mov	al,C_STRT_HGH
		call	InHiLow 		;get real video page address
		mov	[DC.dcScrStart],ax

		mov	al,C_CRSR_LOC_HGH
		call	InHiLow 		;get real cursor position
		mov	[DC.dcCursorPosn],ax

		mov	si,offset IT		;fill in rest of IC from IT
		mov	di,offset IC + icScrType
		mov	ax,SIZE InfoTable
		mov	cl,[DC.dcScrMode]	;mode is index into IT table
		cmp	cl,00Dh 		;if mode < 0Dh
		jb	gmMult			;  continue
		sub	cl,5			;else adjust for discontinuity
gmMult:
		mul	cl			;compute offset into IT table

		add	si,ax			;add it to base of table
		movsb				;copy first byte
		add	di,8			;jump over stuff we already did
		mov	cx,(iclpScr - icPixelsX)/2
		if	(iclpScr - icPixelsX) AND 1
		movsb
		endif
		rep	movsw			;copy majority of table
		add	di,2			;bump to segment portion of lpScr
		movsw				;and copy screen seg

		push	cx			;compute mouse scale factors
		push	cx
		mov	ax,cx
		mov	dx,cx
		mov	ah,[IC.icCharsX]	;ax = CharsX * 256
		mov	cx,640
		div	cx			;dx:ax/cx = MouseScaleX
		mov	[IC.icMouseScaleX],al

		pop	ax
		pop	dx
		mov	ah,[IC.icCharsY]	;ax = CharsY * 256
		mov	cx,200
		cmp	[DC.dcScrMode],00Fh
		jb	gmScaleY
		mov	cx,350
gmScaleY:
		div	cx			;dx:ax/cx = MouseScaleY
		mov	[IC.icMouseScaleY],al
		clc
gsModeRet:
		pop	es
		pop	ds
		pop	di
		pop	si
		pop	dx
		pop	cx
		pop	ax
		ret
GetMode 	endp


		subttl	SetMode
		page


;
; SetMode - set display mode based on DC and IC data structures
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	ax, bx, cx, dx, flags
;
SetMode 	proc	near
		push	ds
		mov	ax,cs
		mov	ds,ax
		assume	ds:_TEXT


		mov	byte ptr cs:[InternalSetModeCall],0ffh
		mov	al,[DC.dcScrMode]	;establish video mode
		xor	ah,ah
		int	010h
		mov	byte ptr cs:[InternalSetModeCall],0h

		mov	ax,01003h		;turn off blinking
		xor	bl,bl
		int	010h

		mov	cx,[DC.dcCursorMode]	;set cursor shape
		mov	ah,1
		int	010h
 	
		mov	dx,CRTC_ADDR AND 0FF0Fh ;get proper CRTC address
		or	dl,[DC.dcAddrPatch]

		mov	bx,[DC.dcScrStart]	;set CRTC video page
		mov	al,C_STRT_HGH
		call	OutHiLow

		mov	bx,[DC.dcCursorPosn]	;set cursor position
		mov	al,C_CRSR_LOC_HGH
		call	OutHiLow
smX:
		pop	ds
		ret
SetMode 	endp




_TEXT		ends
		end

