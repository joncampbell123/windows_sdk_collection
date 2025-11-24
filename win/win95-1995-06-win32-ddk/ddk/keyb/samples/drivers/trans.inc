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

; ========================================================================
; TRANS.INC
; ========================================================================
; definitions for 3.00 keyboard drivers.

; macros for translation table entry.

; This is used for tables in which a byte in one array is used as
; a search key for a byte in a second array.

ktrans macro vk, asc, ?VKFLAG
    if ?VKFLAG EQ 0
        db      vk
    else
        db      asc
    endif
endm

; This is used for tables in which a byte in one array is used as
; a search key for a pair of bytes in a second array (e. g.
; unshifted and shifted translations).

ktrans2 macro vk, asc1, asc2, ?VKFLAG
    if ?VKFLAG EQ 0
        db      vk
    else
        db      asc1
        db      asc2
    endif
endm

; This macro is for the dead key table, which associates an unaccented
; character and an accent deadkey with an accented ANSI character.

deadtrans macro ch, accent, accTrans, ?VKFLAG
    if ?VKFLAG EQ 0
        db      ch
        db      accent
    else
        db      accTrans
    endif
endm


; This macro provides labels for both columns of an associative table.

klabels macro lab1, lab2, ?VKFLAG
    if ?VKFLAG EQ 0
        lab1 label byte
    else
        lab2 label byte
    endif
endm


; This macro provides a label for the end of the key vector of
; an associative table.

klabdef macro deflab, ?VKFLAG
    if ?VKFLAG EQ 0
        deflab label byte
    endif
endm

