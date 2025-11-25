;---------------------------Module-Header------------------------------;
; Module Name: fasttext.asm
;
; Copyright (c) 1992 Microsoft Corporation
;-----------------------------------------------------------------------;
;-----------------------------------------------------------------------;
; VOID vFastText(GLYPHPOS * pGlyphPos, ULONG ulGlyphCount, PBYTE pTempBuffer,
;                ULONG ulBufDelta, ULONG ulCharInc, DEVSURF * pdsurf,
;                RECTL * prclText, RECTL * prclOpaque, INT iFgColor,
;                INT iBgColor, ULONG fDrawFlags, RECTL * prclClip);
; pGlyphPos -
; ulGlyphCount - # of glyphs to draw. Must never be 0.
; pTempBuffer -
; ulBufDelta -
; ulCharInc -
; pdsurf -
; prclText -
; prclOpaque -
; iFgColor -
; iBgColor -
; fDrawFlags -
; prclClip -
;
; Performs accelerated proportional text drawing.
;
;-----------------------------------------------------------------------;
;
; Note: Assumes the text rectangle has a positive height and width. Will
; not work properly if this is not the case.
;
; Note: The opaquing rectangle is assumed to match the text bounding
; rectangle exactly; prclOpaque is used only to determine whether or
; not opaquing is required.
;
; Note: Handles clipping only for opaque text.
;
; Note: For opaque text, handles only unclipped and rectangle-clipped
; cases, not complex-clipped.
;
; Note: For maximum performance, we should not bother to draw fully-
; clipped characters to the temp buffer.
;
; Note: We do not yet support extra rectangles (underlines). We should.
;
; Note: We do not handle clipping or bank spanning in the very fast
; byte-wide-aligned-fixed-pitch console text. This would be an
; opportunity for somewhat faster console text performance.
;
;-----------------------------------------------------------------------;
;
; Set LOOP_UNROLL_SHIFT to the log2 of the number of times you want to unroll
; loops in this module that are implemented with the unrolling macros. For
; example, LOOP_UNROLL_SHIFT of 3 yields 2**3 = 8 times unrolling. This is
; the only thing you need to change to control unrolling.

LOOP_UNROLL_SHIFT equ 2

;-----------------------------------------------------------------------;

        comment $

The overall approach of this module is to draw the text into a system
memory buffer, then copy the buffer to the screen a word at a time
using write mode 3 so that no OUTs and a minimum of display memory reads
are required.

        commend $

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

        .list

;-----------------------------------------------------------------------;

        .data

;-----------------------------------------------------------------------;
; Tables used to branch into glyph-drawing optimizations.
;
; Handles narrow (1-4 bytes wide) glyph drawing, for case where initial byte
; should be MOVed even if it's not aligned (intended for use in drawing the
; first glyph in a string). Table format is:
;  Bits 3-2: dest width
;  Bit   1 : 1 if don't need last source byte, 0 if do need last source byte
;  Bit   0 : 1 if no rotation (aligned), 0 if rotation (non-aligned)
        align   4
MovInitialTableNarrow   label   dword
        dd      draw_prop_done                     ;0 wide
        dd      draw_prop_done                     ;0 wide
        dd      draw_prop_done                     ;0 wide
        dd      draw_prop_done                     ;0 wide
        dd      mov_first_1_wide_rotated_need_last ;nonalign, 1 wide, need last
        dd      mov_first_1_wide_unrotated         ;aligned, 1 wide
        dd      mov_first_1_wide_rotated_no_last   ;nonalign, 1 wide, no last
        dd      mov_first_1_wide_unrotated         ;aligned, 1 wide
        dd      mov_first_2_wide_rotated_need_last ;nonalign, 2 wide, need last
        dd      mov_first_2_wide_unrotated         ;aligned, 2 wide
        dd      mov_first_2_wide_rotated_no_last   ;nonalign, 2 wide, no last
        dd      mov_first_2_wide_unrotated         ;aligned, 2 wide
        dd      mov_first_3_wide_rotated_need_last ;nonalign, 3 wide, need last
        dd      mov_first_3_wide_unrotated         ;aligned, 3 wide
        dd      mov_first_3_wide_rotated_no_last   ;nonalign, 3 wide, no last
        dd      mov_first_3_wide_unrotated         ;aligned, 3 wide
        dd      mov_first_4_wide_rotated_need_last ;nonalign, 4 wide, need last
        dd      mov_first_4_wide_unrotated         ;aligned, 4 wide
        dd      mov_first_4_wide_rotated_no_last   ;nonalign, 4 wide, no last
        dd      mov_first_4_wide_unrotated         ;aligned, 4 wide

; Handles narrow (1-4 bytes wide) glyph drawing, for case where initial byte
; ORed if it's not aligned (intended for use in drawing all but the first glyph
; in a string). Table format is:
;  Bits 3-2: dest width
;  Bit   1 : 1 if don't need last source byte, 0 if do need last source byte
;  Bit   0 : 1 if no rotation (aligned), 0 if rotation (non-aligned)
        align   4
OrInitialTableNarrow    label   dword
        dd      draw_prop_done                     ;0 wide
        dd      draw_prop_done                     ;0 wide
        dd      draw_prop_done                     ;0 wide
        dd      draw_prop_done                     ;0 wide
        dd      or_first_1_wide_rotated_need_last  ;nonalign, 1 wide, need last
        dd      mov_first_1_wide_unrotated         ;aligned, 1 wide
        dd      or_first_1_wide_rotated_no_last    ;nonalign, 1 wide, no last
        dd      mov_first_1_wide_unrotated         ;aligned, 1 wide
        dd      or_first_2_wide_rotated_need_last  ;nonalign, 2 wide, need last
        dd      mov_first_2_wide_unrotated         ;aligned, 2 wide
        dd      or_first_2_wide_rotated_no_last    ;nonalign, 2 wide, no last
        dd      mov_first_2_wide_unrotated         ;aligned, 2 wide
        dd      or_first_3_wide_rotated_need_last  ;nonalign, 3 wide, need last
        dd      mov_first_3_wide_unrotated         ;aligned, 3 wide
        dd      or_first_3_wide_rotated_no_last    ;nonalign, 3 wide, no last
        dd      mov_first_3_wide_unrotated         ;aligned, 3 wide
        dd      or_first_4_wide_rotated_need_last  ;nonalign, 4 wide, need last
        dd      mov_first_4_wide_unrotated         ;aligned, 4 wide
        dd      or_first_4_wide_rotated_no_last    ;nonalign, 4 wide, no last
        dd      mov_first_4_wide_unrotated         ;aligned, 4 wide

; Handles narrow (1-4 bytes wide) glyph drawing, for case where all bytes
; should be ORed (intended for use in drawing potentially overlapping glyphs).
; Table format is:
;  Bits 3-2: dest width
;  Bit   1 : 1 if don't need last source byte, 0 if do need last source byte
;  Bit   0 : 1 if no rotation (aligned), 0 if rotation (non-aligned)
        align   4
OrAllTableNarrow        label   dword
        dd      draw_prop_done                     ;0 wide
        dd      draw_prop_done                     ;0 wide
        dd      draw_prop_done                     ;0 wide
        dd      draw_prop_done                     ;0 wide
        dd      or_all_1_wide_rotated_need_last    ;nonalign, 1 wide, need last
        dd      or_all_1_wide_unrotated            ;aligned, 1 wide
        dd      or_all_1_wide_rotated_no_last      ;nonalign, 1 wide, no last
        dd      or_all_1_wide_unrotated            ;aligned, 1 wide
        dd      or_all_2_wide_rotated_need_last    ;nonalign, 2 wide, need last
        dd      or_all_2_wide_unrotated            ;aligned, 2 wide
        dd      or_all_2_wide_rotated_no_last      ;nonalign, 2 wide, no last
        dd      or_all_2_wide_unrotated            ;aligned, 2 wide
        dd      or_all_3_wide_rotated_need_last    ;nonalign, 3 wide, need last
        dd      or_all_3_wide_unrotated            ;aligned, 3 wide
        dd      or_all_3_wide_rotated_no_last      ;nonalign, 3 wide, no last
        dd      or_all_3_wide_unrotated            ;aligned, 3 wide
        dd      or_all_4_wide_rotated_need_last    ;nonalign, 4 wide, need last
        dd      or_all_4_wide_unrotated            ;aligned, 4 wide
        dd      or_all_4_wide_rotated_no_last      ;nonalign, 4 wide, no last
        dd      or_all_4_wide_unrotated            ;aligned, 4 wide

; Handles arbitrarily wide glyph drawing, for case where initial byte should be
; MOVed even if it's not aligned (intended for use in drawing the first glyph
; in a string). Table format is:
;  Bit   1 : 1 if don't need last source byte, 0 if do need last source byte
;  Bit   0 : 1 if no rotation (aligned), 0 if rotation (non-aligned)
        align   4
MovInitialTableWide     label   dword
        dd      mov_first_N_wide_rotated_need_last      ;nonalign, need last
        dd      mov_first_N_wide_unrotated              ;aligned
        dd      mov_first_N_wide_rotated_no_last        ;nonalign, no last
        dd      mov_first_N_wide_unrotated              ;aligned

; Handles arbitrarily wide glyph drawing, for case where initial byte should be
; ORed if it's not aligned (intended for use in drawing all but the first glyph
; in a string). Table format is:
;  Bit   1 : 1 if don't need last source byte, 0 if do need last source byte
;  Bit   0 : 1 if no rotation (aligned), 0 if rotation (non-aligned)
        align   4
OrInitialTableWide      label   dword
        dd      or_first_N_wide_rotated_need_last       ;nonalign, need last
        dd      mov_first_N_wide_unrotated              ;aligned
        dd      or_first_N_wide_rotated_no_last         ;nonalign, no last
        dd      mov_first_N_wide_unrotated              ;aligned

; Handles arbitrarily wide glyph drawing, for case where all bytes should
; be ORed (intended for use in drawing potentially overlapping glyphs).
; Table format is:
;  Bit   1 : 1 if don't need last source byte, 0 if do need last source byte
;  Bit   0 : 1 if no rotation (aligned), 0 if rotation (non-aligned)
        align   4
OrAllTableWide  label   dword
        dd      or_all_N_wide_rotated_need_last ;nonalign, need last
        dd      or_all_N_wide_unrotated         ;aligned
        dd      or_all_N_wide_rotated_no_last   ;nonalign, no last
        dd      or_all_N_wide_unrotated         ;aligned

; Vectors to entry points for drawing various types of text. '*' means works as
; is but could be acclerated with a custom scanning loop.
        align   4
MasterTextTypeTable     label   dword       ;tops aligned  overlap  fixed pitch
        dd      draw_nf_ntb_o_to_temp_start ;      N          N          N *
        dd      draw_f_ntb_o_to_temp_start  ;      N          N          Y *
        dd      draw_nf_ntb_o_to_temp_start ;      N          Y          N
        dd      draw_f_ntb_o_to_temp_start  ;      N          Y          Y
        dd      draw_nf_tb_no_to_temp_start ;      Y          N          N
        dd      draw_f_tb_no_to_temp_start  ;      Y          N          Y
        dd      draw_nf_ntb_o_to_temp_start ;      Y          Y          N *
        dd      draw_f_ntb_o_to_temp_start  ;      Y          Y          Y *

; Masks for clipping for the eight possible left and right edge alignments
jOpaqueLeftMasks        label   byte
        db      0ffh,07fh,03fh,01fh,00fh,007h,003h,001h

jOpaqueRightMasks       label   byte
        db      0ffh,080h,0c0h,0e0h,0f0h,0f8h,0fch,0feh

;-----------------------------------------------------------------------;

                .code

_TEXT$01   SEGMENT DWORD USE32 PUBLIC 'CODE'
           ASSUME  CS:FLAT, DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;-----------------------------------------------------------------------;

cProc vFastText,48,<\
 uses esi edi ebx,\
 pGlyphPos:ptr,\
 ulGlyphCount:dword,\
 pTempBuffer:ptr,\
 ulBufDelta:dword,\
 ulCharInc:dword,\
 pdsurf:ptr,\
 prclText:ptr,\
 prclOpaque:ptr,\
 iFgColor:dword,\
 iBgColor:dword,\
 fDrawFlags:dword,\
 prclClip:dword>

        local ulGlyDelta:dword  ;width per scan of source glyph, in bytes
        local ulWidthInBytes:dword ;width of glyph, in bytes
        local ulTmpWidthInBytes:dword ;working byte-width count
        local ulGlyphX:dword    ;for fixed-pitch text, maintains the current
                                ; glyph's left-edge X coordinate
        local pGlyphLoop:dword  ;pointer to glyph-processing loop
        local ulTempLeft:dword  ;X coordinate on screen of left edge of temp
                                ; buffer
        local ulTempTop:dword   ;Y coordinate on screen of top edge of temp
                                ; buffer
        local pfnEntry:dword    ;pointer to unrolled loop entry point
        local ulLoopCount:dword ;general loop count storage
        local ulTmpSrcDelta:dword ;distance from end of one buffer text scan to
                                  ; start of next
        local ulTmpDstDelta:dword ;distance from end of one screen text scan to
                                  ; start of next
        local ulTopScan:dword     ;top scan of dest text rect in current bank
        local ulBottomScan:dword  ;bottom scan of dest text rect
        local ulNumScans:dword    ;# of scans to draw
        local ulScreenDelta:dword ;scan-to-scan offset in screen
        local ulTextWidthInBytes:dword ;# of bytes across spanned by text
        local pScreen:dword     ;pointer to first screen byte to which to draw
        local pfnEdgeVector:dword ;pointer to routine to draw any needed edges
        local pfnFirstOpaqVector:dword ;pointer to initial drawing routine
                                       ; called for opaque (either whole
                                       ; bytes, or edge(s) if no whole bytes)
        local ulWholeWidthInWords:dword ;# of whole words to copy
        local ulWholeWidthInWordsMinus1:dword ;# of whole words to copy, -1
        local ulOddByte:dword   ;1 if odd byte in whole word copy
        local ulTextLeft:dword  ;left edge of leftmost glyph
        local ulLeftMask:dword  ;for opaque text, left edge mask for string
        local ulRightMask:dword ;for opaque text, right edge mask for string
        local ulUnrolledCount:dword    ;# of unrolled loop reps
        local ulUnrolledOddCount:dword ;# of unrolled loop odd reps
        local ulYOrigin:dword   ;Y origin of text in string (all glyphs are at
                                ; the same Y origin)
        local rclClippedBounds[16]:byte ;clipped destination rectangle;
                                        ; defined as "byte" due to assembler
                                        ; limitations

;-----------------------------------------------------------------------;

        cld

;-----------------------------------------------------------------------;
; Draws either a fixed or a non-fixed-pitch string to the temporary
; buffer. Assumes this is a horizontal string, so the origins of all glyphs
; are at the same Y coordinate. Draws leftmost glyph entirely with MOVs,
; even if it's not aligned, in order to ensure that the leftmost byte
; gets cleared when we're working with butted characters. For other
; non-aligned glyphs, leftmost byte is ORed, other bytes are MOVed.
;
; Input:
;       pGlyphPos = pointer to array of GLYPHPOS structures to draw
;       ulGlyphCount = # of glyphs to draw
;       ulTempLeft = X coordinate on dest of left edge of temp buffer pointed
;               to by pTempBuffer
;       pTempBuffer = pointer to first byte (upper left corner) of
;               temp buffer into which we're drawing. This should be
;               word-aligned with the destination
;       ulBufDelta = destination scan-to-scan offset
;       ulCharInc = offset from one glyph to next (fixed-pitch only)
;       fDrawFlags = indicate the type of text to be drawn
;       Temp buffer zeroed if text doesn't cover every single pixel
;
; Fixed-pitch means equal spacing between glyph positions, not that all
; glyphs butt together or equal spacing between upper left corners.
;-----------------------------------------------------------------------;

;-----------------------------------------------------------------------;
; If 8 wide, byte aligned, and opaque, handle with very fast special-case
; code.
;-----------------------------------------------------------------------;

        mov     ebx,prclText
        cmp     ulCharInc,8                     ;8 wide?
        jnz     short @F                        ;no
        cmp     fDrawFlags,5                    ;fixed pitch?
        jnz     short @F                        ;no
        cmp     prclOpaque,0                    ;opaque?
        jz      short @F                        ;no
        cmp     prclClip,0                      ;is there clipping?
        jnz     short @F                        ;yes
        test    [ebx].xLeft,111b                ;byte aligned?
        jz      special_8_wide_aligned_opaque   ;yes, special-case
@@:

;-----------------------------------------------------------------------;
; Handle all cases other than 8-wide byte-aligned.
;-----------------------------------------------------------------------;

general_handler:
        mov     esi,pdsurf
        mov     eax,[ebx].yTop
        mov     ulTempTop,eax   ;Y screen coordinate of top edge of temp buf
        mov     eax,[ebx].xLeft
        and     eax,not 7
        mov     ulTempLeft,eax  ;X screen coordinate of left edge of temp buf
        mov     eax,[esi].dsurf_lNextScan
        mov     ulScreenDelta,eax

        mov     eax,fDrawFlags

        jmp     MasterTextTypeTable[eax*4]

;-----------------------------------------------------------------------;
; Entry point for fixed-pitch | tops and bottoms aligned | no overlap.
; Sets up to draw first glyph.
;-----------------------------------------------------------------------;
draw_f_tb_no_to_temp_start:
        mov     ebx,pGlyphPos           ;point to the first glyph to draw
        mov     esi,[ebx].gp_pgdf       ;point to glyph def

        mov     edi,[ebx].gp_x          ;dest X coordinate
        sub     edi,ulTempLeft          ;adjust relative to the left of the
                                        ; temp buffer (we assume the text is
                                        ; right at the top of the text rect
                                        ; and hence the buffer)
        mov     ulGlyphX,edi            ;remember where this glyph started
        mov     esi,[esi].gdf_pgb       ;point to glyph bits
        mov     pGlyphLoop,offset draw_f_tb_no_to_temp_loop
                                        ;draw additional characters with this
                                        ; loop
        jmp     short draw_to_temp_start_entry

;-----------------------------------------------------------------------;
; Entry point for non-fixed-pitch | tops and bottoms aligned | no overlap.
; Sets up to draw first glyph.
;-----------------------------------------------------------------------;
draw_nf_tb_no_to_temp_start:
        mov     ebx,pGlyphPos           ;point to the first glyph to draw
        mov     esi,[ebx].gp_pgdf       ;point to glyph def

        mov     edi,[ebx].gp_x          ;dest X coordinate
        sub     edi,ulTempLeft          ;adjust relative to the left of the
                                        ; temp buffer
        mov     esi,[esi].gdf_pgb       ;point to glyph bits
        mov     pGlyphLoop,offset draw_nf_tb_no_to_temp_loop
                                        ;draw additional characters with this
                                        ; loop
draw_to_temp_start_entry:
        add     edi,[esi].gb_x          ;adjust to position of upper left glyph
                                        ; corner in dest
                                        ;BUGBUG add or sub?
        mov     ecx,edi
        shr     edi,3                   ;byte offset of first column of glyph
                                        ; offset of upper left of glyph in temp
                                        ; buffer
        add     edi,pTempBuffer         ;initial dest byte in temp buffer

        and     ecx,111b                ;bit alignment of upper left in temp

                                        ;calculate scan-to-scan glyph width
        mov     ebx,[esi].gb_cx         ;glyph width in pixels

        lea     eax,[ebx+ecx+7]
        shr     eax,3                   ;# of dest bytes per scan

        add     ebx,7
        shr     ebx,3                   ;# of source bytes per scan

        mov     edx,ulBufDelta          ;width of destination buffer in bytes

        cmp     eax,4                   ;do we have special case code for this
                                        ; dest width?
        ja      short @F                ;no, handle as general case
                                        ;yes, handle as special case
        cmp     ebx,eax                 ;carry if more dest than source bytes
                                        ; (last source byte not needed)
        rcl     eax,1                   ;factor last source byte status in
        cmp     cl,1                    ;carry if aligned
        rcl     eax,1                   ;factor in alignment (aligned or not)
        mov     ebx,[esi].gb_cy         ;# of scans in glyph
        add     esi,gb_aj               ;point to the first glyph byte

        jmp     MovInitialTableNarrow[eax*4]
                                        ;branch to draw the first glyph; never
                                        ; need to OR first glyph, because
                                        ; there's nothing there yet

@@:                                     ;too wide to special case
        mov     ulWidthInBytes,eax      ;# of bytes across dest
        cmp     ebx,eax                 ;carry if more dest than source bytes
                                        ; (last source byte not needed)
        mov     eax,0
        rcl     eax,1                   ;factor last source byte status in
        cmp     cl,1                    ;carry if aligned
        rcl     eax,1                   ;factor in alignment (aligned or not)

        mov     ebx,[esi].gb_cx         ;glyph width in pixels
        add     ebx,7
        shr     ebx,3                   ;glyph width in bytes
        mov     ulGlyDelta,ebx

        mov     ebx,[esi].gb_cy         ;# of scans in glyph
        add     esi,gb_aj               ;point to the first glyph byte

        jmp     MovInitialTableWide[eax*4]
                                        ;branch to draw the first glyph; never
                                        ; need to OR first glyph, because
                                        ; there's nothing there yet

;-----------------------------------------------------------------------;
; Entry point for fixed-pitch | tops and bottoms not aligned | overlap.
; Sets up to draw first glyph.
;-----------------------------------------------------------------------;
draw_f_ntb_o_to_temp_start:
        mov     ebx,pGlyphPos           ;point to the first glyph to draw
        mov     pGlyphLoop,offset draw_f_ntb_o_to_temp_loop
                                        ;draw additional characters with this
                                        ; loop
        mov     edi,[ebx].gp_x          ;dest X coordinate
        mov     esi,[ebx].gp_pgdf       ;point to glyph def
        sub     edi,ulTempLeft          ;adjust relative to the left of the
                                        ; temp buffer
        mov     ulGlyphX,edi            ;remember where this glyph started
        mov     esi,[esi].gdf_pgb       ;point to glyph bits
        add     edi,[esi].gb_x          ;adjust to position of upper left glyph
                                        ; corner in dest
        mov     ecx,edi
        shr     edi,3                   ;byte offset of first column of glyph
                                        ; offset of upper left of glyph in temp
                                        ; buffer
        jmp     short draw_to_temp_start_entry2

;-----------------------------------------------------------------------;
; Entry point for non-fixed-pitch | tops and bottoms not aligned | overlap.
; Sets up to draw first glyph.
;-----------------------------------------------------------------------;
draw_nf_ntb_o_to_temp_start:
        mov     ebx,pGlyphPos           ;point to the first glyph to draw
        mov     pGlyphLoop,offset draw_nf_ntb_o_to_temp_loop
                                        ;draw additional characters with this
                                        ; loop
        mov     edi,[ebx].gp_x          ;dest X coordinate
        mov     esi,[ebx].gp_pgdf       ;point to glyph def
        sub     edi,ulTempLeft          ;adjust relative to the left of the
                                        ; temp buffer
        mov     esi,[esi].gdf_pgb       ;point to glyph bits
        add     edi,[esi].gb_x          ;adjust to position of upper left glyph
                                        ; corner in dest
                                        ;BUGBUG add or sub?
        mov     ecx,edi
        shr     edi,3                   ;byte offset of first column of glyph
                                        ; offset of upper left of glyph in temp
                                        ; buffer
draw_to_temp_start_entry2:
        mov     eax,[ebx].gp_y          ;dest origin Y coordinate
        sub     eax,ulTempTop           ;coord of glyph origin in temp buffer
        mov     ulYOrigin,eax           ;remember the Y origin of all glyphs
                                        ; (necessary because glyph positions
                                        ; after first aren't set for fixed-
                                        ; pitch strings)
        add     eax,[esi].gb_y          ;adjust to position of upper left glyph
                                        ; corner in dest
        mul     ulBufDelta              ;offset in buffer of top glyph scan
        add     eax,pTempBuffer         ;initial dest byte
        add     edi,eax

        and     ecx,111b                ;bit alignment of upper left in temp

                                        ;calculate scan-to-scan glyph width
        mov     ebx,[esi].gb_cx         ;glyph width in pixels

        lea     eax,[ebx+ecx+7]
        shr     eax,3                   ;# of dest bytes per scan

        add     ebx,7
        shr     ebx,3                   ;# of source bytes per scan

        mov     edx,ulBufDelta          ;width of destination buffer in bytes

        cmp     eax,4                   ;do we have special case code for this
                                        ; dest width?
        ja      short @F                ;no, handle as general case
                                        ;yes, handle as special case
        cmp     ebx,eax                 ;carry if more dest than source bytes
                                        ; (last source byte not needed)
        rcl     eax,1                   ;factor last source byte status in
        cmp     cl,1                    ;carry if aligned
        rcl     eax,1                   ;factor in alignment (aligned or not)
        mov     ebx,[esi].gb_cy         ;# of scans in glyph
        add     esi,gb_aj               ;point to the first glyph byte

        jmp     OrAllTableNarrow[eax*4] ;branch to draw the first glyph; OR all
                                        ; glyphs, because text may overlap

@@:                                     ;too wide to special case
        mov     ulWidthInBytes,eax      ;# of bytes across dest
        cmp     ebx,eax                 ;carry if more dest than source bytes
                                        ; (last source byte not needed)
        mov     eax,0
        rcl     eax,1                   ;factor last source byte status in
        cmp     cl,1                    ;carry if aligned
        rcl     eax,1                   ;factor in alignment (aligned or not)

        mov     ebx,[esi].gb_cx         ;glyph width in pixels
        add     ebx,7
        shr     ebx,3                   ;glyph width in bytes
        mov     ulGlyDelta,ebx

        mov     ebx,[esi].gb_cy         ;# of scans in glyph
        add     esi,gb_aj               ;point to the first glyph byte

        jmp     OrAllTableWide[eax*4]   ;branch to draw the first glyph; OR all                                 ; glyphs, because text may overlap never
                                        ; glyphs, because text may overlap

;-----------------------------------------------------------------------;
; Loop to draw all fixed-pitch | tops and bottoms aligned | no overlap
; glyphs after first.
;-----------------------------------------------------------------------;
draw_f_tb_no_to_temp_loop:
        dec     ulGlyphCount            ;any more glyphs to draw?
        jz      draw_to_screen          ;no, done
        mov     ebx,pGlyphPos
        add     ebx,size GLYPHPOS       ;point to the next glyph (the one
        mov     pGlyphPos,ebx           ; we're going to draw this time)
        mov     esi,[ebx].gp_pgdf       ;point to glyph def

        mov     edi,ulGlyphX            ;last glyph's dest X start in temp buf
        add     edi,ulCharInc           ;this glyph's dest X start in temp buf
        mov     ulGlyphX,edi            ;remember for next glyph
        mov     esi,[esi].gdf_pgb       ;point to glyph bits
        jmp     short draw_to_temp_loop_entry

;-----------------------------------------------------------------------;
; Loop to draw all non-fixed-pitch | tops and bottoms aligned | no overlap
; glyphs after first.
;-----------------------------------------------------------------------;
draw_nf_tb_no_to_temp_loop:
        dec     ulGlyphCount            ;any more glyphs to draw?
        jz      draw_to_screen          ;no, done
        mov     ebx,pGlyphPos
        add     ebx,size GLYPHPOS       ;point to the next glyph (the one we're
        mov     pGlyphPos,ebx           ; going to draw this time)
        mov     esi,[ebx].gp_pgdf       ;point to glyph def
        mov     edi,[ebx].gp_x          ;dest X coordinate
        mov     esi,[esi].gdf_pgb       ;point to glyph bits
        sub     edi,ulTempLeft          ;adjust relative to the left edge of
                                        ; the temp buffer

draw_to_temp_loop_entry:
        add     edi,[esi].gb_x          ;adjust to position of upper left glyph
                                        ; corner in dest
        mov     ecx,edi                 ;pixel X coordinate in temp buffer
        shr     edi,3                   ;byte offset of first column = dest
                                        ; offset of upper left of glyph in temp
                                        ; buffer
        add     edi,pTempBuffer         ;initial dest byte

        and     ecx,111b                ;bit alignment of upper left in temp

                                        ;calculate scan-to-scan glyph width
        mov     ebx,[esi].gb_cx         ;glyph width in pixels

        lea     eax,[ebx+ecx+7]
        shr     eax,3                   ;# of dest bytes to copy to per scan

        add     ebx,7
        shr     ebx,3                   ;# of source bytes to copy from per
                                        ; scan
        mov     edx,ulBufDelta          ;width of destination buffer in bytes

        cmp     eax,4                   ;do we have special case code for this
                                        ; dest width?
        ja      short @F                ;no, handle as general case
                                        ;yes, handle as special case
        cmp     ebx,eax                 ;carry if more dest than source bytes
                                        ; (last source byte not needed)
        rcl     eax,1                   ;factor last source byte status in
        cmp     cl,1                    ;carry if aligned
        rcl     eax,1                   ;factor in alignment (aligned or not)
        mov     ebx,[esi].gb_cy         ;# of scans in glyph
        add     esi,gb_aj               ;point to the first glyph byte

        jmp     OrInitialTableNarrow[eax*4] ;branch to draw the first glyph;
                                            ; need to OR the 1st byte if
                                            ; non-aligned to avoid overwriting
                                            ; what's already there
@@:                                     ;too wide to special case
        mov     ulWidthInBytes,eax      ;# of bytes across dest
        cmp     ebx,eax                 ;carry if more dest than source bytes
                                        ; (last source byte not needed)
        mov     eax,0
        rcl     eax,1                   ;factor last source byte status in
        cmp     cl,1                    ;carry if aligned
        rcl     eax,1                   ;factor in alignment (aligned or not)

        mov     ebx,[esi].gb_cx         ;glyph width in pixels
        add     ebx,7
        shr     ebx,3                   ;glyph width in bytes
        mov     ulGlyDelta,ebx

        mov     ebx,[esi].gb_cy         ;# of scans in glyph
        add     esi,gb_aj               ;point to the first glyph byte

        jmp     OrInitialTableWide[eax*4] ;branch to draw the next glyph;
                                          ; need to OR the 1st byte if
                                          ; non-aligned to avoid overwriting
                                          ; what's already there

;-----------------------------------------------------------------------;
; Loop to draw all fixed-pitch | tops and bottoms not aligned | overlap
; glyphs after first.
;-----------------------------------------------------------------------;
draw_f_ntb_o_to_temp_loop:
        dec     ulGlyphCount            ;any more glyphs to draw?
        jz      draw_to_screen          ;no, done
        mov     ebx,pGlyphPos
        add     ebx,size GLYPHPOS       ;point to the next glyph (the one we're
        mov     pGlyphPos,ebx           ; going to draw this time)

        mov     esi,[ebx].gp_pgdf       ;point to glyph def
        mov     edi,ulGlyphX            ;last glyph's dest X start in temp buf
        add     edi,ulCharInc           ;this glyph's dest X start in temp buf
        mov     ulGlyphX,edi            ;remember for next glyph
        mov     esi,[esi].gdf_pgb       ;point to glyph bits

        jmp     short draw_to_temp_loop_entry2

;-----------------------------------------------------------------------;
; Loop to draw all non-fixed-pitch | tops and bottoms not aligned | overlap
; glyphs after first.
;-----------------------------------------------------------------------;
draw_nf_ntb_o_to_temp_loop:
        dec     ulGlyphCount            ;any more glyphs to draw?
        jz      draw_to_screen          ;no, done
        mov     ebx,pGlyphPos
        add     ebx,size GLYPHPOS       ;point to the next glyph (the one we're
        mov     pGlyphPos,ebx           ; going to draw this time)

        mov     esi,[ebx].gp_pgdf       ;point to glyph def
        mov     edi,[ebx].gp_x          ;dest X coordinate
        mov     esi,[esi].gdf_pgb       ;point to glyph bits
        sub     edi,ulTempLeft          ;adjust relative to the left edge of
                                        ; the temp buffer
draw_to_temp_loop_entry2:
        add     edi,[esi].gb_x          ;adjust to position of upper left glyph
                                        ; corner in dest
        mov     ecx,edi                 ;pixel X coordinate in temp buffer
        shr     edi,3                   ;byte offset of first column = dest
                                        ; offset of upper left of glyph in temp
                                        ; buffer
        mov     eax,ulYOrigin           ;dest Y coordinate

        add     eax,[esi].gb_y          ;adjust to position of upper left glyph
                                        ; corner in dest
        mul     ulBufDelta              ;offset in buffer of top glyph scan
        add     eax,pTempBuffer         ;initial dest byte
        add     edi,eax

        and     ecx,111b                ;bit alignment of upper left in temp

                                        ;calculate scan-to-scan glyph width
        mov     ebx,[esi].gb_cx         ;glyph width in pixels

        lea     eax,[ebx+ecx+7]
        shr     eax,3                   ;# of dest bytes to copy to per scan

        add     ebx,7
        shr     ebx,3                   ;# of source bytes to copy from per
                                        ; scan
        mov     edx,ulBufDelta          ;width of destination buffer in bytes

        cmp     eax,4                   ;do we have special case code for this
                                        ; dest width?
        ja      short @F                ;no, handle as general case
                                        ;yes, handle as special case
        cmp     ebx,eax                 ;carry if more dest than source bytes
                                        ; (last source byte not needed)
        rcl     eax,1                   ;factor last source byte status in
        cmp     cl,1                    ;carry if aligned
        rcl     eax,1                   ;factor in alignment (aligned or not)
        mov     ebx,[esi].gb_cy         ;# of scans in glyph
        add     esi,gb_aj               ;point to the first glyph byte

        jmp     OrAllTableNarrow[eax*4] ;branch to draw the next glyph

@@:                                     ;too wide to special case
        mov     ulWidthInBytes,eax      ;# of bytes across dest
        cmp     ebx,eax                 ;carry if more dest than source bytes
                                        ; (last source byte not needed)
        mov     eax,0
        rcl     eax,1                   ;factor last source byte status in
        cmp     cl,1                    ;carry if aligned
        rcl     eax,1                   ;factor in alignment (aligned or not)

        mov     ebx,[esi].gb_cx         ;glyph width in pixels
        add     ebx,7
        shr     ebx,3                   ;glyph width in bytes
        mov     ulGlyDelta,ebx

        mov     ebx,[esi].gb_cy         ;# of scans in glyph
        add     esi,gb_aj               ;point to the first glyph byte

        jmp     OrAllTableWide[eax*4]   ;branch to draw the next glyph

;-----------------------------------------------------------------------;
; Routines to draw all scans of a single glyph into the temp buffer,
; optimized for the following cases:
;
;       1 to 4 byte-wide destination rectangles for each of:
;               No rotation needed
;               Rotation needed, same # of source as dest bytes needed
;               Rotation needed, one less source than dest bytes needed
;
; Additionally, the three cases are handled for 5 and wider cases by a
; general routine for each case.
;
; If rotation is needed, there are three sorts of routines:
;
; 1) The leftmost byte is MOVed, to initialize the byte. Succeeding bytes are
;    MOVed. This is generally used for the leftmost glyph of a string.
; 2) The leftmost byte is ORed into the existing byte. Succeeding bytes are
;    MOVed. This is generally used after the leftmost glyph, because this may
;    not be the first data written to that byte.
; 3) All bytes are ORed. This is for drawing when characters might overlap.
;
; If rotation is not needed, there are two sorts of routines:
;
; 1) The leftmost byte is MOVed, to initialize the byte. Succeeding bytes are
;    MOVed. This is generally used for the leftmost glyph of a string.
; 2) All bytes are ORed. This is for drawing when characters might overlap.
;
; On entry:
;       EBX = # of scans to copy
;       CL  = right rotation
;       EDX = ulBufDelta = width per scan of destination buffer, in bytes
;       ESI = pointer to first glyph byte
;       EDI = pointer to first dest buffer byte
;       DF  = cleared
;       ulGlyDelta = width per scan of source glyph, in bytes (wide case only)
;       ulWidthInBytes = width of glyph, in bytes (required only for 5 and
;               wider cases)
;
; On exit:
;       Any or all of EAX, EBX, ECX, EDX, ESI, and EDI may be trashed.

;-----------------------------------------------------------------------;
; OR first byte, 1 byte wide dest, rotated.
;-----------------------------------------------------------------------;
or_all_1_wide_rotated_need_last:
or_all_1_wide_rotated_no_last:
or_first_1_wide_rotated_need_last:
or_first_1_wide_rotated_no_last:
        SET_UP_UNROLL_AND_BRANCH ebx,eax,or_first_1_wide_rotated_table, \
                           LOOP_UNROLL_SHIFT

        UNROLL_LOOP_ENTRY_TABLE or_first_1_wide_rotated_table,OF1WR, \
                                LOOP_UNROLL_COUNT

MOR_FIRST_1_WIDE_ROTATED macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ch,[esi]
        inc     esi
        shr     ch,cl
        or      [edi],ch
        add     edi,edx
        endm    ;-----------------------------------;

or_first_1_wide_rotated_loop:
        UNROLL_LOOP     MOR_FIRST_1_WIDE_ROTATED,OF1WR,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     or_first_1_wide_rotated_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; MOV first byte, 1 byte wide dest, rotated.
;-----------------------------------------------------------------------;
mov_first_1_wide_rotated_need_last:
mov_first_1_wide_rotated_no_last:
        SET_UP_UNROLL_AND_BRANCH ebx,eax,mov_first_1_wide_rotated_table, \
                           LOOP_UNROLL_SHIFT

        UNROLL_LOOP_ENTRY_TABLE mov_first_1_wide_rotated_table,MF1WR, \
                                LOOP_UNROLL_COUNT

MMOV_FIRST_1_WIDE_ROTATED macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ch,[esi]
        inc     esi
        shr     ch,cl
        mov     [edi],ch
        add     edi,edx
        endm    ;-----------------------------------;

mov_first_1_wide_rotated_loop:
        UNROLL_LOOP     MMOV_FIRST_1_WIDE_ROTATED,MF1WR,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     mov_first_1_wide_rotated_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; MOV first byte, 1 byte wide dest, unrotated.
;-----------------------------------------------------------------------;
mov_first_1_wide_unrotated:

        SET_UP_UNROLL_AND_BRANCH ebx,eax,mov_first_1_wide_unrotated_table, \
                           LOOP_UNROLL_SHIFT

        UNROLL_LOOP_ENTRY_TABLE mov_first_1_wide_unrotated_table,MF1WU, \
                                LOOP_UNROLL_COUNT

MMOV_FIRST_1_WIDE_UNROTATED macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     al,[esi]
        inc     esi
        mov     [edi],al
        add     edi,edx
        endm    ;-----------------------------------;

mov_first_1_wide_unrotated_loop:
        UNROLL_LOOP     MMOV_FIRST_1_WIDE_UNROTATED,MF1WU,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     mov_first_1_wide_unrotated_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR all bytes, 1 byte wide dest, unrotated.
;-----------------------------------------------------------------------;
or_all_1_wide_unrotated:
        SET_UP_UNROLL_AND_BRANCH ebx,eax,or_all_1_wide_unrotated_table, \
                           LOOP_UNROLL_SHIFT

        UNROLL_LOOP_ENTRY_TABLE or_all_1_wide_unrotated_table,OA1WU, \
                                LOOP_UNROLL_COUNT

MOR_ALL_1_WIDE_UNROTATED macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     al,[esi]
        inc     esi
        or      [edi],al
        add     edi,edx
        endm    ;-----------------------------------;

or_all_1_wide_unrotated_loop:
        UNROLL_LOOP     MOR_ALL_1_WIDE_UNROTATED,OA1WU,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     or_all_1_wide_unrotated_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR first byte, 2 bytes wide dest, rotated, need final source byte.
;-----------------------------------------------------------------------;
or_first_2_wide_rotated_need_last:
        SET_UP_UNROLL_AND_BRANCH ebx,eax,or_first_2_wide_rotated_need_table, \
                           LOOP_UNROLL_SHIFT

        UNROLL_LOOP_ENTRY_TABLE or_first_2_wide_rotated_need_table,OF2WRN, \
                                LOOP_UNROLL_COUNT

MOR_FIRST_2_WIDE_ROTATED_NEED macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ax,[esi]
        add     esi,2
        ror     ax,cl
        or      [edi],al
        mov     [edi+1],ah
        add     edi,edx
        endm    ;-----------------------------------;

or_first_2_wide_rotated_need_loop:
        UNROLL_LOOP     MOR_FIRST_2_WIDE_ROTATED_NEED,OF2WRN,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     or_first_2_wide_rotated_need_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR all bytes, 2 bytes wide dest, rotated, need final source byte.
;-----------------------------------------------------------------------;
or_all_2_wide_rotated_need_last:
        SET_UP_UNROLL_AND_BRANCH ebx,eax,or_all_2_wide_rotated_need_table, \
                           LOOP_UNROLL_SHIFT

        UNROLL_LOOP_ENTRY_TABLE or_all_2_wide_rotated_need_table,OA2WRN, \
                                LOOP_UNROLL_COUNT

MOR_ALL_2_WIDE_ROTATED_NEED macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ax,[esi]
        add     esi,2
        ror     ax,cl
        or      [edi],ax
        add     edi,edx
        endm    ;-----------------------------------;

or_all_2_wide_rotated_need_loop:
        UNROLL_LOOP     MOR_ALL_2_WIDE_ROTATED_NEED,OA2WRN,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     or_all_2_wide_rotated_need_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; MOV first byte, 2 bytes wide dest, rotated, need final source byte.
;-----------------------------------------------------------------------;
mov_first_2_wide_rotated_need_last:
        SET_UP_UNROLL_AND_BRANCH ebx,eax,mov_first_2_wide_rotated_need_table, \
                           LOOP_UNROLL_SHIFT

        UNROLL_LOOP_ENTRY_TABLE mov_first_2_wide_rotated_need_table,MF2WRN, \
                                LOOP_UNROLL_COUNT

MMOV_FIRST_2_WIDE_ROTATED_NEED macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ax,[esi]
        add     esi,2
        ror     ax,cl
        mov     [edi],ax
        add     edi,edx
        endm    ;-----------------------------------;

mov_first_2_wide_rotated_need_loop:
        UNROLL_LOOP     MMOV_FIRST_2_WIDE_ROTATED_NEED,MF2WRN,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     mov_first_2_wide_rotated_need_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR first byte, 2 bytes wide dest, rotated, don't need final source byte.
;-----------------------------------------------------------------------;
or_first_2_wide_rotated_no_last:
        SET_UP_UNROLL_AND_BRANCH ebx,eax,or_first_2_wide_rotated_table, \
                           LOOP_UNROLL_SHIFT

        UNROLL_LOOP_ENTRY_TABLE or_first_2_wide_rotated_table,OF2WR, \
                                LOOP_UNROLL_COUNT

MOR_FIRST_2_WIDE_ROTATED macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        sub     eax,eax
        mov     ah,[esi]
        inc     esi
        shr     eax,cl
        or      [edi],ah
        mov     [edi+1],al
        add     edi,edx
        endm    ;-----------------------------------;

or_first_2_wide_rotated_loop:
        UNROLL_LOOP     MOR_FIRST_2_WIDE_ROTATED,OF2WR,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     or_first_2_wide_rotated_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR all bytes, 2 bytes wide dest, rotated, don't need final source byte.
;-----------------------------------------------------------------------;
or_all_2_wide_rotated_no_last:
        SET_UP_UNROLL_AND_BRANCH ebx,eax,or_all_2_wide_rotated_table, \
                           LOOP_UNROLL_SHIFT

        UNROLL_LOOP_ENTRY_TABLE or_all_2_wide_rotated_table,OA2WR, \
                                LOOP_UNROLL_COUNT

MOR_ALL_2_WIDE_ROTATED macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        sub     eax,eax
        mov     al,[esi]
        inc     esi
        ror     ax,cl
        or      [edi],ax
        add     edi,edx
        endm    ;-----------------------------------;

or_all_2_wide_rotated_loop:
        UNROLL_LOOP     MOR_ALL_2_WIDE_ROTATED,OA2WR,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     or_all_2_wide_rotated_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; MOV first byte, 2 bytes wide dest, rotated, don't need final source byte.
;-----------------------------------------------------------------------;
mov_first_2_wide_rotated_no_last:
        SET_UP_UNROLL_AND_BRANCH ebx,eax,mov_first_2_wide_rotated_table, \
                           LOOP_UNROLL_SHIFT

        UNROLL_LOOP_ENTRY_TABLE mov_first_2_wide_rotated_table,MF2WR, \
                                LOOP_UNROLL_COUNT

MMOV_FIRST_2_WIDE_ROTATED macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        sub     eax,eax
        mov     al,[esi]
        inc     esi
        ror     ax,cl
        mov     [edi],ax
        add     edi,edx
        endm    ;-----------------------------------;

mov_first_2_wide_rotated_loop:
        UNROLL_LOOP     MMOV_FIRST_2_WIDE_ROTATED,MF2WR,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     mov_first_2_wide_rotated_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; MOV first byte, 2 bytes wide dest, unrotated.
;-----------------------------------------------------------------------;
mov_first_2_wide_unrotated:
        SET_UP_UNROLL_AND_BRANCH ebx,eax,mov_first_2_wide_unrotated_table, \
                           LOOP_UNROLL_SHIFT

        UNROLL_LOOP_ENTRY_TABLE mov_first_2_wide_unrotated_table,MF2WU, \
                                LOOP_UNROLL_COUNT

MMOV_FIRST_2_WIDE_UNROTATED macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ax,[esi]
        add     esi,2
        mov     [edi],ax
        add     edi,edx
        endm    ;-----------------------------------;

mov_first_2_wide_unrotated_loop:
        UNROLL_LOOP     MMOV_FIRST_2_WIDE_UNROTATED,MF2WU,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     mov_first_2_wide_unrotated_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR all bytes, 2 bytes wide dest, unrotated.
;-----------------------------------------------------------------------;
or_all_2_wide_unrotated:
        SET_UP_UNROLL_AND_BRANCH ebx,eax,or_all_2_wide_unrotated_table, \
                           LOOP_UNROLL_SHIFT

        UNROLL_LOOP_ENTRY_TABLE or_all_2_wide_unrotated_table,OA2WU, \
                                LOOP_UNROLL_COUNT

MOR_ALL_2_WIDE_UNROTATED macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ax,[esi]
        add     esi,2
        or      [edi],ax
        add     edi,edx
        endm    ;-----------------------------------;

or_all_2_wide_unrotated_loop:
        UNROLL_LOOP     MOR_ALL_2_WIDE_UNROTATED,OA2WU,LOOP_UNROLL_COUNT
        dec     ebx
        jnz     or_all_2_wide_unrotated_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR first byte, 3 bytes wide dest, rotated, need final source byte.
;-----------------------------------------------------------------------;
or_first_3_wide_rotated_need_last:
@@:
        mov     al,[esi]
        shr     al,cl
        or      [edi],al
        mov     ax,[esi]
        ror     ax,cl
        mov     [edi+1],ah
        mov     ax,[esi+1]
        add     esi,3
        ror     ax,cl
        mov     [edi+2],ah
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR first byte, 3 bytes wide dest, rotated, need final source byte.
;-----------------------------------------------------------------------;
or_all_3_wide_rotated_need_last:
@@:
        mov     al,[esi]
        shr     al,cl
        or      [edi],al
        mov     ax,[esi]
        ror     ax,cl
        or      [edi+1],ah
        mov     ax,[esi+1]
        add     esi,3
        ror     ax,cl
        or      [edi+2],ah
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; MOV first byte, 3 bytes wide dest, rotated, need final source byte.
;-----------------------------------------------------------------------;
mov_first_3_wide_rotated_need_last:
@@:
        mov     al,[esi]
        shr     al,cl
        mov     [edi],al
        mov     ax,[esi]
        ror     ax,cl
        mov     [edi+1],ah
        mov     ax,[esi+1]
        add     esi,3
        ror     ax,cl
        mov     [edi+2],ah
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR first byte, 3 bytes wide dest, rotated, don't need final source byte.
;-----------------------------------------------------------------------;
or_first_3_wide_rotated_no_last:
        neg     cl
        and     cl,111b         ;convert from right shift to left shift
@@:
        sub     eax,eax
        mov     ax,[esi]
        add     esi,2
        xchg    ah,al
        shl     eax,cl
        mov     [edi+1],ah
        mov     [edi+2],al
        shr     eax,16
        or      [edi],al
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR all bytes, 3 bytes wide dest, rotated, don't need final source byte.
;-----------------------------------------------------------------------;
or_all_3_wide_rotated_no_last:
        neg     cl
        and     cl,111b         ;convert from right shift to left shift
@@:
        sub     eax,eax
        mov     ax,[esi]
        add     esi,2
        xchg    ah,al
        shl     eax,cl
        xchg    ah,al
        or      [edi+1],ax
        shr     eax,16
        or      [edi],al
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; MOV first byte, 3 bytes wide dest, rotated, don't need final source byte.
;-----------------------------------------------------------------------;
mov_first_3_wide_rotated_no_last:
        neg     cl
        and     cl,111b         ;convert from right shift to left shift
@@:
        sub     eax,eax
        mov     ax,[esi]
        add     esi,2
        xchg    ah,al
        shl     eax,cl
        mov     [edi+1],ah
        mov     [edi+2],al
        shr     eax,16
        mov     [edi],al
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; MOV first byte, 3 bytes wide dest, unrotated.
;-----------------------------------------------------------------------;
mov_first_3_wide_unrotated:
@@:
        mov     ax,[esi]
        mov     [edi],ax
        mov     al,[esi+2]
        add     esi,3
        mov     [edi+2],al
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR all bytes, 3 bytes wide dest, unrotated.
;-----------------------------------------------------------------------;
or_all_3_wide_unrotated:
@@:
        mov     ax,[esi]
        or      [edi],ax
        mov     al,[esi+2]
        add     esi,3
        or      [edi+2],al
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR first byte, 4 bytes wide dest, rotated, need final source byte.
;-----------------------------------------------------------------------;
or_first_4_wide_rotated_need_last:
@@:
        mov     eax,[esi]
        add     esi,4
        xchg    ah,al
        ror     eax,16
        xchg    ah,al
        shr     eax,cl
        xchg    ah,al
        mov     [edi+2],ax
        shr     eax,16
        mov     [edi+1],al
        or      [edi],ah
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR all bytes, 4 bytes wide dest, rotated, need final source byte.
;-----------------------------------------------------------------------;
or_all_4_wide_rotated_need_last:
@@:
        mov     eax,[esi]
        add     esi,4
        xchg    ah,al
        ror     eax,16
        xchg    ah,al
        shr     eax,cl
        xchg    ah,al
        ror     eax,16
        xchg    al,ah
        or      [edi],eax
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; MOV first byte, 4 bytes wide dest, rotated, need final source byte.
;-----------------------------------------------------------------------;
mov_first_4_wide_rotated_need_last:
@@:
        mov     eax,[esi]
        add     esi,4
        xchg    ah,al
        ror     eax,16
        xchg    ah,al
        shr     eax,cl
        xchg    ah,al
        ror     eax,16
        xchg    ah,al
        mov     [edi],eax
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR first byte, 4 bytes wide dest, rotated, don't need final source byte.
;-----------------------------------------------------------------------;
or_first_4_wide_rotated_no_last:
@@:
        mov     ax,[esi]
        xchg    ah,al
        shl     eax,16
        mov     ah,[esi+2]
        add     esi,3
        shr     eax,cl
        xchg    ah,al
        mov     [edi+2],ax
        shr     eax,16
        mov     [edi+1],al
        or      [edi],ah
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR all bytes, 4 bytes wide dest, rotated, don't need final source byte.
;-----------------------------------------------------------------------;
or_all_4_wide_rotated_no_last:
@@:
        mov     ax,[esi]
        xchg    ah,al
        shl     eax,16
        mov     ah,[esi+2]
        add     esi,3
        shr     eax,cl
        xchg    ah,al
        ror     eax,16
        xchg    ah,al
        or      [edi],eax
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; MOV first byte, 4 bytes wide dest, rotated, don't need final source byte.
;-----------------------------------------------------------------------;
mov_first_4_wide_rotated_no_last:
@@:
        mov     ax,[esi]
        xchg    ah,al
        shl     eax,16
        mov     ah,[esi+2]
        add     esi,3
        shr     eax,cl
        xchg    ah,al
        ror     eax,16
        xchg    ah,al
        mov     [edi],eax
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; MOV first byte, 4 bytes wide dest, unrotated.
;-----------------------------------------------------------------------;
mov_first_4_wide_unrotated:
@@:
        mov     eax,[esi]
        add     esi,4
        mov     [edi],eax
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR all bytes, 4 bytes wide dest, unrotated.
;-----------------------------------------------------------------------;
or_all_4_wide_unrotated:
@@:
        mov     eax,[esi]
        add     esi,4
        or      [edi],eax
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR first byte, n bytes wide dest, rotated, need final source byte.
;-----------------------------------------------------------------------;
or_first_N_wide_rotated_need_last:
        mov     eax,ulWidthInBytes
        mov     edx,ulBufDelta
        sub     edx,eax
        mov     ulTmpDstDelta,edx
        dec     eax             ;source doesn't advance after first byte, and
                                ; we do the first byte outside the loop
        mov     edx,ulGlyDelta
        sub     edx,eax
        mov     ulTmpSrcDelta,edx
        mov     ulTmpWidthInBytes,eax
ofNwrnl_scan_loop:
        mov     al,[esi]        ;do the initial, ORed byte separately
        shr     al,cl
        or      [edi],al
        inc     edi
        mov     edx,ulTmpWidthInBytes
@@:
        mov     ax,[esi]
        inc     esi
        ror     ax,cl
        mov     [edi],ah
        inc     edi
        dec     edx
        jnz     @B
        add     esi,ulTmpSrcDelta
        add     edi,ulTmpDstDelta
        dec     ebx
        jnz     ofNwrnl_scan_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR all bytes, n bytes wide dest, rotated, need final source byte.
;-----------------------------------------------------------------------;
or_all_N_wide_rotated_need_last:
        mov     eax,ulWidthInBytes
        mov     edx,ulBufDelta
        sub     edx,eax
        mov     ulTmpDstDelta,edx
        dec     eax             ;source doesn't advance after first byte, and
                                ; we do the first byte outside the loop
        mov     edx,ulGlyDelta
        sub     edx,eax
        mov     ulTmpSrcDelta,edx
        mov     ulTmpWidthInBytes,eax
oaNwrnl_scan_loop:
        mov     al,[esi]        ;do the initial, ORed byte separately
        shr     al,cl
        or      [edi],al
        inc     edi
        mov     edx,ulTmpWidthInBytes
@@:
        mov     ax,[esi]
        inc     esi
        ror     ax,cl
        or      [edi],ah
        inc     edi
        dec     edx
        jnz     @B
        add     esi,ulTmpSrcDelta
        add     edi,ulTmpDstDelta
        dec     ebx
        jnz     oaNwrnl_scan_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; MOV first byte, n bytes wide dest, rotated, need final source byte.
;-----------------------------------------------------------------------;
mov_first_N_wide_rotated_need_last:
        mov     eax,ulWidthInBytes
        mov     edx,ulBufDelta
        sub     edx,eax
        mov     ulTmpDstDelta,edx
        mov     eax,ulWidthInBytes
        dec     eax             ;source doesn't advance after first byte, and
                                ; we do the first byte outside the loop
        mov     edx,ulGlyDelta
        sub     edx,eax
        mov     ulTmpSrcDelta,edx
        mov     ulTmpWidthInBytes,eax
mfNwrnl_scan_loop:
        mov     al,[esi]        ;do the initial byte separately
        shr     al,cl
        mov     [edi],al
        inc     edi
        mov     edx,ulTmpWidthInBytes
@@:
        mov     ax,[esi]
        inc     esi
        ror     ax,cl
        mov     [edi],ah
        inc     edi
        dec     edx
        jnz     @B
        add     esi,ulTmpSrcDelta
        add     edi,ulTmpDstDelta
        dec     ebx
        jnz     mfNwrnl_scan_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR first byte, N bytes wide dest, rotated, don't need final source byte.
;-----------------------------------------------------------------------;
or_first_N_wide_rotated_no_last:
        mov     eax,ulWidthInBytes
        dec     eax             ;one less because we don't advance after the
                                ; last byte
        mov     edx,ulBufDelta
        sub     edx,eax
        mov     ulTmpDstDelta,edx
        dec     eax             ;source doesn't advance after first byte, and
                                ; we do the first & last bytes outside the
                                ; loop; already subtracted 1 above
        mov     edx,ulGlyDelta
        sub     edx,eax
        mov     ulTmpSrcDelta,edx
        mov     ulTmpWidthInBytes,eax
ofNwr_scan_loop:
        mov     al,[esi]        ;do the initial, ORed byte separately
        shr     al,cl
        or      [edi],al
        inc     edi
        mov     edx,ulTmpWidthInBytes
@@:
        mov     ax,[esi]
        inc     esi
        ror     ax,cl
        mov     [edi],ah
        inc     edi
        dec     edx
        jnz     @B

        mov     ah,[esi]        ;do the final byte separately
        sub     al,al
        shr     eax,cl
        mov     [edi],al

        add     esi,ulTmpSrcDelta
        add     edi,ulTmpDstDelta
        dec     ebx
        jnz     ofNwr_scan_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR all bytes, N bytes wide dest, rotated, don't need final source byte.
;-----------------------------------------------------------------------;
or_all_N_wide_rotated_no_last:
        mov     eax,ulWidthInBytes
        dec     eax             ;one less because we don't advance after the
                                ; last byte
        mov     edx,ulBufDelta
        sub     edx,eax
        mov     ulTmpDstDelta,edx
        dec     eax             ;source doesn't advance after first byte, and
                                ; we do the first & last bytes outside the
                                ; loop; already subtracted 1 above
        mov     edx,ulGlyDelta
        sub     edx,eax
        mov     ulTmpSrcDelta,edx
        mov     ulTmpWidthInBytes,eax
oaNwr_scan_loop:
        mov     al,[esi]        ;do the initial, ORed byte separately
        shr     al,cl
        or      [edi],al
        inc     edi
        mov     edx,ulTmpWidthInBytes
@@:
        mov     ax,[esi]
        inc     esi
        ror     ax,cl
        or      [edi],ah
        inc     edi
        dec     edx
        jnz     @B

        mov     ah,[esi]        ;do the final byte separately
        sub     al,al
        shr     eax,cl
        or      [edi],al

        add     esi,ulTmpSrcDelta
        add     edi,ulTmpDstDelta
        dec     ebx
        jnz     oaNwr_scan_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; MOV first byte, N bytes wide dest, rotated, don't need final source byte.
;-----------------------------------------------------------------------;
mov_first_N_wide_rotated_no_last:
        mov     eax,ulWidthInBytes
        dec     eax             ;one less because we don't advance after the
                                ; last byte
        mov     edx,ulBufDelta
        sub     edx,eax
        mov     ulTmpDstDelta,edx
        dec     eax             ;source doesn't advance after first byte, and
                                ; we do the first & last bytes outside the
                                ; loop; already subtracted 1 above
        mov     edx,ulGlyDelta
        sub     edx,eax
        mov     ulTmpSrcDelta,edx
        mov     ulTmpWidthInBytes,eax
mfNwr_scan_loop:
        mov     al,[esi]        ;do the initial byte separately
        shr     al,cl
        mov     [edi],al
        inc     edi
        mov     edx,ulTmpWidthInBytes
@@:
        mov     ax,[esi]
        inc     esi
        ror     ax,cl
        mov     [edi],ah
        inc     edi
        dec     edx
        jnz     @B

        mov     ah,[esi]        ;do the final byte separately
        sub     al,al
        shr     eax,cl
        mov     [edi],al

        add     esi,ulTmpSrcDelta
        add     edi,ulTmpDstDelta
        dec     ebx
        jnz     mfNwr_scan_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; MOV first byte, N bytes wide dest, unrotated.
;-----------------------------------------------------------------------;
mov_first_N_wide_unrotated:
        mov     edx,ulBufDelta
        mov     eax,ulWidthInBytes
        sub     edx,eax
        shr     eax,1           ;width in words
        jc      short odd_width ;there's at least one odd byte
        shr     eax,1           ;width in dwords
        jc      short two_odd_bytes ;there's an odd word
                                ;copy width is a dword multiple
@@:
        mov     ecx,eax
        rep     movsd           ;copy as many dwords as possible
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

odd_width:
        shr     eax,1           ;width in dwords
        jc      short three_odd_bytes ;there's an odd word and an odd byte
                                ;there's just an odd byte
        inc     edx             ;because we won't advance after last byte
@@:
        mov     ecx,eax
        rep     movsd           ;copy as many dwords as possible
        mov     cl,[esi]
        inc     esi
        mov     [edi],cl
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

two_odd_bytes:
        add     edx,2           ;because we won't advance after last word
@@:
        mov     ecx,eax
        rep     movsd           ;copy as many dwords as possible
        mov     cx,[esi]
        add     esi,2
        mov     [edi],cx
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

three_odd_bytes:
        add     edx,3           ;because we won't advance after last word/byte
@@:
        mov     ecx,eax
        rep     movsd           ;copy as many dwords as possible
        mov     cx,[esi]
        mov     [edi],cx
        mov     cl,[esi+2]
        add     esi,3
        mov     [edi+2],cl
        add     edi,edx
        dec     ebx
        jnz     @B
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; OR all bytes, N bytes wide dest, unrotated.
;-----------------------------------------------------------------------;
or_all_N_wide_unrotated:
        mov     edx,ulBufDelta
        mov     eax,ulWidthInBytes
        sub     edx,eax
        shr     eax,1           ;width in words
        jc      short or_odd_width ;there's at least one odd byte
        shr     eax,1           ;width in dwords
        jc      short or_two_odd_bytes ;there's an odd word
                                ;copy width is a dword multiple
or_no_odd_bytes_loop:
        push    ebx             ;preserve scan count
        mov     ebx,eax
@@:
        mov     ecx,[esi]
        add     esi,4
        or      [edi],ecx
        add     edi,4           ;copy as many dwords as possible
        dec     ebx
        jnz     @B
        add     edi,edx
        pop     ebx             ;restore scan count
        dec     ebx
        jnz     or_no_odd_bytes_loop
        jmp     pGlyphLoop

or_odd_width:
        shr     eax,1           ;width in dwords
        jc      short three_odd_bytes ;there's an odd word and an odd byte
                                ;there's just an odd byte
        inc     edx             ;skip over last byte too
or_one_odd_bytes_loop:
        push    ebx             ;preserve scan count
        mov     ebx,eax
@@:
        mov     ecx,[esi]
        add     esi,4
        or      [edi],ecx
        add     edi,4           ;copy as many dwords as possible
        dec     ebx
        jnz     @B
        mov     cl,[esi]
        or      [edi],cl
        inc     esi
        add     edi,edx
        pop     ebx             ;restore scan count
        dec     ebx
        jnz     or_one_odd_bytes_loop
        jmp     pGlyphLoop

or_two_odd_bytes:
        add     edx,2           ;skip over last 2 bytes too
or_two_odd_bytes_loop:
        push    ebx             ;preserve scan count
        mov     ebx,eax
@@:
        mov     ecx,[esi]
        add     esi,4
        or      [edi],ecx
        add     edi,4           ;copy as many dwords as possible
        dec     ebx
        jnz     @B
        mov     cx,[esi]
        or      [edi],cx
        add     esi,2
        add     edi,edx
        pop     ebx             ;restore scan count
        dec     ebx
        jnz     or_two_odd_bytes_loop
        jmp     pGlyphLoop

or_three_odd_bytes:
        add     edx,3           ;skip over last 3 bytes too
or_three_odd_bytes_loop:
        push    ebx             ;preserve scan count
        mov     ebx,eax
@@:
        mov     ecx,[esi]
        add     esi,4
        or      [edi],ecx
        add     edi,4           ;copy as many dwords as possible
        dec     ebx
        jnz     @B
        mov     cx,[esi]
        or      [edi],cx
        mov     cl,[esi+2]
        or      [edi+2],cl
        add     esi,3
        add     edi,edx
        pop     ebx             ;restore scan count
        dec     ebx
        jnz     or_three_odd_bytes_loop
        jmp     pGlyphLoop

;-----------------------------------------------------------------------;
; At this point, the text is drawn to the temp buffer.
; Now, draw the temp buffer to the screen.
;
; Input:
;       pdsurf = pointer to target surface (screen)
;       prclText = pointer to text bounding rectangle
;       prclOpaque = pointer to opaquing rectangle, if there is one
;       iFgColor = text color
;       iBgColor = opaquing rectangle color, if there is one
;       ulTempLeft = X coordinate on dest of left edge of temp buffer pointed
;               to by pTempBuffer
;       pTempBuffer = pointer to first byte (upper left corner) of
;               temp buffer into which we're drawing. This should be
;               word-aligned with the destination
;       ulBufDelta = destination scan-to-scan offset
;       Text drawn to temp buffer
;
;-----------------------------------------------------------------------;
draw_to_screen:

;-----------------------------------------------------------------------;
; Is this transparent or opaque text?
;-----------------------------------------------------------------------;

        mov     esi,prclText
        cmp     prclOpaque,0
        jnz     opaque_text

;-----------------------------------------------------------------------;
; Transparent text.
; ESI = prclText
;-----------------------------------------------------------------------;

;-----------------------------------------------------------------------;
; Set up the VGA's hardware for read mode 1 and write mode 3.
;-----------------------------------------------------------------------;

        mov     edx,VGA_BASE + GRAF_ADDR
        mov     eax,GRAF_MODE + ((M_AND_WRITE + M_COLOR_READ) SHL 8)
        out     dx,ax                   ;write mode 3 so we can do masking
                                        ; without OUTs, read mode 1 so we can
                                        ; read 0xFF from memory always, for
                                        ; ANDing (because Color Don't Care is
                                        ; all zeros)

        mov     al,GRAF_SET_RESET
        mov     ah,byte ptr iFgColor
        out     dx,ax                   ;set the drawing color

;-----------------------------------------------------------------------;
; Calculate drawing parameters.
;-----------------------------------------------------------------------;

        mov     eax,[esi].yBottom
        mov     ulBottomScan,eax ;bottom scan of text area
        mov     eax,[esi].xRight
        mov     edx,[esi].xLeft
        and     edx,not 7
        add     eax,7
        sub     eax,edx
        shr     eax,3           ;width of text in bytes, rounded up

        mov     edx,eax         ;width in bytes
        mov     ecx,eax
        and     edx,7           ;# of odd (non-qword) bytes
        mov     edi,XparEntryTable[edx*4]
        mov     pfnEntry,edi    ;entry point into unrolled loop for odd bytes
        add     ecx,7
        shr     ecx,3           ;width in qwords (unrolled loop count)
        mov     ulLoopCount,ecx

        neg     edx             ;amount by which we have to compensate back
        and     edx,07h         ; before starting, because of the hard-wired
                                ; displacements used in the unrolled loop

        add     eax,edx         ;adjust effective width for compensation

        mov     ecx,ulBufDelta
        sub     ecx,eax         ;offset to next scan in temp buffer
        mov     ulTmpSrcDelta,ecx

        mov     ebx,pdsurf
        mov     ecx,ulScreenDelta
        sub     ecx,eax         ;offset to next scan in screen
        mov     ulTmpDstDelta,ecx

        mov     eax,[esi].yTop
        imul    eax,ulScreenDelta ;this form of IMUL so we can avoid wiping
                                  ; out EDX
        mov     edi,[esi].xLeft
        shr     edi,3
        add     edi,eax
        sub     edi,edx         ;compensate back for displacement of initial
                                ; entry point into unrolled loop, below

;-----------------------------------------------------------------------;
; Map in the bank containing the top scan of the text, if it's not
; mapped in already.
;-----------------------------------------------------------------------;

        mov     eax,[esi].yTop  ;top scan line of text
        mov     ulTopScan,eax
        mov     esi,pTempBuffer ;initial source address
        sub     esi,edx         ;compensate back for displacement of initial
                                ; entry point into unrolled loop, below
        cmp     eax,[ebx].dsurf_rcl1WindowClip.yTop ;is text top less than
                                                    ; current bank?
        jl      short xpar_map_init_bank            ;yes, map in proper bank
        cmp     eax,[ebx].dsurf_rcl1WindowClip.yBottom ;text top greater than
                                                       ; current bank?
        jl      short xpar_init_bank_mapped     ;no, proper bank already mapped
xpar_map_init_bank:

; Map in the bank containing the top scan line of the fill.
; Preserves EBX, ESI, and EDI.

        ptrCall <dword ptr [ebx].dsurf_pfnBankControl>,<ebx,eax,JustifyTop>

xpar_init_bank_mapped:

        add     edi,[ebx].dsurf_pvBitmapStart   ;initial destination address

;-----------------------------------------------------------------------;
; Main loop for processing fill in each bank.
;
; At start of loop, EBX->pdsurf
;-----------------------------------------------------------------------;

xpar_bank_loop:
        mov     edx,ulBottomScan        ;bottom of destination rectangle
        cmp     edx,[ebx].dsurf_rcl1WindowClip.yBottom
                                        ;which comes first, the bottom of the
                                        ; text rect or the bottom of the
                                        ; current bank?
        jl      short @F                ;text bottom comes first, so draw to
                                        ; that; this is the last bank in text
        mov     edx,[ebx].dsurf_rcl1WindowClip.yBottom
                                        ;bank bottom comes first; draw to
                                        ; bottom of bank
@@:
        sub     edx,ulTopScan           ;# of scans to draw in bank
        mov     ebx,ulTmpSrcDelta

xpar_scan_loop:
        mov     ecx,ulLoopCount ;unrolled loop count
        jmp     pfnEntry        ;jump into the unrolled loop

;-----------------------------------------------------------------------
; Entry points into unrolled xpar loop for various odd byte counts.
        align   4
XparEntryTable  label   dword
        dd      xpar_enter_8
        dd      xpar_enter_1
        dd      xpar_enter_2
        dd      xpar_enter_3
        dd      xpar_enter_4
        dd      xpar_enter_5
        dd      xpar_enter_6
        dd      xpar_enter_7

;-----------------------------------------------------------------------
; Unrolled xpar loop.
xpar_byte_loop:
xpar_enter_8:
        mov     al,[esi]        ;get next temp buffer byte
        and     al,al           ;is it zero?
        jz      short @F        ;yes, skip it
        and     [edi],al        ;draw the byte with transparency
@@:
xpar_enter_7:
        mov     al,[esi+1]      ;get next temp buffer byte
        and     al,al           ;is it zero?
        jz      short @F        ;yes, skip it
        and     [edi+1],al      ;draw the byte with transparency
@@:
xpar_enter_6:
        mov     al,[esi+2]      ;get next temp buffer byte
        and     al,al           ;is it zero?
        jz      short @F        ;yes, skip it
        and     [edi+2],al      ;draw the byte with transparency
@@:
xpar_enter_5:
        mov     al,[esi+3]      ;get next temp buffer byte
        and     al,al           ;is it zero?
        jz      short @F        ;yes, skip it
        and     [edi+3],al      ;draw the byte with transparency
@@:
xpar_enter_4:
        mov     al,[esi+4]      ;get next temp buffer byte
        and     al,al           ;is it zero?
        jz      short @F        ;yes, skip it
        and     [edi+4],al      ;draw the byte with transparency
@@:
xpar_enter_3:
        mov     al,[esi+5]      ;get next temp buffer byte
        and     al,al           ;is it zero?
        jz      short @F        ;yes, skip it
        and     [edi+5],al      ;draw the byte with transparency
@@:
xpar_enter_2:
        mov     al,[esi+6]      ;get next temp buffer byte
        and     al,al           ;is it zero?
        jz      short @F        ;yes, skip it
        and     [edi+6],al      ;draw the byte with transparency
@@:
xpar_enter_1:
        mov     al,[esi+7]      ;get next temp buffer byte
        and     al,al           ;is it zero?
        jz      short @F        ;yes, skip it
        and     [edi+7],al      ;draw the byte with transparency
@@:
        add     esi,8
        add     edi,8
        dec     ecx
        jnz     xpar_byte_loop

        add     esi,ebx           ;point to next buffer scan
        add     edi,ulTmpDstDelta ;point to next screen scan

        dec     edx             ;count down scans
        jnz     xpar_scan_loop

;-----------------------------------------------------------------------;
; See if there are more banks to draw.
;-----------------------------------------------------------------------;

        mov     ebx,pdsurf
        mov     eax,[ebx].dsurf_rcl1WindowClip.yBottom ;is the text bottom in
        cmp     ulBottomScan,eax                       ; the current bank?
        jnle    short do_next_xpar_bank ;no, map in the next bank and draw
                                        ;yes, so we're done

;-----------------------------------------------------------------------;
; Restore the VGA's hardware state and we're done.
;-----------------------------------------------------------------------;

        mov     edx,VGA_BASE + GRAF_ADDR
        mov     eax,GRAF_MODE + ((M_PROC_WRITE + M_DATA_READ) SHL 8)
        out     dx,ax                   ;restore read mode 0 and write mode 0

draw_prop_done:
        cRet    vFastText

do_next_xpar_bank:
        mov     ulTopScan,eax
        sub     edi,[ebx].dsurf_pvBitmapStart ;convert from address to offset
                                              ; within bitmap
        ptrCall <dword ptr [ebx].dsurf_pfnBankControl>,<ebx,eax,JustifyTop>
                                        ;map in the bank (call preserves EBX,
                                        ; ESI, and EDI)
        add     edi,[ebx].dsurf_pvBitmapStart ;convert from offset within
                                              ; bitmap to address (bitmap start
                                              ; just moved)
        jmp     xpar_bank_loop          ;we're ready to draw in the new bank

;-----------------------------------------------------------------------;
; Handle opaque clipping.
;-----------------------------------------------------------------------;

do_opaque_clip:
        mov     ebx,[esi].yBottom
        cmp     [edi].yBottom,ebx ;is the bottom edge of the text box clipped?
        jg      short @F          ;no
        mov     ebx,[edi].yBottom ;yes
@@:
        mov     dword ptr rclClippedBounds.yBottom,ebx ;set the (possibly
                                                       ; clipped) bottom edge
        mov     eax,[esi].yTop
        cmp     [edi].yTop,eax  ;is the top edge of the text box clipped?
        jle     short @F        ;no
                                ;yes
        sub     eax,[edi].yTop
        neg     eax             ;# of scans we just clipped off
        mul     ulBufDelta      ;# of bytes by which to advance through source
        add     pTempBuffer,eax ;advance in source to account for Y clipping
        mov     eax,[edi].yTop  ;new top edge
@@:
        mov     dword ptr rclClippedBounds.yTop,eax ;set the (possibly clipped)
                                                    ; top edge
        cmp     eax,ebx         ;is there a gap between clipped top & bottom?
        jnl     short opaq_fully_clipped ;no, fully clipped

        mov     edx,[esi].xRight
        cmp     [edi].xRight,edx ;is the right edge of the text box clipped?
        jg      short @F         ;no
        mov     edx,[edi].xRight ;yes
@@:
        mov     dword ptr rclClippedBounds.xRight,edx ;set the (possibly
                                                      ; clipped) right edge
        mov     eax,[esi].xLeft
        cmp     [edi].xLeft,eax ;is the left edge of the text box clipped?
        jle     short @F        ;no
                                ;yes
        mov     ebx,[edi].xLeft ;EBX = new left edge
        and     eax,not 0111b   ;floor the old left edge in its byte
        sub     ebx,eax
        shr     ebx,3           ;# of bytes to advance in source
        add     pTempBuffer,ebx ;advance in source to account for X clipping
        mov     eax,[edi].xLeft ;new left edge
@@:
        mov     dword ptr rclClippedBounds.xLeft,eax ;set the (possibly
                                                     ; clipped) left edge
        cmp     eax,edx         ;is there a gap between clipped left & right?
        jnl     short opaq_fully_clipped ;no, fully clipped

        lea     esi,rclClippedBounds ;this is now the destination rect
        jmp     short clip_set

opaq_fully_clipped:
        jmp     done

;-----------------------------------------------------------------------;
; Opaque text.
; ESI = prclText
;-----------------------------------------------------------------------;

opaque_text:

;-----------------------------------------------------------------------;
; Clip to the clip rectangle, if necessary.
;-----------------------------------------------------------------------;

        mov     edi,prclClip
        and     edi,edi              ;is there clipping?
        jnz     short do_opaque_clip ;yes

; ESI->destination text rectangle at this point
clip_set:

;-----------------------------------------------------------------------;
; Set up the VGA's hardware for read mode 1 and write mode 3.
;-----------------------------------------------------------------------;

        mov     edx,VGA_BASE + GRAF_ADDR
        mov     eax,GRAF_MODE + ((M_AND_WRITE + M_COLOR_READ) SHL 8)
        out     dx,ax                   ;write mode 3 so we can do masking
                                        ; without OUTs, read mode 1 so we can
                                        ; read 0xFF from memory always, for
                                        ; ANDing (because Color Don't Care is
                                        ; all zeros)

;-----------------------------------------------------------------------;
; Calculate the drawing parameters.
;-----------------------------------------------------------------------;

        mov     eax,[esi].yBottom
        mov     ulBottomScan,eax ;bottom scan of text area
        mov     eax,[esi].xRight
        mov     ebx,eax
        and     ebx,111b
        mov     edx,[esi].xLeft
        mov     ch,jOpaqueRightMasks[ebx]       ;set right edge clip mask
        mov     ebx,edx
        and     ebx,111b
        mov     ulRightMask,ecx                 ;mask is expected to be in CH
        mov     ch,jOpaqueLeftMasks[ebx]        ;set left edge clip mask
        mov     ulLeftMask,ecx                  ;mask is expected to be in CH

        mov     ulTextLeft,edx
        and     edx,not 7
        add     eax,7
        sub     eax,edx
        shr     eax,3           ;width of text in bytes, rounded up
        mov     ulTextWidthInBytes,eax

;-----------------------------------------------------------------------;
; Figure out what edges we need to handle, and calculate some info for
; doing whole bytes.
;-----------------------------------------------------------------------;

        cmp     eax,1                   ;only one byte total?
        jnz     short @F                ;no
                                        ;yes, special case
        mov     ecx,offset opaq_check_more_banks  ;assume it's a solid byte
        mov     ebx,ulLeftMask
        and     ebx,ulRightMask
        cmp     bh,0ffh                 ;solid byte?
        jz      short opaq_set_edge_vector ;yes, all set
        mov     ulLeftMask,ebx          ;no, draw as a left edge
        dec     eax                     ;there are no whole bytes
        mov     ecx,offset opaq_draw_left_edge_only
        jmp     short opaq_set_edge_vector ;yes, all set

@@:
        test    [esi].xLeft,111b           ;is left edge a solid byte?
        jz      short opaq_left_edge_solid ;yes
        dec     eax                        ;no, count off from whole bytes
        mov     ecx,offset opaq_draw_left_edge_only ;assume right edge is solid
        test    [esi].xRight,111b          ;is right edge a solid byte?
        jz      short opaq_set_edge_vector ;yes, all set
        dec     eax                        ;no, count off from whole bytes
        mov     ecx,offset opaq_draw_both_edges ;both edges are non-solid
        jmp     short opaq_set_edge_vector

opaq_left_edge_solid:
        mov     ecx,offset opaq_check_more_banks  ;assume right edge is solid
        test    [esi].xRight,111b          ;is right edge a solid byte?
        jz      short opaq_set_edge_vector ;yes, all set
        dec     eax                        ;no, count off from whole bytes
        mov     ecx,offset opaq_draw_right_edge_only ;no, do non-solid right
                                                     ; edge
opaq_set_edge_vector:
        mov     edi,ulBufDelta
        sub     edi,eax         ;whole bytes offset to next scan in temp buffer
        mov     ulTmpSrcDelta,edi

        mov     edi,ulScreenDelta
        sub     edi,eax         ;whole bytes offset to next scan in screen
        mov     ulTmpDstDelta,edi

        mov     pfnEdgeVector,ecx
        mov     edx,eax
        mov     pfnFirstOpaqVector,offset opaq_whole_bytes
                                        ;assume there are whole bytes, in which
                                        ; case we'll draw them first, then the
                                        ; edge bytes
        sub     edi,edi
        shr     edx,1                   ;whole words
        mov     ulWholeWidthInWords,edx
        adc     edi,edi                 ;odd byte status
        mov     ulOddByte,edi
        dec     edx                     ;whole words - 1
        mov     ulWholeWidthInWordsMinus1,edx
        cmp     eax,0                   ;are there any whole bytes at all?
        jg      short opaq_edges_set    ;yes, we're all set
                                        ;no, set up for edge(s) only
        mov     pfnFirstOpaqVector,ecx  ;the edges are first, because there are
                                        ; no whole bytes

opaq_edges_set:

;-----------------------------------------------------------------------;
; Determine the screen offset of the first destination byte.
;-----------------------------------------------------------------------;

        mov     eax,[esi].yTop
        mov     ulTopScan,eax
        mov     ecx,eax
        mul     ulScreenDelta
        mov     edi,[esi].xLeft
        shr     edi,3
        mov     ulTempLeft,edi  ;remember the offset to the first dest byte
        add     edi,eax

;-----------------------------------------------------------------------;
; Map in the bank containing the top scan of the text, if it's not
; mapped in already.
;-----------------------------------------------------------------------;

        mov     ebx,pdsurf
        cmp     ecx,[ebx].dsurf_rcl1WindowClip.yTop ;is text top less than
                                                    ; current bank?
        jl      short opaq_map_init_bank            ;yes, map in proper bank
        cmp     ecx,[ebx].dsurf_rcl1WindowClip.yBottom ;text top greater than
                                                       ; current bank?
        jl      short opaq_init_bank_mapped     ;no, proper bank already mapped
opaq_map_init_bank:

; Map in the bank containing the top scan line of the fill.
; Preserves EBX, ESI, and EDI.

        ptrCall <dword ptr [ebx].dsurf_pfnBankControl>,<ebx,ecx,JustifyTop>

opaq_init_bank_mapped:

        add     edi,[ebx].dsurf_pvBitmapStart   ;initial destination address
        mov     pScreen,edi

        mov     edx,VGA_BASE + GRAF_ADDR
        mov     al,GRAF_SET_RESET
        out     dx,al                   ;leave the GC Index pointing to
                                        ; set/reset

;-----------------------------------------------------------------------;
; Main loop for processing fill in each bank.
;
; At start of loop and on each loop, EBX->pdsurf and EDI->first destination
; byte.
;-----------------------------------------------------------------------;

opaq_bank_loop:
        mov     edx,ulBottomScan        ;bottom of destination rectangle
        cmp     edx,[ebx].dsurf_rcl1WindowClip.yBottom
                                        ;which comes first, the bottom of the
                                        ; text rect or the bottom of the
                                        ; current bank?
        jl      short @F                ;text bottom comes first, so draw to
                                        ; that; this is the last bank in text
        mov     edx,[ebx].dsurf_rcl1WindowClip.yBottom
                                        ;bank bottom comes first; draw to
                                        ; bottom of bank
@@:
        sub     edx,ulTopScan           ;# of scans to draw in bank
        mov     ulNumScans,edx
        jmp     pfnFirstOpaqVector      ;do first sort of drawing (whole
                                        ; bytes, or edge(s) if no whole
                                        ; bytes)

;-----------------------------------------------------------------------;
; Draws whole bytes, which can be handled by simply loading the latches
; with the background color and using write mode 3 to punch the
; foreground color through.
;
; On entry:
;       EDI = first destination byte
;-----------------------------------------------------------------------;
opaq_whole_bytes:
        mov     esi,pTempBuffer
        test    ulTextLeft,111b         ;is left edge solid?
        jz      short @F                ;yes
        inc     esi                     ;no, point to leftmost source and dest
        inc     edi                     ; bytes (skip over partial left edge)
@@:

;-----------------------------------------------------------------------;
; Load the latches with the background color.
;-----------------------------------------------------------------------;

        mov     edx,VGA_BASE + GRAF_DATA
        mov     eax,iBgColor
        out     dx,al                   ;set/reset color = background

        mov     byte ptr [edi],0ffh     ;we're in write mode 3, so write the
                                        ; set/reset = background color
        mov     al,byte ptr [edi]       ;latch the background color

        mov     eax,iFgColor
        out     dx,al                   ;set/reset color = foreground now

        mov     eax,ulTmpSrcDelta
        mov     ebx,ulTmpDstDelta
        mov     edx,ulNumScans
                                        ;decide which copy loop to use
        test    edi,1                   ;is dest word-aligned?
        jnz     short @F                ;no, need leading byte
                                        ;yes, no leading byte
        cmp     ulOddByte,1             ;odd width?
        jnz     short opaq_scan_loop    ;no, no trailing byte
        jmp     short opaq_scan_loop_t  ;yes, trailing byte

@@:
        cmp     ulOddByte,1             ;odd width?
        jz      short opaq_scan_loop_l  ;yes, no trailing byte
        jmp     short opaq_scan_loop_lt ;no, trailing byte

;-----------------------------------------------------------------------;
; Loops for copying whole bytes to the screen, as much as possible a word
; at a time.
; On entry:
;       EAX = offset to next buffer scan
;       EBX = offset to next screen scan
;       EDX = # of scans to draw
;       ESI = pointer to first buffer byte from which to copy
;       EDI = pointer to first screen byte to which to copy
;       DF  = cleared
; LATER could break out and optimize short runs, such as 1, 2, 3, 4 wide.
;-----------------------------------------------------------------------;

; Loop for doing whole opaque words: no leading byte, no trailing byte.
opaq_scan_loop:
@@:
        mov     ecx,ulWholeWidthInWords
        rep     movsw
        add     esi,eax         ;point to next buffer scan
        add     edi,ebx         ;point to next screen scan
        dec     edx             ;count down scans
        jnz     @B
        jmp     pfnEdgeVector   ;do the edge(s)

; Loop for doing whole opaque words: leading byte, no trailing byte.
opaq_scan_loop_l:
@@:
        movsb
        mov     ecx,ulWholeWidthInWords
        rep     movsw
        add     esi,eax         ;point to next buffer scan
        add     edi,ebx         ;point to next screen scan
        dec     edx             ;count down scans
        jnz     @B
        jmp     pfnEdgeVector   ;do the edge(s)

; Loop for doing whole opaque words: leading byte, trailing byte.
opaq_scan_loop_lt:
@@:
        movsb
        mov     ecx,ulWholeWidthInWordsMinus1   ;one word gets done as 2 bytes
        rep     movsw
        movsb
        add     esi,eax         ;point to next buffer scan
        add     edi,ebx         ;point to next screen scan
        dec     edx             ;count down scans
        jnz     @B
        jmp     pfnEdgeVector   ;do the edge(s)

; Loop for doing whole opaque words: no leading byte, trailing byte.
opaq_scan_loop_t:
@@:
        mov     ecx,ulWholeWidthInWords
        rep     movsw
        movsb
        add     esi,eax         ;point to next buffer scan
        add     edi,ebx         ;point to next screen scan
        dec     edx             ;count down scans
        jnz     @B
        jmp     pfnEdgeVector   ;do the edge(s)

;-----------------------------------------------------------------------;
; Draw the left edge only.
;-----------------------------------------------------------------------;
opaq_draw_left_edge_only:

;-----------------------------------------------------------------------;
; First, draw the foreground pixels.
;-----------------------------------------------------------------------;

        push    offset opaq_check_more_banks ;return here when done with edge

opaq_draw_left_edge_only_entry:
        mov     edx,VGA_BASE + GRAF_DATA
        mov     eax,iFgColor
        out     dx,al                   ;set/reset color = foreground

        mov     ebx,ulBufDelta
        mov     edx,ulScreenDelta
        mov     esi,pTempBuffer
        mov     edi,pScreen
        push    esi             ;remember starting buffer and screen offsets
        push    edi             ; for when we do the background pass
        mov     eax,ulLeftMask  ;AH = clip mask
        push    eax             ;preserve clip mask for background pass
        push    ebp             ;preserve stack frame pointer
        mov     ecx,ulNumScans
        mov     ebp,ecx
        add     ecx,3
        shr     ecx,2   ;# of quadscans
        and     ebp,11b
        jmp     dword ptr OpaqFgEdgeTable[ebp*4]

        align   4
OpaqFgEdgeTable label   dword
        dd      opaq_fg_edge_entry_4
        dd      opaq_fg_edge_entry_1
        dd      opaq_fg_edge_entry_2
        dd      opaq_fg_edge_entry_3

opaq_fg_edge_loop:
opaq_fg_edge_entry_4:
        mov     al,[esi]
        add     esi,ebx
        and     al,ah
        and     [edi],al
        add     edi,edx
opaq_fg_edge_entry_3:
        mov     al,[esi]
        add     esi,ebx
        and     al,ah
        and     [edi],al
        add     edi,edx
opaq_fg_edge_entry_2:
        mov     al,[esi]
        add     esi,ebx
        and     al,ah
        and     [edi],al
        add     edi,edx
opaq_fg_edge_entry_1:
        mov     al,[esi]
        add     esi,ebx
        and     al,ah
        and     [edi],al
        add     edi,edx

        dec     ecx
        jnz     opaq_fg_edge_loop

        pop     ebp             ;restore stack frame pointer

;-----------------------------------------------------------------------;
; Now draw the background pixels with inverted buffer contents.
;-----------------------------------------------------------------------;

        mov     edx,VGA_BASE + GRAF_DATA
        mov     eax,iBgColor
        out     dx,al                   ;set/reset color = background

        pop     eax                     ;retrieve clip mask
        pop     edi                     ;retrieve the initial screen and
        pop     esi                     ; buffer offsets

        mov     edx,ulScreenDelta

        push    ebp                     ;preserve stack frame pointer
        mov     ecx,ulNumScans
        mov     ebp,ecx
        and     ebp,11b
        add     ecx,3
        shr     ecx,2   ;# of quadscans
        jmp     dword ptr OpaqBgEdgeTable[ebp*4]

        align   4
OpaqBgEdgeTable label   dword
        dd      opaq_bg_edge_entry_4
        dd      opaq_bg_edge_entry_1
        dd      opaq_bg_edge_entry_2
        dd      opaq_bg_edge_entry_3

opaq_bg_edge_loop:
opaq_bg_edge_entry_4:
        mov     al,[esi]
        add     esi,ebx
        not     al
        and     al,ah
        and     [edi],al
        add     edi,edx
opaq_bg_edge_entry_3:
        mov     al,[esi]
        add     esi,ebx
        not     al
        and     al,ah
        and     [edi],al
        add     edi,edx
opaq_bg_edge_entry_2:
        mov     al,[esi]
        add     esi,ebx
        not     al
        and     al,ah
        and     [edi],al
        add     edi,edx
opaq_bg_edge_entry_1:
        mov     al,[esi]
        add     esi,ebx
        not     al
        and     al,ah
        and     [edi],al
        add     edi,edx

        dec     ecx
        jnz     opaq_bg_edge_loop

        pop     ebp             ;restore stack frame pointer

        retn

;-----------------------------------------------------------------------;
; Draw the right edge only. Once we've set up the pointers, this is done
; with exactly the same code as the left edge.
;-----------------------------------------------------------------------;
opaq_draw_right_edge_only:
        push    offset opaq_check_more_banks ;return here when done with edge

opaq_draw_right_edge_only_entry:
        mov     edx,VGA_BASE + GRAF_DATA
        mov     eax,iFgColor
        out     dx,al                   ;set/reset color = foreground

        mov     ebx,ulBufDelta
        mov     edx,ulScreenDelta
        mov     edi,ulTextWidthInBytes
        dec     edi
        mov     esi,pTempBuffer
        add     esi,edi         ;point to right edge start in buffer
        add     edi,pScreen     ;point to right edge start in screen
        push    esi             ;remember starting buffer and screen offsets
        push    edi             ; for when we do the background pass
        mov     eax,ulRightMask ;AH = clip mask
        push    eax             ;preserve clip mask for background pass
        push    ebp             ;preserve stack frame pointer
        mov     ecx,ulNumScans
        mov     ebp,ecx
        add     ecx,3
        shr     ecx,2   ;# of quadscans
        and     ebp,11b
        jmp     OpaqFgEdgeTable[ebp*4]

;-----------------------------------------------------------------------;
; Draw both the left and right edges. We do this by calling first the
; left and then the right edge drawing code.
;-----------------------------------------------------------------------;
opaq_draw_both_edges:
        call    opaq_draw_left_edge_only_entry
        call    opaq_draw_right_edge_only_entry

;-----------------------------------------------------------------------;
; See if there are more banks to draw.
;-----------------------------------------------------------------------;

opaq_check_more_banks:
        mov     ebx,pdsurf
        mov     eax,[ebx].dsurf_rcl1WindowClip.yBottom ;is the text bottom in
        cmp     ulBottomScan,eax                       ; the current bank?
        jnle    short do_next_opaq_bank ;no, map in the next bank and draw
                                        ;yes, so we're done

;-----------------------------------------------------------------------;
; Restore the VGA's hardware state and we're done.
;-----------------------------------------------------------------------;

opaq_done:
        mov     edx,VGA_BASE + GRAF_ADDR
        mov     eax,GRAF_MODE + ((M_PROC_WRITE + M_DATA_READ) SHL 8)
        out     dx,ax                   ;restore read mode 0 and write mode 0
done:
        cRet    vFastText

do_next_opaq_bank:
        mov     ulTopScan,eax           ;this will be the top of the next bank
        mov     ecx,eax                 ;set aside scan # for bank manager call
        mul     ulScreenDelta
        mov     edi,ulTempLeft
        add     edi,eax                 ;next screen byte to which to copy

        ptrCall <dword ptr [ebx].dsurf_pfnBankControl>,<ebx,ecx,JustifyTop>
                                        ;map in the bank (call preserves EBX,
                                        ; ESI, and EDI)

        add     edi,[ebx].dsurf_pvBitmapStart   ;initial destination address
        mov     pScreen,edi

        mov     eax,ulBufDelta
        mul     ulNumScans
        add     pTempBuffer,eax         ;advance to next temp buffer scan to
                                        ; copy
        jmp     opaq_bank_loop          ;we're ready to draw in the new bank

;-----------------------------------------------------------------------;
; Special 8-wide aligned opaque drawing code. Loads the latches with the
; background color, sets the Set/Reset color to the foreground color,
; then uses write mode 3 to draw the glyphs. Joyously, there are no
; partial bytes to worry about, so we can really crank up the code.
;
; On entry:
;       EBX = prclText
;-----------------------------------------------------------------------;
special_8_wide_aligned_opaque:

        mov     esi,pdsurf
        mov     edi,[ebx].yBottom
        mov     eax,[ebx].yTop
        sub     edi,eax                 ;height of glyphs

;-----------------------------------------------------------------------;
; Map in the bank containing the top scan of the text, if it's not
; mapped in already.
;-----------------------------------------------------------------------;

        cmp     eax,[esi].dsurf_rcl1WindowClip.yTop ;is text top less than
                                                    ; current bank?
        jl      short s8wao_map_init_bank           ;yes, map in proper bank
        cmp     eax,[esi].dsurf_rcl1WindowClip.yBottom ;text top greater than
                                                       ; current bank?
        jl      short s8wa0_init_bank_mapped   ;no, proper bank already mapped
s8wao_map_init_bank:

; Map in the bank containing the top scan line of the text.
; Preserves EBX, ESI, and EDI.

        ptrCall <dword ptr [esi].dsurf_pfnBankControl>,<esi,eax,JustifyTop>

s8wa0_init_bank_mapped:

        mov     eax,[esi].dsurf_rcl1WindowClip.yBottom
        sub     eax,[ebx].yTop          ;maximum run in bank
        cmp     edi,eax                 ;does all the text fit in the bank?
        jg      general_handler         ;no, let general code handle it
                                        ;LATER could handle here

;-----------------------------------------------------------------------;
; Set up variables.
;-----------------------------------------------------------------------;

        mov     eax,edi                 ;# of scans in glyphs
        add     eax,7
        shr     eax,3                   ;# of unrolled loops needed to draw
                                        ; glyph scans
        mov     ulUnrolledCount,eax     ;# of unrolled loop reps
        and     edi,111b                ;# of odd scans in first unrolled
                                        ; loop
        mov     ulUnrolledOddCount,edi  ;# of odd unrolled loop reps

;-----------------------------------------------------------------------;
; Point to the first screen byte at which to draw.
;-----------------------------------------------------------------------;

        mov     eax,[ebx].yTop
        mul     [esi].dsurf_lNextScan
        mov     edi,[ebx].xLeft
        shr     edi,3
        add     edi,eax                 ;next screen byte to which to copy
        add     edi,[esi].dsurf_pvBitmapStart   ;initial destination address
        mov     pScreen,edi

;-----------------------------------------------------------------------;
; Set up the VGA's hardware for read mode 0 and write mode 3.
;-----------------------------------------------------------------------;

        mov     edx,VGA_BASE + GRAF_ADDR
        mov     eax,GRAF_MODE + ((M_AND_WRITE + M_DATA_READ) SHL 8)
        out     dx,ax                   ;write mode 3 so we can do masking
                                        ; without OUTs

;-----------------------------------------------------------------------;
; Load the latches with the background color.
;-----------------------------------------------------------------------;

        mov     ah,byte ptr iBgColor
        mov     al,GRAF_SET_RESET
        out     dx,ax                   ;set/reset color = background

        mov     byte ptr [edi],0ffh     ;we're in write mode 3, so write the
                                        ; set/reset = background color
        mov     al,byte ptr [edi]       ;latch the background color

        inc     edx                     ;point to GC Data register
        mov     eax,iFgColor
        out     dx,al                   ;set/reset color = foreground now

;-----------------------------------------------------------------------;
; Set up the screen scan offset in EDX, and decide whether we need to do
; a leading glyph or not.
;-----------------------------------------------------------------------;

        mov     edx,[esi].dsurf_lNextScan ;offset from one scan to next

        test    edi,1                   ;is leading glyph address even or odd?
        jz      short s8wa0_word        ;even, so no leading glyph

;-----------------------------------------------------------------------;
; Do the leading glyph.
;-----------------------------------------------------------------------;

        mov     ebx,pGlyphPos           ;point to the first glyph to draw
        add     pGlyphPos,size GLYPHPOS ;point to the next glyph
        inc     pScreen                 ;point to the next glyph's screen
                                        ; location
        dec     ulGlyphCount            ;count off this character
        mov     ecx,ulUnrolledCount     ;# of unrolled loop reps
        mov     esi,[ebx].gp_pgdf       ;point to glyph def
        mov     eax,ulUnrolledOddCount  ;# of odd unrolled loop reps
        mov     esi,[esi].gdf_pgb       ;point to glyph bits
        add     esi,gb_aj               ;point to the first glyph byte
        push    offset s8wa0_word       ;return here to do glyph pairs
        jmp     dword ptr s8wao_byte_table[eax*4] ;branch into unrolled loop

s8wa0_word:
        mov     ecx,ulGlyphCount
        shr     ecx,1                   ;# of words we can copy, now that
                                        ; we're word-aligned
        jz      s8wa0_trailing          ;no words to copy
        mov     eax,ulUnrolledOddCount  ;# of odd unrolled loop reps
        mov     ulWholeWidthInWords,ecx
        mov     eax,dword ptr s8wao_word_table[eax*4] ;entry point into
        mov     pGlyphLoop,eax                        ; unrolled loop

s8wa0_word_loop:
        mov     ebx,pGlyphPos           ;point to the next glyph pair to draw
        add     pGlyphPos,((size GLYPHPOS)*2) ;point to the next glyph pair
        mov     edi,pScreen             ;point to current glyph pair's screen
                                        ; location
        add     pScreen,2               ;point to the next glyph pair's
                                        ; screen location
        mov     esi,[ebx].gp_pgdf       ;point to first glyph def
        mov     ebx,[ebx+(size GLYPHPOS)].gp_pgdf ;point to second glyph def
        mov     esi,[esi].gdf_pgb       ;point to first glyph
        mov     ebx,[ebx].gdf_pgb       ;point to second glyph
        add     esi,gb_aj               ;point to the first glyph's bits
        add     ebx,gb_aj               ;point to the second glyph's bits
        mov     ecx,ulUnrolledCount     ;# of unrolled loop reps
        jmp     pGlyphLoop              ;branch into unrolled loop

        align   4
s8wao_word_table        label   dword
        dd      s8wao_word_8
        dd      s8wao_word_1
        dd      s8wao_word_2
        dd      s8wao_word_3
        dd      s8wao_word_4
        dd      s8wao_word_5
        dd      s8wao_word_6
        dd      s8wao_word_7

s8wao_word_loop:
s8wao_word_8:
        mov     al,[esi]
        inc     esi
        mov     ah,[ebx]
        inc     ebx
        mov     [edi],ax
        add     edi,edx
s8wao_word_7:
        mov     al,[esi]
        inc     esi
        mov     ah,[ebx]
        inc     ebx
        mov     [edi],ax
        add     edi,edx
s8wao_word_6:
        mov     al,[esi]
        inc     esi
        mov     ah,[ebx]
        inc     ebx
        mov     [edi],ax
        add     edi,edx
s8wao_word_5:
        mov     al,[esi]
        inc     esi
        mov     ah,[ebx]
        inc     ebx
        mov     [edi],ax
        add     edi,edx
s8wao_word_4:
        mov     al,[esi]
        inc     esi
        mov     ah,[ebx]
        inc     ebx
        mov     [edi],ax
        add     edi,edx
s8wao_word_3:
        mov     al,[esi]
        inc     esi
        mov     ah,[ebx]
        inc     ebx
        mov     [edi],ax
        add     edi,edx
s8wao_word_2:
        mov     al,[esi]
        inc     esi
        mov     ah,[ebx]
        inc     ebx
        mov     [edi],ax
        add     edi,edx
s8wao_word_1:
        mov     al,[esi]
        inc     esi
        mov     ah,[ebx]
        inc     ebx
        mov     [edi],ax
        add     edi,edx

        dec     ecx                     ;count down glyph scans
        jnz     s8wao_word_loop

        dec     ulWholeWidthInWords     ;count down glyph pairs
        jnz     s8wa0_word_loop

;-----------------------------------------------------------------------;
; Do the trailing character, if there is one.
;-----------------------------------------------------------------------;

s8wa0_trailing:
        test    ulGlyphCount,1          ;trailing byte count
                                        ;is there a trailing character?
        jz      opaq_done               ;no, we're done

        mov     ebx,pGlyphPos           ;point to the last glyph to draw
        mov     edi,pScreen             ;point to the final glyph's screen
                                        ; location
        mov     esi,[ebx].gp_pgdf       ;point to glyph def
        mov     ecx,ulUnrolledCount     ;# of unrolled loop reps
        mov     esi,[esi].gdf_pgb       ;point to glyph bits
        mov     eax,ulUnrolledOddCount  ;# of odd unrolled loop reps
        add     esi,gb_aj               ;point to the first glyph byte
        push    offset opaq_done        ;return here to finish up
        jmp     dword ptr s8wao_byte_table[eax*4] ;branch into unrolled loop

        align   4
s8wao_byte_table        label   dword
        dd      s8wao_byte_8
        dd      s8wao_byte_1
        dd      s8wao_byte_2
        dd      s8wao_byte_3
        dd      s8wao_byte_4
        dd      s8wao_byte_5
        dd      s8wao_byte_6
        dd      s8wao_byte_7

s8wao_byte_loop:
s8wao_byte_8:
        mov     al,[esi]
        inc     esi
        mov     [edi],al
        add     edi,edx
s8wao_byte_7:
        mov     al,[esi]
        inc     esi
        mov     [edi],al
        add     edi,edx
s8wao_byte_6:
        mov     al,[esi]
        inc     esi
        mov     [edi],al
        add     edi,edx
s8wao_byte_5:
        mov     al,[esi]
        inc     esi
        mov     [edi],al
        add     edi,edx
s8wao_byte_4:
        mov     al,[esi]
        inc     esi
        mov     [edi],al
        add     edi,edx
s8wao_byte_3:
        mov     al,[esi]
        inc     esi
        mov     [edi],al
        add     edi,edx
s8wao_byte_2:
        mov     al,[esi]
        inc     esi
        mov     [edi],al
        add     edi,edx
s8wao_byte_1:
        mov     al,[esi]
        inc     esi
        mov     [edi],al
        add     edi,edx

        dec     ecx
        jnz     s8wao_byte_loop

        retn

endProc vFastText

;-----------------------------------------------------------------------;
; VOID vClearMemDword(PULONG * pulBuffer, ULONG ulDwordCount);
;
; Clears ulCount dwords starting at pjBuffer.
;-----------------------------------------------------------------------;

pulBuffer    equ [esp+8]
ulDwordCount equ [esp+12]

cProc vClearMemDword,8,<>

        push    edi
        mov     edi,pulBuffer
        mov     ecx,ulDwordCount
        sub     eax,eax
        rep     stosd
        pop     edi

        cRet  vClearMemDword

endProc vClearMemDword

public draw_f_tb_no_to_temp_start
public draw_nf_tb_no_to_temp_start
public draw_to_temp_start_entry
public draw_f_ntb_o_to_temp_start
public draw_nf_ntb_o_to_temp_start
public draw_to_temp_start_entry2
public draw_f_tb_no_to_temp_loop
public draw_nf_tb_no_to_temp_loop
public draw_to_temp_loop_entry
public draw_f_ntb_o_to_temp_loop
public draw_nf_ntb_o_to_temp_loop
public draw_to_temp_loop_entry2
public or_all_1_wide_rotated_need_last
public or_all_1_wide_rotated_no_last
public or_first_1_wide_rotated_need_last
public or_first_1_wide_rotated_no_last
public or_first_1_wide_rotated_loop
public mov_first_1_wide_rotated_need_last
public mov_first_1_wide_rotated_no_last
public mov_first_1_wide_rotated_loop
public mov_first_1_wide_unrotated
public mov_first_1_wide_unrotated_loop
public or_all_1_wide_unrotated
public or_all_1_wide_unrotated_loop
public or_first_2_wide_rotated_need_last
public or_first_2_wide_rotated_need_loop
public or_all_2_wide_rotated_need_last
public or_all_2_wide_rotated_need_loop
public mov_first_2_wide_rotated_need_last
public mov_first_2_wide_rotated_need_loop
public or_first_2_wide_rotated_no_last
public or_first_2_wide_rotated_loop
public or_all_2_wide_rotated_no_last
public or_all_2_wide_rotated_loop
public mov_first_2_wide_rotated_no_last
public mov_first_2_wide_rotated_loop
public mov_first_2_wide_unrotated
public mov_first_2_wide_unrotated_loop
public or_all_2_wide_unrotated
public or_all_2_wide_unrotated_loop
public or_first_3_wide_rotated_need_last
public or_all_3_wide_rotated_need_last
public mov_first_3_wide_rotated_need_last
public or_first_3_wide_rotated_no_last
public or_all_3_wide_rotated_no_last
public mov_first_3_wide_rotated_no_last
public mov_first_3_wide_unrotated
public or_all_3_wide_unrotated
public or_first_4_wide_rotated_need_last
public or_all_4_wide_rotated_need_last
public mov_first_4_wide_rotated_need_last
public or_first_4_wide_rotated_no_last
public or_all_4_wide_rotated_no_last
public mov_first_4_wide_rotated_no_last
public mov_first_4_wide_unrotated
public or_all_4_wide_unrotated
public or_first_N_wide_rotated_need_last
public or_all_N_wide_rotated_need_last
public mov_first_N_wide_rotated_need_last
public or_first_N_wide_rotated_no_last
public or_all_N_wide_rotated_no_last
public mov_first_N_wide_rotated_no_last
public mov_first_N_wide_unrotated
public odd_width
public two_odd_bytes
public three_odd_bytes
public or_all_N_wide_unrotated
public or_no_odd_bytes_loop
public or_odd_width
public or_one_odd_bytes_loop
public or_two_odd_bytes
public or_two_odd_bytes_loop
public or_three_odd_bytes
public or_three_odd_bytes_loop
public draw_to_screen
public xpar_map_init_bank
public xpar_init_bank_mapped
public xpar_bank_loop
public xpar_scan_loop
public xpar_byte_loop
public xpar_enter_8
public xpar_enter_7
public xpar_enter_6
public xpar_enter_5
public xpar_enter_4
public xpar_enter_3
public xpar_enter_2
public xpar_enter_1
public draw_prop_done
public do_next_xpar_bank
public opaque_text
public opaq_left_edge_solid
public opaq_set_edge_vector
public opaq_edges_set
public opaq_map_init_bank
public opaq_init_bank_mapped
public opaq_bank_loop
public opaq_whole_bytes
public opaq_scan_loop
public opaq_scan_loop_l
public opaq_scan_loop_lt
public opaq_scan_loop_t
public opaq_draw_left_edge_only
public opaq_draw_left_edge_only_entry
public opaq_fg_edge_loop
public opaq_fg_edge_entry_4
public opaq_fg_edge_entry_3
public opaq_fg_edge_entry_2
public opaq_fg_edge_entry_1
public opaq_bg_edge_loop
public opaq_bg_edge_entry_4
public opaq_bg_edge_entry_3
public opaq_bg_edge_entry_2
public opaq_bg_edge_entry_1
public opaq_draw_right_edge_only
public opaq_draw_right_edge_only_entry
public opaq_draw_both_edges
public opaq_check_more_banks
public do_next_opaq_bank
public opaq_done
public special_8_wide_aligned_opaque
public s8wa0_init_bank_mapped
public s8wa0_word
public s8wao_word_loop
public s8wao_word_8
public s8wao_word_7
public s8wao_word_6
public s8wao_word_5
public s8wao_word_4
public s8wao_word_3
public s8wao_word_2
public s8wao_word_1
public s8wa0_trailing
public s8wao_byte_loop
public s8wao_byte_8
public s8wao_byte_7
public s8wao_byte_6
public s8wao_byte_5
public s8wao_byte_4
public s8wao_byte_3
public s8wao_byte_2
public s8wao_byte_1
public s8wao_map_init_bank
public do_opaque_clip
public opaq_fully_clipped

_TEXT$01   ends

        end

