;
; copyright (C) Microsoft, 1988
;
BREAK <Device table and Standard Request Header definition>
page ,132

include devhdr.inc

;**	Static Request Header
;
;	This structure defines the fixed first part of a device request
;	record.  Depending upon the request there may be other fields
;	which follow this header.
;

SrHead	STRUC
ReqLen	DB	?		; Length in bytes of request block
ReqUnit DB	?		; Device unit number
ReqFunc DB	?		; Type of request
ReqStat DW	?		; Status Word
	DD	?		; RESERVED
DevLink DD	?		; link field for use by driver to chain
				;   requests.  (A service in DevHelp)
SrHead	ENDS


;*	Other fields beyond the standard 13 byte header


; Status word masks
STERR	EQU	8000H		; Bit 15 - Error
STINTER EQU	0400H		; Bit 10 - Interim character
STBUI	EQU	0200H		; Bit  9 - Busy
STDON	EQU	0100H		; Bit  8 - Done
STECODE EQU	00FFH		; Error code
WRECODE EQU	0

;
;	Replacement definitions for existing device driver definitions.
;	NOTE:	These are provided for completeness - In order to
;		keep modular integrity, we define these here (the
;		volume ID code wants to know if a driver supports
;		volume ID stuff, not that it is DEVLEV_3.
;

DEV_40		EQU	DEVLEV_3
DEV_HASVID	EQU	DEVLEV_3
DEV_320 	EQU	DEVLEV_2	  ; For block devices, this
					  ; device supports Set/Get Map of
					  ; logical drives, and supports
					  ; Generic IOCTL calls.
					  ; For character devices, this
					  ; device supports Generic IOCTL.
					  ; This is a DOS 3.2 device driver.

;**	Device Driver Request Packet Defintions
;


PktMax		equ	18
PktHead 	equ	13

Packet		struc
PktLen		db	?		; length in bytes of packet
PktUnit 	db	?		; subunit number of block device
PktCmd		db	?		; command code
PktStatus	dw	?		; status word
PktDOSLink	dd	?		; reserved
PktDevLink	dd	?		; device multiple-request link
PktData 	db	PktMax dup (?)	; data pertaing to specific packet
Packet		ends

CMDInit 	equ	0			; INIT command
InitcUnit	equ	byte ptr PktData	; number of units returned
InitpEnd	equ	dword ptr PktData+1	; pointer to free mem after dev
InitDevHlp	equ	dword ptr PktData+1	; address of Device Helper router
InitEcode	equ	word ptr PktData+1	; size of code segment
InitEdata	equ	word ptr PktData+3	; size of data segment
InitParms	equ	dword ptr PktData+5	; pointer parameters
InitpBPB	equ	dword ptr PktData+5	; pointer to BPBs
Initdrv 	equ	byte ptr PktData+9	; drive no. assigned to unit 0
						; for resident drivers only:
InitSysiData	equ	byte ptr PktData+11	; SysInit's DGROUP selector

LENInit 	equ	10			; non-resident drivers
LENInitRes	equ	(LENInit + 2)		; resident drivers
ERRNZ		<PktMax LT LENInit>
ERRNZ		<PktMax LT LENInitRes>

CMDMedChk	equ	1			; Media Check
MedChkmedia	equ	byte ptr PktData	; last media byte seen
MedChkflag	equ	byte ptr PktData+1	; -1=change 0=dont know 1=no change
MedChkpVID	equ	dword ptr PktData+2	; pointer to VID
LENMedChk	equ	6
ERRNZ		<PktMax LT LENMedChk>

CMDBldBPB	equ	2			; build BPB
BldBPBmedia	equ	byte ptr PktData	; media byte
BldBPBbuffer	equ	dword ptr PktData+1	; scratch buffer
BldBPBpBPB	equ	dword ptr PktData+5	; returned pointer to BPB
LENBldBPB	equ	9
ERRNZ		<PktMax LT LENBldBPB>

CMDIOCTLR	equ	3			; reserved for 3.x compatability
CMDINPUT	equ	4			; read data from device
CMDOUTPUT	equ	8			; write data to device
CMDOUTPUTV	equ	9			; write data and verify
CMDInputBypass	     EQU    24			; cache bypass read data	?? Needed any more ??
CMDOutputBypass      EQU    25			; cache bypass write data	?? Needed any more ??
CMDOutputBypassV     EQU    26			; cache bypass write data and verify  ?? Needed any more ??
CMDIOCTLW	equ	12			; reserved for 3.x compatability
IOmedia 	equ	byte ptr PktData	; media byte
IOpData 	equ	dword ptr PktData+1	; transfer address
IOcount 	equ	word ptr PktData+5	; count of bytes/sectors
IOstart 	equ	word ptr PktData+7	; starting sector (block)
						; starting sector is a DD
IOSFN		equ	word ptr PktData+11
IOPhysRBA	equ	dword ptr PktData+13	; physical starting sector - Used
						    ; by disk device driver
LENIO		equ	13
ERRNZ		<PktMax LT LENIO>

ReadBypassAdj	equ	CMDInputBypass-CMDInput ; used internally by kernel
WriteBypassAdj	equ	CMDOutputBypass-CMDOutput ; used internally by kernel

CMDNDR		equ	5			; non-destructive read
NDRbyte 	equ	byte ptr PktData	; byte read from device
LENNDR		equ	1
ERRNZ		<PktMax LT LENNDR>

CMDInputS	equ	6			; input status
CMDOutputS	equ	10			; output status
LENStatus	equ	0
ERRNZ		<PktMax LT LENStatus>

CMDInputF	equ	7			; input flush
CMDOutputF	equ	11			; output flush
LENFlush	equ	0
ERRNZ		<PktMax LT LENFlush>

CMDOpen 	equ	13			; device open
CMDClose	equ	14			; device close
OCSFN		equ	word ptr PktData	; sfn of open instance for virtualization
OCLastRef	equ	word ptr PktData+2	; flag indicating last close
LENOpnCls	equ	4
ERRNZ		<PktMax LT LENOpnCls>

CMDRemMed	equ	15			; is media removable
LENRemMed	equ	0
ERRNZ		<PktMax LT LENRemMed>

CMDStart EQU	98				; start console output
CMDStop EQU	99				; stop console output
CStpSKG EQU	BYTE PTR PktData+0		; Screen/Keyboard number
LenStp	EQU	1				; Size of request additions.
ERRNZ		<PktMax LT LenStp>

CMDGenIOCTL	EQU	16			; Generic IOCTL
GIOCategory	EQU	BYTE PTR PktData	; Category Code
GIOFunction	EQU	BYTE PTR PktData+1	; Function code
GIOParaPack	EQU	DWORD PTR PktData+2	; pointer to parameter packet
GIODataPack	EQU	DWORD PTR PktData+6	; pointer to data packet
GIOSFN		EQU	WORD PTR PktData+10
LENGenIOCTL	EQU	12
ERRNZ		<PktMax LT LENGenIOCTL>


CMDDeInstall	EQU	20			; De-Install driver
LENDeIn 	EQU	0
ERRNZ		<PktMax LT LENDeIn>

CMDResMed	EQU	17			; reset media uncertain
LENResMed	equ	0
ERRNZ		<PktMax LT LENResMed>

CMDAddOnPrep	EQU	97			; Prepare for add on
Add_On_NewDD	EQU	DWORD PTR PktData
Add_On_SrvRout	EQU	DWORD PTR PktData
LenAddOn	EQU	4

CMDGetLogMap	EQU	18
CMDSetLogMap	EQU	19
Logical_Drive	EQU	BYTE PTR PktData; logical drive number
LenGetSetLog	EQU	1

CMDPartfixeddisks	EQU  22 	   ; Partitionable Fixed Disks
CMDGetfd_logunitsmap	EQU  23 	   ; Get Fixed Disk/Logical Unit Map

STDDERROR	     EQU     4000H		 ; Equate for Device Driver Defined Error
ERROR_DEVICEDRIVER_DEFINED_BASE EQU 0FE00H	 ; Equate for Device Driver Defined Error High Byte

;
; The next three symbols are used in the files DEV.ASM and SPROC.ASM to
; set up and verify the set of request packets that the kernel keeps in
; a tiled segment, and to control req pkt allocation and initialization.
;
; SwapPackets sets the number of request packets reserved for the swapper
; pool.  Need one for the swapper operation, one for kernel use at volume
; switch time, one for DD use at volume switch time.
;
; StaticPackets sets the number of packets that we always allocate.  We
; currently allocate one pkt per thread + StaticPackets.  This is mostly
; to cover our butts, because maximum system pkt use is non-deterministic,
; and in the worst case unbounded.  We use 16 because it's a power-of-two
; and seems to be enough.  16 is arbitrary rather than magic.
;
; The one pkt per thread allocation is based on the fact that most threads
; will never use more than one packet.	(And we have StaticPackets extra
; for the ones that do.)
;
; See: dev.asm, sproc.asm
;
StaticPackets	 EQU	16
SwapPackets	 EQU	3

ReqPktSize	EQU	32		; size for packet (power of 2)
.ERRNZ	(SIZE Packet - 31)		; Device Driver packet size has changed
;


;  *****************************************
;  Equates used with PhysDiskTbl structure *
;  *****************************************

physdisk_entry	 EQU  0200h	       ;@@ Equate for a Physical Disk Entry in
				       ;@@  in the SFT

;  ***********************************************
;   PhysDiskTbl - Physical Disk Entry Structure  *
;  ***********************************************

 diskentry  STRUC	     ; 

 ddheader	DD  0	     ; Pointer to Device Driver Header
 ereserved	DB  0	     ; Reserved
 reldisknum	DB  0	     ; Relative Physical Disk # for Device Driver
 devpid 	DW  0	     ; PID
 deveflags	DW  0	     ; Flags for the physical disk
 logunitbitmap	DD  0	     ; Logical Unit Bit Map

 diskentry  ends	     ; 

SUBTTL

