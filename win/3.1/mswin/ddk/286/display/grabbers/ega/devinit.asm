
		page	,132
		%out	DevInit
		name	DEVINIT
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include eri.inc
		.list

		extrn	SaveTextSize:word
		extrn	SaveGrphSize:word
		extrn	EnableSave:near

		public	fEriHooks
		public	EriContextSize
		public	DevInit



FALSE		=	0
TRUE		=	1
MIN_ERI_VER	=	002h

fEriHooks	db	FALSE
EriContextSize	dw	0


		subttl	DevInit


;
; DevInit - perform device-specific initializations
;
;	DevInit will be called only once during a given instance of the
;	grabber.  This gives the device-specific portions of the grabber a
;	chance to perform any soft initializations it may need to do EXCEPT
;	anything that may change the state of the display.  When DevInit is
;	called, the standard Windows device layer and video are still enabled.
;
;	If any hardware initializations are needed, these should be defered to
;	the InitScreen procedure, which is called after Windows layer has been
;	disabled and Winoldap has given the thumbs up to exec the oldap.
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	flags
;
DevInit 	proc	near
		assume	ds:_TEXT
		push	ax
		push	bx
		push	es

		push	ds
		call	EnableSave			;kludge for 1.03 Winoldap
		pop	ds

		mov	ah,ERI_INQUIRE
		xor	bx,bx
		int	010h
		or	bx,bx				;if EGA.SYS not present,
		jz	diNoEri 			;  skip over

		cmp	byte ptr es:[bx],MIN_ERI_VER	;if major version below 2,
		jb	diNoEri 			;  skip over

		mov	[fEriHooks],TRUE		;else show EGA.SYS active
		mov	ax,ERI_CONTEXTINFO*256 + ERI_CI_GETSIZE
		int	010h
		mov	[EriContextSize],ax
		add	SaveTextSize,ax 		;and bump these up
		add	SaveGrphSize,ax
diNoEri:
		pop	es
		pop	bx
		pop	ax
		ret
DevInit 	endp


_TEXT		ends
		end

