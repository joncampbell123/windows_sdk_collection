
		page	,132
		%out	GetInfo
		name	GETINFO
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	GETINFO.ASM
;
; DESCRIPTION
;	This file contains the extended grabber subfunction call GetInfo.
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
		include vgaic.inc
		include grabber.inc
		.list

		extrn	IC:byte
		extrn	GetMode:near

		public	GetInfo
		subttl	GetInfo


;
; GetInfo - return GrabInfo structure
;
;	GetInfo fills a buffer pointed to by es:di on entry with data in
;	GrabInfo format.  Note that we just copy our InfoContext structure
;	since it is a superset of the GrabInfo structure (although InfoContext
;	is not as big as GrabInfo due to reserved fields in the latter).
;
;	While Winoldap knows nothing about the reserved fields, we end up
;	filling some of them with the extra data in InfoContext because it is
;	convenient for debugging purposes.  As these fields are still
;	reserved, THEY MAY CHANGE AT ANY TIME.
;
;	For historical reasons, Winoldap insists that the return value from
;	this call also indicate whether the display is in graphics or text
;	mode.  Essentially, this provides an easy way for Windoldap to
;	determine whether or not it should attempt menuing operations, since
;	the Get/Mark/PutBlock functions are not currently supported in their
;	full generality for graphics modes.  If they should ever become
;	completely functional for graphics modes, this call must return a 1 in
;	order for Winoldap to even attempt the menu operation.
;
;	Note that the block functions themselves will also check the current
;	mode and return ERR_UNSUPPORTED to indicate non-support for the
;	request, which is the prefered way to determine support for a given
;	mode.
;
; ENTRY
;	es:di	-> GrabInfo structure to fill
; EXIT
;	ax	=  0 if block ops not supported on current mode (graphics)
;	ax	=  1 if block ops supported on current mode	(text)
; USES
;	ax, di, es, flags
;
GetInfo 	proc	near
		assume	ds:_TEXT
		cld
		push	cx
		push	si

		call	GetMode 			;educate ourselves
		jnc	@F
		xor	ax,ax
		pop	si
		pop	cx
		ret
@@:
		mov	si,offset IC			;prepare to copy
		mov	cx,(SIZE InfoContext)/2
		if	(SIZE InfoContext) AND 1
		movsb
		endif
		rep	movsw				;enlighten others

		xor	ax,ax
		test	[IC.icScrType],ST_GRPH		;if graphics mode
		jnz	giX				;  leave with ax = 0

		inc	ax				;else show text
giX:
		pop	si
		pop	cx
		ret
GetInfo 	endp


_TEXT		ends
		end

