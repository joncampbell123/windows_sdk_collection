
		page	,132
		%out	Save/RestScreen
		name	SRSCR
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; HISTORY
;	1.01	091587	jhc	Fixed RestoreScreen to restore video signal
;				only after a time delay in order to prevent
;				screen 'bounce'.  Also removed an unused
;				error handler.
;


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


NO_STDGRABENTRY =	1

		.xlist
		include dc.inc
		include ic.inc
		include cgaherc.inc
		include abs0.inc
		include oem.inc
		include grabber.inc
		.list

		extrn	MAX_TOTGRPH:abs
		extrn	MAX_CDSIZE:abs

		extrn	DC:byte
		extrn	IC:byte
		extrn	fVectra:byte
		extrn	SaveTextSize:word
		extrn	SaveGrphSize:word

		extrn	GetHpVideoSeg:near
		extrn	OemBeep:near
		extrn	GetMode:near
		extrn	SetMode:near

		public	SaveScreen
		public	RestoreScreen


FALSE		=	0
TRUE		=	1


		subttl	SaveScreen


;
; SaveScreen - save the current display context
;
; ENTRY
;	ax	 =  size in bytes of screen save area
;	ds	 =  cs
;	es:di	 -> screen save area
; EXIT
;	nc	 =  screen was successfully saved
;	cy	 =  unable to save screen
; USES
;	all except bp
;
SaveScreen	proc	far
		assume	ds:_TEXT

		call	Getmode
		mov	[DC.dcfSwitchGmt],FALSE
		cmp	ax,[SaveTextSize]
		je	ssChkAlloc
		mov	[DC.dcfSwitchGmt],TRUE
ssChkAlloc:
		mov	dx,MAX_CDSIZE
		add	dx,[IC.icScrLen]
		cmp	ax,dx
		jae	ssSaveDcIc
		jmp	ssErr
ssSaveDcIc:
		cld
		mov	si,offset DC
		mov	cx,(SIZE DeviceContext + SIZE InfoContext)/2
		if	(SIZE DeviceContext + SIZE InfoContext) AND 1
		movsb
		endif
		rep	movsw
ssSaveBiosData:
		xor	ax,ax
		mov	ds,ax
		assume	ds:Abs0

		mov	si,offset CrtMode
		mov	cx,(SIZE VideoBiosData)/2
		if	(SIZE VideoBiosData) AND 1
		movsb
		endif
		rep	movsw

		cmp	cs:[fVectra],TRUE
		jne	ssSaveScreen0

		call	GetHpVideoSeg
		mov	ds,ax
		assume	ds:nothing
		xor	si,si
		mov	cx,(SIZE HpBiosData)/2
		if	(SIZE HpBiosData) AND 1
		movsb
		endif
		rep	movsw
ssSaveScreen0:
		mov	cx,cs:[IC.icScrLen]
		lds	si,cs:[IC.iclpScr]
		cmp	cs:[DC.dcfSwitchGmt],TRUE
		jne	ssSaveScreen1

		xor	si,si
		mov	cx,MAX_TOTGRPH
ssSaveScreen1:
		shr	cx,1
		rep	movsw
		rcl	cx,1
		rep	movsb
ssX:
		clc
		ret
ssErr:
		call	OemBeep
		stc
		ret
SaveScreen	endp


		subttl	RestoreScreen
		page


;
; RestoreScreen - restore the previously saved display context
;
; ENTRY
;	ax	 =  size in bytes of screen save area
;	ds	 =  cs
;	es:di	 -> screen save area
; EXIT
;	nc	 =  screen was successfully restored
;	cy	 =  unable to restore screen
; USES
;	all except bp
;
RestoreScreen	proc	far
		assume	ds:nothing
		cld
		mov	ax,es			;xchg ds, es
		mov	bx,ds
		mov	es,bx			;now es = cs
		mov	ds,ax			;now ds = es on entry
		mov	si,di
rsRestDcIc:
		mov	di,offset DC
		mov	cx,(SIZE DeviceContext + SIZE InfoContext)/2
		if	(SIZE DeviceContext + SIZE InfoContext) AND 1
		movsb
		endif
		rep	movsw

		call	SetMode
rsRestBiosData:
		xor	ax,ax
		mov	es,ax
		assume	es:Abs0

		mov	di,offset CrtMode
		mov	cx,(SIZE VideoBiosData)/2
		if	(SIZE VideoBiosData) AND 1
		movsb
		endif
		rep	movsw

		cmp	cs:[fVectra],TRUE
		jne	rsRestScreen0

		call	GetHpVideoSeg
		mov	es,ax
		assume	es:nothing
		xor	di,di
		mov	cx,(SIZE HpBiosData)/2
		if	(SIZE HpBiosData) AND 1
		movsb
		endif
		rep	movsw
rsRestScreen0:
		mov	cx,cs:[IC.icScrLen]
		les	di,cs:[IC.iclpScr]
		cmp	cs:[DC.dcfSwitchGmt],TRUE
		jne	rsRestScreen1

		xor	di,di
		mov	cx,MAX_TOTGRPH
rsRestScreen1:
		shr	cx,1
		rep	movsw
		rcl	cx,1
		rep	movsb

		loop	$			;let screen settle before
		loop	$			;final mode set (video on)

		mov	al,cs:[DC.dcModeCtl]
		and	al,NOT MC_ENABLE_BLINK	;kill blinking/turn on video
		mov	dx,MODE_CONTROL
		out	dx,al

		clc
		ret
RestoreScreen	endp


_TEXT		ends
		end

