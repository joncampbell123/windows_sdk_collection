
		page	,132
		%out	Cga
		name	CGA
		title	Copyright (C) by Microsoft Inc. 1985-1987


;
; HISTORY
;	1.01	091587	*** *******	Fixed SetModeDev to keep video off to prevent
;				screen 'bounce'.
;


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


NO_STDGRABENTRY =	1

		.xlist
		include dc.inc
		include ic.inc
		include cgaherc.inc
		include oem.inc
		include abs0.inc
		include grabber.inc
		.list

		extrn	InHiLow:near
		extrn	OutHiLow:near
		extrn	OemBeep:near

		public	MAX_VISTEXT
		public	MAX_VISGRPH
		public	MAX_TOTTEXT
		public	MAX_TOTGRPH
		public	MAX_CDSIZE

		public	DC
		public	IC

		public	GrabTextSize
		public	GrabGrphSize
		public	SaveTextSize
		public	SaveGrphSize
		public	PhysColorTable

		public	InitScreen
		public	DevInit
		public	GetModeDev
		public	SetModeDev


;
; Define equates
;

FALSE		=	0
TRUE		=	1

MAX_GBTEXTSIZE	=	gbWidth
MAX_GBGRPHSIZE	=	gbBits

MAX_CDSIZE0	=	(SIZE DeviceContext) + (SIZE InfoContext)
MAX_CDSIZE1	=	(SIZE VideoBiosData)
MAX_CDSIZE2	=	(SIZE HpBiosData)
MAX_CDSIZE	=	MAX_CDSIZE0 + MAX_CDSIZE1 + MAX_CDSIZE2

MAX_VISTEXT	=	80*25 + 02*25 + 2
MAX_VISGRPH	=	16*1024

MAX_TOTTEXT	=	04*1024
MAX_TOTGRPH	=	16*1024


AddrLow 	dw	0800,0800,1500,1500,3000,3000,3000,0
AddrHi		dw	1500,1500,3000,3000,5000,5000,5000,0

Map1		dw	1500,3000,5000
Map2		dw	0001,0003,0004
MapLen		=	($ - Map2)/2


DTX		label	byte
DT0		DeviceContext	<0,0,0,00607h,02Ch,0,030h,0,0>
DT1		DeviceContext	<1,0,0,00607h,028h,0,030h,0,0>
DT2		DeviceContext	<2,0,0,00607h,02Dh,0,030h,0,0>
DT3		DeviceContext	<3,0,0,00607h,029h,0,030h,0,0>
DT4		DeviceContext	<4,0,0,0A080h,02Ah,0,030h,0,0>
DT5		DeviceContext	<5,0,0,0A080h,02Eh,0,030h,0,0>
DT6		DeviceContext	<6,0,0,0A080h,02Eh,0,03Fh,0,0>


;
; Allocate data structures
;
DC		DeviceContext	<>
IC		InfoContext	<>


fPGA		db	FALSE

GrabTextSize	dw	MAX_GBTEXTSIZE + MAX_VISTEXT
GrabGrphSize	dw	MAX_GBGRPHSIZE + MAX_VISGRPH
SaveTextSize	dw	MAX_CDSIZE + MAX_TOTTEXT
SaveGrphSize	dw	MAX_CDSIZE + MAX_TOTGRPH


PhysColorTable	label	word
		db	071h
		db	017h
		db	018h
		db	079h
		db	06Fh
		db	00Fh
		db	070h
		db	00Fh
		db	01Fh


;
; InitScreen - Initialize screen to a known mode for the oldap
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	none
;
InitScreen	proc	far
		push	ax
		mov	ax,00003h
		int	010h
		pop	ax
		ret
InitScreen	endp



;
; DevInit - perform device-specific initializations
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	flags
;
DevInit 	proc	near
		assume	ds:_TEXT
		push	ax
		push	es

		mov	ax,0C600h
		mov	es,ax
		mov	al,es:[0030Bh]
		mov	ah,es:[0070Bh]
		dec	al
		jz	diFoundPga
		dec	ah
		jnz	diNoPga
diFoundPga:
		mov	[fPGA],TRUE
diNoPga:
		pop	es
		pop	ax
		ret
DevInit 	endp


		subttl	GetmodeDev
		page


;
; GetModeDev -
;
; ENTRY
;	ds	=  cs
;
; EXIT
;
; USES
;	all
;
GetModeDev	proc	near
		assume	ds:_TEXT
		push	es
		xor	ax,ax
		mov	es,ax

		cmp	[fPGA],TRUE
		je	gmdNotRogue

		call	AddrLatch

		xor	bh,bh
		mov	bl,es:[CrtMode]
		shl	bx,1
		cmp	ax,[AddrLow][bx]
		jna	gmdRogueMap
		cmp	ax,[AddrHi][bx]
		jb	gmdNotRogue
gmdRogueMap:
		xor	bx,bx
		mov	cx,maplen
gmdNext:
		cmp	[map1][bx],ax
		ja	gmdGotIt
		inc	bx
		inc	bx
		loop	gmdNext

		call	OemBeep
		loop	$
		call	OemBeep
		mov	bl,4
gmdGotIt:
		mov	si,offset DTX
		mov	di,offset DC
		mov	ax,[map2][bx]
		mov	cx,(SIZE DC)
		mul	cl
		add	si,ax

		mov	ax,cs
		mov	es,ax
		shr	cx,1
		if	(SIZE DC) AND 1
		movsb
		endif
		rep	movsw
		jmp	short	gmdGetCurPos
gmdNotRogue:
		mov	al,es:[CrtMode]
		mov	[DC.dcScrMode],al

		mov	ax,es:[CrtStart]
		mov	[DC.dcScrStart],ax

		mov	ax,es:[CursorMode]
		mov	ds:[DC.dcCursorMode],ax

		mov	al,es:[CrtModeSet]
		mov	[DC.dcModeCtl],al

		mov	al,es:[CrtPalette]
		mov	ds:[DC.dcColorSelect],al
gmdGetCurPos:
		mov	al,C_CRSR_LOC_HGH
		mov	dx,CRTC_ADDR
		call	InHiLow
		mov	[DC.dcCursorPosn],ax

		pop	es
		ret
GetModeDev	endp


		subttl	SetModeDev
		page


;
; SetModeDev -
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	ax, bx, cx, dx, flags
;
SetModeDev	proc	near
		assume	ds:_TEXT
		xor	ah,ah
		mov	al,[DC.dcScrMode]
		int	010h

		mov	al,[DC.dcModeCtl]
		and	al,NOT MC_ENABLE_DSP
		mov	dx,MODE_CONTROL
		out	dx,al

		mov	al,[DC.dcColorSelect]
		mov	dl,COLOR_SELECT AND 0FFh
		out	dx,al
		ret
SetModeDev	endp



		subttl	AddrLatch
		page


;
; AddrLatch -
;
; ENTRY
;	none
; EXIT
;	ax	=  regen length
; USES
;	ax, dx, flags
;
AddrLatch	proc	near
		mov	dx,LPEN_CLEAR
		out	dx,al
		mov	dl,INPUT_STATUS AND 0FFh
vert1:
		in	al,dx
		test	al,IS_V_RETRACE
		jz	vert1
vert2:
		in	al,dx
		test	al,IS_V_RETRACE
		jnz	vert2
vert3:
		in	al,dx
		test	al,IS_H_RETRACE
		jnz	vert3

		cli
		mov	dl,LPEN_SET AND 0FFh
		out	dx,al
		sti

		mov	dl,CRTC_ADDR AND 0FFh
		mov	al,C_LGHT_PEN_HGH
		call	InHiLow
		xchg	al,ah
		push	ax

		mov	dl,LPEN_CLEAR AND 0FFh
		out	dx,al
		mov	dl,INPUT_STATUS AND 0FFh
vert9:
		in	al,dx
		test	al,IS_V_RETRACE
		jz	vert9

		cli
		mov	dl,LPEN_SET AND 0FFh
		out	dx,al
		sti

		mov	dl,CRTC_ADDR AND 0FFh
		mov	al,C_LGHT_PEN_HGH
		call	InHiLow
		xchg	al,ah
		pop	dx				;recover 1st addr
		sub	ax,dx				;get difference

		mov	dx,LPEN_CLEAR
		out	dx,al
		ret
AddrLatch	endp


_TEXT		ends
		end

