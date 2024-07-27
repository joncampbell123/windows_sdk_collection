; The following structure defines the disk parameter table
; pointed to by Interrupt vector 1EH (location 0:78H) on IBM machines.

DISK_PARMS      STRUC
DISK_SPECIFY_1  DB      ?
DISK_SPECIFY_2  DB      ?
DISK_MOTOR_WAIT DB      ?       ; Wait till motor off
DISK_SECTOR_SIZ DB      ?       ; Bytes/Sector (2 = 512)
DISK_EOT        DB      ?       ; Sectors per track (MAX)
DISK_RW_GAP     DB      ?       ; Read Write Gap
DISK_DTL        DB      ?
DISK_FORMAT_GAP DB      ?       ; Format Gap Length
DISK_FILL       DB      ?       ; Format Fill Byte
DISK_HEAD_STTL  DB      ?       ; Head Settle Time (MSec)
DISK_MOTOR_STRT DB      ?       ; Motor start delay
DISK_PARMS      ENDS

ROMStatus = 1
ROMRead   = 2
ROMWrite  = 3
ROMVerify = 4
ROMFormat = 5

; Internal structure of a BPB for MSDOS.  This is valid for
; versions 2.* and above but is not the extended BPB for 5.0.
;
BPB             STRUC
BPB_cbSec       DW  ?       ; number of bytes per sector
BPB_secPerClus  DB  ?       ; number of sectors per cluster
BPB_cSecRes     DW  ?       ; number of reserved sectors */
BPB_cFAT        DB  ?       ; number of FATS */
BPB_cDir        DW  ?       ; number of directory entries in root */
BPB_cSec        DW  ?       ; total number of sectors in image */
BPB_bMedia      DB  ?       ; media descriptor */
BPB_secPerFAT   DW  ?       ; number of sectors per FAT */
BPB_secPerTrack DW  ?       ; number of sectors per track */
BPB_cHead       DW  ?       ; number of heads */
BPB_cSecHidden  DW  ?       ; number of hidden sectors */
BPB             ENDS

SS48 = 2
DS48 = 3
DS96 = 4

; Structure of internal MSDOS drive parameter block.
; Note that this is NOT a complete definition and
; is ONLY valid for versions 2.* through 3.*
;

DPB                 STRUC
DPB_drive           DB  ?
DPB_unit            DB  ?
DPB_sector_size     DW  ?
DPB_cluster_mask    DB  ?
DPB_cluster_shift   DB  ?
DPB_first_FAT       DW  ?
DPB_FAT_count       DB  ?
DPB_root_entries    DW  ?
DPB_first_sector    DW  ?
DPB_max_cluster     DW  ?
DPB_FAT_size        DB  ?
DPB_dir_sector      DW  ?
DPB_reserved1       DD  ?
DPB_media           DB  ?
DPB_first_access    DB  ?
DPB_reserved2       DB  8 DUP (?)
DPB                 ENDS

; structure of MSDOS directory entries
;
DIR                 STRUC
DIR_name            DB  11 DUP (?)
DIR_attr            DB  ?
DIR_pad             DB  10 DUP (?)
DIR_time            DW  ?
DIR_date            DW  ?
DIR_first           DW  ?
DIR_size            DD  ?
DIR                 ENDS

A_RO  = 01h
A_H   = 02h
A_S   = 04h
A_VID = 08h
A_DIR = 10h
A_A   = 20h
A_DEV = 40h

BOOTSECSIZE     EQU 512

; structure of beginning of MSDOS boot sector
;
BOOTSEC         STRUC
BOOT_jump       DB  3 DUP (?)           ; 3 byte jump
BOOT_label      DB  8 DUP (?)           ; oem name and version
BOOT_BPB        DB  size BPB DUP (?)    ; bpb
BOOT_bootdrive  DB  ?                   ; int 13 indicator for boot device
                DB  BOOTSECSIZE-12-3-SIZE BPB DUP (?)
BOOT_phydrv     DB  ?                   ; Same as bootdrive, except for 3.2
BOOT_signature  DW  ?                   ; Boot sector signature
BOOTSEC         ENDS

BOOTSIG   =    0AA55h

FAT_READ  = 1
FAT_WRITE = 2
