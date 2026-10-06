; copyright (C) Microsoft Corporation, 1988

RomData SEGMENT AT 40H
	ORG	0H
ROMPARA LABEL	BYTE			; Used to compute offset to CRT_MODE
Rs232_Base	DW	4 DUP (?)	; Addresses of RS232 Adapters
Printer_Base	DW	4 DUP (?)	; Addresses of Printers
Equip_Flag	DW	?		; Installed hardware.
Mfg_Tst 	DB	?		; Initialization flag
Mem_Size	DW	?		; Memory Size in KBytes
Mfg_Err_Flag	DB	?		; Scratchpad for manufacturing
		DB	?		; error codes
Kb_Flag 	DB	?		; Keyboard shift status
Kb_Flag_1	DB	?		; Second byte of keyboard status
Alt_Input	DB	?		; Storage for alt keypad entry
BufferHead	DW	?
BufferTail	DW	?
KeyBuffer	DW	16 DUP (?)
KeyBufLen	equ	($-KeyBuffer)	; length of KeyBuffer
Seek_Status	DB	?		; Drive recalibration status
					; Bit 3-0 = Drive 3-0 needs
					; recal before next seek if bit=0
					; High bit is on if interrupt occurred
Motor_Status	DB	?		; Motor status
					; Bit 3-0 = Drive 3-0 is currently
					;  running.
					; Bit 7 = Current operation is a
					;  write, requires delay.
Motor_Count	DB	?		; ROM's timeout count
Diskette_Status DB	?		; Return Code status byte
CMD_BLOCK	LABEL	BYTE
HD_ERROR	LABEL	BYTE
NEC_Status	DB	7 DUP (?)	; Status bytes from NEC

;*	Start of CRT data area.
;
CRT_MODE	DB	?
CRT_COLS	DW	?
CRT_ROWS	EQU	24D		; Number of rows on the screen.
CRT_LEN 	DW	?
CRT_START	DW	?
CURSOR_POSN	DW	8 DUP(?)
CURSOR_MODE	DW	?
ACTIVE_PAGE	DB	?
ADDR_6845	DW	?
CRT_MODE_SET	DB	?
CRT_PALETTE	DB	?
CrtLen		EQU	($-CRT_MODE)	; length of screen state area

IO_ROM_INIT	DW	?		; Pntr to Optional IO ROM Init routine
IO_ROM_SEG	DW	?		; Pointer to IO Rom segment
INTR_FLAG	DB	?		; Flag indicates an interrupt happened

TickLow 	DW	1 DUP(?)	; Low timer tick count
TickHigh	DW	1 DUP(?)	; High order count
TickOfl 	DB	1 DUP(?)	; Overflow?

Bios_Break	DB	?		; Bit 7=1 if Break Key has been hit
Reset_Flag	DW	?		; WORD = 1234 if Ctrl-Alt-Del

Disk_Status1	DB	?		; Fixed disk status byte
Hf_Num		DB	?		; Count of fixed disk drives
Control_Byte	DB	?		; Control Byte drive options
PORT_OFF	DB	?		; Port offset
Print_Tim_Out	DB	4 DUP (?)	; Printer time out interval
RS232_Tim_Out	DB	4 DUP (?)	; Serial time out interval
Buffer_Start	DW	?
Buffer_End	DW	?		; Additional keyboard information

;****** 		EGA Specific information			******

ROWS	DB	?				; Rows on the screen
POINTS	DW	?				; BYTES PER Character
INFO	DB	?

;				 INFO
;	D7 - HIGH BIT OF MODE SET, CLEAR/NOT CLEAR REGEN
;	D6 - MEMORY  D6 D5 = 0 0 - 064K    0 1 - 128K
;	D5 - MEMORY	     1 0 - 192K    1 1 - 256K
;	D4 - RESERVED
;	D3 - ega active monitor (0), ega not active (1)
;	D2 - wait for display enable (1)
;	D1 - EGA HAS A MONOCHROME ATTACHED (1)
;	D0 - set c_type emulate active (0)

INFO_3	DB	?

; INFO_3
;	D7-D4	FEATURE BITS
;	D3-D0	SWITCHES

	EGA_LEN 	EQU	$-ROWS

;---------------------------------------
;	ADDITIONAL FLOPPY DATA	       ;
;---------------------------------------
	ORG	8BH
LASTRATE	DB	1 DUP(?)	; LAST DATA RATE SELECTED
;---------------------------------------
;	ADDITIONAL HARD FILE DATA      :
;---------------------------------------
	ORG	8CH
HF_STATUS	DB	1 DUP(?)	; STATUS REGISTER
HF_ERROR	DB	1 DUP(?)	; ERROR REGISTER
HF_INT_FLAG	DB	1 DUP(?)	; HARD FILE INTERRUPT FLAG
HF_CNTRL	DB	1 DUP(?)	; COMBO HARD FILE/FLOPPY CARD BIT 0=1

; Floppy disk data
	org	090H
DSK_STATE	DB	?		; Drive 0 media state
		DB	?		; Drive 1 media state

DSK_OP_STATE	DB	2 DUP (?)	; Drive operation start state

DSK_TRK 	DB	?		; Current cylinder for Drive 0
		DB	?		; Current cylinder for Drive 1
		DB	?		; Reserved

	org	0A1H
Net_Semaphores	DB	?		; Address of network DMA interlock sem
	org	0A4H
Net_Int_13_Rout DD	?		; Storage for disk I/O routine for ROM

save_ptr	DD	?
	EGA_LEN2	EQU	$-save_ptr

;----- save_ptr
;
;	save_ptr is a pointer to a table as described as follows :
;
;	dword_1 	video parameter table pointer
;	dword_2 	dynamic save area pointer
;	dword_3 	alpha mode auxiliary char gen pointer
;	dword_4 	graphics mode auxiliary char gen pointer
;	dword_5 	reserved
;	dword_6 	reserved
;	dword_7 	reserved
;
;	dword_1 	Parameter Table Pointer
;			Initialized to BIOS EGA parameter table.
;			This value MUST exist.
;
;	dword_2 	Parameter Save area pointer
;			Initialized to 0000:0000, this value is optional.
;			When non-zero, this pointer will be used as pointer
;			to a RAM area where certain dynamic values are to
;			be saved. When in EGA operation this RAM area will
;			hold the 16 EGA palette register values plus
;			the overscan value in bytes 0-16d respectively.
;			At least 256 bytes must be allocated for this area.
;
;	dword_3 	Alpha Mode Auxiliary pointer
;			Initialized to 0000:0000, this value is optional.
;			When non-zero, this pointer is used as a pointer
;			to a tables described as follows :
;
;			byte	bytes/character
;			byte	block to load, should be zero for normal
;				operation
;			word	count to store, should be 256d for normal
;				operation
;			word	character offset, should be zero for normal
;				operation
;			dword	pointer to a font table
;			byte	displayable rows
;				if 'FF' the maximum calculated value will be
;				used, else this value will be used
;			byte	consecutive bytes of mode values for which
;				this font description is to be used.
;				The end of this stream is indicated by a
;				byte code of 'FF'
;
;	dword_4 	Graphics Mode Auxiliary pointer
;			Initialized to 0000:0000, this value is optional.
;			When non-zero, this pointer is used as a pointer
;			to a tables described as follows :
;
;			byte	displayable rows
;			word	bytes per character
;			dword	pointer to a font table
;			byte	consecutive bytes of mode values for which
;				this font description is to be used.
;				The end of this stream is indicated by a
;				byte code of 'FF'
;
;	dword_5 thru dword_7
;			Reserved and set to 0000:0000.
;
	ORG	100h
PrtScStatus	DB	0	;

	ORG	104H		; Point to 0:504
ActPDrv DB	0		; Last unit accessed by DOS.

RomData ENDS

;*	Net Semaphore definitions.
;
NET_DMA_FLAG	EQU	080H	; DMA SEMAPHORE

	BREAK	<Equipment flag definitions>

;*	These are definitions of the flags contained in the EQUIP_FLAG
;	variable.
;
F_FLOPPY	EQU	00001H		; Machine has a floppy drive.
F_87		EQU	00002H		; Machine has an 8087/80287
F_MONITOR	EQU	00030H		; Monitor type field

F_COLOR_40	EQU	00010H		; Initial video mode 40X25 BW
F_COLOR_80	EQU	00020H		; Initial video mode 80X25 BW
F_MONOCHROME	EQU	00030H		; Initial video mode 80X25 Monochrome

F_NUMDRIVE	EQU	000C0H		; Number of floppy drives
F_SERIAL	EQU	00E00H		; Number of serial cards attached
F_PRINTER	EQU	0C000H		; Number of printers attached.

;*	Definitions for SEEK_STATUS field
F_FLOPPY_INTR	EQU	080H		; Flag set in SEEK_STATUS when
					; a floppy interrupt occurs.


BREAK	<Keyboard info flags>
BREAK_F EQU	80H			; Break of <KEY> has 80H added to val.

ALT_KEY     equ 56			; Scan code for the ALT key
PS_KEY	    equ 55			; Scan code for the PrtScrn key
NUMLOCK_KEY equ 69			; Scan code for the NumLock key
SHIFT_F equ	03h			; both right and left shift
ALT_F	equ	08h
CTRL_F	equ	04h

BREAK	<Display attribute definitions>
C_F_BLUE	EQU	01H		; Foreground blue color bit
C_F_GREEN	EQU	02H		; Foreground green color bit
C_F_RED 	EQU	04H		; Foreground red color bit
C_B_BLUE	EQU	10H		; Background blue color bit
C_B_GREEN	EQU	20H		; Background green color bit
C_B_RED 	EQU	40H		; Background red color bit
C_BLINK 	EQU	80H		; Blinking attribute bit
C_INTENS	EQU	08H		; Intensity attribute bit

;*	Common attributes
C_F_WHITE	EQU	C_F_RED OR C_F_GREEN OR C_F_BLUE
C_B_WHITE	EQU	C_B_RED OR C_B_GREEN OR C_B_BLUE
C_F_YELLOW	EQU	C_F_RED OR C_F_GREEN
C_F_MAGENTA	EQU	C_F_RED OR C_F_BLUE
C_F_CYAN	EQU	C_F_GREEN OR C_F_BLUE
C_F_BLACK	EQU	0		; NO COLORS - ATTRIBUTE 0.


BREAK	<CRT MODE definitons>
;
;	These are type definitions for the various magic CRT mode
;	parameters in the BIOS.
;
;
MODE_BW40	EQU	00H
MODE_CO40	EQU	01H
MODE_BW80	EQU	02H
MODE_CO80	EQU	03H
MODE_GRBW40	EQU	04H
MODE_GRCO40	EQU	05H
MODE_GR80	EQU	06H
MODE_MONOCHROME EQU	07H
MODE_GRH40	EQU	0DH
MODE_GRH80	EQU	0EH
MODE_GRHMONO	EQU	0FH
MODE_GRHIGH	EQU	10H

MODE_NORETRACE	EQU	04H		; For modes 4 and higher we don't
					; have to wait for retrace.


BREAK	<MACHINE TYPE DEFINITIONS>
;
; Equates for high ROM
;

; Model bytes
mb_PC1	equ	0FFh			; PC-1, Expansion, Old PC2 (PC)
mb_PC2	equ	0FEh			; Newer PC-2		   (XT)
mb_JR	equ	0FDh			; JR
mb_AT	equ	0FCh			; AT





















