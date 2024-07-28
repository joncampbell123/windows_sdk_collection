
		page	,132
		%out	SetSwapDrive
		name	SETSWAP
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	SETSWAP.ASM
;
; DESCRIPTION
;	This file contains the extended subfunction SetSwapDrive.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT

		.xlist
		include dc.inc
		.list

		extrn	DC:byte
		extrn	MakeTempFile:near
		extrn	StrLen:near

		public	fGotSwapPath
		public	SetSwapDrive


FALSE		=	0
TRUE		=	1
MAX_PATHLEN	=	64
NOSWAPDRIVE	=	0FFh			;no swap drive available

fGotSwapPath	db	FALSE			;TRUE = SetSwapDrive called


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
		assume	ds:_TEXT
		cmp	bl,NOSWAPDRIVE		;if no swap drive,
		je	ssdX			;  just leave

		call	StrLen			;else get length of path
		cmp	cx,MAX_PATHLEN - 1	;if path too long,
		ja	ssdX			;  just leave

		push	cx			;save length
		mov	ax,es
		mov	ds,ax			;ds = segment of pathname
		assume	ds:nothing
		mov	si,di			;si = offset  of pathname
		mov	ax,cs
		mov	es,ax			;es = cs
		mov	di,offset DC.dcSwapPath ;di = offset of our buffer
		cld
		rep	movsb			;save path in our DC structure
		pop	cx			;recover length

		std
		mov	al,'~'
		repnz	scasb			;scan backwards for tilde
		add	di,2			;rep goes 2 too far with std
		cld
		mov	ax,"RG"                 ;patch in our id "GRB"
		stosw
		mov	al,"B"
		stosb
		mov	cs:[DC.dcFileNum],di	;save di (ptr to numeric part)
		mov	cs:[fGotSwapPath],TRUE	;show we have a swap path
ssdX:
		ret
SetSwapDrive	endp


_TEXT		ends
		end

