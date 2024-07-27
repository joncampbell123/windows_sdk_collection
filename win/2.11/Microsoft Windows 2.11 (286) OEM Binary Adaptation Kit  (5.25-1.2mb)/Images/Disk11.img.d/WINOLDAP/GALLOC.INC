;
; Include file for global memory manager
;
include handle.inc

; Global arena objects are kept in a doubly linked list.
;
GlobalArena STRUC
ga_sig      DB  ?           ; DOS 2.x 3.x signature byte
ga_owner    DW  ?           ; DOS 2.x 3.x owner field (current task)
ga_size     DW  ?           ; DOS 2.x 3.x size, in paragraphs, not incl. header
ga_client0  DB  ?           ; 1 byte available for random hacks
ga_prev     DW  ?           ; previous arena entry (first points to self)
ga_next     DW  ?           ; next arena entry (last points to self)
ga_handle   DW  ?           ; back link to handle table entry
ga_lruprev  DW  ?           ; Previous handle in lru chain
ga_lrunext  DW  ?           ; Next handle in lru chain
GlobalArena ENDS

GA_SIGNATURE    = 04Dh
GA_ENDSIG       = 05Ah

; Global arena objects are aligned on 2 para. boundaries, leaving the
; low order bit always zero.
;
GA_ALIGN    = 2-1
GA_MASK     = NOT GA_ALIGN
GA_FIXED    = 1
errnz   <GA_FIXED-GA_ALIGN>

; Flags passed to GlobalAlloc (zero is the default case)
;
GA_MOVEABLE     EQU 02h
GA_NOCOMPACT    EQU 10h
GA_USER         EQU 20h         ; Flag to make this be a user request
GA_ZEROINIT     EQU 40h
GA_MODIFY       EQU 80h
GA_SHAREALL     EQU HE_BIT1
GA_SHARE        EQU HE_BIT0
;
GAH_DONT_GROW	equ	02h	; Arena cannot grow

; Data structure that describes the global arena.  Allocated at the end
; of the local heap information structure.
;
GlobalInfo  STRUC
            DB  SIZE HeapInfo DUP (?)
gi_lrulock	DW  ?	; Lock out access to LRU chain from interrupt level
gi_lruchain	DW  ?	; First handle in lru chain (most recently used)
gi_lrucount	DW  ?	; #entries in LRU chain
gi_reserve	DW  ?	; #paras to reserve for disc code, 0 => not enabled
gi_disfence	DW  ?	; Fence for discardable code.
gi_free_count	DW  ?	; Count of all the free partitions.

gi_alt_first	DW  ?	; first entry in alternate arena
gi_alt_last	DW  ?	; last entry in alternate arena
gi_alt_count	DW  ?	; count of entries in alternate arena
gi_alt_lruchain	DW  ?	; First handle in lru chain (most recently used)
gi_alt_lrucount	DW  ?	; #entries in LRU chain
gi_alt_reserve	DW  ?	; alternate reserve
gi_alt_disfence	DW  ?	; Fence for discardable code.
gi_alt_free_count	DW  ?	; Count of all the free partitions.
gi_alt_pPhantom	DW  ?
GlobalInfo  ENDS


; Notify procedure message codes
;
GN_OUTOFMEM = 0         ; Out of memory - arg1 = #bytes needed
GN_MOVE     = 1         ; Object moved - arg1 = handle arg2 = old location
GN_DISCARD  = 2         ; Object discard? - arg1 = handle, arg2 = discard flags
                        ; Returns new discard flags in AX
