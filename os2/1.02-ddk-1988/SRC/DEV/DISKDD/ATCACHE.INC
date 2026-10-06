;
; Copyright (c) Microsoft Corporation,  1988
;


Free_List	    EQU     0	    ; Location within cache control segment of
				    ; where a pointer is kept to the first
				    ; buffer control structure in the free list
				    ; is kept.

hash_table_entry    struc
    Cntrl_Struc_Off	DW	?
    Dummy		DW	-1
hash_table_entry    ends
HashSiz     equ     size hash_table_entry


Buffer_Control	struc
    Next_Ctrl	    DW	    ?	    ; Next control structure in hash list
    Prev_Ctrl	    DW	    ?
    Next_Free	    DW	    ?	    ; Next control structure in free list
    Prev_Free	    DW	    ?
    Buffer_Ptr	    DD	    ?	    ; Physical address of buffer in cache
    Buf_Flags	    DB	    ?
    Buf_Drive	    DB	    ?	    ; Drive of data buffered
    Buf_Sector	    DD	    ?	    ; Sector that is in this buffer
Buffer_Control	ends
Buf_Cntrl_Siz	    equ     size Buffer_Control


;
; Flag bits used in Buf_Flags field
;
fBufUsed	    EQU     01H     ; Buffer is used for data


;
; Structure that contains information pertaining to the segment containing the
; buffer control structures.
;
free_list_entry struc
    first_free_ptr  DW	    ?	    ; Points to first structure in LRU linked list
    size_hash_tab   DW	    ?	    ; Number of entries in hash table
    num_sec_in_buf  DW	    ?	    ; Number of sectors cached in each buffer
free_list_entry ends
free_list_siz	    equ     size free_list_entry

;
; NOTE: The following equate MUST be set up to reflect the sector size
; on particular hard disks. ALL hard disks in the system must have the
; same sector size.
;
Buf_SecSize	    EQU     512 	; Size of sectors cached


;relative selectors for special GDT pool of decriptors needed by disk caching
;and other equates used by the DevHlp PhystoVirt calls

RESULTDSSI	   equ	 0
RESULTESDI	   equ	 1
SELECTOR0	   equ	 0
SELECTOR1	   equ	 1
SELECTOR2	   equ	 2
SELECTOR3	   equ	 3
SELECTOR4	   equ	 4
SELECTOR5	   equ	 5
SELECTOR6	   equ	 6
SELECTOR7	   equ	 7
SELECTOR8	   equ	 8
SELECTOR9	   equ	 9
