;
; This include file defines the PIF file offsets of the various PIF
; values we are interested in
;
; SEE ALSO:
;	page 7-18 of TopView programmers manual
;

PTITLELEN	equ	30			; Length of title field
PPATHLEN	equ	63			; Length of program name field
PATHMAX 	equ	64			; Length of Initial directory field
COMMAX		equ	64			; Length of parameter field

piffile struc
		db	2 DUP(?)		; Reserved
PTITLE		db	PTITLELEN DUP(?)	; Program name (caption)
MAXMEMWORD	dw	?			; Max memory
MINMEMWORD	dw	?			; Min memory
PPATHNAME	db	PPATHLEN DUP(?) 	; Program name (file name)
MSFLAGS 	db	?			; Microsoft behavior bits
		db	?			; Reserved
INITIALDIR	db	PATHMAX DUP(?)		; Initial directory
INITIALCOM	db	COMMAX DUP(?)		; Program parameters
SCREENTYPE	db	?			; We don't use this
SCREENPAGES	db	?			; Number of screen pages
INTVECLOW	db	?			; Low bound of INT vectors
INTVECHIGH	db	?			; high bound of INT vectors
ROWS		db	?			; We don't use this
COLUMNS 	db	?			; We don't use this
ROWOFFS 	db	?			; We don't use this
COLOFFS 	db	?			; We don't use this
SYSTEMMEM	dw	?			; System memory
SHAREDPROG	db	64 DUP(?)		; We don't use this
SHAREDDATA	db	64 DUP(?)		; We don't use this
BEHAVBYTE	db	?			; TopView behavior bits
SYSTEMFLAGS	db	?			; TopView behavior bits
piffile ends

INTVECWORD	equ word ptr INTVECLOW
PIFLEN		equ SIZE piffile		; length of a PIF file

;
; NOTE: The IBM TopView defines the length of PPATHNAME as 64, we support
;	one byte less than this, the last byte being MSFLAGS
;
