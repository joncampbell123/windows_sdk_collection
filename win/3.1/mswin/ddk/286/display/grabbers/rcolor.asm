
		page	,132
		%out	RealizeColor
		name	RCOLOR
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	RCOLOR.ASM
;
; DESCRIPTION
;	This file contains the extended grabber subfunction RealizeColor, used
;	to map Winoldap's logical colors into the display adapter's physical
;	colors.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;       1.01    080989   ac     IFDEFed out portions which have changed or are
;                               not needed under windows 3.0 winoldaps.

IFDEF	GRAB_VER_2

_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include grabber.inc
		.list


		extrn	PhysColorTable:byte
		public	RealizeColor
		subttl	RealizeColor


;
; RealizeColor - map logical color to physical color
;
;	Winoldap works internally with a set of device-independent logical
;	colors for all menuing operations.  Since the PutBlock function
;	operates on attributes at a device level, these logical colors, which
;	are defined in GRABBER.INC, must be mapped to physical colors for the
;	display device in use.
;
;	These logical "colors" do not really represent points in an orthogonal
;	RGB or HSB color space; rather they are simply members of a limited
;	set of attributes which Winoldap might use to display text.  For
;	instance, a request to map logical color number 2 (LC_GRAYED) is
;	simply asking, "Give me a foreground/background attribute pair that
;	would give the impression that a text string displayed with that pair
;	would appear to be disabled, grayed, or otherwise uninteresting with
;	respect to the colors you have chosen to give me for my other
;	RealizeColor requests."
;
;	For all grabbers written to date, this has been easily implementable
;	for both color and monochrome via a statically addressed look-up
;	table.	In some cases (Multimode) it has been desirable to have one
;	more level of indirection so that multiple translate tables could be
;	maintained with a simple change of pointer.  For now however, dynamic
;	patching of the table entries has been sufficient to handle
;	color/monochrome conversions.
;
;	One problem that has persisted on the EGA occurs when an oldap changes
;	the palette.  In such cases, it is not worthwhile to attempt to work
;	with the new palette entries via some complex mapping function.  A
;	grabber entrypoint called BeginPaint has been provided that will
;	restore the default palette, among other things, so that calls to
;	RealizeColor return usable values.  Depending on how the oldap
;	originally programmed the palette, it will be restored during the
;	EndPaint call.
;
; SEE ALSO
;	PutBlock, BeginPaint, EndPaint, GRABBER.INC
;
; ENTRY
;	bh	=  logical color id to be mapped
;	ds	=  cs
; EXIT
;	bh	=  mapped physical color
;	cf	=  0
;
; ERROR EXIT
;	bx	=  0
;	cf	=  1 (implies logical color out of range)
; USES
;	ax, bx, flags
;
RealizeColor	proc	near
		assume	ds:_TEXT
		cmp	bh,MAX_LC_COLOR 	;if color index too hi,
		ja	rcErr			;  leave with error

		mov	al,bh			;prepare for xlat
		mov	bx,offset PhysColorTable
		xlat
		mov	bh,al			;return color in bh
		clc
		ret
rcErr:
		xor	bx,bx			;error exit
		stc
		ret
RealizeColor	endp


_TEXT		ends
		end


ENDIF	;GRAB_VER_2
