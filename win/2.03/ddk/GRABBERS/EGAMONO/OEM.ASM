
		page	,132
		%out	OEM Routines
		name	OEM
		title	Copyright (C) by Microsoft Inc. 1985-1987


;
; HISTORY
;	1.01	080487	*** *******	Moved RT kbd test from OemInit to OemEventISR.
;				This change due to Windows keyboard driver
;				messing with KbFlagRT, which OemInit examined
;				before Windows layer was disabled.
;


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include abs0.inc
		include oem.inc
		.list

		extrn	GrabScreen:near

		public	OEM_INTNUMBER
		public	lpOldEventISR
		public	fVectra

		public	GetID
		public	GetVersion
		public	GetHPVideoSeg
		public	OemBeep
		public	OemInit
		public	OemEventISR


lpOldEventISR	dd	0			;Enable will stash old ISR here
fKbRt		db	FALSE			;TRUE = RT keybd in use
fInProgress	db	FALSE			;TRUE = grab already in progress
fVectra 	db	FALSE			;TRUE = running on HP Vectra


		subttl	GetID/GetVersion


;
; GetID - returns id to indicate presence of grabber extensions
;
;	Note: Winoldap 2.00 will NOT recognize to presence of the 2.00
;	extensions unless the id returned matches 04A43h!
;
; ENTRY
;	none
; EXIT
;	ax	=  grabber id
; USES
;	ax
;
GetID		proc	near
		mov	ax,GRABBER_ID
		ret
GetID		endp


;
; GetVersion - returns version number of OEM extensions
;
;	Note: Winoldap 2.00 will NOT recognize to presence of the 2.00
;	extensions unless the version returned is greater than or equal to
;	2.01
;
; ENTRY
;	none
; EXIT
;	ax	=  OEM version number
; USES
;	ax
;
GetVersion	proc	near
		mov	ax,OEM_VERHI*256 + OEM_VERLO
		ret
GetVersion	endp


		subttl	GetHPVideoSeg
		page


;
; GetHPVideoSeg
;
; ENTRY
;	none
; EXIT
;	ax	=  segment of HP EX-BIOS video data
; USES
;	ax, flags
;
GetHPVideoSeg	proc	near
		push	ds
		push	es
		push	bp
		mov	bp,012h
		mov	ah,004h
		int	06Fh
		mov	ax,es:[058h]
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
;	We only need to perform the following tasks once per instance of the
;	grabber, so a flag will be set to nop this procedure on subsequent
;	calls.
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	flags
;
OemInit 	proc	near
		assume	ds:_TEXT
		push	ax
		push	bx
		push	es

		mov	ax,06F00h			;check for HP Video EX-BIOS
		xor	bx,bx
		int	010h
		cmp	bx,OEM_VIDEO_ID 		;if no EX-BIOS,
		jne	oiX				;  we're done

		mov	[fVectra],TRUE			;else show Vectra BIOS present
oiX:
		pop	es
		pop	bx
		pop	ax
		ret
OemInit 	endp


		subttl	OemEventISR
		page


;
; OemEventISR - handler for OEM event that triggers screen grab (snapshot)
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


_TEXT		ends
		end

