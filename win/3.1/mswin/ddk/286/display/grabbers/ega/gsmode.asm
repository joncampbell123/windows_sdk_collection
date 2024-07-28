
		page	,132
		%out	Get/SetMode
		name	GSMODE
		title	Copyright (c) Hewlett-Packard Co. 1985-1987

;
; HISTORY
;	1.00	060187	jhc	Epoch
;
;       1.10    082589  amitc   forced mode 10h to save 0a000h (as opposed to
;				08000h so that LOTUS 1-2-3's offscreen areas
;			        could be saved and restored
;	1.20	110290	sandeeps added checking for unknown modes.
;		changed header of GetMode.


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT

		.xlist
		include abs0.inc
		include ic.inc
		include dc.inc
		include ega.inc
		include grabber.inc
		.list


		extrn	IC:byte
		extrn	DC:byte
		extrn	fEriHooks:byte
		extrn	InHiLow:near
		extrn	OutHiLow:near

		public	GetMode
		public	SetMode


FALSE		=	0
TRUE		=	1

;
; INFOTABLE STRUCTURE
;
;	InfoTable structure is a subset of the InfoContext structure which we
;	use here to address portions of the data which we will simply rep mov
;	into the IC for the given mode.
;
InfoTable	struc
  itScrType	db	?
  itPixelsX	dw	?
  itPixelsY	dw	?
  itWidthBytes	dw	?
  itBitsPixel	db	?
  itPlanes	db	?
  itInterlaceS	db	?
  itInterlaceM	db	?
  itScrSeg	dd	?
InfoTable	ends


;
; Define InfoTable Parms for all modes
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

MAX_KNOWN_MODE	equ	10h


		subttl	Getmode
		page


;
; GetMode - get display mode and update DC and IC data structures
;
; ENTRY
;	none
; EXIT
;	Carry flag set if error else cleared.
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
		mov	ds,ax
		assume	ds:Abs0
		mov	si,offset CrtMode

		lodsb				;get CrtMode from video bios
		mov	dl,al			;save it
		mov	cs:[DC.dcScrMode],al
		cmp	al,MAX_KNOWN_MODE
		jbe	@F
		stc	
		jmp	GetModeRet
@@:

		lodsw				;get CrtCols from video bios
		mov	cs:[IC.icCharsX],al	;use only low byte

		lodsw				;get CrtLen from video bios

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

		lodsw				;get CrtStart from video bios
		mov	word ptr cs:[IC.iclpScr],ax

		mov	si,offset CursorMode	;skip cursor positions
		lodsw				;get CursorMode from video bios
		mov	cs:[DC.dcCursorMode],ax

		inc	si			;skip ActivePage
		lodsb				;get Addr6845 from video bios
		and	al,11110000b		;isolate D or B nybble
		mov	cs:[DC.dcAddrPatch],al

		mov	si,offset Rows
		lodsb				;get Rows from video bios
		inc	al			;make 1-based
		mov	cs:[IC.icCharsY],al

		mov	ax,cs
		mov	ds,ax
		mov	es,ax
		assume	ds:_TEXT

		mov	dx,CRTC_ADDR AND 0FF0Fh
		or	dl,[DC.dcAddrPatch]	;patch for proper address

		mov	al,C_STRT_HGH
		call	InHiLow 		;get real crtc start address
		mov	[DC.dcScrStart],ax

		mov	al,C_CRSR_LOC_HGH
		call	InHiLow 		;get real cursor position
		mov	[DC.dcCursorPosn],ax

		mov	si,offset IT
		mov	di,offset IC + icScrType
		mov	ax,SIZE InfoTable
		mov	cl,[DC.dcScrMode]
		cmp	cl,00Dh
		jb	gmMult
		sub	cl,5
gmMult:
		mul	cl

		add	si,ax
		movsb
		add	di,8
		mov	cx,(iclpScr - icPixelsX)/2
		if	(iclpScr - icPixelsX) AND 1
		movsb
		endif
		rep	movsw
		add	di,2
		movsw

		push	cx
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
GetModeRet:
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

		mov	al,[DC.dcScrMode]
		xor	ah,ah
		int	010h

		cmp	[fEriHooks],TRUE
		je	smX

		mov	ax,01003h
		xor	bl,bl
		int	010h

		mov	cx,[DC.dcCursorMode]
		mov	ah,1
		int	010h

		mov	dx,CRTC_ADDR AND 0FF0Fh
		or	dl,[DC.dcAddrPatch]

		mov	bx,[DC.dcScrStart]
		mov	al,C_STRT_HGH
		call	OutHiLow

		mov	bx,[DC.dcCursorPosn]
		mov	al,C_CRSR_LOC_HGH
		call	OutHiLow
smX:
		pop	ds
		ret
SetMode 	endp



_TEXT		ends
		end
