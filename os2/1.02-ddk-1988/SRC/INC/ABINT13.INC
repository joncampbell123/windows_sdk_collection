; copyright (C) Microsoft Corporation, 1988

; Fixed disk parameter table. This is the information that is kept in the
; CMOS database on the AT about the type of fixed disk(s) in the system.

FDParmTable	struc
	FDMaxCyl	dw	?	; Maximum number of cylinders
	FDMaxHd 	db	?	; Maximum number of heads
	FDAUnUse	dw	?	; Unused on an AT
	FDWritePComp	dw	?	; Starting write precompensation cyl
	FDBUnUse	db	?	; Unused on an AT
	FDControl	db	?	; Control byte
	FDCUnUse	db	3 dup (?) ; Unused on an AT
	FDLandZon	dw	?	; Landing zone
	FDSecTrk	db	?	; Number of sectors/track
	FDRes		db	?	; Reserved
FDParmTable	ends

Drive_Table	struc			; Rom Bios Drive Parameter Table
	DiskSPECIFY_1	db	?	; Specify bytes
	DiskSPECIFY_2	db	?
	MOTOR_TIMEOUT	db	?	; Motor timeout
	SECSIZE 	db	?	; Sector size
	EOT		db	?	; EOT - Should be either 15 or 9
	GAPLENGTH	db	?	; Gap length
	DTL		db	?	;
	FMTGapLength	db	?	; Gap length for format
	FMTFillByte	db	?	; Fill byte for format
	HeadSTTL	db	?	; Head settle time.
	Motor_Start	db	?	; Motor start time
Drive_Table	ends

; Rates for the various media in the drives.

RATE500 	equ	0		; 500 Kb/sec
RATE300 	equ	1		; 300 Kb/sec
RATE250 	equ	2		; 250 Kb/sec
RATEMAX 	equ	RATE250 	; Maximum rate index that we support

DelayLoad	equ	35		; 35 milliseconds to load head

; Locations in ROM data area of fixed disk parameter tables on an AT

FDTAB_0 	equ	41H*4		; First fixed disk  - Int 41H vector
FDTAB_1 	equ	46H*4		; Second fixed disk - Int 46H vector

; Status codes

FD_SDIO=	01000000B	; Transfer direction (0 -> controller)
FD_SRQM=	10000000B	; Controller ready for next data

; Commands to floppy disk controller

FD_CRESET=	007H		; Recalibrate drive
FD_CSENSE=	008H		; Sense interrupt status
FD_CSEEK=	00FH		; Seek to another track
FD_CREAD=	046H		; MFM read, skip deleted data
FD_CWRITE=	045H		; MFM write, don't skip deleted data
FD_CSPEC=	003H		; Special - step rate, head load/unload
FD_CFORMAT=	04DH		; MFM format track

; Hard disk controller commands

FX_NORETRY=	01H		; do not perform retries on i/o
FX_ECC= 	02H		; ECC mode during i/o
FX_CREAD=	20H		; read
FX_CWRITE=	30H		; write
FX_VERIFY=	40H		; verify track.
FX_SETP=	91H		; Set parameters

; Status bits

FX_BUSY=	10000000B	; Status port busy bit
FX_READY=	01000000B	; Status port ready bit
FX_DRQ= 	00001000B	; Data Request bit
FX_ERROR=	00000001B	; Error status

; I/O ports

; Floppy drives
FD_PSEL=	03F2H		; Controls drive select and motors
FD_PDAT=	03F5H		; Data transfer to/from controller
FD_PSTAT=	03F4H		; Controller status
FD_PCTRL=	03F7h		; Diskette control register

; XT hard drives
HD_PDAT=	0320H		; read/write data
HD_PSTAT=	0321H		; controller status
HD_PSEL=	0322H		; controller select
HD_PMSK=	0323H		; DMA and interrupt mask bits

; AT hard drives
FX_PDAT=	01F0H		; read/write data
FX_PWRP=	01F1H		; write precompensation cylinder register
FX_PERR=	01F1H		; error register
FX_PSECCNT=	01F2H		; sector count register
FX_PSECNUM=	01F3H		; sector number register
FX_PCYLL=	01F4H		; cylinder register (low)
FX_PCYLH=	01F5H		; cylinder register (high)
FX_PDRHD=	01F6H		; drive/head register
FX_PCMD=	01F7H		; command register
FX_PSTAT=	01F7H		; status register
FX_RFDR=	03F6H		; fixed disk register

PDMA=		0		; Base of ports for DMA control
PDMAX=		7FH		; Address extension regs for DMA
				;NOTE base address suitable for ch. 2 & 3 only
DMA_MSKON=	04H		; Mask on = 1 in bit 2
DMA_MSKOFF=	00H		; (Mask off = 0 in bit 2)

; copyright (C) Microsoft Corporation, 1988

FD_DMA= 	2		; floppy disk DMA channel
HD_DMA= 	3		; hard disk DMA channel

;  DMA controller fields: block mode, addr increment, no autoinitialize

DMA_READ=	44H		; DMA read command
DMA_WRITE=	48H		; DMA write command
DMA_VERIFY=	40H		; DMA verify command
DORmask=	0CH		; Not reset, enable DMA & interrupt
