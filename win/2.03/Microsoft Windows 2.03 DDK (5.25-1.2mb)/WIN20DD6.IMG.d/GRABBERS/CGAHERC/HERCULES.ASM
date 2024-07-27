
		page	,132
		%out	Hercules
		name	HERCULES
		title	Copyright (C) by Microsoft Inc. 1985-1987


;
; HISTORY
;	1.01	082187	*** *******	Added MemChk and finished GetGrphPage.
;				Changed attribute in PhysColorTable to
;				display mnemonics with underlines.
;				Changed GetModeDev to get cursor shape from
;				BIOS rather that reseting it to default.
;
;	1.02	091587	*** *******	Fixed SetModeDev to keep video off to prevent
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
MAX_VISGRPH	=	32*1024

MAX_TOTTEXT	=	04*1024
MAX_TOTGRPH	=	32*1024

GRPH_THRESHOLD	=	3000
WIDTHBYTES	=	90
SCANADDR	=	01E96h
SCANCOUNT	=	1


DTX		label	byte
DT0		DeviceContext	<0,0,0,00B0Ch,028h,003h,000h,TextTable,0>
DT1		DeviceContext	<1,0,0,00000h,00Ah,003h,000h,GrphTable,0>
DT2		DeviceContext	<2,0,0,00000h,08Ah,003h,000h,GrphTable,0>


;
; Allocate data structures
;
DC		DeviceContext	<>
IC		InfoContext	<>


GrabTextSize	dw	MAX_GBTEXTSIZE + MAX_VISTEXT
GrabGrphSize	dw	MAX_GBGRPHSIZE + MAX_VISGRPH
SaveTextSize	dw	MAX_CDSIZE + MAX_TOTTEXT
SaveGrphSize	dw	MAX_CDSIZE + MAX_TOTGRPH

EmcMask 	db	?
Scan0Page0	db	SCANCOUNT*WIDTHBYTES	  dup (?)
Scan0Page1	db	SCANCOUNT*WIDTHBYTES	  dup (?)


PhysColorTable	label	word
		db	070h
		db	00Fh
		db	007h
		db	070h
		db	070h
		db	00Fh
		db	070h
		db	00Fh
		db	009h


TextTable	label	byte
		db	061h			;R0 - horizontal total
		db	050h			;R1 - horizontal displayed
		db	052h			;R2 - horizontal sync position
		db	00Fh			;R3 - horizontal sync width
		db	019h			;R4 - vertical total
		db	006h			;R5 - vertical total adjust
		db	019h			;R6 - vertical displayed
		db	019h			;R7 - vertical sync position
		db	002h			;R8 - interlace mode & skew
		db	00Dh			;R9 - max scan line address
		db	00Bh			;RA - cursor start scanline
		db	00Ch			;RB - cursor end scanline

GrphTable	label	byte
		db	035h			;R0 - horizontal total
		db	02Dh			;R1 - horizontal displayed
		db	02Eh			;R2 - horizontal sync position
		db	007h			;R3 - horizontal sync width
		db	05Bh			;R4 - vertical total
		db	002h			;R5 - vertical total adjust
		db	057h			;R6 - vertical displayed
		db	057h			;R7 - vertical sync position
		db	002h			;R8 - interlace mode & skew
		db	003h			;R9 - max scan line address
		db	000h			;RA - cursor start scanline
		db	000h			;RB - cursor end scanline
ParmLen 	=	$ - GrphTable


;
; InitScreen - Initialize screen to a known mode for the oldap
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	ds
;
InitScreen	proc	far
		push	ax
		push	cx
		mov	ax,00007h
		int	010h
		xor	cx,cx
		loop	$
		mov	ds,cx			;082787 XT's int 010h stuffs
		assume ds:Abs0			;082787   a 00607h here which
		mov	[CursorMode],00B0Ch	;082787   is WRONG, WRONG, WRONG
		pop	cx
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
		mov	ax,cs
		mov	es,ax

		call	MemChk			;check if page 1 is addressable
		mov	[EmcMask],bl		;save emc mask for later

		call	AddrLatch
		xor	bx,bx			;(text page implies bl = 0)
		cmp	ax,GRPH_THRESHOLD	;if not above graphics threshold,
		jb	gmdGotMode		;   we know its text mode

		inc	bx				;(grph page 0 implies bl = 1)
		test	[EmcMask],EMC_ALLOW_PAGE1	;if upper 32k is off
		jz	gmdGotMode			;  we know its page 0 graphics

		call	GetGrphPage		;else we must do hard way
		inc	bx			;make bx 1-based
gmdGotMode:
		mov	al,bl
		mov	cx,(SIZE DC)
		mul	cl
		mov	si,offset DTX
		mov	di,offset DC
		add	si,ax

		shr	cx,1
		if	(SIZE DC) AND 1
		movsb
		endif
		rep	movsw

		xor	ax,ax			;081887
		mov	es,ax			;081887
		mov	ax,es:[CursorMode]	;081887
		mov	[DC.dcCursorMode],ax	;081887

		mov	al,[EmcMask]		;082087
		and	[DC.dcExModeCtl],al	;082087

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
		push	si

		xor	ax,ax
		mov	cx,ParmLen
		mov	dx,CRTC_ADDR
		mov	si,[DC.dcCrtcParms]
smdProgramCrtc:
		mov	ah,[si]
		out	dx,ax
		inc	ax
		inc	si
		loop	smdProgramCrtc

		mov	al,[DC.dcModeCtl]
		and	al,NOT MC_ENABLE_DSP
		mov	dl,MODE_CONTROL AND 0FFh
		out	dx,al

		mov	al,[DC.dcExModeCtl]
		mov	dl,EX_MODE_CONTROL AND 0FFh
		out	dx,al

		pop	si
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
		cli
al1:
		in	al,dx
		test	al,IS_V_RETRACE
		jz	al1
al2:
		in	al,dx
		test	al,IS_V_RETRACE
		jnz	al2
al3:
		in	al,dx
		test	al,IS_V_RETRACE
		jz	al3

		mov	dl,LPEN_SET AND 0FFh
		out	dx,al
		sti

		mov	dl,CRTC_ADDR AND 0FFh
		mov	al,C_LGHT_PEN_HGH
		call	InHiLow
		xchg	al,ah

		mov	dx,LPEN_CLEAR
		out	dx,al
		ret
AddrLatch	endp


;
; MemChk - determines if Hercules has upper 32K of memory at 0B800h enabled
;	   which implies that bit 1 of the extended mode control register
;	   should be set.
;
; ENTRY
;	none
; EXIT
;	bl	=  Extended Mode Control mask
; USES
;	ax, bx, flags
;
MemChk		proc	near
		assume	ds:_TEXT
		push	es
		mov	bx,0FFFFh
		mov	ax,0B000h
		mov	es,ax
		mov	al,es:[bx]		;save this byte
		mov	byte ptr es:[bx],ah	;write out test byte
		cmp	byte ptr es:[bx],ah	;read it back in
		mov	es:[bx],al		;restore byte clobbered
		pop	es
		je	mcX			;if byte read != byte written,
		and	bl,NOT EMC_ALLOW_PAGE1	;  page 1 is not addressable
mcX:
		ret
MemChk		endp



;
; GetGrphPage - figures out which graphics page Hercules is currently
;		displaying.
;
; ENTRY
;	none
; EXIT
;	bl	=  graphics page number (0 or 1)
; USES
;	all but bp and seg regs
;
GetGrphPage	proc	near
		assume	ds:_TEXT
		push	ds
		push	es

		mov	ax,cs
		mov	es,ax				;es = cs
		mov	ax,0B000h
		mov	ds,ax				;ds = 0B000h
		assume	ds:nothing
		mov	bx,SCANCOUNT*WIDTHBYTES/2	;makes reloading cx easy

		mov	si,SCANADDR		;save last scan, page 0
		mov	di,offset Scan0Page0
		mov	cx,bx
		rep	movsw

		mov	si,SCANADDR + 08000h	;save last scan, page 1
		mov	cx,bx
		rep	movsw

		mov	es,ax			;es = 0B000h
		xor	ax,ax			;stash 0's
		mov	di,SCANADDR		;in last scan, page 0
		mov	cx,bx
		rep	stosw

		dec	ax			;stash 0FFFFh's
		mov	di,SCANADDR + 08000h	;in last scan, page 1
		mov	cx,bx
		shl	cx,1
		rep	stosw
		mov	cx,bx
		shl	cx,1
		rep	stosw

		mov	dx,CRTC_ADDR
		mov	ax,05806h
		out	dx,ax

		xor	ax,ax
		mov	cx,IS_V_RETRACE*256 + IS_H_RETRACE
		mov	dx,INPUT_STATUS
		cli
ggp1:						;wait for end of vsync
		in	al,dx
		test	al,ch
		jz	ggp1
ggp2:						;wait for vsync
		in	al,dx
		test	al,ch
		jnz	ggp2
ggp3:						;take samples during last scan
		in	al,dx
		or	ah,al
		test	al,cl
		jz	ggp3

		sti
		push	ax
		mov	dx,CRTC_ADDR
		mov	ax,05706h
		out	dx,ax

		mov	ax,cs
		mov	ds,ax			;ds = cs
		mov	si,offset Scan0Page0
		mov	di,SCANADDR		;restore last scan, page 0
		mov	cx,bx
		rep	movsw

		mov	di,SCANADDR + 08000h	;restore last scan, page 1
		mov	cx,bx
		rep	movsw

		pop	bx
		xor	bl,bl
		test	bh,IS_DOTS
		jz	ggpX
		inc	bl
ggpX:
		pop	es
		pop	ds
		ret
GetGrphPage	endp


_TEXT		ends
		end

