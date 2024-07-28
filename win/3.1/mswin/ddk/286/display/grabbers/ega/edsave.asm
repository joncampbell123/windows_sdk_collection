
		page	,132
		%out	Enable/DisableSave
		name	EDSAVE
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include abs0.inc
		.list

		public	EnableSave
		public	DisableSave


lpOldSavePtr	dd	?


		subttl	EnableSave


;
; EnableSave - enable video context switching
;
;	Analogous to EnableGrab, this entrypoint gives the grabber the chance
;	to install any hooks needed for context switching.
;
;	For the EGA, we save the current EGA SavePtr.
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	ax, ds, flags
;
EnableSave	proc	near
		xor	ax,ax
		mov	ds,ax
		assume	ds:ABS0
		lds	ax,[lpSavePtr]
		mov	word ptr cs:[lpOldSavePtr],ax
		mov	word ptr cs:[lpOldSavePtr][2],ds
		ret
EnableSave	endp


		subttl	DisableSave


;
; DisableSave -  disable video context switching
;
;	Analogous to DisableGrab, this entrypoint gives the grabber the chance
;	to remove any hooks installed by EnableSave.
;
;	For the EGA, we restore the EGA SavePtr.
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	ax, ds, flags
;
DisableSave	proc	near
		push	es
		xor	ax,ax
		mov	ds,ax
		assume	ds:ABS0
		les	ax,cs:[lpOldSavePtr]
		cli
		mov	word ptr [lpSavePtr],ax
		mov	word ptr [lpSavePtr][2],es
		sti
		pop	es
		ret
DisableSave	endp


_TEXT		ends
		end

