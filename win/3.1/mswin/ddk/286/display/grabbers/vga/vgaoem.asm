
		page	,132
		%out	OEM Routines
		name	OEM
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	OEM.ASM
;
; DESCRIPTION
;	This file contains most of the OEM-specific code used in the grabber.
;	that does not directly relate to the display adapter.  When adding
;	support for other vendors, it should ideally be added here if
;	possible.  It also contains two of the extended subfunctions GetID and
;	GetVersion, since these are OEM dependent to a certain extent.
;
; HISTORY
;	1.00	060187	jhc	Epoch
;	1.01	080487	jhc	Moved RT kbd test from OemInit to OemEventISR.
;				This change due to Windows keyboard driver
;				messing with KbFlagRT, which OemInit examined
;				before Windows layer was disabled.
;
;	1.02	100587	kmt	Modifications for Newport:
;				  1. disabled GetHPVideoSeg calls
;				  2. set fVectra flag if ADD6F.COM has been
;				     run adding 6F video extensions.
;
;	1.03	111987	jhc	Removed PCDC stuff added in 1.02 since it
;				really isn't needed for Newport.
;
;	1.04	072789	jem	IFDEF unused functions for Windows 3.0
;
;	Copyright (c) 1989  Microsoft.	All Rights Reserved.


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include vgaabs0.inc
		include vgaoem.inc
		.list

IFDEF	GRAB_VER_2
		extrn	GrabScreen:near
		public	lpOldEventISR
		public	OemEventISR
ENDIF	;GRAB_VER_2

		public	OEM_INTNUMBER
		public	fVectra

IFDEF	GRAB_VER_2
		public	GetID
ENDIF	;GRAB_VER_2

		public	GetVersion
		public	GetHPVideoSeg
		public	OemBeep
		public	OemInit


lpOldEventISR	dd	0			;Enable will stash old ISR here
fKbRt		db	FALSE			;TRUE = RT keybd in use
fInProgress	db	FALSE			;TRUE = grab already in progress
fVectra 	db	FALSE			;TRUE = running on HP Vectra


IFDEF	GRAB_VER_2

		subttl	GetID/GetVersion


;
; GetID - returns id to indicate presence of grabber extensions
;
; ENTRY
;	none
; EXIT
;	ax	=  grabber id
; USES
;	ax
; NOTES
;	Winoldap 2.0 (Microsoft's or HP's) will NOT recognize the presence of
;	the 2.0 grabber extensions unless the id returned matches 04A43h (JC).
;
;	Winoldap 1.03 (HP's only) will NOT recognize the presence of the 1.03
;	grabber extensions unless the id returned matches 04850h (HP).
;
;	In fact, this behavior makes GetID more like a version check, and
;	seemingly makes the GetVersion call redundent.	Although not
;	originally intended to be this way, it turned out we had to change the
;	way some of the 1.03 extensions (namely GetInfo) functioned.  Since
;	GetVersion is a 2.0 extension, the 1.03 Winoldap would have assumed
;	it had found a 1.03 grabber if we returned HP.	This assumption would
;	have been fatal on the next call to GetInfo if a 2.0 grabber was
;	actually present.  GetVersion is still useful as a sanity check as
;	well as determining if minor enhancements are in place.
;
GetID		proc	near

		mov	ax,GRABBER_ID
		ret


GetID		endp

ENDIF	;GRAB_VER_2

;
; GetVersion - returns version number of OEM extensions
;
; ENTRY
;	none
; EXIT
;	ax	=  OEM version number
; USES
;	ax
; NOTES
;	Winoldap 2.0 (Microsoft's or HP's) will NOT recognize the presence of
;	the 2.0 grabber extensions unless the version returned is greater than
;	or equal to 2.01.
;
GetVersion	proc	near
		mov	ax,OEM_VERHI*256 + OEM_VERLO
		ret
GetVersion	endp


		subttl	GetHPVideoSeg
		page


;
; GetHPVideoSeg - return segment base of HP EX-BIOS video data
;
; ENTRY
;	none
; EXIT
;	ax	=  segment of HP EX-BIOS video data
; USES
;	ax, flags
;
GetHPVideoSeg	proc	near
		push	ds			;int 06Fh always clobbers these
		push	es
		push	bp
		mov	bp,012h 		;these are magic numbers
		mov	ah,004h 		;given to me by the BIOS team
		int	06Fh
		mov	ax,es:[058h]		;so is this
		pop	bp
		pop	es
		pop	ds
		ret
GetHPVideoSeg	endp


		subttl	OemBeep
		page


;
; OemBeep - hoot the hooter
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	ax, cx, flags
;
OemBeep 	proc	near
		mov	al,0B6H 		;select timer 2
		out	PIT_PORTD,al
		mov	ax,BEEP_TONE1		;divisor for tone 1
		out	PIT_PORTC,al		;write timer 2 count - lsb
		mov	al,ah
		out	PIT_PORTC,al		;write timer 2 count - msb

		in	al,PPI_PORTB		;get current setting of port
		mov	ah,al			;save setting
		or	al,00000011b		;turn speaker on
		out	PPI_PORTB,al
		mov	cx,BEEP_TIME1		;wait awhile
		loop	$

		mov	cx,BEEP_TONE2		;divisor for tone 2
		mov	al,cl
		out	PIT_PORTC,al
		mov	al,ch
		out	PIT_PORTC,al
		mov	cx,BEEP_TIME2		;wait again
		loop	$

		mov	al,ah
		out	PPI_PORTB,al
		ret
OemBeep 	endp


		subttl	OemInit
		page


;
; OemInit - allow OEM to perform initializations
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	flags
; NOTES
;	This call will only be made once per instance of the grabber.
;
OemInit 	proc	near
		assume	ds:_TEXT
		push	ax
		push	bx
		push	es

		mov	ax,06F00h		;check for HP Video EX-BIOS
		xor	bx,bx
		int	010h
		cmp	bx,OEM_VIDEO_ID 	;if no EX-BIOS,
		jne	oiNoHP			;don't do Vectra stuff
oiYesHP:
		mov	[fVectra],TRUE		;else show Vectra BIOS present
oiNoHP:
		pop	es
		pop	bx
		pop	ax
		ret
OemInit 	endp

IFDEF	GRAB_VER_2

		subttl	OemEventISR
		page


;
; OemEventISR - handler for OEM event that triggers screen grab (snapshot)
;
;	In our case, the event that triggers the snapshot will be a keystroke
;	so we will hook into int 009h.	The keystroke trigger is Alt-PrtSc,
;	which incidentally on RT (AT-II) keyboards is accessed by Alt-SysReq.
;	The keystroke will be fed on to the previous int 009h ISR before we
;	use it as the grab trigger.  Keys we don't recognize are passed on
;	through.  A flag is maintained to prevent re-entering a snapshot
;	already in progress.
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	none
;
OemEventISR	proc	far
		push	ax
		push	ds
		xor	ax,ax
		mov	ds,ax
		assume	ds:ABS0

		mov	al,ds:[KbFlag]		;get BIOS shift states
		and	al,OEM_SHIFTMASK	;isolate non-toggle keys
		cmp	al,OEM_SHIFTSTATE	;if not OEM shift state,
		jne	oeiNoTrigger		;  get out now

		in	al,PPI_PORTA		;get key
		cmp	al,OEM_TRIGGER		;if this is our cue to grab,
		je	oeiDoGrab		;  go do it

		test	ds:[KbFlagRt],OEM_KBRT	;if not an RT keyboard,
		jz	oeiNoTrigger		;  skip RT tests

		cmp	al,OEM_TRIGGERRT	;if this is our cue,
		je	oeiDoGrab		;  go do it
oeiNoTrigger:					;not our cue to grab
		pop	ds
		pop	ax
		jmp	cs:[lpOldEventISR]	;old ISR will finish up
oeiDoGrab:
		mov	ax,cs
		mov	ds,ax			;ds = cs
		assume	ds:_TEXT

		pushf
		call	[lpOldEventISR] 	;let old ISR process the event
		mov	al,TRUE 		;check for grab in progress
		xchg	[fInProgress],al
		sti
		or	al,al			;if previous grab in progress,
		jnz	oeiX			;   exit now

		call	GrabScreen		;do it!
		mov	[fInProgress],FALSE	;show we are done
oeiX:
		pop	ds
		pop	ax
		iret
OemEventISR	endp

ENDIF	;GRAB_VER_2


_TEXT		ends
		end
