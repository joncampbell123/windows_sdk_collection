;---------------------------Module-Header------------------------------;
; Module Name: strblt.inc
;
; STRBLT.ASM and GLYPHBLT.ASM include file
;
; Copyright (c) 1992 Microsoft Corporation
;-----------------------------------------------------------------------;

MAX_GLYPH_HEIGHT    equ     256             ; maximum height StrBlt will handle

; Flags for flStr internally used by DrvTextOut()

TO_TARGET_SCREEN        equ 00000001h       ; Output to screen
TO_NO_OPAQUE_RECT       equ 00000002h       ; No opaque rectangle
TO_HORIZ_ALIGN_TEXT     equ 00000004h       ; The string lies on one scanline
TO_NON_JUSTIFIED_TEXT   equ 00000008h       ; The string is non-justified
TO_FIXED_PITCH          equ 00000010h       ; Fixed-pitch glyphs
TO_MULTIPLE_BYTE        equ 00000020h       ; multiple byte-size glyphs
TO_BYTE_ALIGNED         equ 00000040h       ; the string is byte aligned

; define flags for hints from DrvTextOut()

VGB_HORIZ_CLIPPED_GLYPH equ 00000001h       ; the glyph is horizontally clipped
VGB_VERT_CLIPPED_GLYPH  equ 00000002h       ; the glyph is vertically clipped
VGB_OPAQUE_BKGRND       equ 00000004h       ; The opaque background glyphs
VGB_ENTIRE_STRING_BLT   equ 00000008h       ; The entire string is bltted
VGB_FIXED_PITCH         equ 00000010h       ; Fixed-pitch glyphs
VGB_MULTIPLE_BYTE       equ 00000020h       ; multiple-byte fixed-pitch glyph
VGB_BYTE_ALIGNED        equ 00000040h       ; the string is byte-aligned

        .errnz  TO_FIXED_PITCH-VGB_FIXED_PITCH
        .errnz  TO_MULTIPLE_BYTE-VGB_MULTIPLE_BYTE
        .errnz  TO_BYTE_ALIGNED-VGB_BYTE_ALIGNED

; define VGA mode at blt_to_screen time

VGB_MIX_STRING      equ     0
VGB_OPAQUE_STRING   equ     1
