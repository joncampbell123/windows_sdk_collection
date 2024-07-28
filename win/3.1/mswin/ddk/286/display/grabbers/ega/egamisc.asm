
		page	,132
		%out	EGA Grabber Support Routines
		name	EGAMISC
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; HISTORY
;	1.01	091787	jhc	Changed WaitVRetrace to not cli; the vertical
;				retrace interval is long enough that we don't
;				need this.
;


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include abs0.inc
		include dc.inc
		include ega.inc
		include fileio.inc
		.list

		extrn	DC:byte
		extrn	fGotSwapPath:byte
		extrn	AX2Hex:near

		public	BrstDet
		public	WaitVRetrace
		public	ScreenOn
		public	ScreenOff
		public	MakeBase
		public	MakeTempFile
		public	StrLen


FALSE		=	0
TRUE		=	1
RETRY_COUNT	=	3

HexDigits	db	'0123456789ABCDEF'


		subttl	BrstDet
		page


;
; BrstDet - determine number of scan lines for raster
;
;	BrstDet, similar to the IBM function by the same name, determines
;	whether the current raster should be 200 or 350 scanlines based on
;	the switch settings on the rear of the EGA card.  In a nutshell,
;	switch settings 0011 or 1001 indicate 350 lines, otherwise 200 lines.
;
; ENTRY
;	none
; EXIT
;	nc	=  200 scanlines
;	cy	=  350 scanlines
; USES
;	flags
;
BrstDet 	proc	near
		push	ax
		push	ds
		xor	ax,ax
		mov	ds,ax			;ds = 0
		assume	ds:Abs0
		mov	al,[Info3]		;get feature and switch info
		and	al,00001111b		;mask for switches
		cmp	al,00001001b		;most common config
		je	bd350

		cmp	al,00000011b		;less common
		je	bd350

		stc				;else set carry, cmc will clear
bd350:
		cmc				;toggle carry
		pop	ds
		pop	ax
		ret
BrstDet 	endp


		subttl	WaitVRetrace
		page


;
; WaitVRetrace - do just what it says
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	ax, dx, flags
;
WaitVRetrace	proc	near
		mov	dx,INPUT_STATUS_1 AND 0FF0Fh
		or	dl,cs:[DC.dcAddrPatch]
wvrInSync:
		in	al,dx
		test	al,00001000b
		jnz	wvrInSync
wvrOutSync:
		in	al,dx
		test	al,00001000b
		jz	wvrOutSync
		ret
WaitVRetrace	endp



		subttl	ScreenOn
		page


;
; ScreenOn - turn on video
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	ax, dx, flags
;
ScreenOn	proc	near
		mov	dx,INPUT_STATUS_1 AND 0FF0Fh
		or	dl,cs:[DC.dcAddrPatch]
		in	al,dx

		push	dx
		mov	dl,ATTR_ADDR AND 0FFh
		mov	al,020h 		 ;re-enable video
		out	dx,al
		pop	dx

		in	al,dx
		ret
ScreenOn	endp


		subttl	ScreenOff
		page


;
; ScreenOff - turn off video
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	ax, dx, flags
;
ScreenOff	proc	near
		mov	dx,INPUT_STATUS_1 AND 0FF0Fh
		or	dl,cs:[DC.dcAddrPatch]
		in	al,dx

		push	dx
		mov	dl,ATTR_ADDR AND 0FFh
		xor	al,al			;turn off video
		out	dx,al
		pop	dx

		in	al,dx
		ret
ScreenOff	endp


		subttl	MakeBase
		page


;
; MakeBase - find proper video params for given mode
;
;	MakeBase, similar to the IBM function by the same name, will return
;	a pointer to the correct table of video parameters to use when
;	initializing the EGA for a given mode.	The root of the list of tables
;	is derived from the ParmPtr in the SavePtr table.
;
; ENTRY
;	ah	=  video mode
;	ds	=  0
; EXIT
;	es:si	-> base of correct parameter table
; USES
;	ax, si, es, flags
;
MakeBase	proc	near
		assume	ds:Abs0 		;ds must be = 0
		les	si,[lpSavePtr]		;load up SavePtr
		les	si,es:[si]		;load up ParmPtr
		test	[Info],01100000b	;if 64K video memory,
		jz	mb64K			;  skip special graphics tests

		add	si,00440h		;bump to alt 640x350x1
		cmp	ah,00Fh 		;if this is what we want,
		je	mbX			;  we are done

		add	si,00040h		;bump to alt 640x350x4
		cmp	ah,010h 		;if this is what we want,
		je	mbX			;  we are done

		sub	si,00480h		;nope, not special graphics
mb64K:
		cmp	ah,003h 		;if not alpha,
		ja	mbNoAdjust		;  skip special alpha tests

		call	BrstDet 		;if not enhanced config,
		jnc	mbNoAdjust		;  no adjustment needed

		add	si,004C0h		;bump to enhanced alpha parms
mbNoAdjust:
		xor	al,al			;now use mode as final index
		shr	ax,1			;funky math does the job
		shr	ax,1
		add	si,ax
mbX:
		ret				;es:si -> correct table
MakeBase	endp


		subttl	MakeTempFile
		page


;
; MakeTempFile - build and open a Windows compatible temporary filename
;
; ENTRY
;	ds:si	-> numeric part of filename to patch for randomness
;	ds:dx	-> beginning of pathname
; EXIT
;	nc	=  success
;		bx	=  handle to new file
;	cy	=  failure
;		ax	=  error code
;
; USES
;	ax, bx, cx, flags
;
MakeTempFile	proc	near
		assume	ds:_TEXT
		cmp	[fGotSwapPath],TRUE		;if SetSwapDrive got called,
		je	mtfGo				;  proceed
		stc
		ret
mtfGo:
		mov	cx,RETRY_COUNT
mtfChkOpen:
		push	cx
		push	dx
		mov	ah,02Ch
		int	021h				;get time
		mov	ax,dx				;ax=minutes:seconds
		mov	bx,offset HexDigits
		call	AX2Hex				;patch fname for randomness
		pop	dx
		pop	cx
		mov	al,FAC_READWRITE		;file access code
		OpenFile				;if file doesn't exist,
		jc	mtfCreate			;  we may create it

		mov	bx,ax
		CloseFile				;else close it and
		loop	mtfChkOpen			;  try another
		stc
		ret
mtfCreate:
		mov	cx,FA_NORMAL			;file attribute
		CreateFile
		xchg	ax,bx
		ret
MakeTempFile	endp


		subttl	StrLen
		page


;
; StrLen - get length of string
;
; ENTRY
;	es:di	-> ASCIIZ string
; EXIT
;	cx	=  length of string in bytes, not including NULL at end
; USES
;	cx, flags
;
StrLen		proc	near
		cld
		push	ax
		push	di
		xor	al,al
		mov	cx,0FFFFh
		repne	scasb
		inc	cx
		inc	cx
		neg	cx
		pop	di
		pop	ax
		ret
StrLen		endp


_TEXT		ends
		end

