;
; Structure of the spart.par file
;
;
; WARNING WARNING DANGER DANGER!!!!!!!!!!!!!!!!!!!!!!!
;
;      THIS FILE MUST AGREE WITH SPART.H WHICH IS IN THE
;      SPART PROJECT OF WIN386. DO NOT CHANGE ONE WITHOUT
;      CHANGING THE OTHER!!!
;
;
OEM_INFO_MAX		EQU	128
WFP_MAX 		EQU	128
FREEMIN 		EQU	1024*1024*2
PARTMIN 		EQU	1024*1024
PARTMINMARGINAL 	EQU	1024*1024*8
INIT_OVER_COMMIT	EQU	4
;
; This is the aproximate size of the WIN386 fixed kernel
;
WIN386VMMSIZE		EQU	200

DOSPARTFILENM		EQU	"X:\386SPART.PAR",0
DOSPARTDIRNM		EQU	"386SPARTPAR",0

SPARTNAME		EQU	"\spart.par",0

PARTCURVERSION		EQU	00000300h

PFileForm struc
    PFileVersion	dd	?	; Version of partition file
    ;
    ; The following two fields are provided so that the location of the
    ;	  swap partition can be verified via reads and writes through DOS.
    ;
    ;	if PFileForm.PFileName[0] != 0
    ;	  DOS Open PFileForm.PFileName
    ;	      if open fails, swap partition file is corrupted
    ;	  Write Random Pattern Using DOS, 512 bytes at PFileForm.DOSEndOffset
    ;	      if write fails or returns differnt count written,
    ;		 swap partition file is corrupted
    ;	  Write Random Pattern Using DOS, 512 bytes at PFileForm.DOSStartOffset
    ;	      if write fails or returns differnt count written,
    ;		 swap partition file is corrupted
    ;
    ;	  Read using direct DISK call (INT 13, etc.) at location that should
    ;	      correspond to DOSStartOffset (first sector of partition) based
    ;	      on partition info in OEMField.
    ;		  Verify that pattern matches. If Not,
    ;		  swap partition file is corrupted.
    ;
    ;	  Read using direct DISK call (INT 13, etc.) at location that should
    ;	      correspond to DOSEndOffset (last sector of partition) based
    ;	      on partition info in OEMField.
    ;		  Verify that pattern matches. If Not,
    ;		  Swap partition file is corrupted.
    ;
    ; NOTE: The pattern needs to be random (use a date/time seed) because
    ;	    the contents of the swap partition may be randomly left from the
    ;	    the last time the verification of the partition was performed.
    ;
    DOSStartOffset	dd	?	; if PFileForm.PFileName is not NUL */
					;  this is the offset in that file */
					;  of the start sector of the partition   */
    DOSEndOffset	dd	?	; if PFileForm.PFileName is not NUL */
					;  this is the offset in that file */
					;  of the end sector of the partition	*/
    PFileName		db	WFP_MAX dup (?)
    OEMField		db	OEM_INFO_MAX dup(?)
PFileForm ends

DParms struc
	FreeClus	dw	?
	TotalClus	dw	?
	BytesPerSect	dw	?
	SectPerClus	dw	?
	RootDirSect	dd	?
	RootDirSize	dw	?
	FreeBytes	dd	?
	SectPerFat	dw	?
DParms ends

;
; This is the extension structure used on DOS 4.X INT 25/26 for large
;    media. DS:BX -> this structure and CX = -1.
;
ABS_32RW struc
	Big32Sector	dd	?
	Big32Count	dw	?
	Big32BufPtr	dd	?
ABS_32RW ends
