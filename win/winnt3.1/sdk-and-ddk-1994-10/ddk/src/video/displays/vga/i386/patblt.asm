;-----------------------------------------------------------------------;

; Set LOOP_UNROLL_SHIFT to the log2 of the number of times you want loops in
; this module unrolled. For example, LOOP_UNROLL_SHIFT of 3 yields 2**3 = 8
; times unrolling. This is the only thing you need to change to control
; unrolling.

LOOP_UNROLL_SHIFT equ 2

;-----------------------------------------------------------------------;

; This delay is necessitated by a bug in the ATI Ultra when running in
; VGA mode.

SLOW_OUT macro
        push    ecx
        pop     ecx
        out     dx,ax
        endm

                .386

ifndef  DOS_PLATFORM
        .model  small,c
else
ifdef   STD_CALL
        .model  small,c
else
        .model  small,pascal
endif;  STD_CALL
endif;  DOS_PLATFORM

        assume cs:FLAT,ds:FLAT,es:FLAT,ss:FLAT
        assume fs:nothing,gs:nothing

        .xlist
        include stdcall.inc             ;calling convention cmacros
        include i386\egavga.inc
        include i386\strucs.inc
        include i386\unroll.inc
        include i386\ropdefs.inc
        include i386\display.inc         ; Display specific structures

        .list

;-----------------------------------------------------------------------;

        .data

;
; We share some tables with vgablts.asm
;

extrn   jALUFuncTable :byte
extrn   jLeftMask  :byte
extrn   jRightMask :byte
extrn   jForceOnTable :byte
extrn   jNotTable :byte
extrn   jInvertDest :byte
extrn   jForceOffTable :byte
extrn   vTrgBlt@20 :dword

;-----------------------------------------------------------------------;
; Table of routines to be called to draw edges, according to which edges are
; partial and which edges are whole bytes.
        align   4
        public pfnEdgeDrawing
pfnEdgeDrawing  label   dword
        dd      edge_byte_setup
        dd      edge_byte_setup
        dd      check_next_bank
        dd      edge_byte_setup

;-----------------------------------------------------------------------;
; Table of pointers to tables used to find entries points in unrolled wide
; whole byte code.

        align   4
        public pfnWideWholeRep
pfnWideWholeRep label   dword
        dd      pfnDrawWide00Entry
        dd      pfnDrawWide01Entry
        dd      pfnDrawWide10Entry
        dd      pfnDrawWide11Entry

;-----------------------------------------------------------------------;
; Table of pointers to tables used to find entry points in narrow, special-
; cased unrolled replace whole byte code.

; Note: The breakpoint where one should switch from special-casing to
;  REP STOS is purely a guess on my part. 8 seemed reasonable.

; Start address MOD 2 is 0.
        align   4
        public pfnWholeBytesMod0Entries
pfnWholeBytesMod0Entries  label   dword
        dd      0                       ;we never get a 0-wide case
        dd      pfnDraw1WideEvenEntry
        dd      pfnDraw2WideEvenEntry
        dd      pfnDraw3WideEvenEntry
        dd      pfnDraw4WideEvenEntry
        dd      pfnDraw5WideEvenEntry
        dd      pfnDraw6WideEvenEntry
        dd      pfnDraw7WideEvenEntry
        dd      pfnDraw8WideEvenEntry
MAX_REPLACE_SPECIAL equ     ($-pfnWholeBytesMod0Entries)/4

; Start address MOD 2 is 1.
        align   4
        public pfnWholeBytesMod1Entries
pfnWholeBytesMod1Entries  label   dword
        dd      0                       ;we never get a 0-wide case
        dd      pfnDraw1WideOddEntry
        dd      pfnDraw2WideOddEntry
        dd      pfnDraw3WideOddEntry
        dd      pfnDraw4WideOddEntry
        dd      pfnDraw5WideOddEntry
        dd      pfnDraw6WideOddEntry
        dd      pfnDraw7WideOddEntry
        dd      pfnDraw8WideOddEntry


;-----------------------------------------------------------------------;
; Table of pointers to tables used to find entries points in narrow, special-
; cased unrolled non-replace whole byte code.

; Note: The breakpoint where one should switch from special-casing to
;  REP MOVSB is purely a guess on my part. 5 seemed reasonable.

        align   4
pfnWholeBytesNonReplace  label   dword
        dd      0                       ;we never get a 0-wide case
        dd      pfnDraw1RWEntry
        dd      pfnDraw2RWEntry
        dd      pfnDraw3RWEntry
        dd      pfnDraw4RWEntry
MAX_NON_REPLACE_SPECIAL equ     ($-pfnWholeBytesNonReplace)/4

; Master MOD 2 alignment look-up table for entry tables for two possible
; alignments for narrow, special-cased unrolled replace whole byte code.
        align   4
        public pfnWholeBytesSpecial
pfnWholeBytesSpecial      label   dword
        dd      pfnWholeBytesMod0Entries
        dd      pfnWholeBytesMod1Entries

        .code

;=============================================================================

_TEXT$01   SEGMENT DWORD USE32 PUBLIC 'CODE'
           ASSUME  CS:FLAT, DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

cProc   vMonoPatBlt,24,<    \
        uses esi edi ebx, \
        pdsurf: ptr DEVSURF, \
        culRcl: dword,       \
        prcl:   ptr RECTL,   \
        ulMix:  dword,       \
        pBrush: ptr oem_brush_def, \
        pBrushOrg: ptr POINTL >

        local   ulRowOffset :dword      ;Offset from start of scan line
                                        ; first byte to fill
        local   ulWholeBytes :dword     ;# of whole bytes to fill
        local   ulWholeWords :dword     ;# of whole words to fill excluding
                                        ;leading and/or trailing bytes
        local   pfnWholeFn  :dword      ;pointer to routine used to draw
                                        ; whole bytes
        local   ulScanWidth :dword      ;offset from start of one scan to start
                                        ; of next
        local   ulNextScan  :dword      ;offset from end of one scan line's
                                        ; fill to start of next
        local   ulCurrentTopScan :dword ;top scan line to fill in current bank
        local   ulMasks     :dword      ;low byte = right mask, high byte =
                                        ; left mask
        local   ulBottomScan :dword     ;bottom scan line of fill rectangle

        local   jALUFunc   :dword       ;VGA ALU logical operation (SET, AND,
                                        ; OR, or XOR)
        local   pfnStartDrawing :dword  ;pointer to function to call to start
                                        ; drawing
        local   pfnContinueDrawing :dword ;pointer to function to call to
                                        ; continue drawing after doing whole
                                        ; bytes
        local   ulLeftEdgeAdjust :dword ;used to bump the whole bytes start
                                        ; address past the left edge when the
                                        ; left edge is partial
        local   pfnWholeBytes :dword    ;pointer to table of entry points
                                        ; into unrolled loops for whole byte
                                        ; filling
        local   ulSpecialBytes          ;If we are doing a special case wide
                                        ; fill, this will be the width of the
                                        ; fill. We need this so we can properly
                                        ; increment to the next line.
        local   ulVbNextScan :dword     ;Offset from the end of the current
                                        ; wide fill drawing operation to the
                                        ; top of the next venetian blind line
        local   fdInvertDestFirst :dword;1 if the rop requires a pass to invert
                                        ; the destination before the normal
                                        ; pass

        local   ulPatternOrgY: dword    ;Local copy of the pattern offset Y

        local   ulVbBlindCount :dword   ;Temp Height of pattern.

        local   ulVbTopScan :dword      ;slats in our blinds

        local   ulVbStartScan :dword    ;Current to slat

        local   pUlVbPattern:dword      ;inner loop pattern pointer

        local   pUlPattern:dword        ;current pattern with proper Y offset

        local   ulVbMask                ;Inversion mask for partial edges

        local   ulVbYRound              ;

        local   ulVbYShift              ;

        local   RotatedPat[32]:byte     ;Aligned pattern buffer

        local   ulFgClr:dword           ;Local copy of the foreground color

        local   ulBkClr:dword           ;Local copy of the background color

        local   pfnWesTrick:dword       ;Pointer to the desired inner loop
                                        ; wes trick code. While we are doing
                                        ; a ROP to full bytes, this will point
                                        ; to do_wide_wes_trick otherwise it
                                        ; will point to do_edge_wes_trick for
                                        ; the edge cases
        cld

;-----------------------------------------------------------------------;
; Make sure there's something to draw; clip enumerations can be empty.
;-----------------------------------------------------------------------;

        cmp     culRcl,0                ;any rects to fill?
        jz      vMonoPatBlts_done         ;no, we're done

        mov     esi,pBrush              ;point to the brush

        xor     eax,eax
        mov     al,[esi + oem_brush_fg]
        mov     ulFgClr,eax             ;Make local copy of the fg color

        mov     al,[esi + oem_brush_bg]
        mov     ulBkClr,eax             ;Make local copy of the bk color

;-----------------------------------------------------------------------;
; Set up for the desired raster op.
;-----------------------------------------------------------------------;
        sub     ebx,ebx                 ;ignore any background mix; we're only
        mov     bl,byte ptr ulMix       ; concerned with the foreground in this
                                        ; module
        cmp     ebx,R2_NOP              ;is this NOP?
        jz      vMonoPatBlts_done       ;yes, we're done
        sub     eax,eax                 ;we want a dword
        mov     al,jInvertDest[ebx]     ;remember whether we need to invert the
        mov     fdInvertDestFirst,eax   ; destination before finishing the rop
        mov     eax,ulFgClr
        and     al,jForceOffTable[ebx]  ;force color to 0 if necessary
                                        ; (R2_BLACK)
        or      al,jForceOnTable[ebx]   ;force color to 0ffh if necessary
                                        ; (R2_WHITE, R2_NOT)
        xor     al,jNotTable[ebx]       ;invert color if necessary (any Pn mix)
                                        ;at this point, CH has the color we
                                        ; want to draw with; set up the VGA
                                        ; hardware to draw with that color
        mov     ulFgClr,eax

        mov     eax,ulBkClr
        and     al,jForceOffTable[ebx]  ;force color to 0 if necessary
                                        ; (R2_BLACK)
        or      al,jForceOnTable[ebx]   ;force color to 0ffh if necessary
                                        ; (R2_WHITE, R2_NOT)
        xor     al,jNotTable[ebx]       ;invert color if necessary (any Pn mix)
                                        ;at this point, CH has the color we
                                        ; want to draw with; set up the VGA
                                        ; hardware to draw with that color
        mov     ulBkClr,eax

        mov     ah,jALUFuncTable[ebx]   ;get the ALU logical function
        and     ah,ah                   ;is the logical function DR_SET?
        .errnz  DR_SET
        jz      short skip_ALU_set      ;yes, don't have to set because that's
                                        ; the VGA's default state
        mov     edx,VGA_BASE + GRAF_ADDR
        mov     al,GRAF_DATA_ROT
        SLOW_OUT                        ;set the ALU logical function
skip_ALU_set:
        mov     byte ptr jALUFunc,ah    ;remember the ALU logical function

;-----------------------------------------------------------------------;
; Set up variables that are constant for the entire time we're in this
; module.
;-----------------------------------------------------------------------;
        mov     edx,pBrushOrg           ;point to the brush origin

        mov     ecx,[edx].ptl_x
        and     ecx,15                  ;eax mod 16

        mov     eax,[edx].ptl_y
        mov     ulPatternOrgY,eax

        ;We are now going to make a copy of our rotated copy of our pattern.
        ;The reason that we do this is because we may be called with several
        ;rectangles and we don't really want to rotate the pattern data for
        ;each rectangle. We copy this rectangle to be double high so that
        ;we can incorperate our Y offest later without having to worry
        ;about running off the end of the pattern.

        lea     edi,RotatedPat          ;Pattern Dest
        mov     esi,[esi + oem_brush_pmono] ;Pattern Src
        or      cl,cl
        jnz     rotate_and_expand

INDEX=0
        rept    4                       ;patterns are 16x8
        mov     eax,[esi+INDEX]
        mov     [edi+INDEX],eax
        mov     [edi+16+INDEX],eax
INDEX=INDEX+4
        endm    ;-----------------
        jmp     fill_rect_loop

rotate_and_expand:
INDEX=0
        rept    8                       ;patterns are 16x8
        mov     ah,[esi+INDEX]          ;load bytes for shift
        mov     al,[esi+1+INDEX]        ;convert from little to big endian
        ror     ax,cl                   ;shift into position
        mov     [edi+INDEX],ah          ;save result
        mov     [edi+1+INDEX],al
        mov     [edi+16+INDEX],ah       ;save result to second copy
        mov     [edi+17+INDEX],al
INDEX=INDEX+2
        endm    ;-----------------


fill_rect_loop:
;-----------------------------------------------------------------------;
; Set up masks and widths.
;-----------------------------------------------------------------------;
        mov     edi,prcl                ;point to rectangle to fill

        sub     eax,eax
        mov     ulLeftEdgeAdjust,eax    ;initalize variable
        mov     ulSpecialBytes,eax      ;initalize variable

        mov     eax,[edi].yBottom
        mov     ulBottomScan,eax        ;remember the bottom scan line of fill

        mov     ebx,[edi].xRight        ;right edge of fill (non-inclusive)
        mov     ecx,ebx
        and     ecx,0111b               ;intrabyte address of right edge
        mov     ah,jRightMask[ecx]      ;right edge mask

        mov     esi,[edi].xLeft         ;left edge of fill (inclusive)
        mov     ecx,esi
        shr     ecx,3                   ;/8 for start offset from left edge
                                        ; of scan line
        mov     ulRowOffset,ecx         ;remember offset from start of scan
                                        ; line
        sub     ebx,esi                 ;width in pixels of fill

        and     esi,0111b               ;intrabyte address of left edge
        mov     al,jLeftMask[esi]       ;left edge mask

        dec     ebx                     ;make inclusive on right
        add     ebx,esi                 ;inclusive width, starting counting at
                                        ; the beginning of the left edge byte
        shr     ebx,3                   ;width of fill in bytes touched - 1
        jnz     short more_than_1_byte  ;more than 1 byte is involved

; Only one byte will be affected. Combine first/last masks.

        and     al,ah                   ;we'll use first byte mask only
        xor     ah,ah                   ;want last byte mask to be 0
        inc     ebx                     ;so there's one count to subtract below
                                        ; if this isn't a whole edge byte
more_than_1_byte:

; If all pixels in the left edge are altered, combine the first byte into the
; whole byte count and clear the first byte mask, because we can handle solid
; edge bytes faster as part of the whole bytes. Ditto for the right edge.

        sub     ecx,ecx                 ;edge whole-status accumulator
        cmp     al,-1                   ;is left edge a whole byte or partial?
        adc     ecx,ecx                 ;ECX=1 if left edge partial, 0 if whole
        sub     ebx,ecx                 ;if left edge partial, deduct it from
                                        ; the whole bytes count
        mov     ulLeftEdgeAdjust,ecx    ;for skipping over the left edge if
                                        ; it's partial when pointing to the
                                        ; whole bytes
        and     ah,ah                   ;is right edge mask 0, meaning this
                                        ; fill is only 1 byte wide?
        jz      short save_masks        ;yes, no need to do anything
        cmp     ah,-1                   ;is right edge a whole byte or partial?
        jnz     short save_masks        ;partial
        mov     ah,0                    ;
        add     ecx,2                   ;bit 1 of ECX=0 if right edge partial,
                                        ; 1 if whole;
                                        ;bit 1=0 if left edge partial, 1 whole
        inc     ebx                     ;if right edge whole, include it in the
                                        ; whole bytes count
save_masks:
        mov     ulMasks,eax             ;save left and right clip masks
        mov     ulWholeBytes,ebx        ;save # of whole bytes

        mov     ecx,pfnEdgeDrawing[ecx*4] ;set address of routine to draw
        mov     pfnContinueDrawing,ecx    ; all partial (non-whole) edges

        and     ebx,ebx                 ;any whole bytes?
        jz      start_vec_set           ;no
                                        ;yes, so draw the whole bytes before
                                        ; the edge bytes

; The whole bytes loop depends on the type of operation being done. If the
; operation is one which uses DR_SET, then we can use a STOS-type operation,
; else we have to use a MOVSB-type operation (to load the latches with the
; existing contents of display memory to allow the ALUs to work).
        cmp     byte ptr jALUFunc,DR_SET ;is it a replace-type rop?
        jz      short is_replace_type   ;yes
                                        ;no, set up for non-replace whole bytes
        mov     ecx,offset non_replace_wide

        cmp     ebx,MAX_NON_REPLACE_SPECIAL ;too wide to special case?
        jb      short non_replace_spec     ;nope

        lea     eax,pfnDrawRWWideEntry  ;assume too wide to special-case
        mov     pfnWholeBytes,eax       ; table for width

        jmp     short start_vec_set

        align   4
non_replace_spec:

        mov     eax,pfnWholeBytesNonReplace[ebx*4] ;no, point to entry
        mov     pfnWholeBytes,eax       ; table for width
        mov     ulSpecialBytes,ebx
                                        ;narrow enough to special case. Look up
                                        ; the entry table for the special case
                                        ; base on the start alignment

        jmp     short start_vec_set

        align   4
is_replace_type:                        ;set up for replace-type rop
        cmp     ebx,MAX_REPLACE_SPECIAL ;too wide to special case?
        jnb     short is_wide_replace   ;yes

        mov     ulSpecialBytes,ebx
                                        ;narrow enough to special case. Look up
                                        ; the entry table for the special case
                                        ; base on the start alignment
        mov     ecx,ulRowOffset
        add     ecx,ulLeftEdgeAdjust    ;left edge whole bytes start offset
        and     ecx,01b                 ;left edge whole bytes start alignment
                                        ; MOD 2
        mov     ecx,pfnWholeBytesSpecial[ecx*4] ;look up table of entry
                                                      ; tables for alignment
        mov     ecx,[ecx+ebx*4]         ;look up entry table for width
        mov     pfnWholeBytes,ecx       ; table for width
        mov     ecx,offset whole_bytes_rep_wide

        jmp     short start_vec_set

        align   4
is_wide_replace:                        ;set up for wide replace-type op
                                        ;Note: assumes there is at least one
                                        ; full word involved!
        mov     ecx,ulRowOffset
        add     ecx,ulLeftEdgeAdjust    ;left edge whole bytes start offset
        neg     ecx
        and     ecx,01b
        mov     edx,ebx
        sub     edx,ecx                 ;ignore odd leading bytes
        mov     eax,edx
        shr     edx,1                   ;# of whole words across (not counting
                                        ; odd leading & trailing bytes)
        mov     ulWholeWords,edx
        and     eax,01b                 ;# of odd (fractional) trailing bytes
        add     ecx,ecx
        or      ecx,eax                 ;build a look-up index from the number
                                        ; of leading and trailing bytes
        mov     ecx,pfnWideWholeRep[ecx*4] ;proper drawing handler for front/
        mov     pfnWholeBytes,ecx          ; back alignment
        mov     ecx,offset whole_bytes_rep_wide
                                        ;set up to call routine to perform wide
                                        ; whole bytes fill

start_vec_set:
        mov     pfnStartDrawing,ecx     ; all partial (non-whole) edges

        mov     ecx,pdsurf
        mov     eax,[ecx].dsurf_lNextScan
        mov     ulScanWidth,eax         ;local copy of scan line width
        sub     eax,ebx                 ;EAX = delta to next scan
        mov     ulNextScan,eax

        mov     esi,pBrush
        mov     eax,[esi+oem_brush_height]
        dec     eax
        mov     ulVbYRound,eax
        mov     al,[esi + oem_brush_yshft] ; blind to the next.
        mov     ulVbYShift,eax

        mov     cl,al
        mov     eax,UlScanWidth
        shl     eax,cl                  ;ulNextScan * 8
        mov     ulVbNextScan,eax        ;

        cmp     fdInvertDestFirst,1     ;is this an invert-dest-plus-something-
                                        ; else rop that requires two passes?
        jnz     short do_single_pass

        lea     eax,vTrgBlt@20
        ptrCall <eax>,<pdsurf, culRcl, prcl, R2_NOT, -1>

        mov     ah,byte ptr jALUFunc    ;reset the ALU logical function
        mov     edx,VGA_BASE + GRAF_ADDR
        mov     al,GRAF_DATA_ROT
        SLOW_OUT                        ;set the ALU logical function

do_single_pass:
        call    draw_banks

;-----------------------------------------------------------------------;
; See if there are any more rectangles to fill.
;-----------------------------------------------------------------------;

        add     prcl,(size RECTL) ;point to the next rectangle, if there is one
        dec     culRcl            ;count down the rectangles to fill
        jnz     fill_rect_loop


;-----------------------------------------------------------------------;
; We have filled all rectangles.  Restore the VGA to its default state.
;-----------------------------------------------------------------------;

        mov     edx,VGA_BASE + GRAF_ADDR
        mov     eax,0000h + GRAF_ENAB_SR ;disable set/reset
        out     dx,ax
        mov     eax,GRAF_MODE + ((M_PROC_WRITE + M_DATA_READ) SHL 8)
        out     dx,ax                   ;restore read mode 0 and write mode 0
        mov     eax,(DR_SET shl 8) + GRAF_DATA_ROT ;set the logical function to
        out     dx,ax                             ; SET
vMonoPatBlts_done:
        cRet    vMonoPatBlt

;-----------------------------------------------------------------------;
; Fills all banks in the current fill rectangle. Called once per fill
; rectangle, except for destination-inversion-plus-something-else rops.
;-----------------------------------------------------------------------;

        align   4
draw_banks:

;-----------------------------------------------------------------------;
; Map in the bank containing the top scan to fill, if it's not mapped in
; already.
;-----------------------------------------------------------------------;

        mov     edi,prcl                ;point to rectangle to fill
        mov     ecx,pdsurf              ;point to surface
        mov     eax,[edi].yTop          ;top scan line of fill
        mov     ulCurrentTopScan,eax    ;this will be the fill top in 1st bank

        cmp     eax,[ecx].dsurf_rcl1WindowClip.yTop ;is fill top less than
                                                    ; current bank?
        jl      short map_init_bank             ;yes, map in proper bank
        cmp     eax,[ecx].dsurf_rcl1WindowClip.yBottom ;fill top greater than
                                                       ; current bank?
        jl      short init_bank_mapped          ;no, proper bank already mapped
map_init_bank:

; Map in the bank containing the top scan line of the fill.

        ptrCall <dword ptr [ecx].dsurf_pfnBankControl>,<ecx,eax,JustifyTop>

init_bank_mapped:

;-----------------------------------------------------------------------;
; Main loop for processing fill in each bank.
;-----------------------------------------------------------------------;

; Compute the starting address and scan line count for the initial bank.

        mov     eax,pdsurf              ;EAX->target surface
        mov     ebx,ulBottomScan        ;bottom of destination rectangle
        cmp     ebx,[eax].dsurf_rcl1WindowClip.yBottom
                                        ;which comes first, the bottom of the
                                        ; dest rect or the bottom of the
                                        ; current bank?
        jl      short BottomScanSet     ;fill bottom comes first, so draw to
                                        ; that; this is the last bank in fill
        mov     ebx,[eax].dsurf_rcl1WindowClip.yBottom
                                        ;bank bottom comes first; draw to
                                        ; bottom of bank
BottomScanSet:
        mov     edi,ulCurrentTopScan    ;top scan line to fill in current bank
        sub     ebx,edi                 ;# of scans to fill in bank
        imul    edi,ulScanWidth         ;offset of starting scan line

; Note that the start of the bitmap will change each time through the
; bank loop, because the start of the bitmap is varied to map the
; desired scan line to the banking window.

        add     edi,[eax].dsurf_pvBitmapStart ;start of scan in bitmap
        add     edi,ulRowOffset         ;EDI = start offset of fill in bitmap

; We have computed the starting address and scan count. Time to start drawing
; in the initial bank.

        mov     esi,pBrush              ;edx = min(PatternHeight,BltHeight)
        mov     ecx,[esi + oem_brush_height]
        sub     ecx,ebx
        sbb     edx,edx
        and     edx,ecx
        add     edx,ebx
        mov     ulVbBlindCount,edx

; Brush alignment. We need to look at pptlBrush

        mov     eax,ulCurrentTopScan    ;top scan line to fill in current bank
        sub     eax,ulPatternOrgY       ;

        jns     short pos_y_offset      ;
        neg     eax                     ;
        and     eax,7                   ;-eax mod 8
        neg     eax                     ;
        add     eax,8                   ;
        jmp     short save_pat_pointer
pos_y_offset:
        and     eax,7                   ;eax mod 8
save_pat_pointer:
        add     eax,eax                 ;Y Offset * PatternWidth (2 bytes)

        lea     edx,RotatedPat          ;Pattern Dest
        add     eax,edx
        mov     pulPattern,eax          ;Drawing code uses this as the
                                        ;source for the pattern

        jmp     pfnStartDrawing


;-----------------------------------------------------------------------;
; Whole byte fills.
;-----------------------------------------------------------------------;

;-----------------------------------------------------------------------;
; Handles non-replace whole byte fills wider than the maximum special
; case width.
;
; The destination is not involved, so a STOS (or equivalent) can be used
; (no read needed before write).
;-----------------------------------------------------------------------;

        align   4
        public whole_bytes_rep_wide
whole_bytes_rep_wide:
        push    ebx                     ;save scan count
        push    edi                     ;save starting address

        add     edi,ulLeftEdgeAdjust    ;point to first whole byte to fill

        mov     edx,VGA_BASE + GRAF_ADDR
        mov     eax,GRAF_MODE + ((M_COLOR_WRITE + M_COLOR_READ) SHL 8)
        out     dx,ax                   ;write mode 2
        mov     eax,ulBkClr             ;Set the write mode to write mode
        mov     [edi],al                ; three after we load the latches
        mov     al,[edi]                ; with our background color

        mov     al,GRAF_SET_RESET       ;Set the foreground color
        out     dx,al                   ; into set/reset
        inc     edx
        in      al,dx
        and     eax,0f0h
        or      eax,ulFgClr
        out     dx,al
        dec     edx

        mov     eax,GRAF_MODE + ((M_AND_WRITE + M_COLOR_READ) SHL 8)
        out     dx,ax                   ;write mode 3 so we can do the masking
                                        ; without OUTs, read mode 1 so we can
                                        ; read 0xFF from memory always, for
                                        ; ANDing (because Color Don't Care is
                                        ; all zeros)

        mov     esi,pulPattern          ; pointer to pattern bits
        mov     ax,[esi]                ; into place
        add     esi,2
        mov     pulVbPattern,esi


        mov     ulVbTopScan,ebx         ;our pattern is 8 high so we don't
        add     ebx,ulVbYRound          ;Calc the number of lines to do
        mov     ecx,ulVbyShift
        shr     ebx,cl                  ;only need to go through the code
                                        ; count/8 times. We will handle any
                                        ; extra lines at the bottom
                                        ; (ulVbTopScan mod 8) in our loops.
        mov     edx,pfnWholeBytes
        push    ulVbBlindCount

        public wide_bytes_loop
wide_bytes_loop:
        SET_UP_UNROLL_VARS ebx, ecx, ebx, [edx], LOOP_UNROLL_SHIFT

        mov     esi,ulWholeWords        ;number of aligned word writes
        mov     edx,ulVbNextScan        ;offset from end of one scan line to
                                        ; start of next the same scan line
                                        ; in the next pattern.
        sub     edx,ulWholeBytes
        add     edx,ulSpecialBytes


        ; eax = rotated pattern
        ; ebx = unrolled count
        ; ecx = routine address
        ; edx = ulVbNextScan
        ; esi = ulFvWholeWords
        ; edi = pDest
        ;
        push    edi                     ;save out dest pointer
        call    ecx                     ;draw the wide whole bytes
        pop     edi                     ;restore out dest pointer

        add     edi,ulScanWidth         ;advance to next scan line

        dec     ulVbBlindCount
        jz      short wide_bytes_end

        mov     eax,ulVbTopScan           ;restore scan count
        dec     eax                     ;Subtract off completed top line
        mov     ulVbTopScan,eax
        add     eax,ulVbYRound          ;Calc the number of lines to do
        mov     ecx,ulVbyShift
        shr     eax,cl                  ;for this venetian blind pass
        mov     ebx,eax                 ;including any partial patterns
                                        ; at the bottom

        mov     esi,pulVbPattern        ;Pattern data
        mov     ax,[esi]                ;get pattern word
        add     esi,2
        mov     pulVbPattern,esi        ;save pattern pointer for later

        mov     edx,pfnWholeBytes

        jmp     short wide_bytes_loop

wide_bytes_end:
        pop     ulVbBlindCount
        pop     edi                     ;restore screen pointer
        pop     ebx                     ;restore fill scan count

        mov     edx,VGA_BASE + GRAF_ADDR ;restore proper read/write modes
        mov     eax,GRAF_MODE + ((M_PROC_WRITE + M_DATA_READ) SHL 8)
        out     dx,ax

        jmp     pfnContinueDrawing      ;either keep drawing or we're done


;-----------------------------------------------------------------------;
; Handle case where both edges are partial (non-whole) bytes.
;-----------------------------------------------------------------------;

        align   4
        public  non_replace_wide
non_replace_wide:
        push    ebx                     ;Save line count
        push    edi                     ;Save Dest Addr

        add     edi,ulLeftEdgeAdjust    ;point to first whole byte to fill

        lea     eax,do_wide_wes_trick
        mov     pfnWesTrick,eax

        mov     ecx,ulFgClr
        xor     ecx,ulBkClr             ;mask = ulBkClr ^ ulFgClr

        mov     ah,cl                   ;sre = !mask
        not     ah                      ;Set/Reset Enable
        mov     edx,EGA_BASE+GRAF_ADDR
        mov     al,GRAF_ENAB_SR
        out     dx,ax                   ;Set Set/Reset Enable bits

        mov     ah,byte ptr ulBkClr     ;Set/Reset = background color
        mov     al,GRAF_SET_RESET
        out     dx,ax

        mov     eax,GRAF_MODE + ((M_PROC_WRITE + M_COLOR_READ) SHL 8)
        out     dx,ax                   ; Set Read Mode 0

;save the width count and pfn here

        call    wes_trick

        mov     edx,EGA_BASE+SEQ_DATA
        mov     eax,0fh
        out     dx,al

        mov     edx,EGA_BASE+GRAF_ADDR
        mov     eax,GRAF_MODE + ((M_PROC_WRITE + M_DATA_READ) SHL 8)
        out     dx,ax

        mov     eax,GRAF_ENAB_SR
        out     dx,ax                   ;Reset Set/Reset Enable bits

        pop     edi
        pop     ebx

        jmp     pfnContinueDrawing      ;either keep drawing or we're done

;-----------------------------------------------------------------------;
; Process any left/right columns that that have to be done.
;
;   Currently:
;       EBX =   height to fill, in scans
;       EDI --> first byte of left edge
;-----------------------------------------------------------------------;


;-----------------------------------------------------------------------;
; Handle case where both edges are partial (non-whole) bytes.
;-----------------------------------------------------------------------;

        align   4
        public  edge_byte_setup
edge_byte_setup:
        lea     eax,do_edge_wes_trick
        mov     pfnWesTrick,eax

        mov     ecx,ulFgClr
        xor     ecx,ulBkClr             ;mask = ulBkClr ^ ulFgClr

        mov     ah,cl                   ;sre = !mask
        not     ah                      ;Set/Reset Enable
        mov     edx,EGA_BASE+GRAF_ADDR
        mov     al,GRAF_ENAB_SR
        out     dx,ax                   ;Set Set/Reset Enable bits

        mov     ah,byte ptr ulBkClr     ;Set/Reset = foreground color
        mov     al,GRAF_SET_RESET
        out     dx,ax

        mov     eax,ulLeftEdgeAdjust
        or      eax,eax
        jz      short do_right_edge

        mov     eax,ulMasks             ;Get Left/Right edge Masks
        mov     ah,al
        mov     al,GRAF_BIT_MASK
        mov     edx,EGA_BASE+GRAF_ADDR
        out     dx,ax

        inc     pulPattern              ;Adjust Pattern rotation

        push    ebx                     ;Save line count
        push    edi                     ;Save Dest Addr
        call    wes_trick
        pop     edi
        pop     ebx

        mov     eax,ulMasks             ;restore Left/Right edge Masks
        dec     pulPattern              ;Adjust Pattern rotation

do_right_edge:
        mov     eax,ulMasks             ;Get Left/Right edge Masks
        and     ah,0ffh
        jz      edge_done

        mov     al,GRAF_BIT_MASK
        mov     edx,EGA_BASE+GRAF_ADDR
        out     dx,ax

        add     edi,ulLeftEdgeAdjust    ;point to first whole byte to fill
        add     edi,ulWholeBytes        ;point to right edge byte to fill
        call    wes_trick

edge_done:
        mov     edx,EGA_BASE+SEQ_DATA
        mov     eax,0fh
        out     dx,al

        mov     edx,EGA_BASE+GRAF_ADDR
        mov     eax,GRAF_BIT_MASK+0ff00h
        out     dx,ax

        mov     eax,GRAF_ENAB_SR
        out     dx,ax                   ;Reset Set/Reset Enable bits


;-----------------------------------------------------------------------;
; See if there are any more banks to process.
;-----------------------------------------------------------------------;

        public check_next_bank
check_next_bank:

        mov     edi,pdsurf
        mov     eax,[edi].dsurf_rcl1WindowClip.yBottom ;is the fill bottom in
        cmp     ulBottomScan,eax                       ; the current bank?
        jle     short banks_done        ;yes, so we're done
                                        ;no, map in the next bank and fill it
        mov     ulCurrentTopScan,eax    ;remember where the top of the bank
                                        ; we're about to map in is (same as
                                        ; bottom of bank we just did)

        ptrCall <dword ptr [edi].dsurf_pfnBankControl>,<edi,eax,JustifyTop>
                                        ;map in the bank

; Compute the starting address and scan line count in this bank.

        mov     eax,pdsurf              ;EAX->target surface
        mov     ebx,ulBottomScan        ;bottom of destination rectangle
        cmp     ebx,[eax].dsurf_rcl1WindowClip.yBottom
                                        ;which comes first, the bottom of the
                                        ; dest rect or the bottom of the
                                        ; current bank?
        jl      short BottomScanSet2    ;fill bottom comes first, so draw to
                                        ; that; this is the last bank in fill
        mov     ebx,[eax].dsurf_rcl1WindowClip.yBottom
                                        ;bank bottom comes first; draw to
                                        ; bottom of bank
BottomScanSet2:
        mov     edi,ulCurrentTopScan    ;top scan line to fill in current bank
        sub     ebx,edi                 ;# of scans to fill in bank
        imul    edi,ulScanWidth         ;offset of starting scan line

; Note that the start of the bitmap will change each time through the
; bank loop, because the start of the bitmap is varied to map the
; desired scan line to the banking window.

        add     edi,[eax].dsurf_pvBitmapStart ;start of scan in bitmap
        add     edi,ulRowOffset         ;EDI = start offset of fill in bitmap

; We have computed the starting address and scan count. Time to start drawing
; in the initial bank.

        mov     esi,pBrush              ;edx = min(PatternHeight,BltHeight)
        mov     ecx,[esi + oem_brush_height]
        sub     ecx,ebx
        sbb     edx,edx
        and     edx,ecx
        add     edx,ebx
        mov     ulVbBlindCount,edx

; Brush alignment. We need to look at pptlBrush

        mov     eax,ulCurrentTopScan    ;top scan line to fill in current bank
        sub     eax,ulPatternOrgY       ;

        jns     short pos_y_offset1     ;
        neg     eax                     ;
        and     eax,7                   ;-eax mod 8
        neg     eax                     ;
        add     eax,8                   ;
        jmp     short save_pat_pointer1
pos_y_offset1:
        and     eax,7                   ;eax mod 8
save_pat_pointer1:
        add     eax,eax                 ;Y Offset * PatternWidth (2 bytes)

        lea     edx,RotatedPat          ;Pattern Dest
        add     eax,edx
        mov     pulPattern,eax          ;Drawing code uses this as the
                                        ;source for the pattern

; Draw in the new bank.

        jmp     pfnStartDrawing


;-----------------------------------------------------------------------;
; Done with all banks in this fill.

        public banks_done
banks_done:
        PLAIN_RET

endProc vMonoPatBlt

;----------------------------------------------------------------------------
; Wes Trick Setup code. This code decides if this is a one or a two pass
; operation.
;----------------------------------------------------------------------------
        align   4
        public  wes_trick
wes_trick:
        mov     esi,pfnWesTrick

        mov     ecx,ulFgClr             ;
        mov     eax,ecx                 ;
        xor     ecx,ulBkClr             ;mask = ulBkClr ^ ulFgClr

        mov     edx,EGA_BASE+SEQ_DATA   ;Index should be pointing to the
                                        ; plane mask (2)
        mov     ch,cl
        not     ch                      ;Set/Reset Enable bits
        and     cl,al                   ;ulFgdColor & mask
        or      cl,al
        jz      short check_bk_bits     ;if zero - one background pass

        mov     ulVbMask,0              ;We do not want to invert the
                                        ;foreground pass
        or      ch,cl
        mov     al,ch
        out     dx,al                   ;Enable Planes for First Pass

        push    ecx
        push    edi                     ;Save our Dest pointer
        push    ebx                     ;Save our count
        call    esi                     ;Draw the foreground pass
        pop     ebx                     ;restore the line count
        pop     edi                     ;restore the dest pointer
        pop     eax                     ;Restore bk mask
        mov     esi,pfnWesTrick


check_bk_bits:
        not     al
        and     al,MM_ALL
        jnz     short @f
        ret
@@:
        mov     ulVbMask,-1             ;We do not want to invert the
        mov     edx,EGA_BASE+SEQ_DATA   ;Index should be pointing to the
                                        ; plane mask (2)
        out     dx,al
        jmp     esi

;--------------------------------------------------------------------------
; Do the edges here.
;--------------------------------------------------------------------------

        align 4
        public do_edge_wes_trick
do_edge_wes_trick:
        ;       ebx = line count
        ;       edi = dest

        mov     ulVbTopScan,ebx         ;Mod 8 our count for the venetian blind
        add     ebx,ulVbYRound          ;Calc the number of lines to do
        mov     ecx,ulVbyShift
        shr     ebx,cl

        mov     esi,pulPattern
        mov     ax,[esi]                ;get pattern into place
        add     esi,2                   ;patterns stored as words
        xor     eax,ulVbMask            ;Invert the pattern if we are doing
                                        ; a background pass

        push    ulVbBlindCount
        ; Set up variables for entering unrolled loop.
wes_trick_loop:
        SET_UP_UNROLL_VARS ebx,edx, ebx,pfnDraw1WideEntry, LOOP_UNROLL_SHIFT

        mov     ecx,ulVbNextScan        ;offset from one scan to next

        push    edi                     ;save dest pointer
        call    edx                     ;jump into the unrolled loop to draw
        pop     edi                     ;restore dest pointer

        add     edi,ulScanWidth         ;move to next scan line

        dec     ulVbBlindCount
        jz      short wes_trick_loop_done  ;jz if we are finished

        mov     eax,ulVbTopScan         ;restore scan count
        dec     eax                     ;Subtract off completed top line
        mov     ulVbTopScan,eax         ;save for next loop
        add     eax,ulVbYRound          ;Calc the number of lines to do
        mov     ecx,ulVbyShift          ;for this venetian blind pass
        shr     eax,cl                  ;including any partial patterns
        mov     ebx,eax                 ;at the bottom


        mov     ax,[esi]
        add     esi,2                   ;point to the next pattern line
        xor     eax,ulVbMask            ;Invert the pattern if we are doing
                                        ; a background pass

        jmp     short wes_trick_loop

        align   4
wes_trick_loop_done:
        pop     ulVbBlindCount
        ret

;--------------------------------------------------------------------------
; Do the middle bytes here for blts with rops.
;--------------------------------------------------------------------------

        align   4
        public  do_wide_wes_trick
do_wide_wes_trick:
        ;       ebx = line count
        ;       edi = dest

        mov     ulVbTopScan,ebx         ;Mod 8 our count for the venetian blind
        add     ebx,ulVbYRound          ;Calc the number of lines to do
        mov     ecx,ulVbyShift
        shr     ebx,cl

        mov     esi,pulPattern
        mov     al,[esi]                ;get pattern into place
        add     esi,2                   ;patterns stored as words
        mov     pulVbPattern,esi
        xor     eax,ulVbMask            ;Invert the pattern if we are doing
                                        ; a background pass

        push    ulVbBlindCount

        mov     edx,pfnWholeBytes

        ; Set up variables for entering unrolled loop.
wide_wes_trick_loop:

        SET_UP_UNROLL_VARS ebx,ecx,ebx,[edx], LOOP_UNROLL_SHIFT

        mov     esi,ulWholeBytes
        mov     edx,ulVbNextScan        ;offset from one scan to next
        sub     edx,esi
        add     edx,ulSpecialBytes

        push    edi                     ;save dest pointer
        call    ecx                     ;jump into the unrolled loop to draw
        pop     edi                     ;restore dest pointer

        add     edi,ulScanWidth         ;move to next scan line

        dec     ulVbBlindCount
        jz      short wide_wes_trick_loop_done  ;jz if we are finished

        mov     eax,ulVbTopScan         ;restore scan count
        dec     eax                     ;Subtract off completed top line
        mov     ulVbTopScan,eax         ;save for next loop
        add     eax,ulVbYRound          ;Calc the number of lines to do
        mov     ecx,ulVbyShift          ;for this venetian blind pass
        shr     eax,cl                  ;including any partial patterns
        mov     ebx,eax                 ;at the bottom


        mov     esi,pulVbPattern
        mov     al,[esi]                ;get pattern word
        add     esi,2                   ;point to the next pattern line
        mov     pulVbPattern,esi
        xor     eax,ulVbMask            ;Invert the pattern if we are doing
                                        ; a background pass

        mov     edx,pfnWholeBytes
        jmp     short wide_wes_trick_loop

        align   4
wide_wes_trick_loop_done:
        pop     ulVbBlindCount
        ret

;-----------------------------------------------------------------------;
; Unrolled loops.
;-----------------------------------------------------------------------;

;-----------------------------------------------------------------------;
; Unrolled loop stuff for wide replace-type rops (arbitrary width).
;-----------------------------------------------------------------------;

; Tables of entry points into unrolled wide write-only loops.
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWide00Entry,W00,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWide01Entry,W01,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWide10Entry,W10,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWide11Entry,W11,LOOP_UNROLL_COUNT

;-----------------------------------------------------------------------;
; Macro to draw n bytes, 0 leading bytes, 0 trailing bytes, then advance
; to next scan line.

DRAW_WIDE_00 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ecx,esi         ;# of whole words
        rep     stosw           ;fill all whole bytes as dwords
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 0 leading bytes, 0 trailing bytes.
;  EAX = Pattern Byte
;  EBX = count of scans to fill ((total scans/ pattern height) + partial)
;  EDX = offset from end of one scan's fill to start of next similar line
;  ESI = pattern data
;  EDI = target address to fill

        align   4
draw_wide_00_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_00,W00,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_00_loop

        ret

draw_wide_00_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n bytes, 0 leading bytes, 0 trailing bytes, then advance
; to next scan line.

DRAW_WIDE_01 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ecx,esi         ;# of whole words
        rep     stosw           ;fill all whole bytes as dwords
        mov     [edi],al        ;trailing byte
        inc     edi
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 0 leading bytes, 0 trailing bytes.
;  EAX = Pattern Byte
;  EBX = count of scans to fill ((total scans/ pattern height) + partial)
;  EDX = offset from end of one scan's fill to start of next similar line
;  ESI = pattern data
;  EDI = target address to fill

        align   4
draw_wide_01_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_01,W01,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_01_loop
        ret

draw_wide_01_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n bytes, 0 leading bytes, 0 trailing bytes, then advance
; to next scan line.

DRAW_WIDE_10 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ah        ;do leading byte
        inc     edi             ;advance poitner
        mov     ecx,esi         ;# of whole words
        rep     stosw           ;fill all whole bytes as dwords
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 0 leading bytes, 0 trailing bytes.
;  EAX = Pattern Byte
;  EBX = count of scans to fill ((total scans/ pattern height) + partial)
;  EDX = offset from end of one scan's fill to start of next similar line
;  ESI = pattern data
;  EDI = target address to fill

        align   4
draw_wide_10_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_10,W10,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_10_loop
        ret

draw_wide_10_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw n bytes, 0 leading bytes, 0 trailing bytes, then advance
; to next scan line.

DRAW_WIDE_11 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ah        ;do leading byte
        inc     edi             ;advance poitner
        mov     ecx,esi         ;# of whole words
        rep     stosw           ;fill all whole bytes as dwords
        mov     [edi],al        ;trailing byte
        inc     edi
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 0 leading bytes, 0 trailing bytes.
;  EAX = Pattern Byte
;  EBX = count of scans to fill ((total scans/ pattern height) + partial)
;  EDX = offset from end of one scan's fill to start of next similar line
;  ESI = pattern data
;  EDI = target address to fill

        align   4
draw_wide_11_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_11,W11,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_11_loop
        ret

draw_wide_11_loop     endp

;-----------------------------------------------------------------------;
; Unrolled drawing stuff (unrolled to reduce jumps to speed things up),
; for cases where read before write is NOT required.
;-----------------------------------------------------------------------;

; Tables of entry points into unrolled 1-, 2-, 3-, and 4-wide write-only loops.
; Note that there may be separate entry tables for various alignments of a
; specific width, in cases where performance can be improved by using different
; code for different alignments.

        UNROLL_LOOP_ENTRY_TABLE pfnDraw1WideEvenEntry,W1_EVEN,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw1WideOddEntry,W1_ODD,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw2WideEvenEntry,W2_EVEN,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw2WideOddEntry,W2_ODD,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw3WideEvenEntry,W3_EVEN,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw3WideOddEntry,W3_ODD,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw4WideEvenEntry,W4_EVEN,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw4WideOddEntry,W4_ODD,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw5WideEvenEntry,W5_EVEN,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw5WideOddEntry,W5_ODD,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw6WideEvenEntry,W6_EVEN,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw6WideOddEntry,W6_ODD,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw7WideEvenEntry,W7_EVEN,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw7WideOddEntry,W7_ODD,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw8WideEvenEntry,W8_EVEN,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw8WideOddEntry,W8_ODD,LOOP_UNROLL_COUNT

;-----------------------------------------------------------------------;
; Unrolled 1-, 2-, 3-, and 4-wide write-only edge-drawing loops.
;
; Entry:
;       AL/AX/EAX = pixel mask (if AX or EAX, then 0xFFFF or 0xFFFFFFFF)
;       EBX = unrolled loop count
;       ECX = scan line width in bytes
;       EDI = start offset
;
; EBX, EDI modified. All other registers preserved.

;-----------------------------------------------------------------------;
; Macro to draw one write-only byte, then advance to next scan line.

DRAW_1_WIDE_EVEN macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],al                ;we always read 0xFF, so AL is written
                                        ; as-is; because we're in write mode 3,
                                        ; AL becomes the Bit Mask
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 1-wide write-only.

        align   4
draw_1_wide_even_loop     proc    near
        UNROLL_LOOP     DRAW_1_WIDE_EVEN,W1_EVEN,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_1_wide_even_loop

        ret

draw_1_wide_even_loop     endp

DRAW_1_WIDE_ODD macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ah                ;we always read 0xFF, so AL is written
                                        ; as-is; because we're in write mode 3,
                                        ; AL becomes the Bit Mask
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 1-wide write-only.

        align   4
draw_1_wide_odd_loop     proc    near
        UNROLL_LOOP     DRAW_1_WIDE_ODD,W1_ODD,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_1_wide_odd_loop

        ret

draw_1_wide_odd_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw two write-only bytes, then advance to next scan line.

DRAW_2_WIDE_EVEN macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ax
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 2-wide write-only.

        align   4
draw_2_wide_even_loop     proc    near
        UNROLL_LOOP     DRAW_2_WIDE_EVEN,W2_EVEN,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_2_wide_even_loop

        ret

draw_2_wide_even_loop     endp

DRAW_2_WIDE_ODD macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ah
        mov     [edi+1],al
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 2-wide write-only.

        align   4
draw_2_wide_odd_loop     proc    near
        UNROLL_LOOP     DRAW_2_WIDE_ODD,W2_ODD,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_2_wide_odd_loop

        ret

draw_2_wide_odd_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three write-only bytes, then advance to next scan line.
; Optimized for even start address.

DRAW_3_WIDE_EVEN macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ax
        mov     [edi+2],al
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 3-wide write-only, starting at an even address.

        align   4
draw_3_wide_even_loop     proc    near
        UNROLL_LOOP     DRAW_3_WIDE_EVEN,W3_EVEN,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_3_wide_even_loop

        ret

draw_3_wide_even_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three write-only bytes, then advance to next scan line.
; Optimized for odd start address.

DRAW_3_WIDE_ODD macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ah
        mov     [edi+1],ax
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 3-wide write-only, starting at an odd address.

        align   4
draw_3_wide_odd_loop     proc    near
        UNROLL_LOOP     DRAW_3_WIDE_ODD,W3_ODD,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_3_wide_odd_loop

        ret

draw_3_wide_odd_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw three write-only bytes, then advance to next scan line.
; Optimized for even start address.

DRAW_4_WIDE_EVEN macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ax
        mov     [edi+2],ax
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 4-wide write-only, starting at an even address.

        align   4
draw_4_wide_even_loop     proc    near
        UNROLL_LOOP     DRAW_4_WIDE_EVEN,W4_EVEN,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_4_wide_even_loop

        ret

draw_4_wide_even_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three write-only bytes, then advance to next scan line.
; Optimized for odd start address.

DRAW_4_WIDE_ODD macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ah
        mov     [edi+1],ax
        mov     [edi+3],al
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 4-wide write-only, starting at an odd address.

        align   4
draw_4_wide_odd_loop     proc    near
        UNROLL_LOOP     DRAW_4_WIDE_ODD,W4_ODD,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_4_wide_odd_loop

        ret

draw_4_wide_odd_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three write-only bytes, then advance to next scan line.
; Optimized for even start address.

DRAW_5_WIDE_EVEN macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ax
        mov     [edi+2],ax
        mov     [edi+4],al
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 5-wide write-only, starting at an even address.

        align   4
draw_5_wide_even_loop     proc    near
        UNROLL_LOOP     DRAW_5_WIDE_EVEN,W5_EVEN,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_5_wide_even_loop

        ret

draw_5_wide_even_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three write-only bytes, then advance to next scan line.
; Optimized for odd start address.

DRAW_5_WIDE_ODD macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ah
        mov     [edi+1],ax
        mov     [edi+3],ax
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 5-wide write-only, starting at an odd address.

        align  4
draw_5_wide_odd_loop     proc    near
        UNROLL_LOOP     DRAW_5_WIDE_ODD,W5_ODD,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_5_wide_odd_loop

        ret

draw_5_wide_odd_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three write-only bytes, then advance to next scan line.
; Optimized for even start address.

DRAW_6_WIDE_EVEN macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ax
        mov     [edi+2],ax
        mov     [edi+4],ax
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 6-wide write-only, starting at an even address.

        align   4
draw_6_wide_even_loop     proc    near
        UNROLL_LOOP     DRAW_6_WIDE_EVEN,W6_EVEN,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_6_wide_even_loop

        ret

draw_6_wide_even_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three write-only bytes, then advance to next scan line.
; Optimized for odd start address.

DRAW_6_WIDE_ODD macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ah
        mov     [edi+1],ax
        mov     [edi+3],ax
        mov     [edi+5],al
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 6-wide write-only, starting at an odd address.

        align   4
draw_6_wide_odd_loop     proc    near
        UNROLL_LOOP     DRAW_6_WIDE_ODD,W6_ODD,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_6_wide_odd_loop

        ret

draw_6_wide_odd_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three write-only bytes, then advance to next scan line.
; Optimized for even start address.

DRAW_7_WIDE_EVEN macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ax
        mov     [edi+2],ax
        mov     [edi+4],ax
        mov     [edi+6],al
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 7-wide write-only, starting at an even address.

        align   4
draw_7_wide_even_loop     proc    near
        UNROLL_LOOP     DRAW_7_WIDE_EVEN,W7_EVEN,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_7_wide_even_loop

        ret

draw_7_wide_even_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three write-only bytes, then advance to next scan line.
; Optimized for odd start address.

DRAW_7_WIDE_ODD macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ah
        mov     [edi+1],ax
        mov     [edi+3],ax
        mov     [edi+5],ax
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 7-wide write-only, starting at an odd address.

        align   4
draw_7_wide_odd_loop     proc    near
        UNROLL_LOOP     DRAW_7_WIDE_ODD,W7_ODD,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_7_wide_odd_loop

        ret

draw_7_wide_odd_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three write-only bytes, then advance to next scan line.
; Optimized for even start address.

DRAW_8_WIDE_EVEN macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ax
        mov     [edi+2],ax
        mov     [edi+4],ax
        mov     [edi+6],ax
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 8-wide write-only, starting at an even address.

        align   4
draw_8_wide_even_loop     proc    near
        UNROLL_LOOP     DRAW_8_WIDE_EVEN,W8_EVEN,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_8_wide_even_loop

        ret

draw_8_wide_even_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three write-only bytes, then advance to next scan line.
; Optimized for odd start address.

DRAW_8_WIDE_ODD macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ah
        mov     [edi+1],ax
        mov     [edi+3],ax
        mov     [edi+5],ax
        mov     [edi+7],al
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 8-wide write-only, starting at an odd address.

        align   4
draw_8_wide_odd_loop     proc    near
        UNROLL_LOOP     DRAW_8_WIDE_ODD,W8_ODD,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_8_wide_odd_loop

        ret

draw_8_wide_odd_loop     endp

;-----------------------------------------------------------------------;
; Unrolled 1-, 2-, 3-, and 4-wide read before write drawing loops.
;
; Entry:
;       AL = pixel mask
;       EBX = unrolled loop count
;       ECX = scan line width in bytes
;       EDI = start offset
;
; EBX, EDI modified. All other registers preserved.

        UNROLL_LOOP_ENTRY_TABLE pfnDraw1WideEntry,RW1,LOOP_UNROLL_COUNT

;-----------------------------------------------------------------------;
; Macro to draw one read before write byte, then advance to next scan line.

DRAW_1_WIDE macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     dh,[edi]                ;load latches w/o destroying our data
        mov     [edi],al                ;write out our byte
        add     edi,ecx                 ;move to the next blind
        endm    ;-----------------------------------;

; 1-wide read/write.

        align   4
draw_1_wide_loop     proc    near
        UNROLL_LOOP     DRAW_1_WIDE,RW1,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_1_wide_loop

        ret

draw_1_wide_loop     endp

; Tables of entry points into unrolled 1-, 2-, 3-, and 4-wide, and 5-or-wider
; read before write loops.

        UNROLL_LOOP_ENTRY_TABLE pfnDraw1RWEntry,RWW1,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw2RWEntry,RWW2,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw3RWEntry,RWW3,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw4RWEntry,RWW4,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawRWWideEntry,RWWIDE,LOOP_UNROLL_COUNT


;-----------------------------------------------------------------------;
; Unrolled 1-, 2-, 3-, and 4-wide read before write drawing loops.
;
; Entry:
;       AL = pixel mask
;       EBX = unrolled loop count
;       ECX = scan line width in bytes
;       EDI = start offset
;
; EBX, EDI modified. All other registers preserved.

;-----------------------------------------------------------------------;
; Macro to draw one read before write byte, then advance to next scan line.

DRAW_1_WIDE_RW macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ah,[edi]
        mov     [edi],al
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 1-wide read/write.

        align   4
draw_1_wide_rop_loop     proc    near
        UNROLL_LOOP     DRAW_1_WIDE_RW,RWW1,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_1_wide_rop_loop

        ret

draw_1_wide_rop_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw two read before write bytes, then advance to next scan line.

DRAW_2_WIDE_RW macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ah,[edi]
        mov     [edi],al
        mov     ah,[edi+1]
        mov     [edi+1],al
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 2-wide read/write.

        align   4
draw_2_wide_rop_loop     proc    near
        UNROLL_LOOP     DRAW_2_WIDE_RW,RWW2,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_2_wide_rop_loop

        ret

draw_2_wide_rop_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three read before write bytes, then advance to next scan line.

DRAW_3_WIDE_RW macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ah,[edi]
        mov     [edi],al
        mov     ah,[edi+1]
        mov     [edi+1],al
        mov     ah,[edi+2]
        mov     [edi+2],al
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 3-wide read/write.

        align   4
draw_3_wide_rop_loop     proc    near
        UNROLL_LOOP     DRAW_3_WIDE_RW,RWW3,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_3_wide_rop_loop

        ret

draw_3_wide_rop_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw four read before write bytes, then advance to next scan line.

DRAW_4_WIDE_RW macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ah,[edi]
        mov     [edi],al
        mov     ah,[edi+1]
        mov     [edi+1],al
        mov     ah,[edi+2]
        mov     [edi+2],al
        mov     ah,[edi+3]
        mov     [edi+3],al
        add     edi,edx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 4-wide read/write.

        align   4
draw_4_wide_rop_loop     proc    near
        UNROLL_LOOP     DRAW_4_WIDE_RW,RWW4,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_4_wide_rop_loop

        ret

draw_4_wide_rop_loop     endp

;-----------------------------------------------------------------------;
; Unrolled 5-or-wider read before write loop.
;
; Entry:
;       EAX = # of bytes to fill across scan line (needed only by 5-or-wider
;               handler)
;       EBX = unrolled loop count
;       EDX = offset from end of one scan line to the start of the next next
;       EDI = start offset
;
; EBX, ECX, ESI, EDI modified. All other registers preserved.

;-----------------------------------------------------------------------;
; Macro to draw five or more read before write bytes, then advance to
; next scan line. (Actually, will handle any number of bytes,
; including 0, but there are special-case handlers for narrow cases.)
; Works because reads of display memory return 0ffh, which then becomes the
; Bit Mask as it's written in write mode 3.

DRAW_WIDE_RW macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ecx,esi
@@:     mov     ah,[edi]
        mov     [edi],al
        inc     edi
        dec     ecx
        jnz     @b
        add     edi,edx
        endm    ;-----------------------------------;

; 5-or-wider read/write.

        align   4
draw_wide_rop_loop proc  near
        UNROLL_LOOP     DRAW_WIDE_RW,RWWIDE,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_rop_loop

        ret

draw_wide_rop_loop endp

_TEXT$01   ends

        end

;masm386 -Mx -I..\..\inc -I..\..\..\inc -Id:\nt\public\sdk\inc -Di386=1 -DNT_INST=0 -DNT_UP=1 -DSTD_CALL -DDBG=1 -DDEVL=1 i386\patblt.asm,obj\i386\patblt.obj,x.lst;


