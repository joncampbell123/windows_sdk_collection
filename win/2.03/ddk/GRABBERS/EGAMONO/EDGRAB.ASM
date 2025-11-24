
		page	,132
		%out	Enable/DisableGrab
		name	EDGRAB
		title	Copyright (C) by Microsoft Inc. 1985-1987


;
; HISTORY
;	1.01	080787	*** *******	Chnaged Enable/DisableGrab per new specs
;


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		extrn	OEM_INTNUMBER:abs
		extrn	GrabBufferSize:word
		extrn	lpGrabBuffer:dword
		extrn	lpOldEventISR:dword
		extrn	OemEventISR:near

		public	EnableGrab
		public	DisableGrab



FALSE		=	0
TRUE		=	1
fGrabHooks	db	FALSE


		subttl	EnableGrab


;
; EnableGrab - enable screen snapshots
;
; ENTRY
;	ds	=  cs
;
;	if
;	es:di	= NULL,
;
;		We are to do whatever is necessary to enable a grab EXCEPT
;		install the grab event hook into the interrupt system.
;		Winoldap will take responsibility for detecting the grab event
;		and will simulate a grab via a fullscreen MarkBlock/
;		GetBlock/MarkBlock sequence.
;
;		This change to the grabber API was necessary to allow winoldap
;		to provide support for certain "WORSE" apps such as WORD that
;		take int 9.  Without this change, a call to DisableGrab would
;		clobber the apps' int 9 vector because we would restore it to
;		the int 9 handler we saved during EnableGrab, which is NOT the
;		apps' int 9.
;
;		This change also implies that in addition to Block operations
;		on alpha screens, the grabbers must also minimally support
;		the fullscreen versions of MarkBlock and GetBlock on graphics
;		screens in order to provide the same functionality Windows
;		1.03 offered.
;
;	else
;	es:di	-> grab buffer
;	ax	=  size of grab buffer allocated
;
;		We are to do whatever is necessary to enable a grab, including
;		installing the grab event hook into the interrupt system.
;		This is the original EnableGrab functionality, and must be
;		retained to support snapshots for "UGLY" apps, since when
;		running an "UGLY" app, winoldap is not present to do the
;		detection and snapshot simulation.
;
; EXIT
;	none
; USES
;	ax, flags
;
EnableGrab	proc	far
		assume	ds:_TEXT
		push	ax
		mov	ax,es
		or	ax,di
		pop	ax			;recover buffer size, if any
		jnz	egGo			;if es:di != NULL, install hooks
		ret				;else just leave
egGo:
		mov	[fGrabHooks],TRUE

		push	bx
		push	dx
		push	es

		mov	[GrabBufferSize],ax
		mov	word ptr [lpGrabBuffer],di
		mov	word ptr [lpGrabBuffer][2],es

		mov	ax,03500h + OEM_INTNUMBER
		int	021h
		mov	word ptr [lpOldEventISR],bx
		mov	word ptr [lpOldEventISR][2],es

		mov	dx,offset OEMEventISR
		mov	ax,02500h + OEM_INTNUMBER
		int	021h

		pop	es
		pop	dx
		pop	bx
		ret
EnableGrab	endp


		subttl	DisableGrab


;
; DisableGrab - disable screen snapshots
;
;		Note that if EnableGrab did not install any grab event hooks,
;		DisableGrab must not attempt to remove them!
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	ax, ds, flags
;
DisableGrab	proc	far
		assume	ds:_TEXT
		cmp	[fGrabHooks],TRUE		;if event hooks are in,
		je	dgGo				;  go remove them
		ret					;else just leave
dgGo:
		mov	[fGrabHooks],FALSE

		push	dx
		lds	dx,[lpOldEventISR]
		assume	ds:nothing
		mov	ax,02500h + OEM_INTNUMBER
		int	021h
		pop	dx
		ret
DisableGrab	endp


_TEXT		ends
		end

