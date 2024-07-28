
		page	,132
		%out	Begin/End Paint
		name	BEPAINT
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	BEPAINT.ASM
;
; DESCRIPTION
;	This file contains the code for the extended subfunction
;	entrypoints BeginPaint and EndPaint.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;	1.01	081387	jhc	Changed word outs to attribute controller to
;				byte outs...word outs to port 03C0h do not
;				work on VGA, causing whole background to
;				turn a rich shade of blue.
;
;	1.02	091787	jhc	Removed sti instructions since WaitVRetrace no
;				longer does cli.
;	1.10	071588	vvr	Added VGA support. Read directly from VGA



_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include dc.inc
		include vga.inc
		.list

		extrn	DC:byte
		
		extrn	GetMode:near
		extrn	OutHiLow:near
		extrn	MakeBase:near
		extrn	WaitVRetrace:near
		extrn	GetRegSetAttr:near			
		
		public	BeginPaint
		public	EndPaint


FALSE		=	0
TRUE		=	1
REG_COUNT	=	16			;# of VGA palette regs to save

fInPaint	db	0			;nesting count
OldCursorPosn	dw	0			;storage for cursor position
OldPaletteRegs	db	REG_COUNT dup (?)	;storage for EGA palette
OldColorTab	db	8 (?)			;color table values


		subttl	BeginPaint


;
; BeginPaint - prepare for Winoldap to call Get/Put/Mark functions
;
;	BeginPaint is somewhat of a "catch-all" routine that performs a number
;	loosely related functions, that together help Winoldap maintain
;	consistency and speed in menuing operations.
;
;	The Get/Put/MarkBlock functions do not attempt to validate the
;	grabber's DC and IC information structures each time they are called,
;	mainly for performance reasons (it may take hundreds of milliseconds
;	just to validate these structures, especially on the CGAHERC-type of
;	displays where the vertical retrace signal is sampled).  The
;	application may have changed screen modes or color palettes since the
;	last time Winoldap had control of the processor; if these structures
;	do not contain data that reflect the current state of the video
;	subsystem, the results of the block operations will be undefined.
;	Thus, it is crucial that these structures are validated just prior to
;	calling the block functions.  Since BeginPaint ALWAYS validates these
;	structures, proper operation of the block routines can be insured by
;	bracketing calls to them with calls to BeginPaint and EndPaint.
;
;	BeginPaint also allows us to undo any weirdness the oldap may have
;	done to the adapter that might impair the menu displays.
;	Specifically, no attempt has been made by the RealizeColor routine to
;	dynamically track changes in a reprogrammable palette.	Since
;	RealizeColor performs the logical to physical color mapping based on
;	the default palette, BeginPaint will always switch the subsystem to
;	the default.  If the palette had been reprogrammed to non-standard
;	values by the oldap, the effects of this call will be evident as a
;	sudden change of color on the oldap screen.  However, the menus will
;	display as expected.  (Currently, the EGA-style adapters are the only
;	supported subsystems with this reprogrammable capability)
;
;	Finally, Windoldap provides its own software cursors during menuing
;	operations.  Thus a stray blinking hardware cursor is annoying and
;	confusing, so BeginPaint hides it by sending it to an offscreen
;	location.
;
;	BeginPaint attempts to save the current palette and cursor positions
;	so that EndPaint can restore them.  However, the save area is static,
;	making BeginPaint non-reentrant.  Until this is implemented with a
;	stack, a nesting count is maintained that prevents clobbering data on
;	nesting levels greater than 1.
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
		jnc	@F			;get mode fails (unknown mode)
		pop	dx			;@#@#@#@#@
		pop	bx
		ret	
@@:
		mov	ax,[DC.dcCursorPosn]	;save current cursor
		mov	[OldCursorPosn],ax

		mov	al,C_CRSR_LOC_HGH
		mov	bx,0FFFFh
		mov	dx,CRTC_ADDR AND 0FF0Fh
		or	dl,[DC.dcAddrPatch]
		call	OutHiLow		;blow away cursor
bpDoPalette:
		push	cx
		push	di
		push	si
		push	es

		mov	ax,cs
		mov	es,ax			;es = cs

bpColorMode:		

		call	WaitVRetrace
		mov	di,offset OldPaletteRegs
		mov	cx,00010h
		mov	dx,ATTR_ADDR
		cli
		call	GetRegSetAttr
		sti
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
		out	dx,al
		xchg	al,ah
		out	dx,al
		xchg	al,ah

		inc	bx
		inc	ax
		loop	bpAttrLoop

		mov	al,00100000b		;re-enable video
		out	dx,al

		mov	dl,INPUT_STATUS_1 AND 00Fh
		or	dl,[DC.dcAddrPatch]
		in	al,dx			;reset attr flip-flop

		pop	es
		pop	si
		pop	di
		pop	cx
bpX:
		pop	dx
		pop	bx
		ret
BeginPaint	endp


		subttl	EndPaint
		page


;
; EndPaint - cleanup after Winoldap is through painting
;
;	EndPaint rectifies the action of BeginPaint.  EndPaint attempts to
;	restore the screen to its pre-menuing state.  This includes restoring
;	the original cursor position as well as the original palette (on EGA
;	systems only).
;
;	Note that if the app has written directly to regs which we can't read,
;	it will be impossible to guarantee the screen will look the same.  The
;	cursor position is readable on all PC video adapters supported by
;	Windows, so it is always possible to restore its location.  The EGA
;	palette is much more difficult; it can only be restored if:  1)
;	EGA.SYS was installed, and 2) the oldap reprogrammed the palette via
;	int 010h or the EGA Register Interface provided by EGA.SYS.
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

		dec	[fInPaint]		;if nest level = 0,
		jz	epDoCursor		;  we may proceed
		ret				;else leave
epDoCursor:
		push	bx
		push	dx

		mov	al,C_CRSR_LOC_HGH	;restore old cursor position
		mov	bx,[OldCursorPosn]
		mov	dx,CRTC_ADDR AND 0FF0Fh
		or	dl,[DC.dcAddrPatch]
		call	OutHiLow

epDoPalette:
		push	es
		push	cx
		mov	ax,cs
		mov	es,ax

		xor	bx,bx
		xor 	cx,cx
		mov	cl,REG_COUNT
		call	WaitVRetrace		;prevent glitches
		xor	ax,ax
		mov	dx,ATTR_ADDR
epAttrLoop:
		mov	ah,[OldPaletteRegs][bx]		;blast palette regs
		out	dx,al
		xchg	al,ah
		out	dx,al
		xchg	al,ah

		inc	bx
		inc	ax
		loop	epAttrLoop

		mov	dx,ATTR_ADDR 
		mov	al,00100000b
		out	dx,al

		mov	dl,INPUT_STATUS_1 AND 00Fh
		or	dl,[DC.dcAddrPatch]
		in	al,dx			;reset attr flip-flop

		pop	cx
		pop	es
epX:
		pop	dx
		pop	bx
		ret
EndPaint	endp


_TEXT		ends
		end

