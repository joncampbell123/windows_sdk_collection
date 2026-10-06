;
; Copyright (c) Microsoft Corporation,  1988
;
MAXDRIVELETTERS equ	26		; maximum number of drive letters (A-Z)
NUMDISKCMDS	equ	26		; number of commands (0 based)
DSKADR		equ	4*1EH
BIOSEG		equ	70H		; Segment for BIOS
ErrLim		equ	5		; Number of retries on error
NBS		equ	2		; # of bytes per sector (512)
SCRATCHSIZE	equ	512		; size of scratch buffer

; Interrupt control

INT_CONT	equ	020H		; 8259 interrupt controller.
INT_CONT2	equ	0A0H		; Second 8259 controller
EOI		equ	20H		; End of Interrupt
IRQ_Flop	equ	06H		; Floppy interrupt IRQ #
IRQ_Fixed	equ	0EH		; Fixed interrupt IRQ #

; Timer values

TIMER2COUNT	equ	11		; Reload value for timer2
THOUSAND	equ	1000		; used to convert seconds to milliseconds
GLOBINFOSEG	equ	1		; system global info segment
TIMEWAITTIME	equ	3		; seconds to wait before drive not ready

; Dma Overrun				; 

DMATIMEDELAY	equ	2		; wait 2 seconds afer dma error 
VIDEODELAY	equ	1		; delay to allow video to write 

; Indices for CurIndex field

INDEX99 	equ	0		   ; 96tpi in 96tpi drive   (5.25")
INDEX49 	equ	1		   ; 48tpi in 96tpi drive   (5.25")
INDEX44 	equ	2		   ; 48tpi in 48tpi drive   (5.25")
INDEX77 	equ	3		   ; 720 KB in 720KB	    (3.5")
INDEX71 	equ	4		   ; 720 KB in 1.44 MB	    (3.5")
INDEX11 	equ	5		   ; 1.44 MB in 1.44 MB     (3.5")

MAXINDEX	equ	INDEX11      ; Max. index that we support for AT or 7552

; Definitions of DISKOP functions

DISKOP_INTERNREAD	equ	0		; internal read
DISKOP_READ		equ	CMDINPUT	; read
DISKOP_WRITE		equ	CMDOUTPUT	; write
DISKOP_WRITEV		equ	CMDOUTPUTV	; write/verify
DISKOP_IOCTL		equ	CMDGenIOCTL	; IOCTL
DISKOP_NCREAD		equ	24		; read without cache
DISKOP_NCWRITE		equ	25		; write without cache
DISKOP_NCWRITEV 	equ	26		; write/verify without cache
DISKOP_CHKCHNG		equ	NUMDISKCMDS+1	; used on AT
DISKOP_RESET		equ	NUMDISKCMDS+2	; used on NOTAT
DISKOP_GetDevParms	equ	NUMDISKCMDS+3	; USED ON NOTAT
DISKOP_SETDEVPARMS	equ	NUMDISKCMDS+4	; USED ON NOTAT 
DISKOP_SETMEDIATYPE	equ	NUMDISKCMDS+5	; USED ON NOTAT
DISKOP_READBACK 	equ	NUMDISKCMDS+6	; USED ON NOTAT


PktBlkSize     equ     byte ptr PktData 	; Block size for SetDevParms
PktReadGap     equ     byte ptr PktData+1	; Read Gap for SetDevParms
PktDataLen     equ     byte ptr PktData+2	; Data Len for SetDevParms

; BPB media bytes

MB_35_18Sect	equ	0F0H		; 3.5 inch / 1.44 MB	       2/18
MB_HardDrive	equ	0F8H		; hard disk
MB_96		equ	0F9H		; 96TPI hi density drive       2/15
MB_35_9Sect	equ	0F9H		; 3.5 inch / 720KB	       2/9
MB_48_9Sect1S	equ	0FCH		; 48TPI 9 sector single sided  1/9
MB_48_9Sect	equ	0FDH		; 48TPI 9 sector double sided  2/9
MB_48_8Sect1S	equ	0FEH		; 48TPI 8 sector single sided  1/8
MB_48_8Sect	equ	0FEH		; 48TPI 8 sector	       2/8

; Disk Device Driver Trace Equates

TRACE_MAJOR	  EQU	68H	  ; Trace major code

TRACE_READ	  EQU	1	  ; Trace Read minor code
TRACE_WRITE	  EQU	2	  ; Trace Write minor code
TRACE_WRITEV	  EQU	3	  ; Trace Write verify minor code
TRACE_CMPLT	  EQU	80h	  ; Trace Complete mask

; Cache Data area size

size_CacheStat	  EQU	08h	  ; Size of Cache Stats in ATCACHE.ASM
