
		page	,132
		%out	EgaMono
		name	EgaMono
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


NO_STDGRABENTRY =	1

		.xlist
		include dc.inc
		include ic.inc
		include ega.inc
		include oem.inc
		include abs0.inc
		include grabber.inc
		.list


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


;
; Define equates
;
MAX_GBTEXTSIZE	=	gbWidth
MAX_GBGRPHSIZE	=	gbBits

MAX_CDSIZE0	=	(SIZE DeviceContext) + (SIZE InfoContext)
MAX_CDSIZE1	=	(SIZE VideoBiosData) + (SIZE EgaBiosData) + 4
MAX_CDSIZE2	=	(SIZE HpBiosData)
MAX_CDSIZE	=	MAX_CDSIZE0 + MAX_CDSIZE1 + MAX_CDSIZE2

MAX_VISTEXT	=	80*43 + 02*43 + 2
MAX_VISGRPH	=	32*1024

MAX_TOTTEXT	=	80*43*2 + 256
MAX_TOTGRPH	=	32*1024


;
; Allocate data structures
;
DC		DeviceContext	<>
IC		InfoContext	<DI_EGA,,2210,1450,,,,,,,,,,,,,>


GrabTextSize	dw	MAX_GBTEXTSIZE + MAX_VISTEXT
GrabGrphSize	dw	MAX_GBGRPHSIZE + MAX_VISGRPH
SaveTextSize	dw	MAX_CDSIZE + MAX_TOTTEXT
SaveGrphSize	dw	MAX_CDSIZE + MAX_TOTGRPH


IFDEF	GRAB_VER_2

PhysColorTable	label	word
		db	070h
		db	00Fh
		db	007h
		db	079h
		db	070h
		db	00Fh
		db	070h
		db	00Fh
		db	009h

ENDIF	;GRAB_VER_2

;
; InitScreen - Initialize screen to a known mode for the oldap
;
; ENTRY
;	ds	=  cs
;	ax	=  start up no of screen lines
; EXIT
;	none
; USES
;	none
;
InitScreen	proc	far
		push	ax

		mov	ax,00007h
		int	010h

		pop	ax

; now set the number of lines per mode.

		push	cx
		push	ax			;save
		push	bx			;save
		cmp	ax,43			;43 line mode requested
		jae	Set43LineMode		;yes.
		mov	cx,0B0Ch		;already in right font
		jmp	short InitCursor

Set43LineMode:
	       
		mov	ax,1112h	        ;43 line mode (8x8 fonts)
		mov	cx,070Ch		;position of cursor in 43 lines

Set25or43LineMode:

		xor	bx,bx			;bank 0
		int	10h			;set number of lines

InitCursor:

		mov	ax,0100h		;set cursor call
		int	10h
		pop	bx			;restore
		pop	ax			;restore
		pop	cx

		ret
InitScreen	endp


_TEXT		ends
		end

