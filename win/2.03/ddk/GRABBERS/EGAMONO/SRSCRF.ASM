
		page	,132
		%out	Save/RestScreenFile
		name	SRSCRF
		title	Copyright (C) by Microsoft Inc. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include dc.inc
		include ic.inc
		include ega.inc
		include fileio.inc
		.list

		extrn	DC:byte
		extrn	IC:byte
		extrn	MakeTempFile:near

		public	SaveScreenFile
		public	RestScreenFile


		subttl	SaveScreenFile


;
; SaveScreenFile - save hires graphics screen to disk
;
; ENTRY
;	ds	=  cs
; EXIT
;	nc	=  screen file successfully saved
;	cy	=  unnable to save screen file
; USES
;	ax, bx, cx, dx, flags
;
SaveScreenFile	proc	near
		assume	ds:_TEXT
		push	di
		push	ds

		mov	si,[DC.dcFileNum]	;synthesize temp filename
		mov	dx,offset DC.dcSwapPath
		call	MakeTempFile
		jc	ssfErr0

		mov	ax,G_MODE		;init plane read mask/reg index
		mov	dx,GRAPH_ADDR
		out	dx,ax
		dec	ax

		xor	ch,ch
		mov	cl,[IC.icPlanes]	;get plane count
		mov	di,[IC.icScrLen]	;get and save screen length
		push	ds
		lds	dx,[IC.iclpScr] 	;get lp to screen
		assume	ds:nothing
ssfPlaneLoop:
		push	ax			;save plane mask
		push	cx			;save plane count
		push	dx			;save lp to screen
		mov	dx,GRAPH_ADDR		;set EGA plane read mask
		out	dx,ax
		pop	dx

		mov	cx,di			;recover screen length
		WriteFile
		jc	ssfErr1
		cmp	ax,cx
		jne	ssfErr1

		pop	cx			;recover plane count
		pop	ax
		inc	ah			;bump to next plane
		loop	ssfPlaneLoop

		pop	ds
		CloseFile			;clean up
		clc				;all ok
		jmp	short ssfX
ssfErr1:
		pop	cx			;clean up stack
		pop	ax
		pop	ds
		CloseFile			;clean up file
		mov	dx,offset DC.dcSwapPath
		RemoveFile
ssfErr0:
		stc				;had problems
ssfX:
		pop	ds
		pop	di
		ret
SaveScreenFile	endp


		 subttl  RestScreenFile
		 page


;
; RestScreenFile - restore hires graphics screen from disk
;
; ENTRY
;	none
; EXIT
;	nc	=  screen file successfully restored
;	cy	=  unable to restore screen file
; USES
;	ax, bx, cx, dx, flags
;
RestScreenFile	proc	near
		push	si
		push	ds
		mov	ax,cs
		mov	ds,ax			;ds = cs
		assume	ds:_TEXT

		mov	al,FAC_READWRITE	;get file access code
		mov	dx,offset DC.dcSwapPath ;get ptr dcSwapPath
		OpenFile
		jc	rsfX			;jmp if bozo deleted swap file!

		push	dx			;save ptr to dcSwapPath
		mov	bx,ax			;save file handle
		mov	ax,01*256 + S_MAP	;prepare plane mask/reg index

		xor	ch,ch
		mov	cl,[IC.icPlanes]	;get plane count
		mov	si,[IC.icScrLen]	;get and save screen length
		push	ds
		lds	dx,[IC.iclpScr] 	;get lp to screen
		assume	ds:nothing
rsfPlaneLoop:
		push	ax			;save plane mask
		push	cx			;save plane count
		push	dx			;save lp to screen
		mov	dx,SEQ_ADDR		;set EGA plane write mask
		out	dx,ax
		pop	dx

		mov	cx,si			;recover screen length
		ReadFile

		pop	cx
		pop	ax
		shl	ah,1			;bump to next plane
		loop	rsfPlaneLoop

		pop	ds			;recover ds = cs
		assume	ds:_TEXT
		mov	ah,00Fh 		;enable all planes
		mov	dx,SEQ_ADDR
		out	dx,ax

		CloseFile			;clean up
		pop	dx			;recover ptr to dcSwapPath
		RemoveFile
		clc
rsfX:
		pop	ds
		pop	si
		ret
RestScreenFile	endp


_TEXT		ends
		end

