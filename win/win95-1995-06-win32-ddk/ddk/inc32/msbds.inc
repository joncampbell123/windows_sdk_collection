;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;
;   Values for various flags in bds_flags
;

fnon_removable	    equ     01h 	;for non-removable media
fchangeline	    equ     02h 	;if changeline supported on drive
return_fake_bpb     equ     04h 	; when set, don't do a build bpb
					; just return the fake one
good_tracklayout    equ     08h 	; the track layout has no funny sectors
fi_am_mult	    equ     10h 	;if more than one logical for this physical
fi_own_physical     equ     20h 	;signify logical owner of this physical
fchanged	    equ     40h 	;indicates media changed
set_dasd_true	    equ     80h 	; set dasd before next format
fchanged_by_format  equ    100h 	;media changed by format
unformatted_media   equ    200h 	;an001; fixed disk only
x13_access	    equ	   400h		; access drive through extended i13s
dmf_floppy          equ    800h         ; DMF floppy is mounted, so fail writes
;
; DMF fence :: Any access which crosses this DMF_FENCE sector has to be
;		broken into multiple acesses, (since some stupid BIOS'
;		do not pay attention to EOT set in DPT)
;
DMF_FENCE		equ	18

;
;   Bits 12-13 are used for tracking double-buffer status for a drive.
;   They start out as 00, and incremented (with dblbuffer_inc) every time
;   we performed a trial read into a UMB that succeeds.  Once the bits hit
;   10 (dblbuffer_pass), then we assume the controller for that drive can
;   handle linear != physical without double-buffering.  On the other hand,
;   if a trial read ever fails, then all the bits are set (dblbuffer_force),
;   to force double-buffering all the time.
;
;   dblbuffer_mask is simply a mask that isolates the bits for testing.
;
dblbuffer_inc       equ   1000h
dblbuffer_pass      equ   2000h
dblbuffer_force     equ   3000h
dblbuffer_mask      equ   3000h

;
;   Various form factors to describe media
;

ff48tpi 	equ	0
ff96tpi 	equ	1
ffsmall 	equ	2
ffhardfile	equ	5
ffother 	equ	7
ff288		equ	9		; 2.88 MB drive


RBPB		STRUC			; RECOMMENDED BPB (also known as A_BPB)
		dw	512		; bytes per sector
		db	1		; sectors/alloc unit
		dw	1		; reserved sectors for dos
		db	2		; number of allocation tables
		dw	0E0h		; number directory entries
		dw	9*40		; number 512 byte sectors
		db	0F0h		; media descriptor, initially 0f0h
		dw	2		; number of fat sectors
		dw	9		; sector limit
		dw	2		; head limit
		dd	0		; hidden sector count
		dd	0		; number sectors
		db	6 dup (?)
RBPB		ENDS


BDS_FLOP	STRUC	    ; HARD values
bdf_link	dd	-1  ;		; link to next bds
bdf_drivenum	db	0   ; 80	; int 13 drive number
bdf_drivelet	db	0   ; 3 	; dos drive number
;
;   We want to embed a BPB declaration here, but we can't initialize
;   it properly if we do, so we duplicate the byte/word/dword architecture
;   of the BPB declaration.
;
bdf_BPB 	dw	512 ;		; BPB_BYTESPERSECTOR
		db	-1  ; 1 	; BPB_SECTORSPERCLUSTER
		dw	1   ;		; BPB_RESERVEDSECTORS
		db	2   ;		; BPB_NUMBEROFFATS
		dw	64  ; 16	; BPB_ROOTENTRIES
		dw	9*40; 0 	; BPB_TOTALSECTORS
		db	0   ; 0F8h	; BPB_MEDIADESCRIPTOR
		dw	2   ; 1 	; BPB_SECTORSPERFAT
		dw	9   ; 0 	; BPB_SECTORSPERTRACK
		dw	1   ; 0 	; BPB_HEADS
		dd	0   ;		; BPB_HIDDENSECTORS
		dd	0   ;		; BPB_BIGTOTALSECTORS

bdf_fatsiz	db	0		; flags...
bdf_opcnt	dw	0		; open ref. count
bdf_formfactor	db	3		; form factor index
bdf_flags	dw	0020h		; various flags
bdf_ccyln	dw	40		; max number of cylinders
;
;   We want to embed a BPB declaration here, but we can't initialize
;   it properly if we do, so we duplicate the byte/word/dword architecture
;   of the BPB declaration.
;
bdf_RBPB	dw	512		; bytes per sector
		db	1		; sectors/alloc unit
		dw	1		; reserved sectors for dos
		db	2		; number of allocation tables
		dw	0E0h		; number directory entries
		dw	9*40		; number 512 byte sectors
		db	0F0h		; media descriptor, initially 0f0h
		dw	2		; number of fat sectors
		dw	9		; sector limit
		dw	2		; head limit
		dd	0		; hidden sector count
		dd	0		; number sectors
		db	6 dup (?)

bdf_track	db	-1		; last track accessed on drive
bdf_tim_lo	dw	-1  ; 1 	; time of last access. keep
bdf_tim_hi	dw	-1  ; 0 	; these contiguous.
bdf_volid	db	"NO NAME    ",0 ; volume id of medium
bdf_vol_serial	dd	0		; current volume serial number from boot record
bdf_filesys_id	db	"FAT12   ",0	; current file system id from boot record
BDS_FLOP	ENDS


BDS_HARD	equ	<BDS_STRUC>

BDS_STRUC	STRUC
bds_link	dd	-1		; link to next bds
bds_drivenum	db	80		; int 13 drive number
bds_drivelet	db	3		; dos drive number

;   We want to embed a BPB declaration here, but we can't initialize
;   it properly if we do, so we duplicate the byte/word/dword architecture
;   of the BPB declaration.

bds_BPB 	dw	512		; BPB_BYTESPERSECTOR
		db	1		; BPB_SECTORSPERCLUSTER
		dw	1		; BPB_RESERVEDSECTORS
		db	2		; BPB_NUMBEROFFATS
		dw	16		; BPB_ROOTENTRIES
		dw	0		; BPB_TOTALSECTORS
		db	0F8h		; BPB_MEDIADESCRIPTOR
		dw	1		; BPB_SECTORSPERFAT
		dw	0		; BPB_SECTORSPERTRACK
		dw	0		; BPB_HEADS
		dd	0		; BPB_HIDDENSECTORS
		dd	0		; BPB_BIGTOTALSECTORS

bds_fatsiz	db	0		; flags...
bds_opcnt	dw	0		; open ref. count
bds_formfactor	db	3		; form factor index
bds_flags	dw	0020h		; various flags
bds_ccyln	dw	40		; max number of cylinders

bds_RBPB	db  size RBPB dup (0)	; recommended BPB

bds_track	db	-1		; last track accessed on drive
bds_tim_lo	dw	1		; time of last access. keep
bds_tim_hi	dw	0		; these contiguous.
bds_volid	db	"NO NAME    ",0 ; volume id of medium
bds_vol_serial	dd	0		; current volume serial number from boot record
bds_filesys_id	db	"FAT12   ",0	; current file system id from boot record
BDS_STRUC	ENDS


;   The assembler will generate bad data for "size bds_volid", so we'll
;   define an equate here.

VOLID_SIZ	equ	12

bdsm_ismini	equ	bds_tim_lo	; overlapping bds_tim_lo
bdsm_hidden_trks equ	bds_tim_hi	; overlapping bds_tim_hi

MAX_FLOP	equ	4
MAX_BDS 	equ	29

