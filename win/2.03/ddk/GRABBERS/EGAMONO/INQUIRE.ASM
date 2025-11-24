
		page	,132
		%out	InquireGrab/Save
		name	INQUIRE
		title	Copyright (C) by Microsoft Inc. 1985-1987


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
; InquireGrab - ask for size of grab buffer needed or dispatch extended call
;
; ENTRY
;	ax	=  function number requested
;	ds	=  cs
; EXIT
;	dx:ax	=  size in bytes for grab buffer
; USES
;	ax, dx, flags
;
InquireGrab	proc	far
		assume	ds:_TEXT
		cmp	[fInitDone],TRUE
		je	igGo

		call	OemInit
		call	DevInit
		mov	[fInitDone],TRUE
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
; InquireSave - ask for size of screen save buffer needed
;
; ENTRY
;	ax	=  function number requested
;	ds	=  cs
; EXIT
;	dx:ax	=  size in bytes for save buffer
; USES
;	ax, dx, flags
;
InquireSave	proc	far
		assume	ds:_TEXT
		cmp	[fInitDone],TRUE
		je	isGo

		call	OemInit
		call	DevInit
		mov	[fInitDone],TRUE
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

