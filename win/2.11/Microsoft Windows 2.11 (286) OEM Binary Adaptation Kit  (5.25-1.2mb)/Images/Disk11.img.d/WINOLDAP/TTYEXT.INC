;+++++
;
;	DOS function Equates
;
SET_DMA 	equ	1AH
SET_VECTOR	equ	25H
GET_DMA 	equ	2FH
GET_VERSION	equ	30H
GET_VECTOR	equ	35H
DOS_VECTOR	equ	21H		; DOS interrupt vector
;
CREATE_FILE	equ	3C00H
OPEN_FILE	equ	3D00H
CLOSE		equ	3EH
READ		equ	3FH
WRITE		equ	40H
;
ALLOC_MEM	equ	48H
SETBLOCK	equ	4AH
EXEC		equ	4BH
GET_RETURN	equ	4DH		
EXEC_PROG	equ	0H		; Load/Exec program subfunction
;
SET_PDB 	equ	50H
GET_PDB 	equ	51H
GET_DOS_DATA	equ	52H		; Get pointer to DOS global data
;
;+++++
;
;	Winoldap Move directions
;
UP		equ	0		; Low to High memory
DOWN		equ	1		; High to Low
EXT_MODE	equ	1		; Extended memory mode (Windows swapped)
SAVE_W		equ	1		; Save DOS app
;
;+++++
;
;	Interrupt vectors
;
INT12_CS	equ	(4 *12H) +2	; Int 12H CS location in vector table
INT09_CS	equ	(4 *9) +2
INT_COM1_CS	equ	(4 *0CH) +2
INT_COM2_CS	equ	(4 *0BH) +2
INT16_CS	equ	(4 *16H) +2
INT2F_CS	equ	(4 *2FH) +2	; Clipboard dispatcher ~~tqn 052287
;
INT00_IP	equ	0
INT1F_IP	equ	4 *1FH
Int00_1F_Size	equ	(INT1F_IP -INT00_IP)/2 +2 ; Block size in words containing
						   ; int08 to int1FH vectors
INT70_IP	equ	4 *70H
INT77_IP	equ	4 *77H
Int70_77_Size	equ	(INT77_IP -INT70_IP)/2 +2  ; Block size in words containing
;						   ; int70 to int 77 vectors
HWBlock_Size	equ	Int00_1F_Size +Int70_77_Size
;
;+++++
;
TRUE		equ	1
FALSE		equ	0
PDB_PARAS	equ	20H		; DOS PDB + Environment size
PDB_SIZE	equ	10H		; DOS PSP size in paras
SwapArea	equ	0800H		; Minimum swap area   (32 Kb)
SwapAreaEMS	equ	0280H		; Minimum area in EMS (10 kb)
SwapAreaOpt	equ	1900H		; Optimum size of swap area (100 Kb)
TOP_DOS		equ	0A000H		; Top of DOS (640 kb)
EGA_BUFFER_SIZE	equ	02000h		; EGA hi-res display size in paras

;
EXTEND_MASK	equ	TRUE		; Extended switch mask
;
SF2_SIZE	equ	28H		; DOS 2.x System file entry size in bytes
SF3_SIZE	equ	35H		; DOS 3.x System file entry size in bytes
;
;++++++
;
;	SwapTable Entry and DOS EXEC data structures
;
SwapBStruc	Struc
  SWAddress	dw	?		; Block segment address
  SWSize	dw	?		; its size in paras
SwapBStruc	Ends
;
SWLength	equ	(SIZE SwapBStruc)
;
EXECBLock	Struc
  Environment	dw	?
  ComLine	dd	?
  FCB1		dd	?
  FCB2		dd	?
EXECBlock	Ends
;
;	DOS system file table structure
;
SFT		Struc
  sftlink	dd	?	; link to next table (-1 = last table)
  sftcount	dw	?	; number of entries in table
  sftfile	db	?	; first FCB structure in table
SFT		ends
;
;+++++
;
;	Exec error messages
;
Msg_not_enough_disk	equ	85H	; Not enough swap disk space
Exec_not_enough_memory	equ	08H
Exec_BAD_format 	equ	0BH
Msg_swap_win		equ	0DH	; COM in use. Cannot swap Windows

;
;	New EXE HEADER equates
;
NE_RESTAB		equ	26h	; -> Offset to Module name structure
