
		page	,132
		%out	Begin/End Paint
		name	BEPAINT
		title	Copyright (C) by Microsoft Inc. 1985-1987


;
; HISTORY
;	1.01	081387	*** *******	Changed word outs to attribute controller to
;				byte outs...word outs to port 03C0h do not
;				work on VGA, causing whole background to
;				turn a rich shade of blue.
;
;	1.02	091787	*** *******	Removed sti instructions since WaitVRetrace no
;				longer does cli.
;


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include dc.inc
		include eri.inc
		include ega.inc
		.list

		extrn	DC:byte
		extrn	fEriHooks:byte

		extrn	GetMode:near
		extrn	OutHiLow:near
		extrn	MakeBase:near
		extrn	WaitVRetrace:near

		public	BeginPaint
		public	EndPaint


FALSE		=	0
TRUE		=	1
REG_COUNT	=	16

fInPaint	db	0
OldCursorPosn	dw	0
OldPaletteRegs	db	REG_COUNT dup (?)


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
		cmp	[fInPaint],1		;if nest level = 1,
		je	bpDoCursor		;  we may proceed
		ret
bpDoCursor:
		push	bx
		push	dx

		call	GetMode
		mov	ax,[DC.dcCursorPosn]	;save current cursor
		mov	[OldCursorPosn],ax

		mov	al,C_CRSR_LOC_HGH
		mov	bx,0FFFFh
		mov	dx,CRTC_ADDR AND 0FF0Fh
		or	dl,[DC.dcAddrPatch]
		call	OutHiLow		;blow away cursor
bpDoPalette:
		push	cx
		push	si
		push	es
		mov	ax,cs
		mov	es,ax			;es = cs

		cmp	[fEriHooks],TRUE	;if ERI not installed,
		jne	bpDoAttr		;  don't try to save palette

		mov	ah,ERI_READRANGE
		mov	bx,offset OldPaletteRegs
		xor	cx,cx
		mov	cl,REG_COUNT
		mov	dx,ERI_ID_ATTR
		int	010h			;call ERI to save palette regs
bpDoAttr:
		mov	ah,[DC.dcScrMode]	;MakeBase will use mode as
		push	ds			;  an index into the EGA
		xor	bx,bx			;  default parm table
		mov	ds,bx
		call	MakeBase
		pop	ds
		mov	bx,si			;es:bx -> base of parms for given mode
		add	bx,evpAttrRegs
		xor	cx,cx
		mov	cl,REG_COUNT
		call	WaitVRetrace		;this prevents glitching
		xor	al,al
		mov	dx,ATTR_ADDR
bpAttrLoop:
		mov	ah,es:[bx]		;blast to attr regs
;		out	dx,ax			;081387
		out	dx,al			;081387
		xchg	al,ah			;081387
		out	dx,al			;081387
		xchg	al,ah			;081387

		inc	bx
		inc	ax
		loop	bpAttrLoop

		mov	al,00100000b		;re-enable video
		out	dx,al

		mov	dl,INPUT_STATUS_1 AND 00Fh	;reset attr flip-flop
		or	dl,[DC.dcAddrPatch]
		in	al,dx

		pop	es
		pop	si
		pop	cx
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

		mov	al,C_CRSR_LOC_HGH	;restore old cursor position
		mov	bx,[OldCursorPosn]
		mov	dx,CRTC_ADDR AND 0FF0Fh
		or	dl,[DC.dcAddrPatch]
		call	OutHiLow

		cmp	[fEriHooks],TRUE	;if ERI not installed,
		jne	epX			;  we can't restore
epDoPalette:
		push	cx
		push	es
		mov	ax,cs
		mov	es,ax

		xor	bx,bx
		xor	cx,cx
		mov	cl,REG_COUNT
		call	WaitVRetrace		;prevent glitches
		xor	ax,ax
		mov	dx,ATTR_ADDR
epAttrLoop:
		mov	ah,[OldPaletteRegs][bx] ;blast to attr regs
;		out	dx,ax			;081387
		out	dx,al			;081387
		xchg	al,ah			;081387
		out	dx,al			;081387
		xchg	al,ah			;081387

		inc	bx
		inc	ax
		loop	epAttrLoop

		mov	al,00100000b		 ;re-enable video
		out	dx,al

		mov	dl,INPUT_STATUS_1 AND 00Fh
		or	dl,[DC.dcAddrPatch]
		in	al,dx

		pop	es
		pop	cx
epX:
		pop	dx
		pop	bx
		ret
EndPaint	endp


_TEXT		ends
		end

