
		page	,132
		%out	Cga
		name	CGA
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; HISTORY
;	1.01	091587	jhc	Fixed SetModeDev to keep video off to prevent
;				screen 'bounce'.
;	1.02    080989   ac     IFDEFed code that has changed or is no longer
;				needed under windows 3.0 winoldaps.
;       1.03    120590   ac     Put in a hack in the GetMode routine to deal 
;				with a bug on a Zenith portable systems CGA
;				display.
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

IFDEF	GRAB_VER_2
		public	PhysColorTable
ENDIF	;GRAB_VER_2

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


IFDEF	GRAB_VER_2

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

ENDIF	;GRAB_VER_2


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

; what follows is a shade piece of code. The original creator of this code
; did not have any comments. Basically, the code tries to get the length
; of the display area in the current mode using the light pen registers and
; then is trying to guess the mode based on the length and a set of heuristic
; tables. This seems to work on regular CGAs, however, on a Zenith portable
; machine with a CGA display, reading the light pen registers give us some
; strange values. Looking at this piece of code, it seems that we are making
; an assumption that the mode value at 40:49h is correct. To take care of the
; Zenith machine problem, we will continue with the following code till one
; of the mode detail table entries are selected. After this we will try to
; verify whether the mode value in the table matches the one in 40:49h. If
; it does not, we will discard the table entry and assume that the details
; the 40: bios area are valid --- AC.

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

;;		call	OemBeep
;;		loop	$
;;		call	OemBeep


		mov	bl,4
gmdGotIt:
		mov	si,offset DTX
		mov	di,offset DC
		mov	ax,[map2][bx]
		mov	cx,(SIZE DC)
		mul	cl
		add	si,ax

; check to see whether the mode in the table does indeed match the current
; mode or not.

		mov	al,es:[CrtMode]
		cmp	al,[si.dcScrMode]	;does it match ?
		jz	gmdTableEntriesOk	;yes they do.

; the AddressLatch approach did not work. Let's just assume that the BIOS
; data area does really have all the mode information that we need.

		jmp	short gmdNotRogue

gmdTableEntriesOk:

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

