;---------------------------Module-Header------------------------------;
; Module Name: vgablts.asm
;
; Copyright (c) 1992 Microsoft Corporation
;-----------------------------------------------------------------------;
;-----------------------------------------------------------------------;
; VOID vTrgBlt(PDEVSURF pdsurf, ULONG culRcl, RECTL * prcl, MIX ulMix,
;              ULONG ulClr);
; Input:
;  pdsurf - surface to which to draw
;  culRcl - # of rectangles to fill
;  prcl   - pointer to list of rectangles to fill
;  ulMix  - mix rop with which to fill
;  ulClr  - color with which to fill
;
; Performs accelarated solid area fills for all mixes.
;
;-----------------------------------------------------------------------;
;
; Note: Assumes all rectangles have positive heights and widths. Will not
; work properly if this is not the case.
;
;-----------------------------------------------------------------------;
;
; Note: Cases where the width of the whole bytes fill is equal to the
; width of the bitmap could be sped up by using a single REP MOVS or REP
; STOS, but how often does WIN32 do a fill that's the width of the screen?
; Not very.
;
;-----------------------------------------------------------------------;

        comment $

The overall approach of this module is to accept a list of rectangles to
fill, set up the VGA hardware for the desired fill, and then fill the
rectangles one at a time. Each rectangle fill is set up for everything
but vertical parameters, and then decomposed into the sections that
intersect each VGA bank; each section is drawn in turn. The drawing code
is heavily unrolled for performance, and vectors are set up so that the
drawing code appropriate for the desired fill is essentially threaded
together.

        commend $

;-----------------------------------------------------------------------;

; Set LOOP_UNROLL_SHIFT to the log2 of the number of times you want loops in
; this module unrolled. For example, LOOP_UNROLL_SHIFT of 3 yields 2**3 = 8
; times unrolling. This is the only thing you need to change to control
; unrolling.

LOOP_UNROLL_SHIFT equ 2

;-----------------------------------------------------------------------;

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

        .list

;-----------------------------------------------------------------------;

        .data

;-----------------------------------------------------------------------;
; Left edge clip masks for intrabyte start addresses 0 through 7.
; Whole byte cases are flagged as 0ffh.
	public jLeftMask
jLeftMask       label   byte
        db      0ffh,07fh,03fh,01fh,00fh,007h,003h,001h

;-----------------------------------------------------------------------;
; Right edge clip masks for intrabyte end addresses (non-inclusive)
; 0 through 7. Whole byte cases are flagged as 0ffh.
	public jRightMask
jRightMask      label   byte
        db      0ffh,080h,0c0h,0e0h,0f0h,0f8h,0fch,0feh

;-----------------------------------------------------------------------;
; Tables used to set up for the desired raster op. Note that entries for raster
; ops that aren't handled here are generally correct, except that they ignore
; need for inversion of the destination, which those rops require.

; Table used to force off the drawing color for R2_BLACK (0).
; The first entry is ignored; there is no mix 0.
	public jForceOffTable
jForceOffTable  db         0
                db         0,0ffh,0ffh,0ffh,0ffh,0ffh,0ffh,0ffh
                db      0ffh,0ffh,   0,0ffh,0ffh,0ffh,0ffh,0ffh

;-----------------------------------------------------------------------;
; Table used to force on the drawing color for R2_NOT (Dn) and R2_WHITE (1).
; The first entry is ignored; there is no mix 0.
	public	jForceOnTable
jForceOnTable   db      0, 0,0,0,0,0,0ffh,0,0,0,0,0,0,0,0,0,0ffh

;-----------------------------------------------------------------------;
; Table used to invert the passed-in drawing color for Pn mixes.
; The first entry is ignored; there is no mix 0.
	public  jNotTable
jNotTable       db      0, 0,0ffh,0ffh,0ffh,0,0,0,0ffh,0,0ffh,0,0ffh,0,0,0,0

;-----------------------------------------------------------------------;
; Table of VGA ALU logical functions corresponding to mixes. Note that Dn is
; handled as a separate preceding inversion pass when part of a more complex
; mix.
; The first entry is ignored; there is no mix 0.
	public jALUFuncTable	
jALUFuncTable   db      0
                db      DR_SET,DR_AND,DR_AND,DR_SET
                db      DR_AND,DR_XOR,DR_XOR,DR_OR
                db      DR_AND,DR_XOR,     0,DR_OR
                db      DR_SET,DR_OR ,DR_OR ,DR_SET

;-----------------------------------------------------------------------;
; 1 entries mark rops that require two passes, one to invert the destination
; and then another to finish the rop.
; The first entry is ignored; there is no mix 0.
	public	jInvertDest
jInvertDest     db      0, 0,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0

;-----------------------------------------------------------------------;
; Table of routines to be called to draw edges, according to which edges are
; partial and which edges are whole bytes.
        align   4
pfnEdgeDrawing  label   dword
        dd      do_right_edge_bytes
        dd      do_both_edge_bytes
        dd      check_next_bank
        dd      do_left_edge_bytes

;-----------------------------------------------------------------------;
; Table of pointers to tables used to find entries points in unrolled wide
; whole byte code.

        align   4
pfnWideWholeRep label   dword
        dd      pfnDrawWideW00Entry
        dd      pfnDrawWideW01Entry
        dd      pfnDrawWideW02Entry
        dd      pfnDrawWideW03Entry
        dd      pfnDrawWideW10Entry
        dd      pfnDrawWideW11Entry
        dd      pfnDrawWideW12Entry
        dd      pfnDrawWideW13Entry
        dd      pfnDrawWideW20Entry
        dd      pfnDrawWideW21Entry
        dd      pfnDrawWideW22Entry
        dd      pfnDrawWideW23Entry
        dd      pfnDrawWideW30Entry
        dd      pfnDrawWideW31Entry
        dd      pfnDrawWideW32Entry
        dd      pfnDrawWideW33Entry

;-----------------------------------------------------------------------;
; Table of pointers to tables used to find entries points in narrow, special-
; cased unrolled non-replace whole byte code.

; Note: The breakpoint where one should switch from special-casing to
;  REP MOVSB is purely a guess on my part. 5 seemed reasonable.

        align   4
pfnWholeBytesNonReplaceEntries  label   dword
        dd      0                       ;we never get a 0-wide case
        dd      pfnDraw1WideRWEntry
        dd      pfnDraw2WideRWEntry
        dd      pfnDraw3WideRWEntry
        dd      pfnDraw4WideRWEntry
MAX_NON_REPLACE_SPECIAL equ     ($-pfnWholeBytesNonReplaceEntries)/4

;-----------------------------------------------------------------------;
; Table of pointers to tables used to find entry points in narrow, special-
; cased unrolled replace whole byte code.

; Note: The breakpoint where one should switch from special-casing to
;  REP STOS is purely a guess on my part. 8 seemed reasonable.

; Start address MOD 3 is 0.
        align   4
pfnWholeBytesMod0ReplaceEntries  label   dword
        dd      0                       ;we never get a 0-wide case
        dd      pfnDraw1WideWEntry
        dd      pfnDraw2WideWEntry
        dd      pfnDraw3WideWEvenEntry
        dd      pfnDraw4WideWEntry
        dd      pfnDraw5WideWEvenEntry
        dd      pfnDraw6WideWMod3_0Entry
        dd      pfnDraw7WideWMod3_0Entry
        dd      pfnDraw8WideWMod3_0Entry
MAX_REPLACE_SPECIAL equ     ($-pfnWholeBytesMod0ReplaceEntries)/4

; Start address MOD 3 is 1.
        align   4
pfnWholeBytesMod1ReplaceEntries  label   dword
        dd      0                       ;we never get a 0-wide case
        dd      pfnDraw1WideWEntry
        dd      pfnDraw2WideWEntry
        dd      pfnDraw3WideWOddEntry
        dd      pfnDraw4WideWEntry
        dd      pfnDraw5WideWOddEntry
        dd      pfnDraw6WideWMod3_1Entry
        dd      pfnDraw7WideWMod3_1Entry
        dd      pfnDraw8WideWMod3_1Entry

; Start address MOD 3 is 2.
        align   4
pfnWholeBytesMod2ReplaceEntries  label   dword
        dd      0                       ;we never get a 0-wide case
        dd      pfnDraw1WideWEntry
        dd      pfnDraw2WideWEntry
        dd      pfnDraw3WideWEvenEntry
        dd      pfnDraw4WideWEntry
        dd      pfnDraw5WideWEvenEntry
        dd      pfnDraw6WideWMod3_2Entry
        dd      pfnDraw7WideWMod3_2Entry
        dd      pfnDraw8WideWMod3_2Entry

; Start address MOD 3 is 3.
        align   4
pfnWholeBytesMod3ReplaceEntries  label   dword
        dd      0                       ;we never get a 0-wide case
        dd      pfnDraw1WideWEntry
        dd      pfnDraw2WideWEntry
        dd      pfnDraw3WideWOddEntry
        dd      pfnDraw4WideWEntry
        dd      pfnDraw5WideWOddEntry
        dd      pfnDraw6WideWMod3_1Entry
        dd      pfnDraw7WideWMod3_3Entry
        dd      pfnDraw8WideWMod3_3Entry

; Master MOD 3 alignment look-up table for entry tables for four possible
; alignments for narrow, special-cased unrolled replace whole byte code.
        align   4
pfnWholeBytesReplaceMaster      label   dword
        dd      pfnWholeBytesMod0ReplaceEntries
        dd      pfnWholeBytesMod1ReplaceEntries
        dd      pfnWholeBytesMod2ReplaceEntries
        dd      pfnWholeBytesMod3ReplaceEntries

;-----------------------------------------------------------------------;

                .code

_TEXT$01   SEGMENT DWORD USE32 PUBLIC 'CODE'
           ASSUME  CS:FLAT, DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;-----------------------------------------------------------------------;

cProc   vTrgBlt,20,<         \
        uses    esi edi ebx, \
        pdsurf: ptr DEVSURF, \
        culRcl: dword,       \
        prcl:   ptr RECTL,   \
        ulMix:  dword,       \
        ulColor:dword        >

        local   ulRowOffset :dword      ;Offset from start of scan line of
                                        ; first byte to fill
        local   ulWholeBytes :dword     ;# of whole bytes to fill
        local   ulWholeDwords :dword    ;# of whole dwords to fill
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
        local   pfnDraw1WideVector :dword ;address at which to enter unrolled
                                          ; edge loop
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
        local   jInvertDestFirst :dword ;1 if the rop requires a pass to invert
                                        ; the destination before the normal
                                        ; pass
        local   jDrawingColor :dword    ;color with which we're drawing
                                        ; dword to finish out fill

;-----------------------------------------------------------------------;

        cld

;-----------------------------------------------------------------------;
; Make sure there's something to draw; clip enumerations can be empty.
;-----------------------------------------------------------------------;

        cmp     culRcl,0                ;any rects to fill?
        jz      vTrgBlts_done           ;no, we're done


;-----------------------------------------------------------------------;
; Set up variables that are constant for the entire time we're in this
; module.
;-----------------------------------------------------------------------;

;-----------------------------------------------------------------------;
; Set up for the desired raster op.
;-----------------------------------------------------------------------;

        sub     ebx,ebx                 ;ignore any background mix; we're only
        mov     bl,byte ptr ulMix       ; concerned with the foreground in this
                                        ; module
        cmp     ebx,R2_NOP              ;is this NOP?
        jz      vTrgBlts_done           ;yes, we're done
        mov     al,jInvertDest[ebx]          ;remember whether we need to
        mov     byte ptr jInvertDestFirst,al ; invert the destination before
                                             ; finishing the rop
        mov     ah,byte ptr ulColor     ;get the drawing color
        and     ah,jForceOffTable[ebx]  ;force color to 0 if necessary
                                        ; (R2_BLACK)
        or      ah,jForceOnTable[ebx]   ;force color to 0ffh if necessary
                                        ; (R2_WHITE, R2_NOT)
        xor     ah,jNotTable[ebx]       ;invert color if necessary (any Pn mix)
                                        ;at this point, CH has the color we
                                        ; want to draw with; set up the VGA
                                        ; hardware to draw with that color
        mov     byte ptr jDrawingColor,ah ;remember drawing color for restoring
                                          ; after inversion
        mov     edx,VGA_BASE + GRAF_ADDR
        mov     al,GRAF_SET_RESET       ;set/reset = color to write
        out     dx,ax
        mov     eax,0F00h + GRAF_ENAB_SR ;enable set/reset for all planes, so
        out     dx,ax                   ; set/reset color we just set becomes
                                        ; the drawing color, regardless of the
                                        ; value written by the CPU

        mov     ah,jALUFuncTable[ebx]   ;get the ALU logical function
        and     ah,ah                   ;is the logical function DR_SET?
        .errnz  DR_SET
        jz      short skip_ALU_set      ;yes, don't have to set because that's
                                        ; the VGA's default state
        mov     al,GRAF_DATA_ROT
        out     dx,ax                   ;set the ALU logical function
skip_ALU_set:
        mov     byte ptr jALUFunc,ah    ;remember the ALU logical function

	mov	eax,GRAF_MODE + ((M_AND_WRITE + M_COLOR_READ) SHL 8)
	out	dx,ax			;write mode 3 so we can do the masking
					; without OUTs, read mode 1 so we can
					; read 0xFF from memory always, for
					; ANDing (because Color Don't Care is
					; all zeros)

;-----------------------------------------------------------------------;
; Fill the current rectangle with the specified raster op and color.
;-----------------------------------------------------------------------;

fill_rect_loop:

;-----------------------------------------------------------------------;
; Set up variables that are constant from bank to bank during a single
; fill.
;-----------------------------------------------------------------------;

;-----------------------------------------------------------------------;
; Set up masks and widths.
;-----------------------------------------------------------------------;

        mov     edi,prcl                ;point to rectangle to fill
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
        jz      short start_vec_set     ;no
                                        ;yes, so draw the whole bytes before
                                        ; the edge bytes

; The whole bytes loop depends on the type of operation being done. If the
; operation is one which uses DR_SET, then we can use a STOS-type operation,
; else we have to use a MOVSB-type operation (to load the latches with the
; existing contents of display memory to allow the ALUs to work).

        cmp     byte ptr jALUFunc,DR_SET ;is it a replace-type rop?
        jz      short is_replace_type   ;yes
                                        ;no, set up for non-replace whole bytes
        mov     ecx,offset whole_bytes_non_replace_wide
                                        ;assume too wide to special-case
        cmp     ebx,MAX_NON_REPLACE_SPECIAL ;too wide to special case?
        jnb     short start_vec_set     ;yes
        mov     ecx,pfnWholeBytesNonReplaceEntries[ebx*4] ;no, point to entry
        mov     pfnWholeBytes,ecx       ; table for width
        mov     ecx,offset whole_bytes_special
                                        ;set up to call special routine to fill
                                        ; whole bytes
        jmp     short start_vec_set

        align   4
is_replace_type:                        ;set up for replace-type rop
        cmp     ebx,MAX_REPLACE_SPECIAL ;too wide to special case?
        jnb     short is_wide_replace   ;yes
                                        ;narrow enough to special case. Look up
                                        ; the entry table for the special case
                                        ; base on the start alignment
        mov     ecx,ulRowOffset
        add     ecx,ulLeftEdgeAdjust    ;left edge whole bytes start offset
        and     ecx,011b                ;left edge whole bytes start alignment
                                        ; MOD 3
        mov     ecx,pfnWholeBytesReplaceMaster[ecx*4] ;look up table of entry
                                                      ; tables for alignment
        mov     ecx,[ecx+ebx*4]         ;look up entry table for width
        mov     pfnWholeBytes,ecx       ; table for width
        mov     ecx,offset whole_bytes_special
                                        ;set up to call special routine to fill
                                        ; whole bytes
        jmp     short start_vec_set

        align   4
is_wide_replace:                        ;set up for wide replace-type op
                                        ;Note: assumes there is at least one
                                        ; full dword involved!
        mov     ecx,ulRowOffset
        add     ecx,ulLeftEdgeAdjust    ;left edge whole bytes start offset
        neg     ecx
        and     ecx,011b
        mov     edx,ebx
        sub     edx,ecx                 ;ignore odd leading bytes
        mov     eax,edx
        shr     edx,2                   ;# of whole dwords across (not counting
                                        ; odd leading & trailing bytes)
        mov     ulWholeDwords,edx
        and     eax,011b                ;# of odd (fractional) trailing bytes
        shl     ecx,2
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


;-----------------------------------------------------------------------;
; Fill this rectangle.
;-----------------------------------------------------------------------;

        cmp     byte ptr jInvertDestFirst,1
                                        ;is this an invert-dest-plus-something-
                                        ; else rop that requires two passes?
        jz      short do_invert_dest_rop ;yes, special case with two passes

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
	mov	eax,GRAF_MODE + ((M_PROC_WRITE + M_DATA_READ) SHL 8)
	out	dx,ax			;restore read mode 0 and write mode 0
        cmp     byte ptr jALUfunc,DR_SET ;is the logical function already SET?
        jz      short vTrgBlts_done               ;yes, no need to reset it
        mov     eax,(DR_SET shl 8) + GRAF_DATA_ROT ;set the logical function to
        out     dx,ax                             ; SET
vTrgBlts_done:
        cRet    vTrgBlt


;-----------------------------------------------------------------------;
; Handles rops that require two passes, the first being a destination
; inversion pass.
;-----------------------------------------------------------------------;

        align   4
do_invert_dest_rop:

; Set up the VGA's hardware for inversion

        mov     edx,VGA_BASE + GRAF_ADDR
        mov     eax,0ff00h + GRAF_SET_RESET ;set/reset = 0ffh to invert in
        out     dx,ax                       ; conjunction with XOR
        mov     eax,(DR_XOR shl 8) + GRAF_DATA_ROT
        out     dx,ax                   ;logical function = XOR to invert

; Invert the destination

        call    draw_banks

; Restore the VGA's hardware to the state required for the second pass.

        mov     edx,VGA_BASE + GRAF_ADDR
        mov     ah,byte ptr jDrawingColor
        mov     al,GRAF_SET_RESET       ;set/reset = color to write
        out     dx,ax
        mov     ah,byte ptr jALUFunc
        mov     al,GRAF_DATA_ROT
        out     dx,ax                   ;set the ALU logical function

; Perform the second pass to finish the rop.

        jmp     do_single_pass


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
whole_bytes_rep_wide:
        push    ebx                     ;save scan count
        push    edi                     ;save starting address

        mov     eax,pfnWholeBytes       ;point to entry table for unrolled
                                        ; loop for whole byte width
        SET_UP_UNROLL_VARS ebx,ecx, ebx,[eax], LOOP_UNROLL_SHIFT
        add     edi,ulLeftEdgeAdjust    ;point to first whole byte to fill
        mov     esi,ulWholeDwords       ;whole dwords width
	mov	eax,-1			;this will become the Bit Mask,
					; enabling drawing to all bits
        mov     edx,ulNextScan          ;offset from end of one scan line to
                                        ; start of next
        call    ecx                     ;draw the wide whole bytes

        pop     edi                     ;restore screen pointer
        pop     ebx                     ;restore fill scan count
        jmp     pfnContinueDrawing      ;either keep drawing or we're done


;-----------------------------------------------------------------------;
; Handles both replace and non-replace whole byte fills narrow enough to
; special case.
;-----------------------------------------------------------------------;

        align   4
whole_bytes_special:
        push    ebx                     ;save scan count
        push    edi                     ;save starting address

        mov     eax,pfnWholeBytes       ;point to entry table for unrolled
                                        ; loop for whole byte width
        SET_UP_UNROLL_VARS ebx,edx, ebx,[eax], LOOP_UNROLL_SHIFT
        add     edi,ulLeftEdgeAdjust    ;point to first whole byte to fill
        mov     ecx,ulScanWidth         ;offset to next scan line
	mov	eax,-1			;this will become the Bit Mask,
					; enabling drawing to all bits
        call    edx                     ;draw the whole bytes

        pop     edi                     ;restore screen pointer
        pop     ebx                     ;restore fill scan count
        jmp     pfnContinueDrawing      ;either keep drawing or we're done


;-----------------------------------------------------------------------;
; Handles non-replace whole byte fills wider than the maximum special case
; width.
;
; The destination is involved, so a MOVSB (or equivalent) must be
; performed in order to do a read before write to give the ALUs something
; to work with.
;-----------------------------------------------------------------------;

        align   4
whole_bytes_non_replace_wide:
        push    ebx                     ;save scan count
        push    edi                     ;save starting address

        SET_UP_UNROLL_VARS ebx,ecx, ebx,pfnDrawWideRWEntry, LOOP_UNROLL_SHIFT
        add     edi,ulLeftEdgeAdjust    ;point to first whole byte to fill
        mov     eax,ulWholeBytes        ;whole bytes width
        mov     edx,ulNextScan          ;offset from end of one scan line to
                                        ; start of next
        call    ecx                     ;draw the wide whole bytes

        pop     edi                     ;restore screen pointer
        pop     ebx                     ;restore fill scan count
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
	public do_both_edge_bytes
do_both_edge_bytes:

; Set up variables for entering unrolled loop.

        SET_UP_UNROLL_VARS ebx,edx, ebx,pfnDraw1WideRWEntry, LOOP_UNROLL_SHIFT
        mov     pfnDraw1WideVector,edx

        mov     ecx,ulScanWidth         ;offset from one scan to next

        mov     esi,ulWholeBytes        ;ESI = # of whole bytes
        lea     esi,[esi+edi+1]         ;--> start for right edge
        mov     al,byte ptr ulMasks     ;this will become the Bit Mask for the
					; left edge
        push    ebx                     ;preserve scan line count
        call    edx                     ;jump into the unrolled loop to draw
        pop     ebx                     ;restore scan line count

        mov     edi,esi                 ;point to first right edge byte
        mov     al,byte ptr ulMasks+1   ;this will become the Bit Mask for the
					; right edge
        push    offset edges_done       ;return here
        jmp     pfnDraw1WideVector      ;jump into the unrolled loop to draw

;-----------------------------------------------------------------------;
; Handle case where only the left edge is partial (non-whole).
;-----------------------------------------------------------------------;
        align   4
do_left_edge_bytes:

; Set up variables for entering unrolled loop.

        SET_UP_UNROLL_VARS ebx,esi, ebx,pfnDraw1WideRWEntry, LOOP_UNROLL_SHIFT

        mov     ecx,ulScanWidth         ;offset from one scan to next
        mov     al,byte ptr ulMasks     ;this will become the Bit Mask for the
					; left edge
        push    offset edges_done       ;return here
        jmp     esi                     ;jump into the unrolled loop to draw

;-----------------------------------------------------------------------;
; Handle case where only the right edge is partial (non-whole).
;-----------------------------------------------------------------------;
        align   4
do_right_edge_bytes:

; Set up variables for entering unrolled loop.

        SET_UP_UNROLL_VARS ebx,esi, ebx,pfnDraw1WideRWEntry, LOOP_UNROLL_SHIFT

        mov     ecx,ulScanWidth         ;offset from one scan to next
        add     edi,ulWholeBytes        ;--> start for right edge (remember,
                                        ; left edge is whole, so the left edge
                                        ; byte is included in the whole byte
                                        ; count)
        mov     al,byte ptr ulMasks+1   ;this will become the Bit Mask for the
					; right edge
        call    esi                     ;jump into the unrolled loop to draw

;-----------------------------------------------------------------------;
; We have done all partial edges.
;-----------------------------------------------------------------------;

edges_done:

;-----------------------------------------------------------------------;
; See if there are any more banks to process.
;-----------------------------------------------------------------------;

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

; Draw in the new bank.

        jmp     pfnStartDrawing


;-----------------------------------------------------------------------;
; Done with all banks in this fill.

banks_done:
        PLAIN_RET

endProc vTrgBlt


;-----------------------------------------------------------------------;
; Unrolled loops.
; There are two kinds of unrolled loops: read-before-write (to load the
;  latches), and write-only (for replace-type rops).
;-----------------------------------------------------------------------;


;-----------------------------------------------------------------------;
; Unrolled drawing stuff for cases where read before write is required,
; to load the latches.
;-----------------------------------------------------------------------;

; Tables of entry points into unrolled 1-, 2-, 3-, and 4-wide, and 5-or-wider
; read before write loops.

        UNROLL_LOOP_ENTRY_TABLE pfnDraw1WideRWEntry,RW1,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw2WideRWEntry,RW2,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw3WideRWEntry,RW3,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw4WideRWEntry,RW4,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideRWEntry,RWWIDE,LOOP_UNROLL_COUNT

;-----------------------------------------------------------------------;
; Unrolled 1-, 2-, 3-, and 4-wide read before write drawing loops.
;
; Entry:
;	AL = pixel mask
;       EBX = unrolled loop count
;       ECX = scan line width in bytes
;       EDI = start offset
;
; EBX, EDI modified. All other registers preserved.

;-----------------------------------------------------------------------;
; Macro to draw one read before write byte, then advance to next scan line.

DRAW_1_WIDE_RW macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        and     [edi],al                ;we always read 0xFF, so AL is written
					; as-is; because we're in write mode 3,
					; AL becomes the Bit Mask
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 1-wide read/write.

        align   4
draw_1_wide_rw_loop     proc    near
        UNROLL_LOOP     DRAW_1_WIDE_RW,RW1,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_1_wide_rw_loop

        ret

draw_1_wide_rw_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw two read before write bytes, then advance to next scan line.

DRAW_2_WIDE_RW macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        and     [edi],al
        and     [edi+1],al
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 2-wide read/write.

        align   4
draw_2_wide_rw_loop     proc    near
        UNROLL_LOOP     DRAW_2_WIDE_RW,RW2,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_2_wide_rw_loop

        ret

draw_2_wide_rw_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three read before write bytes, then advance to next scan line.

DRAW_3_WIDE_RW macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        and     [edi],al
        and     [edi+1],al
        and     [edi+2],al
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 3-wide read/write.

        align   4
draw_3_wide_rw_loop     proc    near
        UNROLL_LOOP     DRAW_3_WIDE_RW,RW3,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_3_wide_rw_loop

        ret

draw_3_wide_rw_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw four read before write bytes, then advance to next scan line.

DRAW_4_WIDE_RW macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        and     [edi],al
        and     [edi+1],al
        and     [edi+2],al
        and     [edi+3],al
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 4-wide read/write.

        align   4
draw_4_wide_rw_loop     proc    near
        UNROLL_LOOP     DRAW_4_WIDE_RW,RW4,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_4_wide_rw_loop

        ret

draw_4_wide_rw_loop     endp

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
        mov     esi,edi
        mov     ecx,eax
        rep     movsb
        add     edi,edx
        endm    ;-----------------------------------;

; 5-or-wider read/write.

        align   4
draw_wide_rw_loop proc  near
        UNROLL_LOOP     DRAW_WIDE_RW,RWWIDE,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_rw_loop

        ret

draw_wide_rw_loop endp


;-----------------------------------------------------------------------;
; Unrolled drawing stuff (unrolled to reduce jumps to speed things up),
; for cases where read before write is NOT required.
;-----------------------------------------------------------------------;

; Tables of entry points into unrolled 1-, 2-, 3-, and 4-wide write-only loops.
; Note that there may be separate entry tables for various alignments of a
; specific width, in cases where performance can be improved by using different
; code for different alignments.

        UNROLL_LOOP_ENTRY_TABLE pfnDraw1WideWEntry,W1,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw2WideWEntry,W2,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw3WideWEvenEntry,W3_EVEN,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw3WideWOddEntry,W3_ODD,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw4WideWEntry,W4,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw5WideWEvenEntry,W5_EVEN,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw5WideWOddEntry,W5_ODD,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw6WideWMod3_0Entry,W6_MOD3_0,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw6WideWMod3_1Entry,W6_MOD3_1,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw6WideWMod3_2Entry,W6_MOD3_2,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw7WideWMod3_0Entry,W7_MOD3_0,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw7WideWMod3_1Entry,W7_MOD3_1,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw7WideWMod3_2Entry,W7_MOD3_2,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw7WideWMod3_3Entry,W7_MOD3_3,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw8WideWMod3_0Entry,W8_MOD3_0,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw8WideWMod3_1Entry,W8_MOD3_1,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw8WideWMod3_2Entry,W8_MOD3_2,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDraw8WideWMod3_3Entry,W8_MOD3_3,LOOP_UNROLL_COUNT


;-----------------------------------------------------------------------;
; Unrolled 1-, 2-, 3-, and 4-wide write-only edge-drawing loops.
;
; Entry:
;	AL/AX/EAX = pixel mask (if AX or EAX, then 0xFFFF or 0xFFFFFFFF)
;       EBX = unrolled loop count
;       ECX = scan line width in bytes
;       EDI = start offset
;
; EBX, EDI modified. All other registers preserved.

;-----------------------------------------------------------------------;
; Macro to draw one write-only byte, then advance to next scan line.

DRAW_1_WIDE_W macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],al                ;we always read 0xFF, so AL is written
					; as-is; because we're in write mode 3,
					; AL becomes the Bit Mask
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 1-wide write-only.

        align   4
draw_1_wide_w_loop     proc    near
        UNROLL_LOOP     DRAW_1_WIDE_W,W1,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_1_wide_w_loop

        ret

draw_1_wide_w_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw two write-only bytes, then advance to next scan line.

DRAW_2_WIDE_W macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ax
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 2-wide write-only.

        align   4
draw_2_wide_w_loop     proc    near
        UNROLL_LOOP     DRAW_2_WIDE_W,W2,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_2_wide_w_loop

        ret

draw_2_wide_w_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three write-only bytes, then advance to next scan line.
; Optimized for even start address.

DRAW_3_WIDE_W_EVEN macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ax
        mov     [edi+2],al
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 3-wide write-only, starting at an even address.

        align   4
draw_3_wide_w_even_loop     proc    near
        UNROLL_LOOP     DRAW_3_WIDE_W_EVEN,W3_EVEN,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_3_wide_w_even_loop

        ret

draw_3_wide_w_even_loop     endp

;-----------------------------------------------------------------------;
; Macro to draw three write-only bytes, then advance to next scan line.
; Optimized for odd start address.

DRAW_3_WIDE_W_ODD macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],al
        mov     [edi+1],ax
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 3-wide write-only, starting at an odd address.

        align   4
draw_3_wide_w_odd_loop     proc    near
        UNROLL_LOOP     DRAW_3_WIDE_W_ODD,W3_ODD,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_3_wide_w_odd_loop

        ret

draw_3_wide_w_odd_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw four write-only bytes, then advance to next scan line.

DRAW_4_WIDE_W macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],eax
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 4-wide write-only.

        align   4
draw_4_wide_w_loop     proc    near
        UNROLL_LOOP     DRAW_4_WIDE_W,W4,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_4_wide_w_loop

        ret

draw_4_wide_w_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw five write-only bytes, then advance to next scan line.
; Optimized for even start address.

DRAW_5_WIDE_W_EVEN macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],eax
        mov     [edi+4],al
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 5-wide write-only, starting at an even address.

        align   4
draw_5_wide_w_even_loop     proc    near
        UNROLL_LOOP     DRAW_5_WIDE_W_EVEN,W5_EVEN,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_5_wide_w_even_loop

        ret

draw_5_wide_w_even_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw five write-only bytes, then advance to next scan line.
; Optimized for odd start address.

DRAW_5_WIDE_W_ODD macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],al
        mov     [edi+1],eax
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 5-wide write-only, starting at an odd address.

        align   4
draw_5_wide_w_odd_loop     proc    near
        UNROLL_LOOP     DRAW_5_WIDE_W_ODD,W5_ODD,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_5_wide_w_odd_loop

        ret

draw_5_wide_w_odd_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw six write-only bytes, then advance to next scan line.
; Optimized for start address MOD 3 == 0.

DRAW_6_WIDE_W_MOD3_0 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],eax
        mov     [edi+4],ax
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 6-wide write-only, starting at MOD 3 == 0.

        align   4
draw_6_wide_w_mod3_0_loop     proc    near
        UNROLL_LOOP     DRAW_6_WIDE_W_MOD3_0,W6_MOD3_0,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_6_wide_w_mod3_0_loop

        ret

draw_6_wide_w_mod3_0_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw six write-only bytes, then advance to next scan line.
; Optimized for start address MOD 3 == 1 or 3.

DRAW_6_WIDE_W_MOD3_1 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],al
        mov     [edi+1],eax
        mov     [edi+5],al
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 6-wide write-only, starting at MOD 3 == 1 or 3.

        align   4
draw_6_wide_w_mod3_1_loop     proc    near
        UNROLL_LOOP     DRAW_6_WIDE_W_MOD3_1,W6_MOD3_1,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_6_wide_w_mod3_1_loop

        ret

draw_6_wide_w_mod3_1_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw six write-only bytes, then advance to next scan line.
; Optimized for start address MOD 3 == 2.

DRAW_6_WIDE_W_MOD3_2 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ax
        mov     [edi+2],eax
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 6-wide write-only, starting at MOD 3 == 2.

        align   4
draw_6_wide_w_mod3_2_loop     proc    near
        UNROLL_LOOP     DRAW_6_WIDE_W_MOD3_2,W6_MOD3_2,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_6_wide_w_mod3_2_loop

        ret

draw_6_wide_w_mod3_2_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw seven write-only bytes, then advance to next scan line.
; Optimized for start address MOD 3 == 0.

DRAW_7_WIDE_W_MOD3_0 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],eax
        mov     [edi+4],ax
        mov     [edi+6],al
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 7-wide write-only, starting at MOD 3 == 0.

        align   4
draw_7_wide_w_mod3_0_loop     proc    near
        UNROLL_LOOP     DRAW_7_WIDE_W_MOD3_0,W7_MOD3_0,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_7_wide_w_mod3_0_loop

        ret

draw_7_wide_w_mod3_0_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw seven write-only bytes, then advance to next scan line.
; Optimized for start address MOD 3 == 1.

DRAW_7_WIDE_W_MOD3_1 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],al
        mov     [edi+1],ax
        mov     [edi+3],eax
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 7-wide write-only, starting at MOD 3 == 0.

        align   4
draw_7_wide_w_mod3_1_loop     proc    near
        UNROLL_LOOP     DRAW_7_WIDE_W_MOD3_1,W7_MOD3_1,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_7_wide_w_mod3_1_loop

        ret

draw_7_wide_w_mod3_1_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw seven write-only bytes, then advance to next scan line.
; Optimized for start address MOD 3 == 2.

DRAW_7_WIDE_W_MOD3_2 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ax
        mov     [edi+2],eax
        mov     [edi+6],al
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 7-wide write-only, starting at MOD 3 == 2.

        align   4
draw_7_wide_w_mod3_2_loop     proc    near
        UNROLL_LOOP     DRAW_7_WIDE_W_MOD3_2,W7_MOD3_2,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_7_wide_w_mod3_2_loop

        ret

draw_7_wide_w_mod3_2_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw seven write-only bytes, then advance to next scan line.
; Optimized for start address MOD 3 == 3.

DRAW_7_WIDE_W_MOD3_3 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],al
        mov     [edi+1],eax
        mov     [edi+5],ax
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 7-wide write-only, starting at MOD 3 == 3.

        align   4
draw_7_wide_w_mod3_3_loop     proc    near
        UNROLL_LOOP     DRAW_7_WIDE_W_MOD3_3,W7_MOD3_3,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_7_wide_w_mod3_3_loop

        ret

draw_7_wide_w_mod3_3_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw eight write-only bytes, then advance to next scan line.
; Optimized for start address MOD 3 == 0.

DRAW_8_WIDE_W_MOD3_0 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],eax
        mov     [edi+4],eax
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 8-wide write-only, starting at MOD 3 == 0.

        align   4
draw_8_wide_w_mod3_0_loop     proc    near
        UNROLL_LOOP     DRAW_8_WIDE_W_MOD3_0,W8_MOD3_0,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_8_wide_w_mod3_0_loop

        ret

draw_8_wide_w_mod3_0_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw eight write-only bytes, then advance to next scan line.
; Optimized for start address MOD 3 == 1.

DRAW_8_WIDE_W_MOD3_1 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],al
        mov     [edi+1],ax
        mov     [edi+3],eax
        mov     [edi+7],al
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 8-wide write-only, starting at MOD 3 == 0.

        align   4
draw_8_wide_w_mod3_1_loop     proc    near
        UNROLL_LOOP     DRAW_8_WIDE_W_MOD3_1,W8_MOD3_1,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_8_wide_w_mod3_1_loop

        ret

draw_8_wide_w_mod3_1_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw eight write-only bytes, then advance to next scan line.
; Optimized for start address MOD 3 == 2.

DRAW_8_WIDE_W_MOD3_2 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],ax
        mov     [edi+2],eax
        mov     [edi+6],ax
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 8-wide write-only, starting at MOD 3 == 2.

        align   4
draw_8_wide_w_mod3_2_loop     proc    near
        UNROLL_LOOP     DRAW_8_WIDE_W_MOD3_2,W8_MOD3_2,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_8_wide_w_mod3_2_loop

        ret

draw_8_wide_w_mod3_2_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw eight write-only bytes, then advance to next scan line.
; Optimized for start address MOD 3 == 3.

DRAW_8_WIDE_W_MOD3_3 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     [edi],al
        mov     [edi+1],eax
        mov     [edi+5],ax
        mov     [edi+7],al
        add     edi,ecx                 ;point to the next scan line
        endm    ;-----------------------------------;

; 8-wide write-only, starting at MOD 3 == 3.

        align   4
draw_8_wide_w_mod3_3_loop     proc    near
        UNROLL_LOOP     DRAW_8_WIDE_W_MOD3_3,W8_MOD3_3,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_8_wide_w_mod3_3_loop

        ret

draw_8_wide_w_mod3_3_loop     endp

;-----------------------------------------------------------------------;
; Unrolled loop stuff for wide replace-type rops (arbitrary width).
;-----------------------------------------------------------------------;

; Tables of entry points into unrolled wide write-only loops.
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW00Entry,W_00,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW01Entry,W_01,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW02Entry,W_02,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW03Entry,W_03,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW10Entry,W_10,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW11Entry,W_11,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW12Entry,W_12,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW13Entry,W_13,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW20Entry,W_20,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW21Entry,W_21,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW22Entry,W_22,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW23Entry,W_23,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW30Entry,W_30,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW31Entry,W_31,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW32Entry,W_32,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnDrawWideW33Entry,W_33,LOOP_UNROLL_COUNT

;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 0 leading bytes, 0 trailing bytes,
; then advance to next scan line.

DRAW_WIDE_W_00 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill all whole bytes as dwords
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 0 leading bytes, 0 trailing bytes.
;  EAX = 0ffffh
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_00_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_00,W_00,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_00_loop

        ret

draw_wide_w_00_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 0 leading bytes, 1 trailing byte,
; then advance to next scan line.

DRAW_WIDE_W_01 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill whole bytes as dwords
        stosb                   ;fill the trailing byte
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 0 leading bytes, 1 trailing byte.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_01_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_01,W_01,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_01_loop

        ret

draw_wide_w_01_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 0 leading bytes, 2 trailing bytes,
; then advance to next scan line.

DRAW_WIDE_W_02 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill whole bytes as dwords
        stosw                   ;fill the trailing word
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 0 leading bytes, 2 trailing byte.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_02_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_02,W_02,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_02_loop

        ret

draw_wide_w_02_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 0 leading bytes, 3 trailing bytes,
; then advance to next scan line.

DRAW_WIDE_W_03 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill whole bytes as dwords
        stosw                   ;fill the trailing word
        stosb                   ;fill the trailing byte
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 0 leading bytes, 3 trailing bytes.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_03_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_03,W_03,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_03_loop

        ret

draw_wide_w_03_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 1 leading byte, 0 trailing bytes,
; then advance to next scan line.

DRAW_WIDE_W_10 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        stosb                   ;fill the leading byte
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill all whole bytes as dwords
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 1 leading byte, 0 trailing bytes.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_10_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_10,W_10,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_10_loop

        ret

draw_wide_w_10_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 1 leading byte, 1 trailing byte,
; then advance to next scan line.

DRAW_WIDE_W_11 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        stosb                   ;fill the leading byte
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill whole bytes as dwords
        stosb                   ;fill the trailing byte
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 1 leading bytes, 1 trailing byte.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_11_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_11,W_11,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_11_loop

        ret

draw_wide_w_11_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 1 leading byte, 2 trailing bytes,
; then advance to next scan line.

DRAW_WIDE_W_12 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        stosb                   ;fill the leading byte
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill whole bytes as dwords
        stosw                   ;fill the trailing word
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 1 leading bytes, 2 trailing byte.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_12_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_12,W_12,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_12_loop

        ret

draw_wide_w_12_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 1 leading byte, 3 trailing bytes,
; then advance to next scan line.

DRAW_WIDE_W_13 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        stosb                   ;fill the leading byte
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill whole bytes as dwords
        stosw                   ;fill the trailing word
        stosb                   ;fill the trailing byte
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 0 leading bytes, 3 trailing bytes.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_13_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_13,W_13,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_13_loop

        ret

draw_wide_w_13_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 2 leading bytes, 0 trailing bytes,
; then advance to next scan line.

DRAW_WIDE_W_20 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        stosw                   ;fill the leading word
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill all whole bytes as dwords
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 2 leading bytes, 0 trailing bytes.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_20_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_20,W_20,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_20_loop

        ret

draw_wide_w_20_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 2 leading bytes, 1 trailing byte,
; then advance to next scan line.

DRAW_WIDE_W_21 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        stosw                   ;fill the leading word
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill whole bytes as dwords
        stosb                   ;fill the trailing byte
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 2 leading bytess, 1 trailing byte.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_21_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_21,W_21,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_21_loop

        ret

draw_wide_w_21_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 2 leading bytes, 2 trailing bytes,
; then advance to next scan line.

DRAW_WIDE_W_22 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        stosw                   ;fill the leading word
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill whole bytes as dwords
        stosw                   ;fill the trailing word
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 2 leading bytess, 2 trailing byte.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_22_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_22,W_22,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_22_loop

        ret

draw_wide_w_22_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 2 leading bytes, 3 trailing bytes,
; then advance to next scan line.

DRAW_WIDE_W_23 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        stosw                   ;fill the leading word
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill whole bytes as dwords
        stosw                   ;fill the trailing word
        stosb                   ;fill the trailing byte
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 0 leading bytes, 3 trailing bytes.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_23_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_23,W_23,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_23_loop

        ret

draw_wide_w_23_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 3 leading bytes, 0 trailing bytes,
; then advance to next scan line.

DRAW_WIDE_W_30 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        stosb                   ;fill the leading byte
        stosw                   ;fill the leading word
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill all whole bytes as dwords
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 3 leading bytes, 0 trailing bytes.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_30_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_30,W_30,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_30_loop

        ret

draw_wide_w_30_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 3 leading bytes, 1 trailing byte,
; then advance to next scan line.

DRAW_WIDE_W_31 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        stosb                   ;fill the leading byte
        stosw                   ;fill the leading word
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill whole bytes as dwords
        stosb                   ;fill the trailing byte
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 3 leading bytess, 1 trailing byte.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_31_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_31,W_31,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_31_loop

        ret

draw_wide_w_31_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 3 leading bytes, 2 trailing bytes,
; then advance to next scan line.

DRAW_WIDE_W_32 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        stosb                   ;fill the leading byte
        stosw                   ;fill the leading word
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill whole bytes as dwords
        stosw                   ;fill the trailing word
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 3 leading bytess, 2 trailing byte.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_32_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_32,W_32,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_32_loop

        ret

draw_wide_w_32_loop     endp


;-----------------------------------------------------------------------;
; Macro to draw n write-only bytes, 3 leading bytes, 3 trailing bytes,
; then advance to next scan line.

DRAW_WIDE_W_33 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        stosb                   ;fill the leading byte
        stosw                   ;fill the leading word
        mov     ecx,esi         ;# of whole dwords
        rep     stosd           ;fill whole bytes as dwords
        stosw                   ;fill the trailing word
        stosb                   ;fill the trailing byte
        add     edi,edx         ;point to the next scan line
        endm    ;-----------------------------------;

; N-wide write-only, 0 leading bytes, 3 trailing bytes.
;  EAX = # of dwords to fill
;  EBX = count of scans to fill
;  EDX = offset from end of one scan's fill to start of next
;  ESI = # of dwords to fill
;  EDI = target address to fill

        align   4
draw_wide_w_33_loop     proc    near
        UNROLL_LOOP     DRAW_WIDE_W_33,W_33,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     draw_wide_w_33_loop

        ret

draw_wide_w_33_loop     endp


;--------------------------Private-Routine------------------------------;
; comp_byte_interval
;
;   A interval will be computed for byte boundaries.
;
;   A first mask and a last mask will be calculated, and possibly
;   combined into the inner loop count.  If no first byte exists,
;   the start address will be incremented to adjust for it.
;
; Entry:
;       EBX = right coordinate (exclusive)
;       EDX = left coordinate  (inclusive)
; Returns:
;       EDI = offset to first byte to be altered in the scan
;       ESI = inner loop count
;       AL  = first byte mask (possibly 0)
;       AH  = last  byte mask (possibly 0)
; Error Returns:
;       None
; Registers Preserved:
;       ES,BP
; Registers Destroyed:
;       AX,BX,CX,DX,SI,DI,FLAGS
; Calls:
;       None
; History:
;       Sat 11-Apr-1987 20:39:10 -by-  Walt Moore [waltm]
;       Created.
;-----------------------------------------------------------------------;

cProc   comp_byte_interval

        sub     ebx,edx                 ;Compute extent of interval
        dec     ebx                     ;Make interval inclusive
        mov     edi,edx                 ;Don't destroy starting X
        shr     edi,3                   ;/8 for byte address

        and     edx,00000111b           ;Compute bit index for left side
        add     ebx,edx                 ;Compute bit index for right side
        mov     esi,ebx                 ;(save for inner loop count)
        and     ebx,00000111b
        mov     cl,dl                   ;Compute left side altered bits mask
        mov     eax,0FFFFFFFFh
        mov     edx,eax                 ;Need this here later
        shr     al,cl                   ;AL = left side altered bytes mask
        mov     cl,bl                   ;Compute right side altered bits mask
        mov     ah,80h
        sar     ah,cl                   ;AH = right side altered bits mask
        shr     esi,3                   ;Compute inner byte count
        jnz     short comp_byte_dont_combine ;loop count + 1 > 0, check it out

; Only one byte will be affected.  Combine first/last masks, set loop count = 0

        and     al,ah                   ;Will use first byte mask only
        xor     ah,ah                   ;Want last byte mask to be 0
        inc     esi                     ;Fall through to set 0

comp_byte_dont_combine:
        dec     esi                     ;Dec inner loop count (might become 0)


; If all pixels in the first byte are altered, combine the first byte into the
; inner loop and clear the first byte mask.  Ditto for the last byte mask.

        cmp     al,dl                   ;Set 'C' if not all pixels 1
        sbb     esi,edx                 ;If no 'C', sub -1 (add 1), else sub 0
        cmp     al,dl                   ;Set 'C' if not all pixels 1
        sbb     al,dl                   ;If no 'C', sub -1 (add 1), else sub 0

        cmp     ah,dl                   ;Set 'C' if not all pixels 1
        sbb     esi,edx                 ;If no 'C', sub -1 (add 1), else sub 0
        cmp     ah,dl                   ;Set 'C' if not all pixels 1
        sbb     ah,dl                   ;If no 'C', sub -1 (add 1), else sub 0
        cRet    comp_byte_interval

endProc comp_byte_interval

_TEXT$01   ends

        end


