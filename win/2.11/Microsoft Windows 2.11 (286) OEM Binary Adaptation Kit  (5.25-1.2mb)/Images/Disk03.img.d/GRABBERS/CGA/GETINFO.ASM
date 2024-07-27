
		page	,132
		%out	GetInfo
		name	GETINFO
		title	Copyright (C) by Microsoft Inc. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include ic.inc
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
;	GrabInfo format.  Note that we just blast our InfoContext since it
;	is a superset of GrabInfo (yet InfoContext is not as big as GrabInfo
;	due to reserved fields).
;
;	While Winoldap knows nothing about the reserved fields, we end up
;	filling some of them with the extra data in InfoContext because it is
;	convienient for debugging purposes.  As these fields are still
;	reserved, THIS MAY CHANGE AT ANY TIME.	Winoldap take note:  Do NOT go
;	using this reserved data!
;
;	Although its kind of kludgey, Winoldap insists that this call also
;	indicate whether Get/Put/Mark block functions are supported for the
;	current screen mode.  Graphics modes are currently unsupported, so we
;	return a 0.  ...one day I'm going to return a 1 just to see what
;	they do with the ERR_UNSUPPORTED return they are SUPPOSED to be
;	using from the block functions themselves!
;
; ENTRY
;	es:di	-> GrabInfo structure to fill
; EXIT
;	ax	=  0 if block ops not supported on current screen
;	ax	=  1 if block ops supported on current screen
; USES
;	ax, di, es, flags
;
GetInfo 	proc	near
		assume	ds:_TEXT
		cld
		push	cx
		push	si

		call	GetMode
		mov	si,offset IC
		mov	cx,(SIZE InfoContext)/2
		if	(SIZE InfoContext) AND 1
		movsb
		endif
		rep	movsw

		xor	ax,ax
		test	[IC.icScrType],ST_GRPH
		jnz	giX

		inc	ax
giX:
		pop	si
		pop	cx
		ret
GetInfo 	endp


_TEXT		ends
		end

