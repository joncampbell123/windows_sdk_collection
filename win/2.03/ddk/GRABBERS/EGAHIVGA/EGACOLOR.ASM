
		page	,132
		%out	EgaColor
		name	EgaCOLOR
		title	Copyright (C) by Microsoft Inc. 1985-1987


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

		public	PhysColorTable
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
MAX_VISGRPH	=	16*1024

MAX_TOTTEXT	=	80*43*2 + 256
MAX_TOTGRPH	=	16*1024


;
; Allocate data structures
;
DC		DeviceContext	<>
IC		InfoContext	<DI_EGA,,2400,1750,,,,,,,,,,,,,>


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


_TEXT		ends
		end

