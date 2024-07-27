
		page	,132
		%out	Save/RestFontFile
		name	SRFONTF
		title	Copyright (C) by Microsoft Inc. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include dc.inc
		include eri.inc
		include ega.inc
		include fileio.inc
		.list

		extrn	DC:byte
		extrn	OemBeep:near
		extrn	MakeTempFile:near

		public	SaveFontFile
		public	RestFontFile


FALSE		=	0
TRUE		=	1

fFileOpen	db	FALSE
OldSeqRegs	db	5 dup (?)
OldGrphRegs	db	3 dup (?)
NewSeqRegs	db	03, 01, 04, 00, 07
NewGrphRegs	db	02, 00, 04



		subttl	BeginFontIO


;
; BeginFontIO - prepare sequencer and graphics controller for font r/w
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	ax, cx, dx, flags
;
BeginFontIO	proc	near
		assume	ds:nothing
		push	bx
		mov	ax,cs
		mov	es,ax

		mov	ah,ERI_READRANGE
		mov	bx,offset OldSeqRegs
		mov	cx,00005h		;start index = 0; reg count = 5
		mov	dx,ERI_ID_SEQ
		int	010h

		mov	bx,offset OldGrphRegs
		mov	cx,00403h		;start index = 4; reg count = 3
		mov	dx,ERI_ID_GRAPH
		int	010h

		xor	ax,ax
		xor	bx,bx
		mov	cx,5
		mov	dx,SEQ_ADDR
bfiSeqLoop:
		mov	ah,cs:[NewSeqRegs][bx]
		out	dx,ax
		inc	ax
		inc	bx
		loop	bfiSeqLoop

		mov	al,4
		xor	bx,bx
		mov	cl,3
		mov	dl,GRAPH_ADDR AND 0FFh
bfiGrphLoop:
		mov	ah,cs:[NewGrphRegs][bx]
		out	dx,ax
		inc	ax
		inc	bx
		loop	bfiGrphLoop

		pop	bx
		ret
BeginFontIO	endp


		subttl	EndFontIO
		page


;
; EndFontIO - recover sequencer and graphics controller from font r/w
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	ax, cx, dx, flags
;
EndFontIO	proc	near
		assume	ds:nothing
		push	bx
		xor	ax,ax
		xor	bx,bx
		mov	cx,5
		mov	dx,SEQ_ADDR
efiSeqLoop:
		mov	ah,cs:[OldSeqRegs][bx]
		out	dx,ax
		inc	ax
		inc	bx
		loop	efiSeqLoop

		mov	al,4
		xor	bx,bx
		mov	cl,3
		mov	dl,GRAPH_ADDR AND 0FFh
efiGrphLoop:
		mov	ah,cs:[OldGrphRegs][bx]
		out	dx,ax
		inc	ax
		inc	bx
		loop	efiGrphLoop

		pop	bx
		ret
EndFontIO	endp


		subttl	SaveFontFile
		page


;
; SaveFontFile - save custom fonts to disk
;
; ENTRY
;	ds	=  cs
; EXIT
;	nc	=  success
;	cy	=  failure
; USES
;	ax, bx, cx, dx, flags
;
SaveFontFile	proc	near
		assume	ds:_TEXT
		push	di
		push	ds
		push	es

		mov	ax,cs
		mov	es,ax
		mov	bx,offset DC.dcFontBank
		mov	ax,ERI_CONTEXTINFO*256 + ERI_CI_FONTINFO
		int	010h

		mov	cx,FI_MAXINDEX
sffChkCustom0:
		cmp	byte ptr [DC.dcFontBank][di],FI_CUSTOM
		je	sffFoundCustom
		inc	di
		loop	sffChkCustom0

		clc
		jmp	short sffX
sffFoundCustom:
		push	si
		mov	si,[DC.dcFileNum]	;synthesize temp filename
		mov	dx,offset DC.dcSwapPath
		call	MakeTempFile
		pop	si
		jc	sffErr0

		call	BeginFontIO
		mov	cx,8*1024		;each bank has 8K of data
		mov	ax,0A000h
		mov	ds,ax			;font segment
sffSaveCustom:
		mov	ax,di
		mul	cx
		shl	ax,1			;each bank 16K apart
		mov	dx,ax			;font offset
		assume	ds:nothing

		WriteFile
		jc	sffErr1 		;did it work?
		cmp	ax,cx			;able to write all bytes?
		jne	sffErr1
sffChkCustom1:
		inc	di
		cmp	di,FI_MAXINDEX
		jae	sffFinishUp

		cmp	byte ptr cs:[DC.dcFontBank][di],FI_CUSTOM
		je	sffSaveCustom
		jmp	short sffChkCustom1
sffFinishUp:
		call	EndFontIO
		CloseFile
		clc
		jmp	short sffX
sffErr1:
		call	EndFontIO
		CloseFile
		mov	ax,cs
		mov	ds,ax
		mov	dx,offset DC.dcSwapPath
		RemoveFile
sffErr0:
		stc
sffX:
		pop	es
		pop	ds
		pop	di
		ret
SaveFontFile	endp


		subttl	RestFontFile
		page


;
; RestFontFile - restore standard fonts from ROM or custom fonts from disk
;
; NOTE: The ability to restore custom fonts from disk is currently broken
;	and therefore disabled.  For now, this routine only restores the EGA
;	internal 8x8 or 8x14 font set.
;
; ENTRY
;	tba
; EXIT
;	tba
; USES
;	tba
;
RestFontFile	proc	near
		assume	ds:nothing
		push	di
		push	ds

		mov	ax,cs
		mov	ds,ax
		assume	ds:_TEXT
		mov	[fFileOpen],FALSE
		xor	di,di
rffChkFont:
		mov	al,[DC.dcFontBank][di]
		cmp	al,FI_EMPTY
		jne	rffFoundFont
rffNextFont:
		inc	di
		cmp	di,FI_MAXINDEX
		jb	rffChkFont
		jmp	short rffDone
rffFoundFont:
		or	al,al
		jz	rffFoundCustom

		mov	ah,011h
		push	bx
		mov	bx,di
		int	010h
		pop	bx
		jmp	short rffNextFont

rffFoundCustom:
		cmp	[fFileOpen],TRUE
		je	rffReadFonts

		mov	al,FAC_READWRITE
		mov	dx,offset DC.dcSwapPath
		OpenFile
		jc	rffErr1

		mov	bx,ax
		mov	[fFileOpen],TRUE
rffReadFonts:
		call	BeginFontIO
		mov	ax,di
		mov	cx,8*1024
		mul	cx
		shl	ax,1
		mov	dx,ax			;dx = offset of bank to fill

		push	ds
		mov	ax,0A000h
		mov	ds,ax			;ds = seg of bank to fill
		assume	ds:nothing

		ReadFile
		pop	ds
		jc	rffErr1
		cmp	ax,cx
		jne	rffErr1

		call	EndFontIO
		jmp	short rffNextFont
rffErr1:
		call	OemBeep
		jmp	short rffNextFont
rffDone:
		cmp	[fFileOpen],TRUE
		jne	rffX

		mov	dx,offset DC.dcSwapPath
		CloseFile
		RemoveFile
rffX:
		clc
		pop	ds
		pop	di
		ret
RestFontFile	endp


_TEXT		ends
		end

