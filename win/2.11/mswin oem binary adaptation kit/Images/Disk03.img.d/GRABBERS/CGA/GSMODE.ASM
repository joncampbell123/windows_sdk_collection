
		page	,132
		%out	Get/SetMode
		name	GSMODE
                title   Copyright (c) Microsoft Corp. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include abs0.inc
		include ic.inc
		include dc.inc
		include cgaherc.inc
		include grabber.inc
		.list

		extrn	IC:byte
		extrn	DC:byte
		extrn	InHiLow:near
		extrn	OutHiLow:near
		extrn	GetModeDev:near
		extrn	SetModeDev:near

		public	GetMode
		public	SetMode


FALSE		=	0
TRUE		=	1

;
;	DISCLAIMER
;
;	Get/SetMode for the CGAHERC branch is untuned.	Currently, these
;	routines, and their corresponding device-specific counterparts
;	(Get/SetModeDev) are heavily table-driven.  While it is very clear
;	these tables contain enormous amounts of redundant data, they remain
;	for now because it is much easier to debug tables than a whole
;	fistfull of twisted conditional logic.	Also beware that, due to its
;	'green' state, much of the documentation (or lack thereof) in this
;	branch of the grabber source tree is not up to the usual standards.
;


IT		label	byte
		ifdef	CGA
IT0		InfoContext	<DI_CGA,ST_TEXT,2400,1800,40,25,16,32,320,200,080,1,1,0,0,0B8000000h,00800h>
IT1		InfoContext	<DI_CGA,ST_TEXT,2400,1800,40,25,16,32,320,200,080,1,1,0,0,0B8000000h,00800h>
IT2		InfoContext	<DI_CGA,ST_TEXT,2400,1800,80,25,32,32,640,200,160,1,1,0,0,0B8000000h,01000h>
IT3		InfoContext	<DI_CGA,ST_TEXT,2400,1800,80,25,32,32,640,200,160,1,1,0,0,0B8000000h,01000h>
IT4		InfoContext	<DI_CGA,ST_GRPH,2400,1800,40,25,16,32,320,200,080,2,1,1,1,0B8000000h,04000h>
IT5		InfoContext	<DI_CGA,ST_GRPH,2400,1800,40,25,16,32,320,200,080,2,1,1,1,0B8000000h,04000h>
IT6		InfoContext	<DI_CGA,ST_GRPH,2400,1800,80,25,32,32,640,200,080,1,1,1,1,0B8000000h,04000h>
		endif


		ifdef	HERCULES
IT0		InfoContext	<DI_HERCULES,ST_TEXT,2250,1450,80,25,32,32,640,200,160,1,1,0,0,0B0000000h,01000h>
IT1		InfoContext	<DI_HERCULES,ST_GRPH,2250,1450,80,25,28,18,720,348,090,1,1,2,3,0B0000000h,08000h>
IT2		InfoContext	<DI_HERCULES,ST_GRPH,2250,1450,80,25,28,18,720,348,090,1,1,2,3,0B8000000h,08000h>
		endif


		ifdef	MULTIMODE
IT0		InfoContext	<DI_MULTIMODE,ST_TEXT,2100,1640,40,25,16,32,320,200,080,1,1,0,0,0B8000000h,00800h>
IT1		InfoContext	<DI_MULTIMODE,ST_TEXT,2100,1640,40,25,16,32,320,200,080,1,1,0,0,0B8000000h,00800h>
IT2		InfoContext	<DI_MULTIMODE,ST_TEXT,2100,1640,80,25,32,32,640,200,160,1,1,0,0,0B8000000h,01000h>
IT3		InfoContext	<DI_MULTIMODE,ST_TEXT,2100,1640,80,25,32,32,640,200,160,1,1,0,0,0B8000000h,01000h>
IT4		InfoContext	<DI_MULTIMODE,ST_GRPH,2100,1640,40,25,16,32,320,200,080,2,1,1,1,0B8000000h,04000h>
IT5		InfoContext	<DI_MULTIMODE,ST_GRPH,2100,1640,40,25,16,32,320,200,080,2,1,1,1,0B8000000h,04000h>
IT6		InfoContext	<DI_MULTIMODE,ST_GRPH,2100,1640,80,25,32,32,640,200,080,1,1,1,1,0B8000000h,04000h>
IT7		InfoContext	<DI_MULTIMODE,ST_TEXT,2100,1640,80,25,32,32,640,200,160,1,1,0,0,0B0000000h,01000h>

IT8		InfoContext	<DI_MULTIMODE,ST_TEXT,2100,1640,80,27,32,35,640,200,160,1,1,0,0,0B8000000h,010E0h>
IT9		InfoContext	<DI_MULTIMODE,ST_TEXT,2100,1640,80,27,32,35,640,200,160,1,1,0,0,0B8000000h,010E0h>
ITA		InfoContext	<DI_MULTIMODE,ST_TEXT,2100,1640,40,27,16,35,320,200,080,1,1,0,0,0B8000000h,00870h>
ITB		InfoContext	<DI_MULTIMODE,ST_TEXT,2100,1640,40,27,16,35,320,200,080,1,1,0,0,0B8000000h,00870h>

ITC		InfoContext	<DI_MULTIMODE,ST_GRPH,2100,1640,80,25,32,32,640,400,080,1,1,2,3,0B8000000h,08000h>
ITD		InfoContext	<DI_MULTIMODE,ST_GRPH,2100,1640,80,25,32,32,640,400,080,1,1,2,3,0B8000000h,08000h>
ITE		InfoContext	<DI_MULTIMODE,ST_GRPH,2100,1640,40,25,16,32,320,400,080,2,1,2,3,0B8000000h,08000h>
ITF		InfoContext	<DI_MULTIMODE,ST_GRPH,2100,1640,40,25,16,32,320,400,080,2,1,2,3,0B8000000h,08000h>
		endif


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
		push	bx
		push	cx
		push	dx
		push	si
		push	di
		push	ds
		push	es

		call	GetModeDev

		mov	ax,cs
		mov	ds,ax
		mov	es,ax
		assume	ds:_TEXT

		mov	si,offset IT
		mov	di,offset IC
		mov	al,[DC.dcScrMode]
		mov	cx,(SIZE InfoContext)
		mul	cl
		add	si,ax
		shr	cx,1
		if	(SIZE InfoContext) AND 1
		movsb
		endif
		rep	movsw

		mov	ax,[DC.dcScrStart]
		mov	word ptr [IC.iclpScr],ax

		pop	es
		pop	ds
		pop	di
		pop	si
		pop	dx
		pop	cx
		pop	bx
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
		push	ax
		push	bx
		push	cx
		push	dx
		push	si
		push	di
		push	ds
		push	es

		mov	ax,cs
		mov	ds,ax
		assume	ds:_TEXT

		call	SetModeDev

		mov	dx,CRTC_ADDR
		mov	al,C_STRT_HGH
		mov	bx,[DC.dcScrStart]
		sar	bx,1
		xchg	bl,bh
		call	OutHiLow

		mov	al,C_CRSR_LOC_HGH
		mov	bx,[DC.dcCursorPosn]
		call	OutHiLow

		mov	al,C_CRSR_START
		mov	bx,[DC.dcCursorMode]
		xchg	bl,bh
		call	OutHiLow

		pop	es
		pop	ds
		pop	di
		pop	si
		pop	dx
		pop	cx
		pop	bx
		pop	ax
		ret
SetMode 	endp


_TEXT		ends
		end

