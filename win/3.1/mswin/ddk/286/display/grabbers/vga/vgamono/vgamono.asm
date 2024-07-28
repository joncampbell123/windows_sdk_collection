
		page	,132
		%out	VgaMono
		name	VgaMONO
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	VGAMONO.ASM
;
; DESCRIPTION
;	This file contains all the low level code and device specific
;	constants used throughout the grabber.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;
;	1.10	071588	vvr	changed MaxGrph and MaxVisGraph sizes
;				also changed no of text rows to be 50 from 43
;

_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


NO_STDGRABENTRY =	1

		.xlist
		include dc.inc
		include vgaic.inc
		include vga.inc
		include oem.inc
		include vgaabs0.inc
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
; Define buffer size equates
;

MAX_GBTEXTSIZE	=	gbWidth
MAX_GBGRPHSIZE	=	gbBits

MAX_CDSIZE0	=	(SIZE DeviceContext) + (SIZE InfoContext)
MAX_CDSIZE1	=	(SIZE VideoBiosData) + (SIZE VgaBiosData) + 4
MAX_CDSIZE2	=	(SIZE HpBiosData)
MAX_CDSIZE	=	MAX_CDSIZE0 + MAX_CDSIZE1 + MAX_CDSIZE2

MAX_VISTEXT	=	80*50 + 02*50 + 2		;changed from 43 to 50 ~vvr
MAX_VISGRPH	=	16*1024 		;should really be 16*1000

MAX_TOTTEXT	=	80*50*2 + 256		;256 takes us to end of page ~vvr
MAX_TOTGRPH	=	16*1024


;
; Allocate and initialize primary data structures
;

DC		DeviceContext	<>
IC		InfoContext	<DI_EGA,,2400,1800,,,,,,,,,,,,,>


;
; Allocate and initialize variables used by InquireGrab and InquireSave
;

GrabTextSize	dw	MAX_GBTEXTSIZE + MAX_VISTEXT
GrabGrphSize	dw	MAX_GBGRPHSIZE + MAX_VISGRPH
SaveTextSize	dw	MAX_CDSIZE     + MAX_TOTTEXT
SaveGrphSize	dw	MAX_CDSIZE     + MAX_TOTGRPH


IFDEF	GRAB_VER_2

PhysColorTable	label	word
;		db	070h		;PC_SELECTED:	    black on white
		db	07Fh		;PC_SELECTED:	    black on white
		db	00Fh		;PC_UNSELECTED:     hi-white on black
		db	007h		;PC_GRAYED:	    white on black
		db	079h		;PC_SELECTGRAY:     black on white
		db	070h		;PC_TITLEBAR:	    black on white
		db	00Fh		;PC_SYSTEMUNSELECT: hi-white on black
		db	070h		;PC_SYSTEMSELECT:   black on white
		db	00Fh		;PC_APPTITLE:	    hi-white on black
;		db	009h		;PC_MNEMONIC:	    underline on black
		db	07Fh		;PC_MNEMONIC:	    underline on black

ENDIF	;GRAB_VER_2

;
; InitScreen - initialize screen to a known mode for the oldap
;
;	This routine will be called by Winoldap once before the oldap starts
;	up.  Winoldap will also call this routine just before it returns
;	control to the oldap after a context switch from Windows back to the
;	oldap.	The latter of these two cases is redundent, since the grabbers
;	RestoreScreen routine will be called shortly thereafter to restore the
;	application's display context.  However, since we cannot discern these
;	two cases, we always set the display to a known mode rvgardless.
;
; ENTRY
;	ds	=  cs
;	ax	=  number of text lines in screen
; EXIT
;	none
; USES
;	none
;
; NOTES	
;	This initializes font info at the start of app also. Since contents 
;	of DC are restored after this call, no harm done.
;
;	In 25 line mode do not set the scanlines or download fonts (these
;	are already in the 25 line mode after a mode change) - as this upsets
;	the code page support.

InitScreen	proc	far

		mov	word ptr cs:[DC.dcFontBank][0],0FFFFh	;nothing
		mov	word ptr cs:[DC.dcFontBank][2],0FFFFh	;nothing
		mov	word ptr cs:[DC.dcFontBank][4],0FFFFh	;nothing
		mov	word ptr cs:[DC.dcFontBank][6],0FFFFh	;nothing

		push	ax

; depending on the no of lines, set it to 400 (for 25 and 50 lines) or
; 350 (for 43) scan line mode

		push	ax			;save
		cmp	ax,50			;50 line needed ?
		jae	Set400ScanLines		;set to 400 scan lines
		cmp	ax,25			;25 line needed ?
		jbe	InitMode		;already in 400 scan lines
		mov	ax,1201h		;set 350 scan lines
		jmp	short SetScanLines

Set400ScanLines:

		mov	ax,1202h		;set 400 scan lines

SetScanLines:

		mov	bl,30h			;contant
		int	10h			;scan lines set

InitMode:

		pop	ax			;get back no of lines

; now set the mode

		mov	ax,00007h
		int	010h

; on apricot vgamono, mode 7 doesn't work after mode c080 command.
;In this case, we set it mode 7

		mov	ah,0fh
		int	10h			;get mode 
		cmp	al,7			;is it 7 ?
		jz	@F
		mov	ax,03h
		int	010h
@@: 
		pop	ax

; now set the number of lines per mode.

		push	ax			;save
		push	bx			;save
		cmp	ax,25			;25 line mode
		jbe	InitCursor		;yes, don't down load fonts
		cmp	ax,43			;43 or 50 line mode requested ?
		jae	Set8by8Fonts		;set 8x8 fonts
		mov	ax,1114h		;(8x16 fonts)
		jmp	short SetFonts

Set8by8Fonts:
	       
		mov	ax,1112h	        ;43 line mode (8x8 fonts)

SetFonts:
		xor	bx,bx			;bank 0
		int	10h			;no of lines set

InitCursor:

; now set the cursor

		mov	ax,0100h		;set cursor
		mov	cx,607h			
		int	10h

		pop	bx			;restore
		pop	ax			;restore

		ret
InitScreen	endp


_TEXT		ends
		end

