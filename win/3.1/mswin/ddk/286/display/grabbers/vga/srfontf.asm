
		page	,132
		%out	Save/RestFontFile
		name	SRFONTF
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	SRFONTF.ASM
;
; DESCRIPTION
;	This file contains the routines needed to save and restore the
;	EGA's alpha font banks in plane 2 to and from a disk file.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;	1.10	071588	vvr	Removed the ERI dependency and read directly
;				from VGA registers
;	1.11	062989	vvr	Fixed a bug to save and restore all eight
;				banks properly;
;

_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT

		.xlist
		include dc.inc
		include vga.inc
		include fileio.inc
		.list

		extrn	DC:byte
		extrn	OemBeep:near
		extrn	MakeTempFile:near
		extrn	GetRegSet:near
		extrn	RestRegSet:near
		extrn	ReadVideoBufFromFile:near
		extrn	WriteVideoBufToFile:near

		public	SaveFontFile
		public	RestFontFile
		public	RestBiosFonts
	

FALSE		=	0
TRUE		=	1

fFileOpen	db	FALSE			;TRUE = swapfile is open

OldSeqRegs	db	5 dup (?)		;temp save area for current
OldGrphRegs	db	3 dup (?)		;  sequencer and graphics regs

NewSeqRegs	db	03, 01, 04, 00, 07	;sequencer and graphics ctrlr
NewGrphRegs	db	02, 00, 04		;  reg values to r/w fonts



		subttl	BeginFontIO


;
; BeginFontIO - prepare sequencer and graphics controller for font r/w
;
;	When set up for standard alphanumeric operation, plane 2 is where
;	bitmaps for the alpha character generator on the EGA are stored.
;	However, plane 2 is not accessible until the Sequencer and Graphics
;	controllers are reprogrammed to allow such access.  Overall, this
;	is a very tricky operation because some EGA cards apparently loose
;	memory refresh when either the Sequencer or Graphics controller are
;	reprogrammed on the fly.
;
;	This routine requires that the EGA Register Interface (ERI, aka
;	EGA.SYS) be installed.	Without it, the results of EndFontIO will be
;	less than pleasing.
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	ax, cx, dx,flags
;
BeginFontIO	proc	near
		assume	ds:nothing

		push	di
		push	bx
		mov	ax,cs
		mov	es,ax			;es = cs

		mov	di,offset OldSeqRegs
		mov	cx,00005h		;start index = 0; reg count = 5
		mov	dx,SEQ_ADDR
		call	GetRegSet

		mov	di,offset OldGrphRegs	;save a range of grph regs
		mov	cx,00403h		;start index = 4; reg count = 3
		mov	dx,GRAPH_ADDR
		call	GetRegSet

		xor	ax,ax			;start at seq index 0
		xor	bx,bx
		mov	cx,5			;5 seq regs to write
		mov	dx,SEQ_ADDR
bfiSeqLoop:
		mov	ah,cs:[NewSeqRegs][bx]
		out	dx,ax
		inc	ax
		inc	bx
		loop	bfiSeqLoop

		mov	al,4			;start at grph index 4
		xor	bx,bx
		mov	cl,3			;3 grph regs to write
		mov	dl,GRAPH_ADDR AND 0FFh
bfiGrphLoop:
		mov	ah,cs:[NewGrphRegs][bx]
		out	dx,ax
		inc	ax
		inc	bx
		loop	bfiGrphLoop

		pop	bx
		pop	di
		ret
BeginFontIO	endp


		subttl	EndFontIO
		page


;
; EndFontIO - restore sequencer and graphics controller from font r/w
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	ax, cx, dx, di, flags
;
EndFontIO	proc	near
		assume	ds:nothing
		push	bx
		push	di

		mov	ax,cs
		mov	es,ax
		xor	ax,ax
		mov	cx,5
		mov	dx,SEQ_ADDR
		mov	di,offset OldSeqRegs
		call	RestRegSet
		
		mov	al,4				;start at grph index 4
		xor	bx,bx
		mov	cl,3				;3 grph regs to write
		mov	dl,GRAPH_ADDR AND 0FFh
efiGrphLoop:
		mov	ah,cs:[OldGrphRegs][bx]
		out	dx,ax
		inc	ax
		inc	bx
		loop	efiGrphLoop

		pop	di
		pop	bx
		ret
EndFontIO	endp


		subttl	SaveFontFile
		page


;
; SaveFontFile - save custom EGA fonts to disk
;
; ENTRY
;	di	=  0	(this is a bug and should be fixed in future)
;	ds	=  cs
; EXIT
;	cf	=  0
; ERROR EXIT
;	cf	=  1
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
		mov	ds,ax
		xor	di,di
		mov	cx,FI_MAXINDEX		;now scan thru it
sffChkCustom0:
		cmp	byte ptr [DC.dcFontBank][di],FI_CUSTOM	;if custom font,
		je	sffFoundCustom				;  process it
		inc	di			;else keep scanning
		loop	sffChkCustom0

		clc				;done scanning
		jmp	short sffX
sffFoundCustom:
		push	di
		push	si
		mov	si,[DC.dcFileNum]	;synthesize temp filename
		mov	dx,offset DC.dcSwapPath
		call	MakeTempFile
		pop	si
		pop	di
		jc	sffErr0

		call	BeginFontIO		;enable plane 2 addressing
		mov	cx,8*1024		;each bank has 8K of data
		mov	ax,0A000h		;and starts at this segment
		mov	ds,ax			;ds = seg of bank to read
;~vvr For VGA there are 8 banks
;		push	si			;save si
;		mov	si,di
		cmp	di,3
		mov	ax,di
		jle	sffSaveCustom
		sub	ax,4			;ax = bank index
sffSaveCustom:
;		mov	ax,di			
		mul	cx			;ax = ax * bank size
		shl	ax,1			;each bank 16K apart
;~vvr		
		cmp	di,3
		jle	sffEGACustom
	     	add	ax,8*1024		;add to offset of bank
sffEGACustom:
;		pop	si			;restore si
		mov	dx,ax			;dx = offset of bank to read
		
		assume	ds:nothing

		call	WriteVideoBufToFile	;write out to a file
		jc	sffErr1 		;if write failed, leave
		cmp	ax,cx			;if not all bytes written,
		jne	sffErr1 		;  leave
sffChkCustom1:
		inc	di			;else move to next bank
		cmp	di,FI_MAXINDEX		;if last bank processed,
		jae	sffFinishUp		;  then finish up

		cmp	byte ptr cs:[DC.dcFontBank][di],FI_CUSTOM
		je	sffSaveCustom
		jmp	short sffChkCustom1
sffFinishUp:
;		pop	si			;restore si
		call	EndFontIO		;restore EGA state
		CloseFile
		clc				;show success
		jmp	short sffX
sffErr1:
		call	EndFontIO		;restore EGA state
		CloseFile			;close file so we can delete it
		mov	ax,cs
		mov	ds,ax
		mov	dx,offset DC.dcSwapPath
		RemoveFile			;delete partial file
sffErr0:
		stc				;show error
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
; ENTRY
;	none
; EXIT
;	none
; USES
;	ax, bx, cx, dx, flags
; NOTES
; This routine just resotres custom fonts. Bios fonts would have been 
; restored earlier by the RestBiosFonts routine.
;
;	An error while trying to restore a custom font is not considered
;	fatal.	A beep will occur to indicate there was a problem, but the
;	algorithm will continue trying to restore the remainder of the banks.
;	This is so that at least ROM fonts and custom fonts that can be
;	restored without error will be properly loaded into the EGA when the
;	oldap context is restored.  There really isn't much else we can do.
;
RestFontFile	proc	near
		assume	ds:nothing
		push	di
		push	ds

		mov	ax,cs
		mov	ds,ax			;ds = cs
		assume	ds:_TEXT
		mov	[fFileOpen],FALSE	;show no file open yet
		xor	di,di
rffChkFont:
		mov	al,[DC.dcFontBank][di]	;get first font bank id
		cmp	al,FI_EMPTY		;if bank was not empty,
		jne	rffFoundFont		;  go load the font bank
rffNextFont:
		inc	di			;else move to next bank
		cmp	di,FI_MAXINDEX		;if not last bank,
		jb	rffChkFont		;  keep scanning
		jmp	short rffDone		;else finish up
rffFoundFont:

		or	al,al
		jnz	rffNextFont		;ignore BIOS fonts now

rffFoundCustom:
		cmp	[fFileOpen],TRUE	;if not the first custom font
		je	rffReadFonts		;  then file is already open
		
		push	di
		mov	al,FAC_READWRITE	;file access code
		mov	dx,offset DC.dcSwapPath
		OpenFile			;open font file
		pop	di
		jc	rffErr1 		;if error, beep, try next one
		

		mov	bx,ax			;bx = file handle
		mov	[fFileOpen],TRUE	;show file is open
rffReadFonts:
		call	BeginFontIO		;enable plane 2 addressing
;~vvr 
;		mov	ax,di			;ax = bank index
;		mov	cx,8*1024		;each bank has 8K of data
;		mul	cx			;ax = ax * bank size
;		shl	ax,1			;each bank 16K apart
;		mov	dx,ax			;dx = offset of bank to write
;~vvr

;~vvr For VGA there are 8 banks
;		push	si			;save si
;		mov	si,di
		mov	ax,di
		cmp	di,3
		jle	rffSaveCustom
		sub	ax,4			;ax = bank index
rffSaveCustom:
;		mov	ax,di			
		mov	cx,8*1024
		mul	cx			;ax = ax * bank size
		shl	ax,1			;each bank 16K apart
;~vvr	       
		cmp	di,3
		jle	rffEGACustom
	     	add	ax,8*1024		;add to offset of bank
rffEGACustom:
;		pop	si			;restore si
		mov	dx,ax			;dx = offset of bank to read

		push	ds
		mov	ax,0A000h
		mov	ds,ax			;ds = seg of bank to write
		assume	ds:nothing

		call	ReadVideoBufFromFile	;read back fonts 
		pop	ds
		jc	rffErr1 		;if read failed, show error
		cmp	ax,cx			;if not all bytes read,
		jne	rffErr1 		;  show error

		call	EndFontIO		;restore EGA state
		jmp	short rffNextFont	;move to next bank
rffErr1:
		call	OemBeep 		;indicate a problem to user
		jmp	short rffNextFont	;we must try all fonts though
rffDone:
		cmp	[fFileOpen],TRUE	;if no font file opened
		jne	rffX			;  then none to close

		mov	dx,offset DC.dcSwapPath
		CloseFile			;else close the file
		RemoveFile			;and delete it
rffX:
		clc
		pop	ds
		pop	di
		ret
RestFontFile	endp

; This routine just restores the BIOS fonts, custom fonts would be restored
; later.


RestBiosFonts proc	near

		assume	ds:nothing
		push	di
		push	ds

		mov	ax,cs
		mov	ds,ax
		assume	ds:_TEXT
		xor	di,di
rbfChkFont:
		mov	al,[DC.dcFontBank][di]
		or	al,al		;custom font ?
		jz	rbfNextFont	;ignore it now.
		cmp	al,FI_EMPTY
		jne	rbfFoundFont
rbfNextFont:
		inc	di
		cmp	di,FI_MAXINDEX
		jb	rbfChkFont
		jmp	short rbfDone
rbfFoundFont:

		mov	ah,011h
		or	al,10h
		push	bx
		mov	bx,di
		int	010h
		pop	bx
		jmp	short rbfNextFont

rbfDone:
		clc
		pop	ds
		pop	di
		ret
RestBiosFonts	endp


_TEXT		ends
		end


