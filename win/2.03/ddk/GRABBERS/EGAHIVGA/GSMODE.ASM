
		page	,132
		%out	Get/SetMode
		name	GSMODE
		title	Copyright (C) by Microsoft Inc. 1985-1987


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
  itWidthBytes	db	?
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


		subttl	Getmode
		page


;
; GetMode - get display mode and update DC and IC data structures
;
; ENTRY
;	none
; EXIT
;	none
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
		mov	cs:[DC.dcScrMode],al

		lodsw				;get CrtCols from video bios
		mov	cs:[IC.icCharsX],al	;use only low byte

		lodsw				;get CrtLen from video bios
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

