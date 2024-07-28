
		page	,132
		%out	SetSwapDrive
		name	SETSWAP
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		public	SetSwapDrive
		subttl	SetSwapDrive


;
; SetSwapDrive - allows Winoldap to give grabber the current swap drive letter
;
;	The next SaveScreen call following this call will use the given swap
;	drive letter to open the swapfile, if needed.  Failure to call this
;	function at least once before SaveScreen may result in a failure to
;	context switch.  This is stored as a static value, and need not be set
;	before every call to SaveScreen.  If it is known that the swap drive
;	will remain constant for the 'life' of the grabber instance, then it
;	is recommended that this call be done once, after Winoldap's decision
;	to allow context switching has been made.
;
; ENTRY
;	bl	=   ASCII drive letter for swap ('A', or 'B', or 'C', ...)
;		    0FFh if no swap drive available
;
;	es:di	->  d:\path\fname template for Windows temp file format
;	ds	=   cs
; EXIT
;	none
; USES
;	ax, bx, flags
;
SetSwapDrive	proc	near
		ret				;nop for these grabbers
SetSwapDrive	endp


_TEXT		ends
		end

