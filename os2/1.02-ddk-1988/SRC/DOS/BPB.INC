
;
; Copyright (c) Microsoft Corporation  1988
;

BREAK  <Bios Parameter Block>

; Bios Parameter Block definition
; This structure is used to build a full DPB

BPBLOCK STRUC
    BPSECSZ	    DW	    ?	    ; Size in bytes of physical sector
    BPCLUS	    DB	    ?	    ; Sectors/Alloc unit
    BPRES	    DW	    ?	    ; Number of reserved sectors
    BPFTCNT	    DB	    ?	    ; Number of FATs
    BPDRCNT	    DW	    ?	    ; Number of directory entries
    BPSCCNT	    DW	    ?	    ; Total number of sectors
    BPMEDIA	    DB	    ?	    ; Media descriptor byte
    BPFTSEC	    DW	    ?	    ; Number of sectors taken up by one FAT
    ; mandatory for 2.0 and above
    BPBSecPerTrack  dw	    ?	    ; sectors per track
    BPBcHeads	    dw	    ?	    ; number of heads
    BPBcSecHidden   dw	    ?	    ; number of hidden sectors before the reserved sectors
    ; mandatory for 4.0 and above
    BPBcSecHiddenH  dw	    ?	    ; High word of hidden sectors
    BPBcSecTotal    dd	    ?	    ; Big total sectors (if BPSCCNT = 0)
    ; The following byte is NOT part of the BPB but is set by SYS and format
    ; We should NOT change its position.
    PHYDRV	    db	    ?	    ; PHYSICAL DRIVE NUMBER (0 OR 80H)
    CURHD	    db	    ?	    ; Unitialized
BPBLOCK ENDS


; This structure is used by Format and generic IOCTL support code in the BIOS


a_BPB		      struc
    BPB_BytesPerSector	  dw	  ?
    BPB_SectorsPerCluster db	  ?
    BPB_ReservedSectors   dw	  ?
    BPB_NumberOfFATs	  db	  ?
    BPB_RootEntries	  dw	  ?
    BPB_TotalSectors	  dw	  ?
    BPB_MediaDescriptor   db	  ?
    BPB_SectorsPerFAT	  dw	  ?
    BPB_SectorsPerTrack   dw	  ?
    BPB_Heads		  dw	  ?
    BPB_HiddenSectors	  dw	  ?
			  dw	  ?
    BPB_BigTotalSectors   dw	  ?
			  dw	  ?
			  db	  6 dup(?)
a_BPB		      ends
