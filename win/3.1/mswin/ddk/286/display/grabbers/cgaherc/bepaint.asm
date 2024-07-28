
		page	,132
		%out	Begin/End Paint
		name	BEPAINT
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include dc.inc
		include cgaherc.inc
		.list

		extrn	DC:byte
		extrn	GetMode:near
		extrn	OutHiLow:near

		public	BeginPaint
		public	EndPaint


fInPaint	db	0
OldExModeCtl	db	0
OldCursorPosn	dw	0


		subttl	BeginPaint


;
; BeginPaint - prepare for Winoldap to call Get/Put/Mark functions
;
;	BeginPaint is called by Winoldap prior to calling any of the
;	Get/Put/Mark functions.  This gives the grabber the chance to validate
;	its internal data structures (IC, DC) in preparation for screen i/o.
;	It also allows us to undo any weirdness the oldap may have done to the
;	adapter that might impair the menu displays in addition to hiding the
;	cursor.
;
;	Since the save areas are static, and since Begin/EndPaint calls may be
;	nested, a reference count is maintained to prevent re-entrancy.
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	ax, flags
;
BeginPaint	proc	near
		assume	ds:_TEXT
		inc	[fInPaint]
		cmp	[fInPaint],1
		je	bpDoCursor
		ret
bpDoCursor:
		push	bx
		push	dx

		call	GetMode
		mov	ax,[DC.dcCursorPosn]	;save current cursor
		mov	[OldCursorPosn],ax

		mov	al,C_CRSR_LOC_HGH
		mov	bx,0FFFFh
		mov	dx,CRTC_ADDR
		call	OutHiLow		;blow away cursor

		ifdef	MULTIMODE
		mov	al,[DC.dcExModeCtl]
		mov	[OldExModeCtl],al
		and	al,11111001b
		mov	dl,EX_MODE_CONTROL AND 0FFh
		out	dx,al
		endif
bpX:
		pop	dx
		pop	bx
		ret
BeginPaint	endp


		subttl	EndPaint
		page


;
; EndPaint - cleanup after Winoldap is thru painting
;
;	EndPaint rectifies the action of BeginPaint.  EndPaint should attempt
;	to restore the screen to its pre-menuing state.  Note that if the app
;	has written directly to regs which we can't read, it will be impossible
;	to guarantee the screen will look the same.
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	ax, flags
;
EndPaint	proc	near
		assume	ds:_TEXT
		dec	[fInPaint]
		jz	epDoCursor
		ret
epDoCursor:
		push	bx
		push	dx

		mov	al,C_CRSR_LOC_HGH
		mov	bx,[OldCursorPosn]
		mov	dx,CRTC_ADDR
		call	OutHiLow

		ifdef	MULTIMODE
		mov	al,[OldExModeCtl]
		mov	dl,EX_MODE_CONTROL AND 0FFh
		out	dx,al
		endif
epX:
		pop	dx
		pop	bx
		ret
EndPaint	endp


_TEXT		ends
		end

