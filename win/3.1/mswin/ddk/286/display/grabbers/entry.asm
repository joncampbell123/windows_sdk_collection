
		page	,132
		%out	Grabber Entrypoints
		name	ENTRY
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	ENTRY.ASM
;
; DESCRIPTION
;	This file contains a jump table for the original grabber entrypoints
;	and a function table used by InquireGrab to satisfy extended function
;	calls.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;	1.01    080989   ac     IFDEFed functions not needed in version 3.0
;       1.02    080989   ac     IFDEFed out portions which have changed or are
;                               not needed under windows 3.0 winoldaps.


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		extrn	InquireGrab:near

IFDEF	GRAB_VER_2

		extrn	EnableGrab:near
		extrn	DisableGrab:near
		extrn	PromptGrab:near
		extrn	MarkBlock:near
		extrn	PutBlock:near
		extrn	RealizeColor:near
		extrn	GetID:near

ENDIF	;GRAB_VER_2

		extrn	InquireSave:near
		extrn	SaveScreen:near
		extrn	RestoreScreen:near
		extrn	InitScreen:near

		extrn	EndPaint:near
		extrn	BeginPaint:near
		extrn	GetBlock:near
		extrn	GetVersion:near
		extrn	DisableSave:near
		extrn	EnableSave:near
		extrn	SetSwapDrive:near
		extrn	GetInfo:near

		public	StdFuncTable
		public	ExtFuncTable
		public	ReadVideoBufFromFile
		public	WriteVideoBufToFile
		public	TempSaveAreaSize
		public	TempSaveAreaSeg
		public	TempSaveAreaOff


;
; STANDARD FUNCTION DISPATCH TABLE
;
;	Since the grabbers are loaded by Winoldap as a binary image, Winoldap
;	transfers control to them via a jump table at offset 0 of the grabber
;	code segment.  With the exception of InitScreen, which is an optional
;	entrypoint, the format of this table MUST remain fixed and MUST reside
;	at offset 0!
;
;	Winoldap computes the offset of the desired entrypoint using the
;	knowledge that a near jump is 3 bytes of opcode.  However, as Winoldap
;	will be making a far call to the jmp, we MUST return far even though
;	our functions are near.  Winoldap must always set ds equal to the
;	grabber's cs before making the jump.
;
;	Winoldap checks for the existence of the jmp opcode at offset 015h,
;	and if it exists, assumes that the InitScreen entrypoint is present.
;	If present, InitScreen will be called when an oldap starts up, and
;	subsequently after every context switch from Windows to that oldap.
;

		org	0

StdFuncTable	label	word
		jmp	InquireGrab		;Func 00001h

IFDEF	GRAB_VER_2
		jmp	EnableGrab		;Func 00002h
		jmp	DisableGrab		;Func 00003h
ELSE
		jmp	ObsoleteFunction	;NOP
		jmp	ObsoleteFunction	;NOP
ENDIF	;GRAB_VER_2

IFDEF	GRAB_VER_2
		jmp	PromptGrab		;Func 00004h
ELSE
		jmp	ObsoleteFunction	;NOP
ENDIF	;GRAB_VER_2

		jmp	InquireSave		;Func 00005h
		jmp	SaveScreen		;Func 00006h
		jmp	RestoreScreen		;Func 00007h
		jmp	InitScreen		;Func 00008h


;
; EXTENDED FUNCTION DISPATCH TABLE
;
;	Due to the nature of the table above, any extensions to the
;	Winoldap/grabber interface must be made via subfunction calls to an
;	existing standard function entrypoint if we are to avoid crashing
;	due to Winoldap jumping to an entrypoint that does not exist.
;	The standard function used for this purpose is the benign InquireGrab
;	call.  InquireGrab will dispatch control to one of the routines below
;	if upon entry ax contains a function number in the indicated range.
;
;	We are guaranteed not to crash with older Winoldaps written to the
;	Microsoft OEM specification since ax will always contain 1 or 2
;	indicating an original InquireGrab buffer size request.  Older
;	grabbers used with the new Winoldap will not crash either, since they
;	will either interpret an ax value other than 1 or 2 as an invalid
;	request or blindly return an undefined buffer size.  The GetID and
;	GetVersion routines below are provided so that Winoldap can compare
;	the return value with a standard and cease to call the extended
;	interface if the value returned is inconsistent with the value
;	expected.
;

ExtFuncTable	label	word
		dw	EndPaint		;Func 0FFF4h
		dw	BeginPaint		;Func 0FFF5h

IFDEF	GRAB_VER_2
		dw	MarkBlock		;Func 0FFF6h
		dw	PutBlock		;Func 0FFF7h
ELSE
		dw	ObsoleteSubFunction	;NOP
		dw	ObsoleteSubFunction	;NOP
ENDIF	;GRAB_VER_2

		dw	GetBlock		;Func 0FFF8h

IFDEF	GRAB_VER_2
		dw	RealizeColor		;Func 0FFF9h
ELSE
		dw	ObsoleteSubFunction	;NOP
ENDIF	;GRAB_VER_2

		dw	GetVersion		;Func 0FFFAh
		dw	DisableSave		;Func 0FFFBh
		dw	EnableSave		;Func 0FFFCh
		dw	SetSwapDrive		;Func 0FFFDh
		dw	GetInfo 		;Func 0FFFEh

IFDEF	GRAB_VER_2
		dw	GetID			;Func 0FFFFh
ELSE
		dw	ObsoleteSubFunction	;NOP
ENDIF	;GRAB_VER_2


; define some local storage for the size of the save area and a pointer to it

TempSaveAreaSize dw	?		;size of save area
TempSaveAreaSeg	 dw	?		;segment of save area
TempSaveAreaOff	 dw	?		;offset of save area


;----------------------------------------------------------------------------;
; WriteVideoBufToFile:							     ;
;									     ;
; Does the write of a buffer to a file. The write is buffered through the    ;
; save  area provided by winoldap. This is done as on some machines, i/o to  ;
; hard disk will be DMAed and this may not work for video memory.            ;
; (this routine will be called to write out data from video memory only).    ;
;									     ;
;	DS,DX,CX,BX are set up on entry for an INT 21H/AH=40H write call     ;
;       exit paremeters are also similar to the return from the above call   ;
;       except that AX = CX always, but carry will be set if a short write   ;
;       has been done.							     ;
;----------------------------------------------------------------------------;

WriteVideoBufToFile	proc near

	push	es
	push	si
	push	di			;save, these registers will be thrashed.
	push	cx			;save write count
	push	dx			;save source offset
	mov	es,cs:TempSaveAreaSeg	;segment of temp buffer
	mov	di,cs:TempSaveAreaOff	;offset of temp buffer
	mov	si,dx			;get the source offset

LoopBufToFile:
	
	clc				;assume no error
	jcxz	WriteVideoBufToFileDone	;done.
	mov	ax,cs:TempSaveAreaSize	;get size of temp buffer
	cmp	cx,ax			;can be buffer all of remaining ?
	ja	@f			;no.
	mov	ax,cx			;amount to transfer
@@:
	sub	cx,ax			;update count left to write
	push	cx			;save
	mov	cx,ax			;count to arite in this attempt
	push	cx			;save count
	push 	di			;save pointer
	shr	cx,1			;will do word moves
	rep	movsw			;do the move
	jnc	@f			;was a word sized move
	movsb				;move the last byte
@@:
	pop	di			;restore the last pointer
	pop	cx			;restore xfer count
	push	ds			;save ds
	push	es
	pop	ds			;write to file from temp segment
	mov	dx,di			;ds:dx points to temp buffer
	mov	ah,40h			;write code
	int	21h			;write to file
	pop	ds			;restore original source segment
	cmp	ax,cx			;set carry is wriiten < asked
	pop	cx			;get back count left to write
	jnc	LoopBufToFile		;continue writing

WriteVideoBufToFileDone:

	pop	dx
	pop	cx
	mov	ax,cx			;get amount wriiten
	pop	di
	pop	si
	pop	es			;restore registers

WriteVideoBufToFileRet:

	ret				;carry set if error.

WriteVideoBufToFile	endp
;----------------------------------------------------------------------------;
; ReadVideoBufFromFile:							     ;
;									     ;
; Reads a file back into abuffer. The read in buffered through the buffer    ;
; provided by winoldap. This averts DMA problem from videobuffer, if any.    ;
;									     ;
;	DS,DX,CX,BX are set up on entry for an INT 21H/AH=3FH read call      ;
;       exit paremeters are also similar to the return from the above call   ;
;       except that AX = CX always, but carry will be set if a short read    ;
;       has been done.							     ;
;----------------------------------------------------------------------------;

ReadVideoBufFromFile	proc near

	push	es
	push	si
	push	di			;save, these registers will be thrashed.
	push	cx			;save Read count
	push	dx			;save source offset
	mov	es,cs:TempSaveAreaSeg	;segment of temp buffer
	mov	si,cs:TempSaveAreaOff	;offset of temp buffer
	mov	di,dx			;get the source offset

LoopBufFromFile:
	
	clc				;assume no error
	jcxz	ReadVideoBufFromFileDone;done.
	mov	ax,cs:TempSaveAreaSize	;get size of temp buffer
	cmp	cx,ax			;can be buffer all of remaining ?
	ja	@f			;no.
	mov	ax,cx			;amount to transfer
@@:
	sub	cx,ax			;update count left to Read
	push	cx			;save
	mov	cx,ax			;count to arite in this attempt
	push	ds			;save
	push	es
	pop	ds			;ds points to temp buffer
	mov	dx,si			;read in here
	mov	ah,3fh			;read code
	int	21h			;read in the first part
	pop	ds
	cmp	ax,cx			;set carry if written < asked
	jc	ReadVideoBufError	;error in read, abort now.
	push	si			;save source pointer
	push	ds
	push	es			;save
	mov	ax,ds			;get actual destination
	push	es	
	pop	ds			;ds:si has temp buffer
	mov	es,ax			;es:di has actual buffer
	shr	cx,1			;want to doa word move
	rep	movsw			;move it
	jnc	@f			;word alligned move
	movsb				;move the last byte
@@:
	pop	es
	pop	ds
	pop	si			;restore registeres
	pop	cx			;restore count left
	jmp	short  LoopBufFromFile	;continue banding

ReadVideoBufError:
	
	pop	cx			;restore pushed value
	stc				;set error

ReadVideoBufFromFileDone:

	pop	dx
	pop	cx
	mov	ax,cx			;get amount read
	pop	di
	pop	si
	pop	es

ReadVideoBufFromFileRet:

	ret

ReadVideoBufFromFile endp
;----------------------------------------------------------------------------;
; ObsoleteFunction:							     ;
;									     ;
; This function traps the entry level functions that are no longer needed    ;
; in the version 3 of grabbers. It sets carry and does a RETF.		     ;
;----------------------------------------------------------------------------;
ObsoleteFunction proc near

	stc				;error, unsupported function

farp_dummy proc far			;want to have far ret
	ret
farp_dummy endp

ObsoleteFunction endp
;----------------------------------------------------------------------------;
; ObsoleteSubFunction:							     ;
;									     ;
; This function traps those subfunction of InquireGrab that are no longer    ;
; supported under the 3.0 grabber. It simply sets the carry and does a       ;
; near return.								     ;
;----------------------------------------------------------------------------;

ObsoleteSubFunction  proc near

	stc				;error, unsupported sub function
	ret

ObsoleteSubFunction  endp
;----------------------------------------------------------------------------;

_TEXT		ends
		end

