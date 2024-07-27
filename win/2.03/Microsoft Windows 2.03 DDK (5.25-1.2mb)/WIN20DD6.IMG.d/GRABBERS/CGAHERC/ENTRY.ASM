
		page	,132
		%out	Grabber Entrypoints
		name	ENTRY
		title	Copyright (C) by Microsoft Inc. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		extrn	InquireGrab:near
		extrn	EnableGrab:near
		extrn	DisableGrab:near
		extrn	PromptGrab:near
		extrn	InquireSave:near
		extrn	SaveScreen:near
		extrn	RestoreScreen:near
		extrn	InitScreen:near

		extrn	EndPaint:near
		extrn	BeginPaint:near
		extrn	MarkBlock:near
		extrn	PutBlock:near
		extrn	GetBlock:near
		extrn	RealizeColor:near
		extrn	GetVersion:near
		extrn	DisableSave:near
		extrn	EnableSave:near
		extrn	SetSwapDrive:near
		extrn	GetInfo:near
		extrn	GetID:near

		public	StdFuncTable
		public	ExtFuncTable



;
; Entrypoint jump table
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
;	our functions are near.
;
;	Winoldap checks for the existence of the jmp opcode at offset 015h,
;	and if it exists, assumes that the InitScreen entrypoint is present.
;	If present, InitScreen will be called when an oldap starts up, and
;	subsequently after every context switch from Windows to that oldap.
;

		org	0

StdFuncTable	label	word
		jmp	InquireGrab		;Func 00001h
		jmp	EnableGrab		;Func 00002h
		jmp	DisableGrab		;Func 00003h
		jmp	PromptGrab		;Func 00004h
		jmp	InquireSave		;Func 00005h
		jmp	SaveScreen		;Func 00006h
		jmp	RestoreScreen		;Func 00007h
		jmp	InitScreen		;Func 00008h


ExtFuncTable	label	word
		dw	EndPaint		;Func 0FFF4h
		dw	BeginPaint		;Func 0FFF5h
		dw	MarkBlock		;Func 0FFF6h
		dw	PutBlock		;Func 0FFF7h
		dw	GetBlock		;Func 0FFF8h
		dw	RealizeColor		;Func 0FFF9h
		dw	GetVersion		;Func 0FFFAh
		dw	DisableSave		;Func 0FFFBh
		dw	EnableSave		;Func 0FFFCh
		dw	SetSwapDrive		;Func 0FFFDh
		dw	GetInfo 		;Func 0FFFEh
		dw	GetID			;Func 0FFFFh


_TEXT		ends
		end

