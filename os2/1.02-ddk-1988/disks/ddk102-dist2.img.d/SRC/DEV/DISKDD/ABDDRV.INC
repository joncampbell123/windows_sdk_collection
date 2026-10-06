;
; Copyright (C) Microsoft, 1988
;
	include devsym.inc

RBD	equ	070H			; max length is now 128 bytes

DMATIMEDELAYCNT   equ	  120	  ; 120 retries

;**	The following structure is used to store all information about
;	the state of the current I/O operation on a drive
;
;	Note that the "Current" field indicates the drive currently being
;	accessed / most recently accessed via the hardware.  The Unit
;	field indicates the logically active unit.  In the case of single-
;	drive floppy systems Unit indicates the pseudo drive in effect;
;	Current indicates the physical drive in use.

IOStruc struc

    Queue	dd	0		; queue anchor
    Active	db	0,0		; semaphore
    State	db	0		; driver state
    Current	db	-1		; current logical drive #
		db	0		;    accessible as word
    PhysDrv	db	-1		; current physical drive number
		db	0		;    accessible as word
    ErrCnt	dw	0		; # of errors this request.
    Flags	dw	0		; various flags for I/O
    CacheFlg	dw	0		; Flags for cache
    DOR 	db	0		; copy of Select/Motor reg

; Following values are set by Setup from the request packet and are
; updated after each transfer is completed.

    First	dd     0	       ; 1st sector of request
    RealAddr	dd     0	       ; saved addr when using ScratchBuffer
    Count	dw     0	       ; Number of sectors to xfer

; Following used for fixed disks on an AT to keep track of which sector
; has been read in or written out, since we do them one at a time

    CurrAddr	dd     0	       ; Pointer to data area/buffer
    SecToGo	dw     0	       ; Number of sectors left in request

; Following values are set by MapSector.

    Cyl 	dw   0		     ; Cylinder
    Sector	db   0		     ; zero based sector
    Head	db   0		     ; Head
    NumSectors	dw   0		     ; Number of sectors to do
    NumBytes	dw   0		     ; Number of bytes
    Addr	dd   0		     ; Pointer to addr
    SectorSize	db   2		     ; Sector size index
    XferNumSec	DW	0	    ; Number of sectors as part of request
    XferNumBytes DW	0	    ; Number of bytes as part of request

; Following values set by CacheSector if fCache bit set in Fixed.Flags

    XferSec	DB	0	    ; 0-based sector index into cache buffer
    XferAddr	DD	0	    ; Address of buffer in cache (if used)

IOStruc ends


; Device dependent data.  Not used if ROM is called to do disk I/O

ATStruc struc

	  db  SIZE IOStruc dup (?)
  ST0	  db	  0		  ; floppy controller ST0
  ST1	  db	  0		  ; floppy controller ST1
  ST2	  db	  0		  ; Floppy controller ST2
  CHRN	  db	  0,0,0,0,0	  ; other floppy status returns.

ATStruc ends


DCB		equ	ST1		; fixed disk Device Control Block

; Flags for IOStruc Flags field

fACTIVE 	equ	001H		;
fREAD		equ	002H		;
fWRITE		equ	004H		;
fVERIFY 	equ	008H		;
fFORMAT 	equ	010H		;
fWRAP		equ	020H		; we are currently using ScratchBuffer
fSPECIAL	equ	040H		; not an I/O operation
fIOCTL		equ	080H		; IOCTL I/O operation
fINTERNAL	equ	100H		; internal BIOS request
fCALCULATE	equ	400H		; request needs CHS calculation
fDODONEREQUEST	equ	800H		; request is complete and so is cache
fMORE		equ    1000H		; cache has broken the request into
					; multiple reads
fDMAREADBACK	equ    8000H		; set if readback has been done

;*** Flag bits for the CacheFlg field in IOStruc
;
; The flags are used as follows:
;
;	- fCach_NoCache - Set by SetUp to indicate that this I/O request is
;			  not to go through the cache. This is determined from
;			  the command code in the request packet.
;
;	- fCach_Active	- Set by CacheSector to indicate that this part of the
;			  I/O request is going through the cache, i.e. a cache
;			  buffer had been allocated for the transfer.
;
;	- fCach_Reset	- The I/O into the cache encountered an error. Try the
;			  I/O without the cache. This is set by the Error state
;			  in the hard disk state machine if the I/O was going
;			  through the cache and it encountered an error.
;
;	- fCach_InvalidBuf - This is set by CacheSector to indicate the the
;			  full request will by-pass the cache. This would be
;			  true for all WRITEs, and for READs that involve a
;			  transfer of more than the cut-off sector count.
;
;	- fCach_Hit	- Set by CacheHit (CacheSector) to indicate the the
;			  data requested is already in the cache.
;
fCach_NoCache	    equ     0001H	     ; do not cache this request
fCach_Active	    equ     0002H	     ; set by CacheInit if caching is ON
fCach_Reset	    equ     0004H	     ; the reset command
fCach_InvalidBuf    equ     0008H
fCach_Hit	    equ     0010H


;** This structure describes each physical device or hard disk
;   partition.

; Flags for DrvFlag

Frestor 	    equ     1H		; restore needed
Fmotoron	    equ     2H		; motor is on
FUncert_Media	    equ     4H		; Media value unknown
FForceRdWrt	    equ     8H		; Force the next rd/wrt to succeed
FMedInd 	    equ     10H 	; We have correct index for medium in drive

; Flags for DrvTypFlag - indicate type of drive and medium

fNon_Removable	    equ     0001H	; For non-removable media
fChangeline	    equ     0002H	; If changeline supported on drive
fNoDOSPartition     equ     0004H	; Fixed disk with no DOS partition
GOOD_TRACKLAYOUT    equ     0008H	; The track layout has no funny sectors
fI_Am_Mult	    equ     0010H	; If more than one logical for this physical
fI_Own_Physical     equ     0020H	; Signify logical owner of this physical
fChanged	    equ     0040H	; Indicates media changed
SET_DASD_true	    equ     0080H	; Set DASD before next format
fChanged_By_Format  equ     0100H	; Media changed by format
RETURN_FAKE_BPB     equ     0200H	; When set, don't do a build BPB
fchangedNotAT	    equ     8000H	; When set media change reported @p2061

;** DriveStruc
;
;  One of these control blocks exists for each physical device and logical
;  fixed disk partition. They form the "BDS chain" linked list anchored at
;  Start_BDS.

DriveStruc  struc

Link		dw	?	; link to next BDS
DriveUnit	db	?	; device driver unit #
DriveLet	db	?	; DOS drive number
DriveNum	db	?	; INT 13H drive number
BPBsecsiz	dw	512	; 0	Physical sector size
BPBsecpau	db	1	; 2	Sectors/Allocation unit
BPBressec	dw	1	; 3	Reserved sectors for DOS
BPBnfat 	db	2	; 5	# of allocation tables
BPBndir 	dw	224	; 6	# of directory entries
BPBnsec 	dw	2400	; 8	Number of sectors
BPBmediab	db	0F9H	; 10	Media descriptor
BPBnfatsec	dw	7	; 11	# of FAT sectors
BPBtrksiz	dw	15	; 13	# of sectors/track
BPBnhead	dw	2	; 15	# of heads
BPBhidsec	dw	0	; 17	Hidden sector count
BPBhidsecH	dw	0	; 19   MSW of hidden sector count
BPBlargetotal	dd	0	; 21	Large total sectors
		db	6 dup (?)      ; For extended BPB
BPBRsecsiz	dw	512	; 0	Physical sector size
BPBRsecpau	db	1	; 2	Sectors/Allocation unit
BPBRressec	dw	1	; 3	Reserved sectors for DOS
BPBRnfat	db	2	; 5	# of allocation tables
BPBRndir	dw	64	; 6	# of directory entries
BPBRnsec	dw	9*40	; 8	Number of sectors
BPBRmediab	db	0FCH	; 10	Media descriptor
BPBRnfatsec	dw	2	; 11	# of FAT sectors
BPBRtrksiz	dw	9	; 13	# of sectors/track
BPBRnhead	dw	1	; 15	# of heads
BPBRhidsec	dw	0	; 17	Hidden sector count
BPBRhidsecHi	dw	0	;* 19	MSW of hidden sector count
		db	10 dup (?)	;* For extended BPB
;* 		db	12 dup (?)	; For extended BPB
FatSiz		db	0	; Non zero if 16 bit FAT
TotalPhysCyln	dw	40	; Max. cylinders on device
cCyln		dw	40	; Total cylinders in partition
Last_Access	dd	0	; Time of last access
Timer		db	0	; Countdown for motor off
DrvFlag 	dw	Frestor + FForceRdWrt
CurCyl		dw     -1	; Current cylinder
CurIndex	db	0	; Current index for medium in drive
LastInd 	db	0	; last one used for this drive
WrtPrCmp	dw	0	; hard disk write precompensation cyl
FormFactor	db	0	; form factor index
DrvTypFlag	dw	fI_Own_Physical + fChanged
ParentDisk	dw	-1	; offset to parent DriveStruc
IOSoffset	dw	-1	; offset of IOStruc for this device
PartitionOffset dd	0	; sector offset to start of partition
FillByteFor	db	?	; Fill Byte for Format
DataLength	db	?	; Data length for Set Media type
FormatGap	db	?	; Format Gap for Set Media type
CmosUnitType	dw	?	; Unit Type in Cmos
RetryCnt	db	5	; Retry Count
DeviceControl	dw	0	; Device COntrol Flags
DmaOverrunCnt	db	DMATIMEDELAYCNT ; Dma Overrun Retry Count
RWVGap		db	?	; Read gap for rwv 
PartitionNumber	db	0	;* Primary DOS Partition no. (always 0 for IBM)
DriveStruc	ends

; Various form factors to describe media

ff48tpi 	    equ     0
ff96tpi 	    equ     1
ffSmall 	    equ     2
ff35_9		    equ     2
ffHardFile	    equ     5
ff35_18 	    equ     7
ffOther 	    equ     8

;   FatSiz values

fTooBig EQU	80H
fBIG	EQU	40H
