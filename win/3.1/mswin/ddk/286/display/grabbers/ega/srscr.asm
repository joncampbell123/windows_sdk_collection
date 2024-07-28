
		page	,132
		%out	Save/RestScreen
		name	SRSCR
		title	Copyright (c) Hewlett-Packard Co. 1985-1987

; Added code to check for failure of GetMode

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
		extrn	TempSaveAreaSize:word
		extrn	TempSaveAreaSeg:word
		extrn	TempSaveAreaOff:word

		extrn	GetHpVideoSeg:near
		extrn	ScreenOn:near
		extrn	ScreenOff:near
		extrn	OemBeep:near
		extrn	GetMode:near
		extrn	SetMode:near
		extrn	SaveFontFile:near
		extrn	RestFontFile:near
		extrn	RestBiosFonts:near

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

;----------------------------------------------------------------------------;
; save the segment of the buffer resesrved by winoldap that can be used to   ;
; buffer i/o to/from video buffer. It is not advisable to read or write dire-;
; -ctly from the video buffer as on some machines the i/o could use DMA and  ;
; not all adaptor cards respond to DMA handskaking well (VGA card on PS/2    ;
; Model 50Z does not). The initial part of the buffer will actually hold     ;
; useful information like register save data etc and will not be touched. The;
; work area offset will start after this and the size of the area will appro-;
; -priately be adjusted. Values saved here will also be accessed by RestorSc-;
; -reen as they are expected to be same. This work area will only be used in ;
; hires graphics modes.							     ;
;----------------------------------------------------------------------------;

		mov	TempSaveAreaSeg,es	;save segment of save area
		push	ax			;save
		push	di			;save
		push	cx
		mov	cx,MAX_CDSIZE		;di = size of DC & IC structures
		add	cx,EriContextSize	;di = di + ERI context size
		add	di,cx			;offset past these areas
		mov	TempSaveAreaOff,di	;save resuable area offset
		sub	ax,cx			;modify size
		mov	TempSaveAreaSize,ax	;save size
		pop	cx
		pop	di
		pop	ax

		call	GetMode
		jnc	@F
		jmp	ssErr
@@:	
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

		call	ScreenOff	;disable display
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
		call	ScreenOn	;re-enable the display
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

;----------------------------------------------------------------------------;
; Note: Any custom fonts that might have been saved in a temp disk file must ;
; be restored after the screen has been restored because the code might use  ;
; Winoldap's screen save area and this will destroy any text mode screens    ;
; saved in the same area.						     ;
;----------------------------------------------------------------------------;

rsRestScreenFile:
		call	ScreenOff
		ifdef	EGACOLOR
		test	cs:[IC.icScrType],ST_LARGE
		jz	rsRestBiosData

		call	RestScreenFile
		jnc	@f
		jmp	rsErr
@@:
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

; now restore the BIOS fonts if any were loaded. Custom fonts would be restored
; later.

		call	RestBiosFonts

		cmp	cs:[fEriHooks],TRUE
		jne	rsX

		mov	ax,ds
		mov	es,ax
		mov	bx,si
		mov	ax,ERI_CONTEXTINFO*256 + ERI_CI_RESTORE
		int	010h
 		call	ScreenOn		;turn on video

;----------------------------------------------------------------------------;
; now restore fonts.							     ;
;----------------------------------------------------------------------------;

rsRestFontFile:
		call	RestFontFile
rsX:
		call	ScreenOn

		clc
		ret
rsErr:
		call	OemBeep
		stc
		ret
RestoreScreen	endp


_TEXT		ends
		end

