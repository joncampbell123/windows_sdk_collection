
		page	,132
		%out	Save/RestScreen
		name	SRSCR
		title	Copyright (C) by Microsoft Inc. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


NO_STDGRABENTRY =	1

		.xlist
		include dc.inc
		include ic.inc
		include eri.inc
		include ega.inc
		include abs0.inc
		include oem.inc
		include grabber.inc
		.list

		extrn	MAX_TOTGRPH:abs
		extrn	MAX_CDSIZE:abs

		extrn	DC:byte
		extrn	IC:byte
		extrn	fVectra:byte
		extrn	fEriHooks:byte
		extrn	SaveTextSize:word
		extrn	SaveGrphSize:word
		extrn	EriContextSize:word

		extrn	GetHpVideoSeg:near
		extrn	ScreenOn:near
		extrn	ScreenOff:near
		extrn	OemBeep:near
		extrn	GetMode:near
		extrn	SetMode:near
		extrn	SaveFontFile:near
		extrn	RestFontFile:near

		ifdef	EGACOLOR
		extrn	SaveScreenFile:near
		extrn	RestScreenFile:near
		endif

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
		add	dx,EriContextSize

		ifdef	EGACOLOR
		test	[IC.icScrType],ST_LARGE
		jnz	ssNoAdd
		endif

		add	dx,[IC.icScrLen]
ssNoAdd:
		cmp	ax,dx
		jae	ssSaveFontFile
		jmp	ssErr
ssSaveFontFile:
		cmp	[fEriHooks],TRUE
		jne	ssSaveScreenFile

		call	SaveFontFile
		jc	ssErr
ssSaveScreenFile:
		ifdef	EGACOLOR
		test	[IC.icScrType],ST_LARGE
		jz	ssSaveDcIc

		call	SaveScreenFile
		jc	ssErr
		endif
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

		mov	si,offset Rows
		mov	cx,(SIZE EgaBiosData)/2
		if	(SIZE EgaBiosData) AND 1
		movsb
		endif
		rep	movsw

		mov	si,offset lpSavePtr
		movsw
		movsw

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
		ifdef	EGACOLOR
		test	cs:[IC.icScrType],ST_LARGE
		jnz	ssSaveEgaRegs
		endif

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
ssSaveEgaRegs:
		cmp	cs:[fEriHooks],TRUE
		jne	ssX

		mov	bx,di
		mov	ax,ERI_CONTEXTINFO*256 + ERI_CI_SAVE
		int	010h
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
rsRestFontFile:
		cmp	cs:[fEriHooks],TRUE
		jne	rsRestScreenFile

		call	RestFontFile
rsRestScreenFile:
		call	ScreenOff
		ifdef	EGACOLOR
		test	cs:[IC.icScrType],ST_LARGE
		jz	rsRestBiosData

		call	RestScreenFile
		jc	rsErr
		endif
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

		mov	di,offset Rows
		mov	cx,(SIZE EgaBiosData)/2
		if	(SIZE EgaBiosData) AND 1
		movsb
		endif
		rep	movsw

		mov	di,offset lpSavePtr
		movsw
		movsw

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
		ifdef	EGACOLOR
		test	cs:[IC.icScrType],ST_LARGE
		jnz	rsRestEgaRegs
		endif

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
rsRestEgaRegs:
		loop	$
		loop	$
		call	ScreenOn
		cmp	cs:[fEriHooks],TRUE
		jne	rsX

		mov	ax,ds
		mov	es,ax
		mov	bx,si
		mov	ax,ERI_CONTEXTINFO*256 + ERI_CI_RESTORE
		int	010h
rsX:
		clc
		ret
rsErr:
		call	OemBeep
		stc
		ret
RestoreScreen	endp


_TEXT		ends
		end

