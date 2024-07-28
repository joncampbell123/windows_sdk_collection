
		page	,132
		%out	InquireGrab/Save
		name	INQUIRE
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	INQUIRE.ASM
;
; DESCRIPTION
;	This file contains two of the original grabber entrypoints:
;	InquireGrab and InquireSave.  InquireGrab also serves as the
;	function dispatcher for the extended subfunctions, and therefore
;	can be considered their entrypoint as well.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


NO_STDGRABENTRY =	1

		.xlist
		include grabber.inc
		.list

		extrn	GrabTextSize:word
		extrn	GrabGrphSize:word
		extrn	SaveTextSize:word
		extrn	SaveGrphSize:word
		extrn	ExtFuncTable:word

		extrn	OemInit:near
		extrn	DevInit:near

		public	InquireGrab
		public	InquireSave


FALSE		=	0
TRUE		=	1
fInitDone	db	0


		subttl	InquireGrab


;
; InquireGrab - return size of grab buffer needed or dispatch extended call
;
;	In Microsoft Windows 1.0x, this routine only handled grab buffer
;	size requests.	In HP's version of Microsoft Windows 1.03, 5 new
;	subfunction numbers were added to perform various helper tasks
;	to aid Winoldap in implementing HP's extensions (Mark/Copy/Paste
;	and "extended mode" oldap memory management).
;
;	Microsoft Windows 2.0x adopted HP's 1.03 extensions as well as
;	defining another 7 subfunctions to support the increased functionality
;	of Winoldap 2.0x.
;
;	For an explanation of the new subfunctions, refer to the subfunction
;	descriptions.
;
; ENTRY
;	ds	=  cs
;	ax	=  n
;
;	where n is either:
;		1	Inquire text grab buffer size
;		2	Inquire graphics grab buffer size
;		n > 2	Extended subfunction request
;
; EXIT
;	if function number 1 or 2,
;	dx:ax	=  size in bytes for grab buffer
;
;	else
;	exit status depends on the extended call
;
; USES
;	if function number 1 or 2,
;	ax, dx, flags
;
;	else
;	register usage depends on the extended call
;
; NOTES
;	In order to get any useful work from the grabber, Winoldap must call
;	either InquireGrab or InquireSave before any other call (InitScreen is
;	a possible exception).	We use this behavior as an opportunity to
;	initialize our module if it is the first time one of these entrypoints
;	has been called.
;
; SEE ALSO
;	InquireSave, GRABBER.INC, ENTRY.ASM
;
InquireGrab	proc	far
		assume	ds:_TEXT
		cmp	[fInitDone],TRUE	;if not the first time called,
		je	igGo			;  don't bother with init code
						;else
		call	OemInit 		;  initialize OEM layer
		call	DevInit 		;  initialize device layer
		mov	[fInitDone],TRUE	;  show we did it
igGo:
		cmp	ax,MIN_EXTCALL		;if a new extended call,
		jae	igDoExt 		;  go dispatch it

		cmp	ax,MAX_STDCALL		;if not a call we recognize,
		ja	igX			;  get out now

		mov	ax,GrabGrphSize 	;if they want graphics size,
		je	igRetSize		;  we're done

		mov	ax,GrabTextSize 	;else they want text size
igRetSize:
		xor	dx,dx			;clear hi-word of long value
igX:
		ret

igDoExt:
		push	si
		mov	si,ax			;prepare for index
		sub	si,MIN_EXTCALL		;zero-adjust subfunction
		shl	si,1			;*2 for words
		call	ExtFuncTable[si]	;dispatch
		pop	si
		ret
InquireGrab	endp


		subttl	InquireSave
		page


;
; InquireSave - return size of screen save buffer needed
;
; ENTRY
;	ds	=  cs
;	ax	=  n
;
;	where n is either:
;		1	Inquire text save buffer size
;		2	Inquire graphics save buffer size
; EXIT
;	dx:ax	=  size in bytes for save buffer
; USES
;	ax, dx, flags
;
InquireSave	proc	far
		assume	ds:_TEXT
		cmp	[fInitDone],TRUE	;if not the first time called,
		je	isGo			;  don't bother with init code
						;else
		call	OemInit 		;  initialize OEM layer
		call	DevInit 		;  initialize device layer
		mov	[fInitDone],TRUE	;  show we did it
isGo:
		cmp	ax,MAX_STDCALL		;if not a call we recognize,
		ja	isX			;  get out now

		mov	ax,SaveGrphSize 	;if they want graphics size,
		je	isRetSize		;  we're done

		mov	ax,SaveTextSize 	;else they want text size
isRetSize:
		xor	dx,dx			;clear hi-word of long value
isX:
		ret
InquireSave	endp


_TEXT		ends
		end

