
		page	,132
		%out	VgaColor
		name	VgaCOLOR
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	VGACOLOR.ASM
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
;	1.11	062989	vvr	added code to put the display in 400 line
;				mode through int 10. Why ?
;
;				If an application puts the display in 350 line
;				mode and context switches to Windows, grabber
;				only saves this information. When a new app is
;				is started, it sees 350 line and 8x14 font 
;				instead of the default 400 line and 8x16 font
;				that VGA mode 3 provides. In order to eliminate
;				this situation, we put the display in 400 line
;				mode anyway (since it is default).			
;
;				The other way to do this is to get the hor. res
;				info in DEVINIT module and set the display mode
;				accodingly.
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
		db	071h		;PC_SELECTED:	    blue on white
		db	017h		;PC_UNSELECTED:     white on blue
		db	018h		;PC_GRAYED:	    grey on blue
		db	079h		;PC_SELECTGRAY:     hi-blue on white
		db	06Fh		;PC_TITLEBAR:	    hi-white on brown
		db	00Fh		;PC_SYSTEMUNSELECT: hi-white on black
		db	070h		;PC_SYSTEMSELECT:   black on white
		db	00Fh		;PC_APPTITLE:	    hi-white on black
		db	01Fh		;PC_MNEMONIC:	    hi-white on blue

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
;	two cases, we always set the display to a known mode regardless.	       \
;
; ENTRY
;	ds	=  cs
;	ax 	=  number of lines perscreen. If >= 43, 43 lime mode is
;		   set, else 25 line mode is set.
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
;
InitScreen	proc	far

		mov	word ptr cs:[DC.dcFontBank][0],0FFFFh	;nothing
		mov	word ptr cs:[DC.dcFontBank][2],0FFFFh	;nothing
		mov	word ptr cs:[DC.dcFontBank][4],0FFFFh	;nothing
		mov	word ptr cs:[DC.dcFontBank][6],0FFFFh	;nothing

		push	ax

		push	es
		push	ax
		mov	ax,40h
		mov	es,ax
		and	byte ptr es:[10h],0cfh	
		or	byte ptr es:[10h],20h
		pop	ax
		pop	es

; depending on the no of lines, set it to 400 (for 25 and 50 lines) or
; 350 (for 43) scan line mode

		push	ax			;save
		cmp	ax,50			;50 line needed ?
		jae	Set400ScanLines		;set to 400 scan lines
; this was jumping to initmode and causing BUG # 5669
		cmp	ax,43			;below 43 lines ?
		jb	Set400ScanLines		;set to 400 scan lines
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

		mov	ax,00003h
		int	010h

		pop	ax

; now set the number of lines per mode.

		push	ax			;save
		push	bx			;save
		cmp	ax,25			;25 line mode ?
		jbe	InitCursor		;yes, don't down load fonts
		cmp	ax,43			;more than 25 lines ?
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

