;---------------------------- Include File Header ---------------------------;
; lines.inc
;
; Line drawing constants and structures.
;
; NOTE: This file mirrors LINES.H.  Changes here must be reflected in
; the .h file!
;
;----------------------------------------------------------------------------;

; We have special strip routines when all strips have at most this many
; pixels:

MAX_SHORT_STROKE_LENGTH equ 15

; # of strip drawers in every group:

NUM_STRIP_DRAW_DIRECTIONS equ 4

; # of strip drawers for doing either solid lines or styled lines:

NUM_STRIP_DRAW_STYLES equ 8

STYLE_MAX_COUNT         equ 16
STYLE_MAX_VALUE         equ 3fffh
RUN_MAX                 equ 20
STRIP_MAX               equ 100
STYLE_DENSITY           equ 3

; Flip and round flags:

FL_H_ROUND_DOWN         equ 00000080h   ; Set if x = 1/2 rounds to x = 1
FL_V_ROUND_DOWN         equ 00008000h   ; Set if y = 1/2 rounds to y = 1

FL_FLIP_D               equ 00000005h   ; Diagonal flip
FL_FLIP_V               equ 00000008h   ; Vertical flip
FL_FLIP_SLOPE_ONE       equ 00000010h   ; If slope is exactly one
FL_FLIP_HALF            equ 00000002h   ; Half flip
FL_FLIP_H               equ 00000200h   ; Horizontal (right-to-left) exchange

FL_ROUND_MASK           equ 0000001ch   ; Rounding is dependent on diagonal,
FL_ROUND_SHIFT          equ 2           ;  vertical flips and if slope is one

FL_RECTLCLIP_MASK       equ 0000000ch   ; When we unflip to do clipping, we
FL_RECTLCLIP_SHIFT      equ 2           ;  only have to look at diagonal and
                                        ;  vertical flips

FL_STRIP_MASK           equ 00000003h   ; Which strip drawer we call is based
FL_STRIP_SHIFT          equ 0           ;  on if we did diagonal and/or half
                                        ;  flips

FL_SIMPLE_CLIP          equ 00000020h   ; Denotes our clipping complexity
FL_COMPLEX_CLIP         equ 00000040h
FL_CLIP                 equ (FL_SIMPLE_CLIP + FL_COMPLEX_CLIP)

FL_SET                  equ 00000800h   ; If ROP is such that we don't have to
                                        ;  read video memory
FL_ARBITRARYSTYLED      equ 00000400h   ; If the line is styled
FL_STYLED               equ (FL_ARBITRARYSTYLED)

FL_STRIP_ARRAY_MASK     equ 00000C00h   ; Which strip arrays we look up depends
FL_STRIP_ARRAY_SHIFT    equ 10          ;  on if we're doing style and/or have
                                        ;  a 'set' style ROP

; Simple flag bits in high byte:

FL_DONT_DO_HALF_FLIP    equ 00002000h
FL_PHYSICAL_DEVICE      equ 00004000h

; Miscellaneous DDA defines:

F                       equ 16
FLOG2                   equ 4

STRIPS struc
    ST_cStrips	        dd  ?
    ST_flFlips          dd  ?
    ST_ptlStart         dd  2 dup(?)
    ST_alStrips	        dd  STRIP_MAX dup (?)
STRIPS ends

LINESTATE struc
    LS_lNextScan        dd  ?
    LS_chAnd            db  ?
    LS_chXor            db  ?
    LS_filler1          db  2 dup (?)
    LS_pspStart         dd  ?
    LS_pspEnd           dd  ?
    LS_psp              dd  ?
    LS_spRemaining      dd  ?
    LS_spTotal          dd  ?
    LS_spTotal2         dd  ?
    LS_spNext           dd  ?
    LS_spComplex        dd  ?
    LS_aspRtoL          dd  ?
    LS_aspLtoR          dd  ?
    LS_jStyleMask       db  ?
    LS_filler2          db  3 dup (?)
    LS_xyDensity        dd  ?
    LS_cStyle           dd  ?
    LS_ulStyleMaskLtoR  dd  ?
    LS_ulStyleMaskRtoL  dd  ?
    LS_jStartMask       db  ?
    LS_filler3          db  3 dup (?)
LINESTATE ends

RUN struc
    RUN_iStart          dd  ?
    RUN_iStop           dd  ?
RUN ends


; S3 Equates

FIFO_7_STATUS           equ 0080h
FIFO_6_STATUS           equ 0040h
FIFO_5_STATUS           equ 0020h
FIFO_4_STATUS           equ 0010h
FIFO_3_STATUS           equ 0008h
FIFO_2_STATUS           equ 0004h
FIFO_1_STATUS           equ 0002h
FIFO_0_STATUS           equ 0001h

FIFO_1_EMPTY            equ FIFO_7_STATUS
FIFO_2_EMPTY            equ FIFO_6_STATUS
FIFO_3_EMPTY            equ FIFO_5_STATUS
FIFO_4_EMPTY            equ FIFO_4_STATUS
FIFO_5_EMPTY            equ FIFO_3_STATUS
FIFO_6_EMPTY            equ FIFO_2_STATUS
FIFO_7_EMPTY            equ FIFO_1_STATUS
FIFO_8_EMPTY            equ FIFO_0_STATUS

DRAWING_DIRECTION_0     equ 0000h
DRAWING_DIRECTION_45    equ 0020h
DRAWING_DIRECTION_90    equ 0040h
DRAWING_DIRECTION_135   equ 0060h
DRAWING_DIRECTION_180   equ 0080h
DRAWING_DIRECTION_225   equ 00A0h
DRAWING_DIRECTION_270   equ 00C0h
DRAWING_DIRECTION_315   equ 00E0h

PLUS_X                  equ 0020h
PLUS_Y                  equ 0080h
MAJOR_Y                 equ 0040h
DRAW                    equ 0010h
DIR_TYPE_XY             equ 0000h
DIR_TYPE_RADIAL         equ 0008h
MULTIPLE_PIXELS         equ 0002h
WRITE                   equ 0001h
DRAW_LINE               equ 2000h
LAST_PIXEL_OFF		    equ 0004h

DEFAULT_DRAW_CMD	    equ (DRAW_LINE+DRAW+DIR_TYPE_XY+MULTIPLE_PIXELS+WRITE+ \
                             LAST_PIXEL_OFF)
