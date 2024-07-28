
		page	,132
		%out	Save/RestScreenFile
		name	SRSCRF
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	SRSCRF.ASM
;
; DESCRIPTION
;	This file contains the routines needed to save and restore
;	EGA screens (for modes that have type ST_LARGE) to and from
;	a disk file.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;
;	1.10	071588	vvr	Added code to support mode 13 of VGA
;	

_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include dc.inc
		include vgaic.inc
		include vga.inc
		include fileio.inc
		.list

		extrn	DC:byte
		extrn	IC:byte
		extrn	MakeTempFile:near
		
		extrn	GetRegSet:near
		extrn	RestRegSet:near
		extrn	ReadVideoBufFromFile:near
		extrn	WriteVideoBufToFile:near

		public	SaveScreenFile
		public	RestScreenFile


		subttl	SaveScreenFile


;
; SaveScreenFile - save hires graphics screen to disk
;
; ENTRY
;	ds	=  cs
; EXIT
;	cf	=  0: screen file successfully saved
; ERROR EXIT
;	cf	=  1: unnable to save screen file
; USES
;	ax, bx, cx, dx, flags
;
SaveScreenFile	proc	near
		assume	ds:_TEXT
		push	di
		push	ds
		push	es

; the MODE and MAP SELECT registers will be modified here, we must save the
; current values first.

		mov	dx,GRAPH_ADDR		;GRX address register
		mov	al,04h			;MAP SELECT register
		out	dx,al			;select it
		inc	dx			;data register
		in	al,dx			;read in the value
		mov	cl,al			;save it
		dec	dx			;back to address register
		mov	al,5			;MODE register
		out	dx,al			;select it
		inc	dx			;data register
		in	al,dx			;get the value
		mov	ch,al			;save it
		push	cx			;store it

		mov	si,[DC.dcFileNum]	;synthesize temp filename
		mov	dx,offset DC.dcSwapPath
		call	MakeTempFile		
		jnc	ssfOk			;long jump
		jmp	ssfErr0
ssfOk:
		
		cmp	[DC.dcScrMode],13h	;is it mode 13	
		jz	ssfMode13		;no so, old way

ssfOthers:

		mov	ax,G_MODE
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
		call	WriteVideoBufToFile	;write to file

		jc	ssfErr1 		;if write failed, leave
		cmp	ax,cx			;if not all bytes written,
		jne	ssfErr1 		;  leave

		pop	cx			;recover plane count
		pop	ax
		inc	ah			;bump to next plane
		loop	ssfPlaneLoop
		pop	ds
		jmp	ssfContinue

ssfMode13:
		mov	cx,[IC.icScrLen]	;just copy 64k from A0000
		lds	dx,[IC.iclpScr]		; to file
		shl	cx,1
		shl	cx,1

		call	WriteVideoBufToFile	;write to file
		cmp	ax,cx			;is copy ok?
		jnz	ssfM13Err
		add	dx,ax
		call	WriteVideoBufToFile	;write to file
		cmp	ax,cx			;is copy ok?
		jnz	ssfM13Err		; not ok
ssfContinue:
		CloseFile
		clc				;show success
		jmp	short ssfX
ssfErr1:
		pop	cx			;clean up stack
		pop	ax
		pop	ds
ssfM13Err:
		CloseFile			;close file so we can delete it
		mov	dx,offset DC.dcSwapPath
		RemoveFile			;delete partial file
ssfErr0:
		stc				;show error
ssfX:

; restore the MAP SELECT and MODE registers

		pop	cx			;ch=reg 5, cl = reg 4

		mov	dx,GRAPH_ADDR		;GRX address register
		mov	al,04h			;MAP SELECT register
		out	dx,al			;select it
		inc	dx			;data register
		mov	al,cl			;get the original value
		out	dx,al			;restore the value
		dec	dx			;back to address register
		mov	al,5			;MODE register
		out	dx,al			;select it
		inc	dx			;data register
		mov	al,ch			;get the value
		out	dx,al			;reset the value

		pop	es
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
;	cf	=  0: screen file successfully restored
; ERROR EXIT
;	cf	=  1: unnable to open screen file
; USES
;	ax, bx, cx, dx, flags
; NOTES
;	An error while trying to open the screen file is considered fatal and
;	terminates this routine with an error condition; an error while
;	reading the file, should it be openable, is not even checked.  We
;	blindly try to keep restoring all planes in hope that as much of the
;	user's screen will be restored as possible.
;
RestScreenFile	proc	near
		push	si
		push	ds
		push	es
		mov	ax,cs
		mov	ds,ax			;ds = cs
		assume	ds:_TEXT

		mov	al,FAC_READWRITE	;get file access code
		mov	dx,offset DC.dcSwapPath ;get ptr dcSwapPath
		OpenFile
		jc	rsfX			;jmp if bozo deleted swap file!


		push	dx			;save ptr to dcSwapPath

		cmp	[DC.dcScrMode],13h
		jnz	rsfOthers

		push	ds  			;save file ptr
		mov	bx,ax			;get file handle
		mov	cx,[IC.icScrLen]
		lds	dx,[IC.iclpScr]
		shl	cx,1
		shl	cx,1
		
		call	ReadVideoBufFromFile
		add	dx,ax
		call	ReadVideoBufFromFile
		pop	ds			;restore file pointer
		jmp	rsfContinue

rsfOthers:

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
		call	ReadVideoBufFromFile

		pop	cx
		pop	ax
		shl	ah,1			;bump to next plane
		loop	rsfPlaneLoop

		pop	ds			;recover ds = cs
		assume	ds:_TEXT
		mov	ah,00Fh 		;enable all planes
		mov	dx,SEQ_ADDR
		out	dx,ax

rsfContinue:

		CloseFile			;clean up
		pop	dx			;recover ptr to dcSwapPath
		RemoveFile
		clc
rsfX:

		pop	es
		pop	ds
		pop	si
		ret
RestScreenFile	endp


_TEXT		ends
		end

