page ,132

;
; Copyright (c) Microsoft Corporation,  1988
;

include devhdr.inc

;**	Static Reguest Header
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

CMDMedChk	equ	1			; Media Check
MedChkmedia	equ	byte ptr PktData	; last media byte seen
MedChkflag	equ	byte ptr PktData+1	; -1=change 0=dont know 1=no change
MedChkpVID	equ	dword ptr PktData+2	; pointer to VID
LENMedChk	equ	6

CMDBldBPB	equ	2			; build BPB
BldBPBmedia	equ	byte ptr PktData	; media byte
BldBPBbuffer	equ	dword ptr PktData+1	; scratch buffer
BldBPBpBPB	equ	dword ptr PktData+5	; returned pointer to BPB
LENBldBPB	equ	9

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
IOPhysRBA	equ	dword ptr PktData+7	; physical starting sector
IOsknum 	equ	byte ptr PktData+7	; screen/kbd number (char)
IOsscod 	equ	word ptr PktData+8	; sub-screen code (char)
LENIO		equ	13

CMDNDR		equ	5			; non-destructive read
NDRbyte 	equ	byte ptr PktData	; byte read from device
LENNDR		equ	1

CMDInputS	equ	6			; input status
CMDOutputS	equ	10			; output status
LENStatus	equ	0

CMDInputF	equ	7			; input flush
CMDOutputF	equ	11			; output flush
LENFlush	equ	0

CMDOpen 	equ	13			; device open
CMDClose	equ	14			; device close
LENOpnCls	equ	0

CMDRemMed	equ	15			; is media removable
LENRemMed	equ	0

CMDStart EQU	98				; start console output
CMDStop EQU	99				; stop console output
CStpSKG EQU	BYTE PTR PktData+0		; Screen/Keyboard number
LenStp	EQU	1				; Size of request additions.

CMDGenIOCTL	EQU	16			; Generic IOCTL
GIOCategory	EQU	BYTE PTR PktData	; Category Code
GIOFunction	EQU	BYTE PTR PktData+1	; Function code
GIOParaPack	EQU	DWORD PTR PktData+2	; pointer to parameter packet
GIODataPack	EQU	DWORD PTR PktData+6	; pointer to data packet
LENGenIOCTL	EQU	10			; Packet is 10 bytes long


CMDDeInstall	EQU	20			; De-Install driver
LENDeIn 	EQU	0

CMDResMed	EQU	17			; reset media uncertain
LENResMed	equ	0

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

;*****************************************
;Equates used with PhysDiskTbl structure *
;*****************************************

physdisk_entry	 EQU  0200h	       ;Equate for a Physical Disk Entry in
				       ;in the SFT

;***********************************************
; PhysDiskTbl - Physical Disk Entry Structure  *
;***********************************************

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