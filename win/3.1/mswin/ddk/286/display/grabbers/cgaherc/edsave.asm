
		page	,132
		%out	Enable/DisableSave
		name	EDSAVE
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		public	EnableSave
		public	DisableSave


		subttl	EnableSave


;
; EnableSave - enable video context switching
;
;	Analogous to EnableGrab, this entrypoint gives the grabber the chance
;	to install any hooks needed for context switching.
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	ax, ds, flags
;
EnableSave	proc	near
		ret				;nop for these grabbers
EnableSave	endp


		subttl	DisableSave


;
; DisableSave -  disable video context switching
;
;	Analogous to DisableGrab, this entrypoint gives the grabber the chance
;	to remove any hooks installed by EnableSave.
;
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	ax, ds, flags
;
DisableSave	proc	near
		ret				;nop for these grabbers
DisableSave	endp


_TEXT		ends
		end

