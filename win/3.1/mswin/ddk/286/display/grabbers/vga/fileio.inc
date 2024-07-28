
;
; NAME
;	FILEIO.INC
;
; DESCRIPTION
;	This file defines DOS constants, equates, and macros for handling the
;	Unix-style file I/O calls.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;


;
; Define DOS file access codes
;

FAC_READ	=	0
FAC_WRITE	=	1
FAC_READWRITE	=	2


;
; Define DOS file attribute codes
;

FA_NORMAL	=	00000000b
FA_READONLY	=	00000001b
FA_HIDDEN	=	00000010b
FA_SYSTEM	=	00000100b
FA_VOLLABEL	=	00001000b
FA_DIRECTORY	=	00010000b
FA_ARCHIVE	=	00100000b


;
; Define DOS file I/O macros
;

CreateFile	macro
		mov	ah,03Ch
		int	021h
		endm


OpenFile	macro
		mov	ah,03Dh
		int	021h
		endm


CloseFile	macro
		mov	ah,03Eh
		int	021h
		endm


ReadFile	macro
		mov	ah,03Fh
		int	021h
		endm


WriteFile	macro
		mov	ah,040h
		int	021h
		endm


RemoveFile	macro
		mov	ah,041h
		int	021h
		endm

