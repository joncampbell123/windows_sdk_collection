;-----------------------------------------------------------------
;
; Equates and Macros for the INT33 mouse interface
;
;-----------------------------------------------------------------

;
; Constants
;
HPMOUSE_SIZE	= 0A0H
TRACK_SIZE	= 280H

; Equates for EX-BIOS interrupt number and vector address.
HPENTRY 		equ	6FH
F_HPENTRY		equ	HPENTRY
HP_ID			equ	4850H	;'HP'

V_STRACK		equ	005AH	; Common cursor control funcs.
V_LHPMOUSE		equ	00CCH	; Microsoft/Mouse System's
					;   Compatible Driver
F33_INQUIRE		equ	6F00H	; HP inquire function
F_IO_CONTROL		equ	02H*2	; Device/Driver Dependent
					;   Functions
SF_MOUSE_DSIZE		equ	0006H	; Return sizes of HPMouse and HPTrack

;
; SetMouse flags
;
SM_RESET		equ    00001b	; Just reset the mouse
SM_SHOW_CURSOR		equ    00010b	; Show the mouse cursor
SM_HIDE_CURSOR		equ    00100b	; Hide the mouse cursor
SM_SET_POSITION 	equ    01000b	; Set the cursor position
SM_CURSOR_TYPE		equ    10000b	; Set the cursor type
SM_MENU_MODE		equ    11010b	; Reset, show, and position cursor

;
; Mouse Cursor Masks
;
MOUSE_CURSOR		equ	0B1H
BUTTON_FLAGS		equ	01EH	;Mask for User routine Button changes
RIGHT_DOWN		equ	008H	;User Routine Right button down bit
LEFT_DOWN		equ	002H	;User Routine Left button down bit
RIGHT_STATE		equ	002H	;Mouse Status Right Button State bit
LEFT_STATE		equ	001H	;Mouse Status Left Button State bit

;MouseInit Flags

MOUSE_INIT		equ	001H	;Mouse has been initialized
MOUSE_REMOVED		equ	002H	;Mouse has been removed but not inited
MOUSE_CLEAR		equ	000H	;Mouse not initialized
;
; Character Cell Size
MOUSE_CELL_HEIGHT	equ	8
MOUSE_CELL_WIDTH	equ	8

;
; Macros
;

RESET_MOUSE		macro
	xor	ax, ax
	int	33h
	endm

SHOW_MOUSE_CURSOR	macro
	push	ax
	mov	ax, 1
	int	33h
	pop	ax
	endm

HIDE_MOUSE_CURSOR	macro
	push	ax
	mov	ax, 2
	int	33h
	pop	ax
	endm

GET_MOUSE_STATUS	macro
	mov	ax, 3
	int	33h
	endm

SET_MOUSE_POSITION	macro
	mov	ax, 4
	int	33h
	endm

GET_BUTTON_RELEASE	macro
	mov	ax, 6
	xor	bx, bx
	int	33h
	shr	cx, 1
	shr	cx, 1
	shr	cx, 1
	shr	dx, 1
	shr	dx, 1
	shr	dx, 1
	mov	dh, dl
	mov	dl, cl
	endm

;	Macros for disabling and restoring hardware interrupt enable flag
;
;	The LeaveCrit macro has been updated for the mask problem on
;	the 80286 processor.


EnterCrit   MACRO
	pushf
	cli
ENDM

LeaveCrit   MACRO
	POPFF
ENDM

POPFF	MACRO
	local	a
	jmp	$+3
a	label	near
	iret
	push	cs
	call	a
ENDM

;
;	SYSCALL [vector_address]
;
syscall 		macro	vector
 ifnb			 <vector>
			mov	bp,vector
 endif
			int	HPENTRY
			endm


