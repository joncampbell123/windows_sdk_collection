;---------------------------Module-Header------------------------------;
; Module Name: strblt.asm
;
; Output a string of glyphs onto VGA screen
;
; Created: Fri 19-Jul-1991 13:06:49
; Author: Viroon Touranachun [viroont]
;
; Copyright (c) 1991 Microsoft Corporation
;-----------------------------------------------------------------------;
        page    ,132
        title   String Display

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
        .list

        include i386\strblt.inc

; characteristic flags for glyphs and strings

VSB_MIX_STRING      equ     00000001h       ; the glyph is mixed
VSB_OPAQUE_BKGND    equ     00000002h       ; the glyph bakground is opaque
VSB_LMB_ZERO        equ     00000004h       ; the left-most bit in left mask is zero
VSB_SAVE_BITS       equ     00000008h       ; save the glyph bits in buffer
VSB_FIRST_GLYPH     equ     00000010h       ; the first glyph

        .code

        EXTRNP      vGlyphBlt,32

;---------------------------Public-Routine------------------------------;
; vStringBlt
;
;   Draw an entire string to the VGA screen
;
; History:
;   Mon 08-Jul-1991 15:13:54 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

ProcName    xxxvStringBlt,vStringBlt,32

        ALIGN 4
xxxvStringBlt   proc uses    esi edi ebx,   \
        pdsurf: ptr DEVSURF,            \
        prcl:   ptr RECTL,              \
        cStr:   DWORD,                  \
        pgp:    ptr GLYPHPOS,           \
        iForeClr: dword,                \
        iBackClr: dword,                \
        ulMode:   dword,                \
        flOption: dword

        local   pjScreen    :ptr        ; screen pointer
        local   pjGlyph     :ptr        ; pointer to glyph bits
        local   pjImage     :ptr        ; pointer to image buffer
        local   flAccel     :dword      ; accelarator flags
        local   pfnFirstByte:dword      ; pointer to the first glyph byte function
        local   pfnLastGlyph:dword      ; pointer to the last glyph byte function
        local   pfnMidGlyph :dword      ; pointer to the middle glyph byte entry
        local   cGlyph      :dword      ; # of glyphs invloved
        local   pjFirstGlyph:dword      ; ptr to the first glyph
        local   cScan       :dword      ; visible glyph height
        local   cScanBlock  :dword      ; visible glyph scan block
        local   cLeadScan   :dword      ; visible glyph leading scan
        local   cOffsetScan :dword      ; offset to the 1st scan
        local   cBufferOffset:dword     ; offset into buffer for leading scans
        local   xStartCell  :dword      ; start of the current glyph cell
        local   xNextCell   :dword      ; start of the next glyph cell
        local   cjBytes     :dword      ; glyph scan size in bytes
        local   cScanCount  :dword      ; temporary scan count
        local   cScreenInc  :dword      ; temporary screen increment
        local   cInnerBytes :dword      ; glyph's middle full byte count
        local   cFixGlyphWidth:dword    ; glyph's middle fixed byte count
        local   cFixScanOffset:dword    ; byte offset into the 1st visible scan
        local   ulNextScan  :dword      ; distance from start of one dest scan
                                        ; to start of next
        local   fjLastMask  :byte       ; glyph's last byte screen mask
        local   cfLastBits  :byte       ; glyph's last byte screen mask bitcount
        local   fjOBOSMask  :byte       ; accumulated screen byte mask
        local   bAlign      :byte       ; dword aligned
; BUGBUG this should should come from the work buffer pointed to by the DSURF,
; or, if it's on the stack, should be allocated to the minimum needed size
        local   abStrImage[CY_SCREEN_MAX]:byte


;----------------------------------------------------------------------------
; Initialize stuff
;----------------------------------------------------------------------------

        cld

; Calculate the screen location


        mov     edi,pdsurf
        mov     esi,prcl
        mov     eax,[esi].yTop
        mov     ecx,[edi].dsurf_lNextScan
        mov     ulNextScan,ecx
        mul     ecx                     ; EAX = byte offset to scan

; Initialize some parameters to zero (EDX = 0 from multiplication above)

        mov     xStartCell,edx          ; byte offset into 1st glyph
        mov     cOffsetScan,edx         ; offset to the 1st glyph scan
        mov     cInnerBytes,edx         ; assume no middle byte
        mov     fjLastMask,dl           ; assume no last byte
        mov     fjOBOSMask,dl           ; Init mask = DL = 0
        lea     ecx,abStrImage
        mov     pjImage,ecx

; Calculate pointer to the 1st screen byte to blt

        mov     ecx,[esi].xLeft
        shr     ecx,3
        add     eax,ecx
        add     eax,[edi].dsurf_pvBitmapStart
        mov     pjScreen,eax            ; Base address of blit

;----------------------------------------------------------------------------
; Get the glyph information
; Look for the 1st glyph and count the number of glyphs displayed
;----------------------------------------------------------------------------

sblt_glyph_search:
        mov     edi,pgp                 ; the first glyph to display
        mov     edx,cStr                ; # of glyphs in string

        .errnz  VGB_HORIZ_CLIPPED_GLYPH-1 ; flag must match those in flClipRect

        test    byte ptr flOption,VGB_HORIZ_CLIPPED_GLYPH
        jz      short sblt_found_both   ; if non-clipped, display entire string

sblt_glyph_search_last:
        mov     ecx,edx                 ; # of glyphs in string
        dec     edx                     ; index to the last glyph in string
        imul    edx,size GLYPHPOS
        add     edi,edx                 ; EDI => last glyph in the string
        mov     eax,[esi].xRight

        ALIGN 4
sblt_glyph_search_last_loop:
        mov     ebx,[edi].gp_pgdf
        mov     ebx,[ebx].gdf_pgb       ; ptr to glyph bits
        mov     ebx,[ebx].gb_x          ; glyph cell offset
        add     ebx,[edi].gp_x          ; actual glyph position
        cmp     ebx,eax
        jl      short sblt_glyph_search_1st ; we found the last glyph
        sub     edi,size GLYPHPOS       ; back one glyph
        loop    sblt_glyph_search_last_loop
        jmp     sblt_exit               ; nothing to show

        ALIGN 4
sblt_glyph_search_1st:
        mov     edx,ecx                 ; mark the last visible glyph
        mov     eax,[esi].xLeft
        jmp     short @F                ; we have not advance to next glyph

        ALIGN 4
sblt_glyph_search_1st_loop:
        mov     ebx,[edi].gp_pgdf
        mov     ebx,[ebx].gdf_pgb       ; ptr to glyph bits
        mov     ebx,[ebx].gb_x          ; glyph cell offset
        add     ebx,[edi].gp_x          ; actual glyph position
@@:
        cmp     ebx,eax
        jle     short sblt_count_glyph  ; we found the first glyph
        sub     edi,size GLYPHPOS       ; back one glyph
        loop    sblt_glyph_search_1st_loop
        jmp     sblt_exit               ; something wrong! bail out!

        ALIGN 4
sblt_count_glyph:
        dec     ecx                     ; also include the current glyph
        sub     edx,ecx                 ; glyph count = last - first

sblt_found_both:
        mov     pjFirstGlyph,edi        ; pointer to the first glyph
        mov     cGlyph,edx              ; # of glyphs to display

;----------------------------------------------------------------------------
; Calculate number of scans to bltting cell
;----------------------------------------------------------------------------

sblt_calc_glyph_height:
        mov     eax,[esi].yBottom       ; bottom of display rectangle
        mov     ecx,[esi].yTop          ; top of display rectangle
        sub     eax,ecx                 ; Number of scans = bottom - top
        mov     cScan,eax

        .errnz  VGB_VERT_CLIPPED_GLYPH-2 ; flags must match those in flClipRect

        test    byte ptr flOption,VGB_VERT_CLIPPED_GLYPH ; if non-clipped,
        jz      short @F                ; we display from the first glyph scan
        mov     ebx,[edi].gp_pgdf
        mov     ebx,[ebx].gdf_pgb       ; ptr to glyph bits
        mov     ebx,[ebx].gb_y          ; glyph cell offset
        add     ebx,[edi].gp_y          ; actual glyph position
        sub     ecx,ebx                 ; Vertical offset into glyph
        mov     cOffsetScan,ecx         ; offset to the 1st glyph scan
@@:
        mov     ecx,eax                 ; EAX = # of scans
        add     ecx,7
        shr     ecx,3                   ; we will transfer 8 scans at a time
         and     eax,07h                 ; so we may have these leading scans
        mov     cLeadScan,eax
        mov     cScanBlock,ecx

        xor     ebx,ebx                 ; because of leading scans, we
        mov     bl,offset FLAT:xgi_buffer_offset[eax]
        mov     cBufferOffset,ebx       ; cache the byte offset to image buffer

;----------------------------------------------------------------------------
; Setup accelerator flags
;----------------------------------------------------------------------------

sblt_accel_flag:
        .errnz  VGB_MIX_STRING
        .errnz  VGB_OPAQUE_STRING-1
        .errnz  VGB_OPAQUE_BKGRND and 0FFFFFF00h
        .errnz  VSB_OPAQUE_BKGND-2
        .errnz  VSB_MIX_STRING-1

        xor     eax,eax                 ; no need for this 'cause EAX and 07 above
        mov     ebx,flOption            ; get the option flag
        test    bl,VGB_OPAQUE_BKGRND    ; get OPAQUE_BKGND flag
        setnz   al                      ; AL = EAX = 1 if opaque background
        shr     ulMode,1                ; get VGB_OPAQUE_STRING flag
        cmc                             ; convert to VSB_MIX_STRING flag
        adc     eax,eax                 ; combine two flags in EAX
        or      al,VSB_FIRST_GLYPH      ; indicate the first glyph
        mov     flAccel,eax

; Since we always have opaque solid color background, if it is a mixed glyph
; we can mix the foreground solid color now and treat it as an opaque glyph

        cmp     al,VSB_FIRST_GLYPH or VSB_OPAQUE_BKGND or VSB_MIX_STRING
        jne     short sblt_special_case
        mov     al,byte ptr iBackClr
        xor     byte ptr iForeClr,al    ; !! We only have XOR !!??
        and     byte ptr flAccel,not VSB_MIX_STRING

;----------------------------------------------------------------------------
; We can treat the following as special cases:
;   1) Non-clipped String with exact-byte glyphs with phase aligned with screen
;   1) Clipped String with exact-byte glyphs with phase aligned with screen
;----------------------------------------------------------------------------

sblt_special_case:
        and     bl,VGB_BYTE_ALIGNED or VGB_MULTIPLE_BYTE or VGB_HORIZ_CLIPPED_GLYPH
        cmp     bl,VGB_BYTE_ALIGNED or VGB_MULTIPLE_BYTE
        jne     short @F
        push    offset FLAT:sblt_exit       ; we have a non-clipped string
        jmp     fixed_pitch_aligned_sblt    ; we handle entire string at once

@@:
        mov     pfnMidGlyph,offset FLAT:sblt_next_glyph_cell
        cmp     bl,VGB_BYTE_ALIGNED or VGB_MULTIPLE_BYTE or VGB_HORIZ_CLIPPED_GLYPH
        jne     short sblt_init_jmp_table
        mov     pfnMidGlyph,offset FLAT:sblt_middle_fpa_glyph

;----------------------------------------------------------------------------
; Not a special case. Initialize some function pointers
;----------------------------------------------------------------------------

sblt_init_jmp_table:
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte_1st_scrn
        mov     pfnLastGlyph,offset FLAT:xgi_last_byte_table
        dec     edx                     ; if only glyph, specially handle it
        jnz     short sblt_1st_glyph_cell

comment~
;----------------------------------------------------------------------------
; We should go right to vGlyphBlt() if there is only one glyph
;----------------------------------------------------------------------------

sblt_call_vglyphblt:

        test    byte ptr flOption,VGB_OPAQUE_BKGRND
        jnz     short @F
        mov     ebx,[edi].gp_pgdf
        mov     ebx,[ebx].gdf_pgb       ; ptr to glyph bits
        mov     ebx,[ebx].gb_y          ; glyph cell offset
        add     [edi].gp_y,ebx          ; actual glyph position

        cCall   vGlyphBlt,<pdsurf,prcl,cStr,edi,iForeClr,iBackClr,ulMode,flOption>

        jmp     sblt_exit
comment~

@@:
        mov     pfnLastGlyph,offset FLAT:xgi_last_byte_last_glyph

; This is bad! If we have only one glyph and it will fall within only one
; screen byte, we will have to treat the visible byte as the last byte of the
; last glyph.

        mov     eax,[esi].xLeft         ; left edge
        shr     eax,3                   ; the screen byte of the left edge
        mov     ecx,[esi].xRight        ; right edge
        shr     ecx,3                   ; the screen byte of the right edge
        cmp     eax,ecx                 ; equal if in the same byte
        jne     short sblt_1st_glyph_cell
        mov     pfnFirstByte,offset FLAT:xgi_last_byte_last_glyph
        and     byte ptr flAccel,not VSB_FIRST_GLYPH

;----------------------------------------------------------------------------
; Let's display glyphs one byte at a time.
;----------------------------------------------------------------------------

; Starting from the first glyph. Find the glyph dimension.

        ALIGN 4
sblt_1st_glyph_cell:
        mov     ecx,[esi].xLeft         ; certainly within 1st glyph
        mov     xNextCell,ecx           ; position of the first glyph cell

        test    byte ptr flOption,VGB_HORIZ_CLIPPED_GLYPH ; if non-clipped,
        jz      sblt_full_glyph_cell          ; it is a full glyph cell

        mov     ebx,[esi].xRight        ; assume we have only one glyph
        mov     esi,[edi].gp_pgdf
        mov     esi,[esi].gdf_pgb       ; ptr to glyph bits
        mov     eax,[edi].gp_x
        add     eax,[esi].gb_x          ; glyph cell offset
        or      edx,edx                 ; EDX = cGlyph-1 => 0 if one glyph
        jz      short @F                ; yes, we have the right edge
        mov     ebx,[esi].gb_cx         ; full glyph cell width
        add     ebx,eax
        mov     xNextCell,ebx           ; position of the next glyph
@@:
        sub     ebx,ecx                 ; EBX = RightEdge-LeftEdge = width in pels

        mov     edx,ecx                 ; ECX = string left edge
        sub     edx,eax                 ; EAX = 1st glyph left edge
        mov     eax,edx                 ; EDX = pel offset into 1st glyph
        shr     edx,3
        mov     xStartCell,edx          ; byte offset into 1st glyph

        and     al,7                    ; AL = bit offset within 1st glyph byte
        sub     al,8
        neg     al
        mov     dl,al                   ; DL = # of bits left in 1st glyph byte
        jmp     short sblt_left_mask

;----------------------------------------------------------------------------
; Update information for the last glyph
;----------------------------------------------------------------------------

        ALIGN 4
sblt_last_glyph_cell:
        mov     ecx,xNextCell           ; start of the last glyph
        mov     edi,pjFirstGlyph
        add     edi,size GLYPHPOS       ; move to the next glyph
        mov     esi,[edi].gp_pgdf
        mov     esi,[esi].gdf_pgb       ; ptr to glyph bits

sblt_last_fpa_glyph_cell:
        mov     dl,8                    ; guarantee full byte from glyph
        mov     ebx,prcl                ; clipped rectangle
        mov     ebx,[ebx].xRight        ; right-trimmed glyph width
        sub     ebx,ecx
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte_last_glyph
        mov     pfnLastGlyph,offset FLAT:xgi_last_byte_last_glyph
        jmp     short sblt_left_mask

;----------------------------------------------------------------------------
; Update information for the middle pfa glyphs
;----------------------------------------------------------------------------

        ALIGN 4
sblt_middle_fpa_glyph:
        mov     edi,pjFirstGlyph
        add     edi,size GLYPHPOS       ; advance to the next glyph
        dec     cGlyph                  ; we will not do the last glyph
        push    cInnerBytes             ; save it because it may change
        call    fixed_pitch_aligned_sblt_inner
        pop     cInnerBytes             ; restore it
        inc     cGlyph                  ; cGlyph = 1 for the last glyph
        add     edi,size GLYPHPOS       ; move to the next glyph
        mov     esi,[edi].gp_pgdf
        mov     esi,[esi].gdf_pgb       ; ptr to glyph bits
        mov     ecx,[edi].gp_x
        add     ecx,[esi].gb_x          ; start of the last glyph
        jmp     short sblt_last_fpa_glyph_cell

;----------------------------------------------------------------------------
; Update information for the next glyph
;----------------------------------------------------------------------------

        ALIGN 4
sblt_next_glyph_cell:
        mov     ecx,xNextCell           ; position of the next glyph cell
        mov     edi,pjFirstGlyph
        add     edi,size GLYPHPOS       ; advance to the next glyph
        mov     pjFirstGlyph,edi

sblt_full_glyph_cell:
        mov     esi,[edi].gp_pgdf
        mov     esi,[esi].gdf_pgb       ; ptr to glyph bits
        mov     dl,8                    ; guarantee full byte from glyph

; For full glyph, we will always start from the left edge.

        mov     ebx,[esi].gb_cx         ; full glyph cell width
        add     xNextCell,ebx           ; advance to the following glyph cell

;----------------------------------------------------------------------------
; Calculate screen bit masks
;----------------------------------------------------------------------------

; Calculate the screen left edge mask

        ALIGN 4
sblt_left_mask:
        and     ecx,7                   ; CH = 0, CL = bits offset into 1st byte
        setnz   al                      ; AL = 1 if left most bit is zero
        neg     al                      ; AL = FF if left most bit is zero
        and     eax,VSB_LMB_ZERO        ; AL = VSB_LMB_ZERO if left most bit is zero
        or      byte ptr flAccel,al

        .errnz  VSB_LMB_ZERO and 0FFFFFF00h

        mov     al,8
        sub     al,cl                   ; AL = bits in mask

        dec     ch                      ; CH = FF
        shr     ch,cl                   ; CH = left mask


; Assume cell is at least one byte wide

        sub     ebx,eax                 ; EBX = glyph width, EAX = bits in left mask
        jg      short sblt_inner_count  ; But is it true?
        je      short sblt_left_bitcount; we know there is only one byte

; It is less than one byte wide, calculate the right edge mask

sblt_one_byte_mask:
        mov     cl,bl
        neg     cl                      ; CL = bits in mask - glyph width
        inc     ah                      ; AH = 1
        shl     ah,cl
        neg     ah                      ; AH = right mask
        and     ch,ah                   ; composite mask = leftmask & rightmask
        cmp     cGlyph,1                ; is it the last glyph?
        je      short sblt_left_bitcount; yes, display it
        or      byte ptr flAccel,VSB_SAVE_BITS ; no, save glyph bits in buffer
        jmp     short sblt_left_bitcount; we know there is only one glyph byte

; Calculate the inner count

        ALIGN 4
sblt_inner_count:
        mov     cl,bl                   ; EBX = # bits left in cell
        shr     ebx,3                   ; EBX = inner bytecount
        mov     cInnerBytes,ebx

; Calculate the last byte mask

sblt_last_mask:
        and     cl,7                    ; CL = # of trailing bits
        mov     cfLastBits,cl
        dec     ah                      ; AH = 0FFh
        shr     ah,cl
        not     ah                      ; AH = trailing byte mask
        mov     fjLastMask,ah

        ALIGN 4
sblt_left_bitcount:
        mov     cl,al                   ; CL = left mask bitcount

;----------------------------------------------------------------------------
; Now get information of the current glyph
;----------------------------------------------------------------------------

; Calculate the glyph scan size

sblt_glyph_info:
        mov     eax,[esi].gb_cx         ; Glyph width in pels
        add     eax,7
        shr     eax,3
        mov     cjBytes,eax             ; bytecount for glyph scan increment

; Assume the glyph is not clipped

        lea     esi,[esi].gb_aj
        mov     pjGlyph,esi             ; Start of glyph
        test    byte ptr flOption,VGB_VERT_CLIPPED_GLYPH or VGB_HORIZ_CLIPPED_GLYPH
        jz      short sblt_xfer_glyph_image

; Calculate byte offset of the 1st scan in glyph

sblt_glyph_offset:
        mov     ebx,cOffsetScan         ; EBX = 1st visible glyph scan
        imul    eax,ebx
        xor     ebx,ebx
        xchg    ebx,xStartCell          ; other glyph always start from 1st bit
        add     eax,ebx                 ; offset to the 1st visible glyph byte
        add     esi,eax                 ; ESI = ptr to 1st visible glyph byte
        mov     pjGlyph,esi

;----------------------------------------------------------------------------
; Transfer the current glyph bits to screen or buffer
;       CH = 1st byte screen mask
;       CL = 1st byte screen mask bitcount
;       DL = 1st glyph byte bitcount
;----------------------------------------------------------------------------

sblt_xfer_glyph_image:
        mov     eax,flAccel             ; Accelarator flag
        and     flAccel,VSB_OPAQUE_BKGND or VSB_MIX_STRING; Prepare for next glyph

        sub     cl,dl                   ; if screen mask bitcount > bitcount
        setg    dl                      ; in 1st glyph byte, we need two bytes
        shr     dl,1                    ; CY =1 if two byte fetch
        adc     eax,eax
        and     cl,7                    ; CL = phase shift
        mov     ebx,pfnFirstByte        ; use the appropriate jump table
        call    [ebx][eax*4]            ; transfer 1st image byte

xgi_inner_bytes:
        mov     edx,cInnerBytes
        or      edx,edx                 ; do we have inner bytes?
        jz      short xgi_last_byte     ; no. go to last byte
        mov     esi,pjGlyph             ; Read glyph from here
        mov     eax,flAccel             ; Accelerator flags
        xor     ch,ch
        sub     ch,cl                   ; CY = 1 if we need another byte
        adc     eax,eax                 ; EAX = offset to jump table
        dec     edx                     ; is there only one middle byte?
        jnz     short xgi_multi_inner_bytes

xgi_single_inner_byte:
        mov     cInnerBytes,edx         ; clear inner bytecount for next glyph
        push    offset FLAT:xgi_last_byte
        jmp     offset FLAT:xgi_middle_byte_table[eax*4]

xgi_multi_inner_bytes:
        inc     edx                     ; restore the bytes count
        mov     edi,pjScreen            ; Always write glyph to screen
        add     pjGlyph,edx             ; advance pointer to last glyph byte
        add     pjScreen,edx            ; advance screen pointer to last column
        sub     cjBytes,edx             ; glyph scan incremental
        mov     ebx,ulNextScan
        sub     ebx,edx                 ; screen scan incremental
        mov     edx,cScan               ; display area height
        mov     cScanCount,edx
        call    offset FLAT:xgi_middle_bytes_table[eax*4]
        xor     eax,eax
        xchg    eax,cInnerBytes         ; clear inner bytecount for next glyph
        add     cjBytes,eax             ; restore its original value

xgi_last_byte:
        mov     ch,fjLastMask           ; CH = last byte mask
        or      ch,ch                   ; do we have the last byte?
        jz      short sblt_xfer_one_glyph; no. then we are done

        mov     esi,pjGlyph             ; Read glyph from here
        mov     eax,flAccel             ; AL = EAX = Accelerator flags, AH = 0
        mov     fjLastMask,ah           ; clear last mask for next glyph
        mov     dh,8
        sub     dh,cl                   ; DH = # bits available in next byte
        sub     dh,cfLastBits           ; CY = 1 if we need another byte
        adc     eax,eax
        mov     ebx,pfnLastGlyph        ; use an appropriate jump table
        call    [ebx][eax*4]            ; transfer the last glyph byte

sblt_xfer_one_glyph:
        dec     cGlyph
        jz      short sblt_exit         ; we are finished
        cmp     cGlyph,1
        je      sblt_last_glyph_cell    ; the last glyph to transfer
        jmp     [pfnMidGlyph]           ; to the right entry point

;----------------------------------------------------------------------------
; Everything is done, go home
;----------------------------------------------------------------------------

sblt_exit:
        cRet    vStringBlt

;---------------------------Private-Routine------------------------------;
; fixed_pitch_aligned_sblt
;
;   Draw string of fixed-pitch glyph with byte width that is byte aligned
;   onto screen.
;
; History:
;   Wed 02-Oct-1991 09:16:15 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
fixed_pitch_aligned_sblt::
        test    byte ptr flAccel,VSB_OPAQUE_BKGND
        jz      short fixed_pitch_aligned_sblt_inner; if we do opaque background
        mov     esi,edi                 ; save ptr to 1st glyph
        call    xgi_set_opaque_bkgnd    ; we need to init VGA registers
        mov     edi,esi                 ; restore ptr to 1st glyph

        ALIGN 4
fixed_pitch_aligned_sblt_inner:
        mov     esi,[edi].gp_pgdf
        mov     esi,[esi].gdf_pgb       ; ptr to glyph bits
        mov     edx,[esi].gb_cx         ; full glyph cell width
        mov     eax,edx
        shr     edx,3                   ; number of bytes it covers
        mov     cFixGlyphWidth,edx

        add     eax,7
        shr     eax,3
        mov     cjBytes,eax             ; bytecount for glyph scan increment
        imul    eax,cOffsetScan
        mov     cFixScanOffset,eax      ; fixed byte offset to 1st visible scan

        mov     eax,flAccel             ; mix modes are here
        and     eax,VSB_OPAQUE_BKGND or VSB_MIX_STRING
        add     eax,eax                 ; we will always do one fetch

        sub     edi,size GLYPHPOS       ; prepare to advance to 1st glyph
        dec     edx                     ; use different routine if one byte
        jnz     short fpa_multi_byte_glyph

fpa_single_byte_glyph:
        mov     eax,offset FLAT:xgi_middle_byte_table[eax*4]
        mov     pfnFirstByte,eax

        ALIGN 4
fpa_next_single_byte_glyph:
        add     edi,size GLYPHPOS       ; move to the next glyph
        mov     esi,[edi].gp_pgdf
        mov     esi,[esi].gdf_pgb       ; ptr to glyph bits
        lea     esi,[esi].gb_aj         ; start of the glyph bits
        add     esi,cFixScanOffset      ; advance to 1st visible byte
        push    edi                     ; save pointer to the current glyph
        call    [pfnFirstByte]          ; display the current glyph
        pop     edi                     ; restore the current glyph ptr
        dec     cGlyph
        jnz     short fpa_next_single_byte_glyph
        jmp     short fpa_exit          ; we are done

        ALIGN 4
fpa_multi_byte_glyph:
        mov     eax,offset FLAT:xgi_middle_bytes_table[eax*4]
        mov     pfnFirstByte,eax
        inc     edx                     ; restore the glyph width
        mov     cInnerBytes,edx
        sub     cjBytes,edx             ; glyph scan incremental
        mov     ebx,ulNextScan
        sub     ebx,edx                 ; screen scan incremental

        ALIGN 4
fpa_next_multi_byte_glyph:
        add     edi,size GLYPHPOS       ; move to the next glyph
        mov     esi,[edi].gp_pgdf
        mov     esi,[esi].gdf_pgb       ; ptr to glyph bits
        lea     esi,[esi].gb_aj         ; start of the glyph bits
        add     esi,cFixScanOffset      ; advance to 1st visible byte
        push    edi                     ; save pointer to the current glyph
        mov     edi,pjScreen            ; Always write glyph to screen
        mov     eax,cFixGlyphWidth      ; glyph width in byte
        add     pjScreen,eax            ; advance screen pointer to last column
        mov     eax,cScan               ; display area height
        mov     cScanCount,eax
        call    [pfnFirstByte]
        pop     edi                     ; restore the current glyph ptr
        dec     cGlyph
        jnz     short fpa_next_multi_byte_glyph

        ALIGN 4
fpa_exit:
        retn

;*************************************************************************
;
;   The following routines transfer the glyph image to reserved memory area.
;
;*************************************************************************

;---------------------------Private-Routine------------------------------;
; xgi1_buffer_overwrite
;
;   Draw a glyph byte onto the reserved memory area.
;   For opaque background, collect the current mask for displaying it later.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_buffer_over_obos::
        mov     fjOBOSMask,ch           ; new mask for new byte

        ALIGN 4
xgi1_buffer_over::
        mov     ebx,cScanBlock          ; Scans to draw
        mov     edi,pjImage             ; we are just saving the bits
        mov     eax,cLeadScan
        mov     edx,cjBytes
        jmp     offset FLAT:xgi1_fob_over[eax*4]; start at the correct location

        ALIGN 4
xgi_fetch_one_byte_over_loop8:
        mov     al,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch

xgi_fetch_one_byte_over_loop7:
        mov     ah,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     ah,cl                   ; Put usable glyph data in place
        and     ah,ch
        shl     eax,16

xgi_fetch_one_byte_over_loop6:
        mov     al,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch

xgi_fetch_one_byte_over_loop5:
        mov     ah,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     ah,cl                   ; Put usable glyph data in place
        and     ah,ch
        rol     eax,16
        stosd

xgi_fetch_one_byte_over_loop4:
        mov     al,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch

xgi_fetch_one_byte_over_loop3:
        mov     ah,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     ah,cl                   ; Put usable glyph data in place
        and     ah,ch
        shl     eax,16

xgi_fetch_one_byte_over_loop2:
        mov     al,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch

xgi_fetch_one_byte_over_loop1:
        mov     ah,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     ah,cl                   ; Put usable glyph data in place
        and     ah,ch
        rol     eax,16
        stosd

        dec     ebx
        jnz     short xgi_fetch_one_byte_over_loop8 ; finish column

xbov1_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi1_buffer_or
;
;   Merge a glyph byte with bits in the reserved memory area.
;   For opaque background, collect the current mask for displaying it later.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_buffer_or_obos::
        or      fjOBOSMask,ch           ; accumulate mask for new byte

        ALIGN 4
xgi1_buffer_or::
        mov     ebx,cScanBlock          ; Scans to draw
        mov     edi,pjImage             ; we are just saving the bits
        mov     eax,cLeadScan
        mov     edx,cjBytes
        jmp     offset FLAT:xgi1_fob_or[eax*4]; start at the correct location

        ALIGN 4
xgi_fetch_one_byte_or_loop8:
        mov     al,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch

xgi_fetch_one_byte_or_loop7:
        mov     ah,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     ah,cl                   ; Put usable glyph data in place
        and     ah,ch
        shl     eax,16

xgi_fetch_one_byte_or_loop6:
        mov     al,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch

xgi_fetch_one_byte_or_loop5:
        mov     ah,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     ah,cl                   ; Put usable glyph data in place
        and     ah,ch
        rol     eax,16
        or      [edi],eax
        add     edi,4                   ; advance buffer pointer

xgi_fetch_one_byte_or_loop4:
        mov     al,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch

xgi_fetch_one_byte_or_loop3:
        mov     ah,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     ah,cl                   ; Put usable glyph data in place
        and     ah,ch
        shl     eax,16

xgi_fetch_one_byte_or_loop2:
        mov     al,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch

xgi_fetch_one_byte_or_loop1:
        mov     ah,[esi]                ; AL = glyph data
        add     esi,edx                 ; Next line of glyph
        rol     ah,cl                   ; Put usable glyph data in place
        and     ah,ch
        rol     eax,16
        or      [edi],eax
        add     edi,4                   ; advance buffer pointer

        dec     ebx
        jnz     short xgi_fetch_one_byte_or_loop8 ; finish column

xbor1_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi2_buffer_over
;
;   Draw two glyph bytes onto the reserved memory area.
;   For opaque background, collect the current mask for displaying it later.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_buffer_over_obos::
        mov     fjOBOSMask,ch           ; new mask for new byte

        ALIGN 4
xgi2_buffer_over::
        mov     ebx,cScanBlock          ; Scans to draw
        mov     edi,pjImage             ; we are just saving the bits
        mov     eax,cLeadScan

        push    ebp                     ; *************************************
        mov     ebp,cjBytes             ; ** No parameter access beyond this **

        jmp     offset FLAT:xgi2_ftb_over[eax*4] ; start at the correct location

        ALIGN 4
xgi_fetch_two_bytes_over_loop8:
        mov     ax,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; AL = glyph data
        and     al,ch

xgi_fetch_two_bytes_over_loop7:
        mov     dx,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     dx,cl                   ; AL = glyph data
        and     dl,ch
        mov     ah,dl
        shl     eax,16

xgi_fetch_two_bytes_over_loop6:
        mov     ax,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; AL = glyph data
        and     al,ch

xgi_fetch_two_bytes_over_loop5:
        mov     dx,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     dx,cl                   ; AL = glyph data
        and     dl,ch
        mov     ah,dl
        rol     eax,16
        stosd

xgi_fetch_two_bytes_over_loop4:
        mov     ax,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; AL = glyph data
        and     al,ch

xgi_fetch_two_bytes_over_loop3:
        mov     dx,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     dx,cl                   ; AL = glyph data
        and     dl,ch
        mov     ah,dl
        shl     eax,16

xgi_fetch_two_bytes_over_loop2:
        mov     ax,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; AL = glyph data
        and     al,ch

xgi_fetch_two_bytes_over_loop1:
        mov     dx,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     dx,cl                   ; AL = glyph data
        and     dl,ch
        mov     ah,dl
        rol     eax,16
        stosd

        dec     ebx
        jnz     short xgi_fetch_two_bytes_over_loop8 ; finish column

        pop     ebp                     ; ** Parameter can be accessed ********
        inc     pjGlyph                 ; always advance glyph ptr for 2 bytes

xbov2_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi2_buffer_or
;
;   Merge two glyph bytes with bits in the reserved memory area.
;   For opaque background, collect the current mask for displaying it later.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_buffer_or_obos::
        or      fjOBOSMask,ch           ; accumulate mask for new byte

        ALIGN 4
xgi2_buffer_or::
        mov     ebx,cScanBlock          ; Scans to draw
        mov     edi,pjImage             ; we are just saving the bits
        mov     eax,cLeadScan

        push    ebp                     ; *************************************
        mov     ebp,cjBytes             ; ** No parameter access beyond this **

        jmp     offset FLAT:xgi2_ftb_or[eax*4] ; start at the correct location

        ALIGN 4
xgi_fetch_two_bytes_or_loop8:
        mov     ax,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; AL = glyph data
        and     al,ch

xgi_fetch_two_bytes_or_loop7:
        mov     dx,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     dx,cl                   ; AL = glyph data
        and     dl,ch
        mov     ah,dl
        shl     eax,16

xgi_fetch_two_bytes_or_loop6:
        mov     ax,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; AL = glyph data
        and     al,ch

xgi_fetch_two_bytes_or_loop5:
        mov     dx,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     dx,cl                   ; AL = glyph data
        and     dl,ch
        mov     ah,dl
        rol     eax,16
        or      [edi],eax
        add     edi,4                   ; Next scan on VGA

xgi_fetch_two_bytes_or_loop4:
        mov     ax,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; AL = glyph data
        and     al,ch

xgi_fetch_two_bytes_or_loop3:
        mov     dx,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     dx,cl                   ; AL = glyph data
        and     dl,ch
        mov     ah,dl
        shl     eax,16

xgi_fetch_two_bytes_or_loop2:
        mov     ax,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; AL = glyph data
        and     al,ch

xgi_fetch_two_bytes_or_loop1:
        mov     dx,[esi]                ; AL = curr byte, AH = next byte
        add     esi,ebp                 ; Next line of glyph
        rol     dx,cl                   ; AL = glyph data
        and     dl,ch
        mov     ah,dl
        rol     eax,16
        or      [edi],eax
        add     edi,4                   ; Next scan on VGA

        dec     ebx
        jnz     short xgi_fetch_two_bytes_or_loop8 ; finish column

        pop     ebp                     ; ** Parameter can be accessed ********
        inc     pjGlyph                 ; always advance glyph ptr for 2 bytes

xbor2_exit:
        retn

;*************************************************************************
;
;   The following routines handle opaque glyph xparent background to screen
;
;*************************************************************************

;---------------------------Private-Routine------------------------------;
; xgi1_scrn_g_xbos
;
;   Draw an opaque glyph byte with xparent background to screen.
;   If it's the 1st screen byte, adjust jump tables.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_scrn_g_xbos_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte

        ALIGN 4
xgi1_scrn_g_xbos::
        push    ecx                     ; preserve phase info
        mov     ebx,cScanBlock          ; Scans to draw
        mov     ecx,ulNextScan          ; scan line width
        mov     edi,pjScreen            ; we are displaying the glyph
        mov     edx,cjBytes
        mov     eax,cLeadScan
        jmp     offset FLAT:xsg1_xbos_dispatch[eax*4]; start at the correct location

        ALIGN 4
xsg1_xbos_loop8:
        mov     al,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ecx                 ; Next line on screen

xsg1_xbos_loop7:
        mov     al,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ecx                 ; Next line on screen

xsg1_xbos_loop6:
        mov     al,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ecx                 ; Next line on screen

xsg1_xbos_loop5:
        mov     al,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ecx                 ; Next line on screen

xsg1_xbos_loop4:
        mov     al,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ecx                 ; Next line on screen

xsg1_xbos_loop3:
        mov     al,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ecx                 ; Next line on screen

xsg1_xbos_loop2:
        mov     al,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ecx                 ; Next line on screen

xsg1_xbos_loop1:
        mov     al,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ecx                 ; Next line on screen

        dec     ebx
        jnz     short xsg1_xbos_loop8   ; finish column

xsg1_xbos_update_ptr:
        inc     pjGlyph                 ; advance glyph pointer
        inc     pjScreen                ; advance display ptr

        pop     ecx                     ; restore phase info

        retn

;---------------------------Private-Routine------------------------------;
; xgi1_scrn_gs_xbos
;
;   Draw opaque glyph bytes with xparent background to screen.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_scrn_gs_xbos::
        mov     edx,cjBytes

        ALIGN 4
xsgs1_xbos_next_scan:
        mov     ecx,cInnerBytes

        ALIGN 4
xsgs1_xbos_loop:
        lodsb                           ; get the glyph byte
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        inc     edi
        loop    xsgs1_xbos_loop

        add     esi,edx                 ; Next line of glyph
        add     edi,ebx                 ; advance screen pointer to next scan
        dec     cScanCount
        jnz     short xsgs1_xbos_next_scan; finish this scan

xsgs1_xbos_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi1_scrn_gm_xbos
;
;   Draw a masked opaque glyph byte with xparent background to screen.
;   For the 1st screen byte, update the jump tables.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_scrn_gm_xbos_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte

        ALIGN 4
xgi1_scrn_gm_xbos::
        mov     ebx,cScan               ; number of scans
        mov     edi,pjScreen            ; we will output to screen
        mov     edx,cjBytes

        ALIGN 4
xsgm1_xbos_loop:
        mov     al,[esi]                ; get the glyph byte
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        jz      short @F
        xchg    [edi],al
@@:
        add     esi,edx                 ; Next line of glyph
        add     edi,ulNextScan          ; advance screen pointer to next scan
        dec     ebx
        jnz     short xsgm1_xbos_loop   ; finish column

xsgm1_xbos_update_ptr:
        sub     bl,cl                   ; if phase aligned, CY = 0
        cmc                             ; if phase aligned, CY = 1
        adc     pjGlyph,0               ; if phase aligned, advance glyph ptr
        inc     pjScreen

xsgm1_xbos_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi1_scrn_gmb_xbos
;
;   Draw an masked opaque glyph byte and bits from the reserved memory area
;   with xparent background to screen. Update jump tables if 1st screen byte.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_scrn_gmb_xbos_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte

        ALIGN 4
xgi1_scrn_gmb_xbos::
        push    ebp                     ; remember stack frame pointer
        mov     eax,cLeadScan           ; First scan to draw
        mov     edx,pjImage             ; access bits saved in buffer
        mov     edi,pjScreen            ; we are displaying the glyph
        mov     ebx,ulNextScan          ; offset to next screen scan
        mov     eax,xsgmb1_xbos_dispatch[eax*4]
        push    eax                     ; start at the correct location
        mov     ah,byte ptr cScanBlock  ; Number of scan blocks to draw
                                        ; ***2K scans max***
        mov     ebp,cjBytes             ; offset to next glyph scan
                                        ; ***can't access stack frame***
        retn                            ;jump into unrolled loop

        ALIGN 4
xsgmb1_xbos_loop8:
        mov     al,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx

xsgmb1_xbos_loop7:
        mov     al,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][1]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx

xsgmb1_xbos_loop6:
        mov     al,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][2]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx

xsgmb1_xbos_loop5:
        mov     al,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][3]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx
        add     edx,4

xsgmb1_xbos_loop4:
        mov     al,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx

xsgmb1_xbos_loop3:
        mov     al,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][1]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx

xsgmb1_xbos_loop2:
        mov     al,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][2]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx

xsgmb1_xbos_loop1:
        mov     al,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][3]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx
        add     edx,4

        dec     ah
        jnz     xsgmb1_xbos_loop8       ; finish column

        pop     ebp                     ;***can access stack frame again***

xsgmb1_xbos_update_ptr:
        inc     pjScreen

xsgmb1_xbos_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi2_scrn_g_xbos
;
;   Draw a glyph to the reserved memory area.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_scrn_g_xbos_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte

        ALIGN 4
xgi2_scrn_g_xbos::
        push    ecx                     ; preserve phase info
        mov     eax,cLeadScan
        mov     ch,byte ptr cScanBlock  ; Number of scan blocks to draw
                                        ; ***2K scans max***
        mov     edi,pjScreen            ; where we are displaying the glyph
        mov     edx,cjBytes
        mov     ebx,ulNextScan          ; offset to next screen scan

        jmp     offset FLAT:xsg2_xbos_dispatch[eax*4]; start at the correct location

        ALIGN 4
xsg2_xbos_loop8:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; next screen scan line

xsg2_xbos_loop7:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; next screen scan line

xsg2_xbos_loop6:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; next screen scan line

xsg2_xbos_loop5:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; next screen scan line

xsg2_xbos_loop4:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; next screen scan line

xsg2_xbos_loop3:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; next screen scan line

xsg2_xbos_loop2:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; next screen scan line

xsg2_xbos_loop1:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; next screen scan line

        dec     ch
        jnz     xsg2_xbos_loop8         ; finish column

xsg2_xbos_update_ptr:
        inc     pjScreen                ; advance screen ptr
        inc     pjGlyph                 ; always advance glyph ptr for 2 bytes

        pop     ecx                     ; restore phase info

        retn

;---------------------------Private-Routine------------------------------;
; xgi2_scrn_gs_xbos
;
;   Draw a glyph to the reserved memory area.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_scrn_gs_xbos::
xsgs2_xbos_next_scan:
        mov     edx,cInnerBytes

        ALIGN 4
xsgs2_xbos_loop:
        mov     ax,[esi]                ; get the glyph byte
        rol     ax,cl
        or      al,al
        jz      short @F
        xchg    [edi],al
@@:
        inc     esi
        inc     edi
        dec     edx
        jnz     short xsgs2_xbos_loop

        add     esi,cjBytes             ; Next line of glyph
        add     edi,ebx                 ; advance screen pointer to next scan
        dec     cScanCount
        jnz     short xsgs2_xbos_next_scan; finish this scan

xsgs2_xbos_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi2_scrn_gm_xbos
;
;   Draw a glyph to the reserved memory area.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_scrn_gm_xbos_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte

        ALIGN 4
xgi2_scrn_gm_xbos::
        mov     ebx,cScan               ; number of scans
        mov     edi,pjScreen            ; we will output to screen
        mov     edx,cjBytes

        ALIGN 4
xsgm2_xbos_loop:
        mov     ax,[esi]                ; get the glyph byte
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        jz      short @F
        xchg    [edi],al
@@:
        add     esi,edx                 ; Next line of glyph
        add     edi,ulNextScan          ; advance screen pointer to next scan
        dec     ebx
        jnz     short xsgm2_xbos_loop   ; finish column

xsgm2_xbos_update_ptr:
        inc     pjScreen                ; advance screen ptr
        inc     pjGlyph                 ; always advance glyph ptr for 2 bytes

xsgm2_xbos_exit:
        retn

;*************************************************************************
;
;   The following routines handle mixed glyph xparent background to screen
;
;*************************************************************************

;---------------------------Private-Routine------------------------------;
; xgi1_scrn_g_xbms
;
;   Draw a mixed glyph byte with xparent background to screen.
;   If it's the 1st screen byte, adjust jump tables.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_scrn_g_xbms_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte

        ALIGN 4
xgi1_scrn_g_xbms::
        push    ecx                     ; preserve phase info
        mov     ebx,cScan               ; number of scans
        mov     edi,pjScreen            ; we will output to screen
        mov     ecx,ulNextScan          ; distance from one screen scan to next

        push    ebp                     ; *************************************
        mov     ebp,cjBytes             ; ** No parameter access beyond this **

        mov     edx,VGA_BASE + GRAF_ADDR ; Leave this in a register
        mov     al,GRAF_BIT_MASK
        out     dx,al                   ; leave GC Index pointing to Bit Mask
        inc     edx                     ; leave DX pointing to GC Data

        ALIGN 4
xsg1_xbms_loop:
        mov     al,[esi]                ; AH = glyph data
        or      al,al
        jz      short @F                ; Don't waste time

        out     dx,al
        xchg    al,[edi]                ; Set the bits

@@:
        add     esi,ebp                 ; Next line of glyph
        add     edi,ecx                 ; advance screen pointer to next scan
        dec     ebx
        jnz     short xsg1_xbms_loop    ; finish column

        pop     ebp                     ; ** Parameter can be accessed ********

xsg1_xbms_update_ptr:
        inc     pjGlyph                 ; advance glyph pointer
        inc     pjScreen                ; advance screen ptr

        pop     ecx                     ; restore phase info

        retn

;---------------------------Private-Routine------------------------------;
; xgi1_scrn_gs_xbms
;
;   Draw mixed glyph bytes with xparent background to screen.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_scrn_gs_xbms::
        mov     dx,VGA_BASE + GRAF_ADDR ; Leave this in a register
        mov     al,GRAF_BIT_MASK        ; leave it in AL

        ALIGN 4
xsgs1_xbms_next_scan:
        mov     ecx,cInnerBytes

        ALIGN 4
xsgs1_xbms_loop:
        mov     ah,[esi]                ; AH = glyph data
        or      ah,ah
        jz      short @F                ; Don't waste time

        out     dx,ax
        xchg    ah,[edi]                ; Set the bits
@@:
        inc     esi                     ; next glyph byte
        inc     edi                     ; next screen byte
        loop    xsgs1_xbms_loop

        add     esi,cjBytes             ; Next line of glyph
        add     edi,ebx                 ; advance screen pointer to next scan
        dec     cScanCount
        jnz     short xsgs1_xbms_next_scan; finish this scan

xsgs1_xbms_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi1_scrn_gm_xbms
;
;   Draw a masked mixed glyph byte with xparent background to screen.
;   For the 1st screen byte, update the jump tables.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_scrn_gm_xbms_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte

        ALIGN 4
xgi1_scrn_gm_xbms::
        mov     ebx,cScan               ; number of scans
        mov     edi,pjScreen            ; we will output to screen

        mov     dx,VGA_BASE + GRAF_ADDR ; Leave this in a register
        mov     al,GRAF_BIT_MASK
        out     dx,al                   ; leave GC Index pointing to Bit Mask
        inc     edx                     ; leave DX pointing to GC Data

        ALIGN 4
xsgm1_xbms_loop:
        mov     al,[esi]                ; AH = glyph data
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        jz      short @F                ; Don't waste time

        out     dx,al
        xchg    al,[edi]                ; Set the bits

@@:
        add     esi,cjBytes             ; Next line of glyph
        add     edi,ulNextScan          ; advance screen pointer to next scan
        dec     ebx
        jnz     short xsgm1_xbms_loop   ; finish column

xsgm1_xbms_update_ptr:
        sub     bl,cl                   ; if phase aligned, CY = 0
        cmc                             ; if phase aligned, CY = 1
        adc     pjGlyph,0               ; if phase aligned, advance glyph ptr
        inc     pjScreen

xsgm1_xbms_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi1_scrn_gmb_xbms
;
;   Draw an masked mixed glyph byte and bits from the reserved memory area
;   with xparent background to screen. Update jump tables if 1st screen byte.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_scrn_gmb_xbms_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte

        ALIGN 4
xgi1_scrn_gmb_xbms::
        mov     eax,cScan               ; number of scans
        mov     cScanCount,eax          ; scans count
        mov     ebx,pjImage             ; don't forget bits we saved
        add     ebx,cBufferOffset
        mov     edi,pjScreen            ; we will output to screen

        mov     dx,VGA_BASE + GRAF_ADDR ; Leave this in a register
        mov     al,GRAF_BIT_MASK
        out     dx,al                   ; leave GC Index pointing to Bit Mask
        inc     edx                     ; leave DX pointing to GC Data

        ALIGN 4
xsgmb1_xbms_loop:
        mov     al,[esi]                ; AH = glyph data
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[ebx]
        jz      short @F                ; Don't waste time

        out     dx,al
        xchg    al,[edi]                ; Set the bits

@@:
        add     esi,cjBytes             ; Next line of glyph
        add     edi,ulNextScan          ; advance screen pointer to next scan
        inc     ebx
        dec     cScanCount
        jnz     short xsgmb1_xbms_loop  ; finish column

xsgmb1_xbms_update_ptr:
        inc     pjScreen

xsgmb1_xbms_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi2_scrn_g_xbms
;
;   Draw a glyph to the reserved memory area.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_scrn_g_xbms_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte

        ALIGN 4
xgi2_scrn_g_xbms::
        mov     ebx,cScan               ; number of scans
        mov     edi,pjScreen            ; we will output to screen

        mov     edx,VGA_BASE + GRAF_ADDR ; Leave this in a register
        mov     al,GRAF_BIT_MASK
        out     dx,al                   ; leave GC Index pointing to Bit Mask
        inc     edx                     ; leave DX pointing to GC Data

        ALIGN 4
xsg2_xbms_loop:
        mov     ax,[esi]                ; AH = glyph data
        rol     ax,cl
        or      al,al
        jz      short @F                ; Don't waste time

        out     dx,al
        xchg    al,[edi]                ; Set the bits

@@:
        add     esi,cjBytes             ; Next line of glyph
        add     edi,ulNextScan          ; advance screen pointer to next scan
        dec     ebx
        jnz     short xsg2_xbms_loop    ; finish column

xsg2_xbms_update_ptr:
        inc     pjScreen                ; advance screen ptr
        inc     pjGlyph                 ; always advance glyph ptr for 2 bytes

xsg2_xbms_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi2_scrn_gs_xbms
;
;   Draw a glyph to the reserved memory area.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_scrn_gs_xbms::
        mov     dx,VGA_BASE + GRAF_ADDR ; Leave this in a register
        add     cl,8                    ; we want the glyph byte in AH
        mov     cScreenInc,ebx

        ALIGN 4
xsgs2_xbms_next_scan:
        mov     ebx,cInnerBytes

        ALIGN 4
xsgs2_xbms_loop:
        mov     ax,[esi]                ; AH = glyph data
        rol     ax,cl
        or      ah,ah
        jz      short @F                ; Don't waste time

        mov     al,GRAF_BIT_MASK        ; leave it in AL
        out     dx,ax
        xchg    ah,[edi]                ; Set the bits
@@:
        inc     esi                     ; next glyph byte
        inc     edi                     ; next screen byte
        dec     ebx
        jnz     short xsgs2_xbms_loop

        add     esi,cjBytes             ; Next line of glyph
        add     edi,cScreenInc          ; advance screen pointer to next scan
        dec     cScanCount
        jnz     short xsgs2_xbms_next_scan; finish this scan
        sub     cl,8                    ; restore phase shift

xsgs2_xbms_exit:
        mov     ebx,cScreenInc
        retn

;---------------------------Private-Routine------------------------------;
; xgi2_scrn_gm_xbms
;
;   Draw a glyph to the reserved memory area.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_scrn_gm_xbms_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte

        ALIGN 4
xgi2_scrn_gm_xbms::
        mov     ebx,cScan               ; number of scans
        mov     edi,pjScreen            ; we will output to screen

        mov     edx,VGA_BASE + GRAF_ADDR ; Leave this in a register
        mov     al,GRAF_BIT_MASK
        out     dx,al                   ; leave GC Index pointing to Bit Mask
        inc     edx                     ; leave DX pointing to GC Data

        ALIGN 4
xsgm2_xbms_loop:
        mov     ax,[esi]                ; AH = glyph data
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        jz      short @F                ; Don't waste time

        out     dx,al
        xchg    al,[edi]                ; Set the bits

@@:
        add     esi,cjBytes             ; Next line of glyph
        add     edi,ulNextScan          ; advance screen pointer to next scan
        dec     ebx
        jnz     short xsgm2_xbms_loop   ; finish column

xsgm2_xbms_update_ptr:
        inc     pjScreen                ; advance screen ptr
        inc     pjGlyph                 ; always advance glyph ptr for 2 bytes

        retn

;*************************************************************************
;
;   The following routines handle opaque glyph opaque background to screen
;
;*************************************************************************

;---------------------------Private-Routine------------------------------;
; xgi1_scrn_g_obos
;
;   Draw an opaque glyph byte with opaque background to screen.
;   If it's the 1st screen byte, adjust jump tables.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_scrn_g_obos_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte
        call    xgi_set_opaque_bkgnd    ; registers were never set

        ALIGN 4
xgi1_scrn_g_obos::
        push    ecx                     ; preserve phase info
        mov     eax,cLeadScan
        mov     ebx,cScanBlock          ; Scans to draw
        mov     ecx,ulNextScan          ; offset from one screen scan to next
        mov     edi,pjScreen            ; we are displaying the glyph
        mov     edx,cjBytes
        jmp     offset FLAT:xsg1_obos_dispatch[eax*4]; start at the correct location

        ALIGN 4
xsg1_obos_loop8:
        mov     al,[esi]
        add     esi,edx                 ; Next line of glyph
        mov     [edi],al
        add     edi,ecx                 ; next screen scan

xsg1_obos_loop7:
        mov     al,[esi]
        add     esi,edx                 ; Next line of glyph
        mov     [edi],al
        add     edi,ecx                 ; next screen scan

xsg1_obos_loop6:
        mov     al,[esi]
        add     esi,edx                 ; Next line of glyph
        mov     [edi],al
        add     edi,ecx                 ; next screen scan

xsg1_obos_loop5:
        mov     al,[esi]
        add     esi,edx                 ; Next line of glyph
        mov     [edi],al
        add     edi,ecx                 ; next screen scan

xsg1_obos_loop4:
        mov     al,[esi]
        add     esi,edx                 ; Next line of glyph
        mov     [edi],al
        add     edi,ecx                 ; next screen scan

xsg1_obos_loop3:
        mov     al,[esi]
        add     esi,edx                 ; Next line of glyph
        mov     [edi],al
        add     edi,ecx                 ; next screen scan

xsg1_obos_loop2:
        mov     al,[esi]
        add     esi,edx                 ; Next line of glyph
        mov     [edi],al
        add     edi,ecx                 ; next screen scan

xsg1_obos_loop1:
        mov     al,[esi]
        add     esi,edx                 ; Next line of glyph
        mov     [edi],al
        add     edi,ecx                 ; next screen scan

        dec     ebx
        jnz     short xsg1_obos_loop8   ; finish column

xsg1_obos_update_ptr:
        inc     pjGlyph                 ; advance glyph pointer
        inc     pjScreen                ; advance the display ptr

        pop     ecx                     ; restore phase info

        retn

;---------------------------Private-Routine------------------------------;
; xgi1_scrn_gs_obos
;
;   Draw opaque glyph bytes with opaque background to screen.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_scrn_gs_obos::
        mov     edx,cjBytes

        ALIGN 4
xsgs1_obos_next_scan:
        mov     ecx,cInnerBytes         ; inner glyph byte
        rep     movsb
        add     esi,edx                 ; Next line of glyph
        add     edi,ebx                 ; Next scan on VGA
        dec     cScanCount
        jnz     short xsgs1_obos_next_scan ; finish column

xsgs1_obos_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi1_scrn_gmb_obos
;
;   Draw an masked opaque glyph byte and bits from the reserved memory area
;   with opaque background to screen.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_scrn_gmb_obos::
        push    ebp                     ; remember stack frame pointer
        mov     eax,cLeadScan
        mov     edx,pjImage             ; along with bits saved in buffer
        mov     edi,pjScreen            ; we are displaying the glyph
        mov     ebx,ulNextScan          ; offset to next screen scan
        mov     eax,xsgmb1_obos_dispatch[eax*4]
        push    eax                     ; start at the correct location
        mov     ah,byte ptr cScanBlock  ; Number of scan blocks to draw
                                        ; ***2K scans max***
        mov     ebp,cjBytes             ; ** No parameter access beyond this **

        retn                            ;jump into unrolled loop

        ALIGN 4
xsgmb1_obos_loop8:
        mov     al,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx]
        mov     [edi],al
        add     edi,ebx

xsgmb1_obos_loop7:
        mov     al,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][1]
        mov     [edi],al
        add     edi,ebx

xsgmb1_obos_loop6:
        mov     al,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][2]
        mov     [edi],al
        add     edi,ebx

xsgmb1_obos_loop5:
        mov     al,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][3]
        mov     [edi],al
        add     edi,ebx
        add     edx,4

xsgmb1_obos_loop4:
        mov     al,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx]
        mov     [edi],al
        add     edi,ebx

xsgmb1_obos_loop3:
        mov     al,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][1]
        mov     [edi],al
        add     edi,ebx

xsgmb1_obos_loop2:
        mov     al,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][2]
        mov     [edi],al
        add     edi,ebx

xsgmb1_obos_loop1:
        mov     al,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][3]
        mov     [edi],al
        add     edi,ebx
        add     edx,4

        dec     ah
        jnz     xsgmb1_obos_loop8       ; finish column

        pop     ebp                     ; ** Parameter can be accessed ********

xsgmb1_obos_update_ptr:
        inc     pjScreen

xsgmb1_obos_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi2_scrn_g_obos
;
;   Draw an opaque glyph byte (2-fetch) with opaque background to screen.
;   If it's the 1st screen byte, adjust jump tables.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_scrn_g_obos_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte
        call    xgi_set_opaque_bkgnd

        ALIGN 4
xgi2_scrn_g_obos::
        push    ebp                     ; remember stack frame pointer
        mov     eax,cLeadScan
        mov     ebx,cScanBlock          ; Scans to draw
        mov     edi,pjScreen            ; we are displaying the glyph
        mov     edx,cjBytes             ; glyph distance to next scan
        mov     ebp,ulNextScan          ; distance from one scan to next
                                        ; ***stack frame unavailable***
        jmp     offset FLAT:xsg2_obos_dispatch[eax*4]; start at the correct location

        ALIGN 4
xsg2_obos_loop8:
        mov     ax,[esi]
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        mov     [edi],al
        add     edi,ebp

xsg2_obos_loop7:
        mov     ax,[esi]
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        mov     [edi],al
        add     edi,ebp

xsg2_obos_loop6:
        mov     ax,[esi]
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        mov     [edi],al
        add     edi,ebp

xsg2_obos_loop5:
        mov     ax,[esi]
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        mov     [edi],al
        add     edi,ebp

xsg2_obos_loop4:
        mov     ax,[esi]
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        mov     [edi],al
        add     edi,ebp

xsg2_obos_loop3:
        mov     ax,[esi]
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        mov     [edi],al
        add     edi,ebp

xsg2_obos_loop2:
        mov     ax,[esi]
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        mov     [edi],al
        add     edi,ebp

xsg2_obos_loop1:
        mov     ax,[esi]
        add     esi,edx                 ; Next line of glyph
        rol     ax,cl
        mov     [edi],al
        add     edi,ebp

        dec     ebx
        jnz     short xsg2_obos_loop8   ; finish column

        pop     ebp                     ; restore stack frame pointer
                                        ; ***stack frame available***
xsg2_obos_update_ptr:
        inc     pjScreen                ; advance screen ptr
        inc     pjGlyph                 ; always advance glyph ptr for 2 bytes

        retn

;---------------------------Private-Routine------------------------------;
; xgi2_scrn_gs_obos
;
;   Draw multiple opaque glyph bytes (2-fetch) with opaque background to
;   screen. If it's the 1st screen byte, adjust jump tables.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_scrn_gs_obos::
xsgs2_obos_next_scan:
        mov     edx,cInnerBytes         ; inner glyph byte

        ALIGN 4
xsgs2_obos_loop:
        mov     ax,[esi]
        rol     ax,cl
        stosb
        inc     esi
        dec     edx
        jnz     short xsgs2_obos_loop

        add     esi,cjBytes             ; Next line of glyph
        add     edi,ebx                 ; Next scan on VGA
        dec     cScanCount
        jnz     short xsgs2_obos_next_scan ; finish column

xsgs2_obos_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi1_scrn_gm_obos
;
;   Draw a masked opaque glyph byte with opaque background to
;   screen. If it's the 1st screen byte, adjust jump tables.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_scrn_gm_obos_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte
        call    xgi1_scrn_gm_obos
        jmp     xgi_set_opaque_bkgnd_test_full_byte

        ALIGN 4
xgi1_scrn_gm_obos_lg::
        test    ch,1                    ; if there is a right bitmask
        jz      short xgi1_scrn_gm_obos ; set up VGA regs
        inc     cInnerBytes             ; otherwise, include it in the middle
        retn

        ALIGN 4
xgi1_scrn_gm_obos::

; Setup bit mask and stuff.

        call    xgi_setup_opaque_bkgnd_edge

; The following AND leaves 1 bits in AH for the planes which
; CANNOT be done on the second pass.  So if this is zero we can
; skip the first pass.

        and     ah,bl                   ; BL = BkColor = color where data is 1
        or      ah,bl                   ; planes to enable
        jz      short xsgm1_obos_skip_first_pass
        or      al,ah                   ; Include "Set/Reset" planes.
        out     dx,al                   ; Enable planes for first pass.

        push    eax
        mov     ebx,cScan

        ALIGN 4
xsgm1_obos_first_pass:
        mov     al,[esi]
        rol     al,cl                   ; Put usable glyph data in place
        xchg    al,[edi]
        add     esi,cjBytes
        add     edi,ulNextScan
        dec     ebx
        jnz     short xsgm1_obos_first_pass

        pop     eax
        mov     esi,pjGlyph             ; restore the glyph pointer
        mov     edi,pjScreen            ; restore the screen pointer

        ALIGN 4
xsgm1_obos_skip_first_pass:

; Enable the other planes.

        not     ah
        and     ah,MM_ALL
        jz      short xsgm1_obos_no_planes_left
        mov     al,ah
        out     dx,al
        mov     ebx,cScan

        ALIGN 4
xsgm1_obos_second_pass:
        mov     al,[esi]
        rol     al,cl                   ; Put usable glyph data in place
        not     al
        xchg    al,[edi]
        add     esi,cjBytes
        add     edi,ulNextScan
        dec     ebx
        jnz     short xsgm1_obos_second_pass

        ALIGN 4
xsgm1_obos_no_planes_left:
        mov     al,MM_ALL
        out     dx,al

xsgm1_obos_exit:
        sub     bl,cl                   ; if phase aligned, CY = 0
        cmc                             ; if phase aligned, CY = 1
        adc     pjGlyph,0               ; if phase aligned, advance glyph ptr
        inc     pjScreen                ; advance the display ptr
        retn                            ; no need to restore anything

;---------------------------Private-Routine------------------------------;
; xgi2_scrn_gm_obos
;
;   Draw a masked opaque glyph bytes (2-fetch) with opaque background to
;   screen. If it's the 1st screen byte, adjust jump tables.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_scrn_gm_obos_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte
        call    xgi2_scrn_gm_obos
        jmp     xgi_set_opaque_bkgnd_test_full_byte

        ALIGN 4
xgi2_scrn_gm_obos_lg::
        test    ch,1                    ; if there is a right bitmask
        jz      short xgi2_scrn_gm_obos ; set up VGA regs
        inc     cInnerBytes             ; otherwise, include it in the middle
        retn

        ALIGN 4
xgi2_scrn_gm_obos::

; Setup bit mask and stuff.

        call    xgi_setup_opaque_bkgnd_edge

; The following AND leaves 1 bits in AH for the planes which
; CANNOT be done on the second pass.  So if this is zero we can
; skip the first pass.

        and     ah,bl                   ; BL = BkColor = color where data is 1
        or      ah,bl                   ; planes to enable
        jz      short xsgm2_obos_skip_first_pass
        or      al,ah                   ; Include "Set/Reset" planes.
        out     dx,al                   ; Enable planes for first pass.

        push    eax
        mov     ebx,cScan

        ALIGN 4
xsgm2_obos_first_pass:
        mov     ax,[esi]
        rol     ax,cl                   ; Put usable glyph data in place
        xchg    al,[edi]
        add     esi,cjBytes
        add     edi,ulNextScan
        dec     ebx
        jnz     short xsgm2_obos_first_pass

        pop     eax
        mov     esi,pjGlyph             ; restore the glyph pointer
        mov     edi,pjScreen            ; restore the screen pointer

        ALIGN 4
xsgm2_obos_skip_first_pass:

; Enable the other planes.

        not     ah
        and     ah,MM_ALL
        jz      short xsgm2_obos_no_planes_left
        mov     al,ah
        out     dx,al
        mov     ebx,cScan

        ALIGN 4
xsgm2_obos_second_pass:
        mov     ax,[esi]
        rol     ax,cl                   ; Put usable glyph data in place
        not     al
        xchg    al,[edi]
        add     esi,cjBytes
        add     edi,ulNextScan
        dec     ebx
        jnz     short xsgm2_obos_second_pass

        ALIGN 4
xsgm2_obos_no_planes_left:
        mov     al,MM_ALL
        out     dx,al

xsgm2_obos_exit:
        inc     pjScreen                ; advance the display ptr
        inc     pjGlyph                 ; always advance glyph ptr for 2 bytes
        retn                            ; no need to restore anything


;---------------------------Private-Routine------------------------------;
; xgi1_scrn_gmb_obos_1st_scrn
;
;   Draw a masked opaque glyph byte and bits in reserved buffer with
;   opaque background to the 1st screen byte.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_scrn_gmb_obos_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte

; Here we need to setup the bitmask including those in buffer. It can be
; calculated here from the x starting point.

        ALIGN 4
xgmb1_obos_1scrn_bitmask:
        xor     bh,bh                   ; we use and clear
        xchg    bh,fjOBOSMask           ; the acculmulated mask
        or      bh,ch                   ; also include the current mask
        jns     short xsgmb1_call_obos_edge  ; there is really a mask
        call    xgi_set_opaque_bkgnd    ; no mask, treat it as a full byte
        jmp     xgi1_scrn_gmb_obos

        ALIGN 4
xsgmb1_call_obos_edge:
        call    xsgmb1_obos_edge
        jmp     xgi_set_opaque_bkgnd_test_full_byte

;---------------------------Private-Routine------------------------------;
; xgi1_scrn_gmb_obos_last_glyph
;
;   Draw a masked opaque byte from the last glyph and bits in reserved
;   buffer with opaque background to the screen.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi1_scrn_gmb_obos_lg::
        xor     bh,bh
        xchg    bh,fjOBOSMask           ; use and clear the accumulated mask
        or      bh,ch                   ; plus the current mask
        inc     bh                      ; if mask is all 1s
        jnz     short @F
        call    xgi_set_opaque_bkgnd    ; treat it as a full byte
        jmp     xgi1_scrn_gmb_obos
@@:
        dec     bh
        call    xsgmb1_obos_edge        ; draw this first byte
        cmp     cInnerBytes,0           ; last glyph has full middle bytes?
        jz      short @F                ; no, then get out
        call    xgi_set_opaque_bkgnd    ; set the full byte
@@:
        retn

;---------------------------Private-Routine------------------------------;
; xgi1_scrn_gmb_obos_edge
;
;   Draw a masked opaque glyph byte and bits in reserved
;   buffer with opaque background to the screen.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xsgmb1_obos_edge:
        xchg    ch,bh                   ; set with the accumulated mask
        call    xgi_setup_opaque_bkgnd_edge
        xchg    ch,bh                   ; restore the current byte mask

; The following AND leaves 1 bits in AH for the planes which
; CANNOT be done on the second pass.  So if this is zero we can
; skip the first pass.

        and     ah,bl                   ; BL = BkColor = color where data is 1
        or      ah,bl                   ; planes to enable
        jz      short xsgmbe1_obos_skip_first_pass
        or      al,ah                   ; Include "Set/Reset" planes.
        out     dx,al                   ; Enable planes for first pass.

        push    eax
        mov     ebx,cScan
        mov     edx,pjImage
        add     edx,cBufferOffset

        ALIGN 4
xsgmbe1_obos_first_pass:
        mov     al,[esi]
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx]
        xchg    al,[edi]
        add     esi,cjBytes
        add     edi,ulNextScan
        inc     edx
        dec     ebx
        jnz     short xsgmbe1_obos_first_pass

        pop     eax
        mov     esi,pjGlyph             ; restore the glyph pointer
        mov     edi,pjScreen            ; restore the screen pointer
        mov     dx,VGA_BASE + SEQ_DATA  ; The rest of the OUTs are here.

        ALIGN 4
xsgmbe1_obos_skip_first_pass:

; Enable the other planes.

        not     ah
        and     ah,MM_ALL
        jz      short xsgmbe1_obos_no_planes_left
        mov     al,ah
        out     dx,al
        mov     ebx,cScan
        mov     edx,pjImage
        add     edx,cBufferOffset

        ALIGN 4
xsgmbe1_obos_second_pass:
        mov     al,[esi]
        rol     al,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx]
        not     al
        xchg    al,[edi]
        add     esi,cjBytes
        add     edi,ulNextScan
        inc     edx
        dec     ebx
        jnz     short xsgmbe1_obos_second_pass
        mov     dx,VGA_BASE + SEQ_DATA  ; The rest of the OUTs are here.

        ALIGN 4
xsgmbe1_obos_no_planes_left:
        mov     al,MM_ALL
        out     dx,al

xsgmbe1_obos_exit:
        sub     bl,cl                   ; if phase aligned, CY = 0
        cmc                             ; if phase aligned, CY = 1
        adc     pjGlyph,0               ; if phase aligned, advance glyph ptr
        inc     pjScreen                ; advance the display ptr
        retn

;---------------------------Private-Routine------------------------------;
; xgi_set_opaque_bkgnd_edge
;
;   Set the VGA registers for drawing a masked screen byte with opaque
;   glyph and opaque background.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi_setup_opaque_bkgnd_edge::

        mov     edi,pjScreen            ; screen pointer
        mov     dx,VGA_BASE + GRAF_ADDR
        mov     ax,GRAF_DATA_ROT + 256 * DR_SET
        out     dx,ax
        mov     ah,ch                   ; put mask in AH
        mov     al,GRAF_BIT_MASK
        out     dx,ax

; Put foreground color in Set/Reset and enable planes where colors
; match.

        mov     ah,byte ptr (iBackClr)
        mov     bl,byte ptr (iForeClr)
        mov     al,GRAF_SET_RESET
        out     dx,ax
        xor     ah,bl
        not     ah                      ; gives 1 where colors match
        mov     al,GRAF_ENAB_SR
        out     dx,ax
        mov     dx,VGA_BASE + SEQ_DATA  ; The rest of the OUTs are here.
        mov     al,ah
        not     ah                      ; Gives 1 where colors mismatch.
        retn

;---------------------------Private-Routine------------------------------;
; xgi_set_opaque_bkgnd
;
;   Set the VGA registers for drawing a full screen byte with opaque
;   glyph and opaque background.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi_set_opaque_bkgnd_test_full_byte::
        mov     edi,prcl
        mov     edx,[edi].xLeft
        mov     eax,[edi].xRight
        sub     eax,edx                 ; EAX = display area width
        neg     edx
        and     edx,7                   ; EDX = bits in left partial byte
        sub     eax,edx
        shr     eax,3                   ; EAX = inner full bytes
        jnz     short xgi_set_opaque_bkgnd
        retn                            ; there'll be no full byte

        ALIGN 4
xgi_set_opaque_bkgnd::

        mov     dx,VGA_BASE + GRAF_ADDR
        mov     ax,(DR_SET shl 8) + GRAF_DATA_ROT
        out     dx,ax

; First we put the foreground color into the latches.  We do this
; by putting this color into SET_RESET, writing it, then reading it.
; The memory location we will use is the first byte where we will blt.

        mov     ah,byte ptr iBackClr    ; for appropriate color
        mov     al,GRAF_SET_RESET
        out     dx,ax
        mov     ax,0F00h + GRAF_ENAB_SR
        out     dx,ax

; Set bit mask = FF.
        mov     ax,0FF00h + GRAF_BIT_MASK
        out     dx,ax

; Fill the latches.
        mov     edi,pjScreen            ; we will overwrite this byte anyway
        mov     [edi],al                ; color in SetReset is written, not AL
        mov     al,[edi]                ; read to fill latches

; Go to XOR mode.
        mov     ax,GRAF_DATA_ROT + 256 * DR_XOR
        out     dx,ax

; Now setup SET_RESET.

        mov     ah,byte ptr iBackClr    ; for appropriate color
        xor     ah,byte ptr iForeClr    ; gives 0 where colors match
        mov     al,GRAF_SET_RESET
        out     dx,ax
        not     ah
        mov     al,GRAF_ENAB_SR
        out     dx,ax                   ; enable Set/Reset where colors match

sob_exit:
        retn

IFNDEF  GMB_2_FETCH_POSSIBLE

;*****************************************************************************
;
;   We claim that when we are displaying a glyph image plus bits saved in the
;   memory buffer, we will never need to fetch 2 bytes from the current glyph
;   because we will never need more than 8 bits from the current glyph.
;
;*****************************************************************************

xgi2_scrn_gmb::

if      DBG
        int     3
endif;  DBG

        retn

ELSE    ;GMB_2_FETCH_POSSIBLE

;---------------------------Private-Routine------------------------------;
; xgi2_scrn_gmb_xbos
;
;   Draw a glyph to the reserved memory area.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_scrn_gmb_xbos_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte

        ALIGN 4
xgi2_scrn_gmb_xbos::
        mov     eax,cLeadScan
        mov     edx,pjImage             ; access bits saved in buffer
        mov     edi,pjScreen            ; we will output to screen
        mov     eax,xsgmb2_xbos_dispatch[eax*4] ; start at the correct location
        push    eax
        mov     eax,cScanBlock          ; Number of scan blocks to draw
        shl     eax,16                  ; put count in high word of EAX
                                        ; ***64K scan lines max***
        mov     ebx,ulNextScan          ; distance from one screen scan to next
        push    ebp                     ; remember stack frame pointer
        mov     ebp,cjBytes             ; distance to next glyph scan
                                        ; ***stack frame not available***
        retn                            ; branch into unrolled loop

        ALIGN 4
xsgmb2_xbos_loop8:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; point to next screen scan

xsgmb2_xbos_loop7:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][1]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; point to next screen scan

xsgmb2_xbos_loop6:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][2]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; point to next screen scan

xsgmb2_xbos_loop5:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][3]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; point to next screen scan
        add     edx,4

xsgmb2_xbos_loop4:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; point to next screen scan

xsgmb2_xbos_loop3:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][1]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; point to next screen scan

xsgmb2_xbos_loop2:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][2]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; point to next screen scan

xsgmb2_xbos_loop1:
        mov     ax,[esi]                ; get the glyph byte
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][3]
        jz      short @F
        xchg    [edi],al
@@:
        add     edi,ebx                 ; point to next screen scan
        add     edx,4

        and     eax,0ffff0000h
        sub     eax,000010000h
        jnz     short xsgmb2_xbos_loop8 ; finish column

        pop     ebp                     ; ***stack frame available***

xsgmb2_xbos_update_ptr:
        inc     pjScreen                ; advance screen ptr
        inc     pjGlyph                 ; always advance glyph ptr for 2 bytes

        retn


;---------------------------Private-Routine------------------------------;
; xgi2_scrn_gmb_obos
;
;   Draw a masked opaque glyph bytes (2-fetch) and bits from reserved
;   memory area with opaque background to screen.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_scrn_gmb_obos::
        mov     eax,cLeadScan
        mov     edx,pjImage             ; along with bits saved in buffer
        mov     edi,pjScreen            ; we are displaying the glyph
        mov     eax,xsgmb2_obos_dispatch[eax*4]; start at the correct location
        push    eax
        mov     eax,cScanBlock          ; Number of scan blocks to draw
        shl     eax,16                  ; put count in high word of EAX
                                        ; ***64K scan lines max***
        mov     ebx,ulNextScan          ; distance from one screen scan to next
        push    ebp                     ; remember stack frame pointer
        mov     ebp,cjBytes             ; distance to next glyph scan
                                        ; ***stack frame not available***
        retn                            ; branch into unrolled loop

        ALIGN 4
xsgmb2_obos_loop8:
        mov     ax,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx]
        mov     [edi],al
        add     edi,ebx                 ; point to next screen scan

xsgmb2_obos_loop7:
        mov     ax,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][1]
        mov     [edi],al
        add     edi,ebx                 ; point to next screen scan

xsgmb2_obos_loop6:
        mov     ax,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][2]
        mov     [edi],al
        add     edi,ebx                 ; point to next screen scan

xsgmb2_obos_loop5:
        mov     ax,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][3]
        mov     [edi],al
        add     edi,ebx                 ; point to next screen scan
        add     edx,4

xsgmb2_obos_loop4:
        mov     ax,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx]
        mov     [edi],al
        add     edi,ebx                 ; point to next screen scan

xsgmb2_obos_loop3:
        mov     ax,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][1]
        mov     [edi],al
        add     edi,ebx                 ; point to next screen scan

xsgmb2_obos_loop2:
        mov     ax,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][2]
        mov     [edi],al
        add     edi,ebx                 ; point to next screen scan

xsgmb2_obos_loop1:
        mov     ax,[esi]
        add     esi,ebp                 ; Next line of glyph
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx][3]
        mov     [edi],al
        add     edi,ebx                 ; point to next screen scan
        add     edx,4

        and     eax,0ffff0000h
        sub     eax,000010000h
        jnz     short xsgmb2_obos_loop8 ; finish column

        pop     ebp                     ; ***stack frame available***

xsgmb2_obos_update_ptr:
        inc     pjScreen                ; advance screen ptr
        inc     pjGlyph                 ; always advance glyph ptr for 2 bytes

xsgmb2_obos_exit:
        retn

;---------------------------Private-Routine------------------------------;
; xgi2_scrn_gmb_obos_1st_scrn
;
;   Draw a masked opaque glyph byte (2-fetch) and bits in reserved buffer
;   with opaque background to the 1st screen byte.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_scrn_gmb_obos_1scrn::
        mov     pfnFirstByte,offset FLAT:xgi_1st_byte

; Here we need to setup the bitmask including those in buffer. It can be
; calculated here from the x starting point.

        ALIGN 4
xgmb2_obos_1scrn_bitmask:
        xor     bh,bh                   ; we use and clear
        xchg    bh,fjOBOSMask           ; the acculmulated mask
        or      bh,ch                   ; also include the current mask
        jns     short xsgmb2_call_obos_edge  ; there is really a mask
        call    xgi_set_opaque_bkgnd    ; no mask, treat it as a full byte
        jmp     xgi2_scrn_gmb_obos

xsgmb2_call_obos_edge:
        call    xsgmb2_obos_edge
        jmp     xgi_set_opaque_bkgnd_test_full_byte

;---------------------------Private-Routine------------------------------;
; xgi2_scrn_gmb_obos_last_glyph
;
;   Draw a masked opaque byte from the last glyph (2-fetch) and bits in
;   reserved buffer with opaque background to the screen.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xgi2_scrn_gmb_obos_lg::
        xor     bh,bh
        xchg    bh,fjOBOSMask           ; use and clear the accumulated mask
        or      bh,ch                   ; plus the current mask
        inc     bh                      ; if mask is all 1s
        jnz     short @F
        call    xgi_set_opaque_bkgnd    ; treat it as a full byte
        jmp     xgi2_scrn_gmb_obos
@@:
        dec     bh
        call    xsgmb2_obos_edge        ; draw this first byte
        cmp     cInnerBytes,0           ; last glyph has full middle bytes?
        jz      short @F                ; no, then get out
        call    xgi_set_opaque_bkgnd    ; set the full byte
@@:
        retn

;---------------------------Private-Routine------------------------------;
; xgi2_scrn_gmb_obos_edge
;
;   Draw a masked opaque byte from the glyph (2-fetch) and bits in
;   reserved buffer with opaque background to the screen.
;
; History:
;   Thu 18-Jul-1991 13:10:27 -by- Viroon Touranachun [viroont]
;   Created.
;-----------------------------------------------------------------------;

        ALIGN 4
xsgmb2_obos_edge:
        xchg    bh,ch                   ; set with the accumulated mask
        call    xgi_setup_opaque_bkgnd_edge
        xchg    bh,ch                   ; restore the current byte mask

; The following AND leaves 1 bits in AH for the planes which
; CANNOT be done on the second pass.  So if this is zero we can
; skip the first pass.

        and     ah,bl                   ; BL = BkColor = color where data is 1
        or      ah,bl                   ; planes to enable
        jz      xsgmbe2_obos_skip_first_pass
        or      al,ah                   ; Include "Set/Reset" planes.
        out     dx,al                   ; Enable planes for first pass.

        push    eax
        mov     ebx,cScan
        mov     edx,pjImage
        add     edx,cBufferOffset

        ALIGN 4
xsgmbe2_obos_first_pass:
        mov     ax,[esi]
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx]
        xchg    al,[edi]
        add     esi,cjBytes
        add     edi,ulNextScan
        inc     edx
        dec     ebx
        jnz     short xsgmbe2_obos_first_pass

        pop     eax
        mov     esi,pjGlyph             ; restore the glyph pointer
        mov     edi,pjScreen            ; restore the screen pointer
        mov     dx,VGA_BASE + SEQ_DATA  ; The rest of the OUTs are here.

        ALIGN 4
xsgmbe2_obos_skip_first_pass:

; Enable the other planes.

        not     ah
        and     ah,MM_ALL
        jz      short xsgmbe2_obos_no_planes_left
        mov     al,ah
        out     dx,al
        mov     ebx,cScan
        mov     edx,pjImage
        add     edx,cBufferOffset

        ALIGN 4
xsgmbe2_obos_second_pass:
        mov     ax,[esi]
        rol     ax,cl                   ; Put usable glyph data in place
        and     al,ch
        or      al,[edx]
        not     al
        xchg    al,[edi]
        add     esi,cjBytes
        add     edi,ulNextScan
        inc     edx
        dec     ebx
        jnz     short xsgmbe2_obos_second_pass
        mov     dx,VGA_BASE + SEQ_DATA  ; The rest of the OUTs are here.

xsgmbe2_obos_no_planes_left:
        mov     al,MM_ALL
        out     dx,al

xsgmbe2_obos_exit:
        inc     pjScreen                ; advance the display ptr
        inc     pjGlyph                 ; always advance glyph ptr for 2 bytes
        retn

ENDIF   ;GMB_2_FETCH_POSSIBLE

        ALIGN   4

xgi_1st_byte_1st_scrn   equ this dword              ;FG  SB LMB OB  MS  2F
        dd  offset FLAT:xgi1_scrn_g_xbos_1scrn      ; 0   0  0   0   0  0
        dd  offset FLAT:xgi2_scrn_g_xbos_1scrn      ; 0   0  0   0   0  1
        dd  offset FLAT:xgi1_scrn_g_xbms_1scrn      ; 0   0  0   0   1  0
        dd  offset FLAT:xgi2_scrn_g_xbms_1scrn      ; 0   0  0   0   1  1
        dd  offset FLAT:xgi1_scrn_g_obos_1scrn      ; 0   0  0   1   0  0
        dd  offset FLAT:xgi2_scrn_g_obos_1scrn      ; 0   0  0   1   0  1
        dd  offset FLAT:xgi1_scrn_g_obos_1scrn      ; 0   0  0   1   1  0
        dd  offset FLAT:xgi2_scrn_g_obos_1scrn      ; 0   0  0   1   1  1
        dd  offset FLAT:xgi1_scrn_gmb_xbos_1scrn    ; 0   0  1   0   0  0
        dd  offset FLAT:xgi2_scrn_gmb               ; 0   0  1   0   0  1
        dd  offset FLAT:xgi1_scrn_gmb_xbms_1scrn    ; 0   0  1   0   1  0
        dd  offset FLAT:xgi2_scrn_gmb               ; 0   0  1   0   1  1
        dd  offset FLAT:xgi1_scrn_gmb_obos_1scrn    ; 0   0  1   1   0  0
        dd  offset FLAT:xgi2_scrn_gmb               ; 0   0  1   1   0  1
        dd  offset FLAT:xgi1_scrn_gmb_obos_1scrn    ; 0   0  1   1   1  0
        dd  offset FLAT:xgi2_scrn_gmb               ; 0   0  1   1   1  1
        dd  offset FLAT:xgi1_buffer_or              ; 0   1  0   0   0  0
        dd  offset FLAT:xgi2_buffer_or              ; 0   1  0   0   0  1
        dd  offset FLAT:xgi1_buffer_or              ; 0   1  0   0   1  0
        dd  offset FLAT:xgi2_buffer_or              ; 0   1  0   0   1  1
        dd  offset FLAT:xgi1_buffer_or_obos         ; 0   1  0   1   0  0
        dd  offset FLAT:xgi2_buffer_or_obos         ; 0   1  0   1   0  1
        dd  offset FLAT:xgi1_buffer_or_obos         ; 0   1  0   1   1  0
        dd  offset FLAT:xgi2_buffer_or_obos         ; 0   1  0   1   1  1
        dd  offset FLAT:xgi1_buffer_or              ; 0   1  1   0   0  0
        dd  offset FLAT:xgi2_buffer_or              ; 0   1  1   0   0  1
        dd  offset FLAT:xgi1_buffer_or              ; 0   1  1   0   1  0
        dd  offset FLAT:xgi2_buffer_or              ; 0   1  1   0   1  1
        dd  offset FLAT:xgi1_buffer_or_obos         ; 0   1  1   1   0  0
        dd  offset FLAT:xgi2_buffer_or_obos         ; 0   1  1   1   0  1
        dd  offset FLAT:xgi1_buffer_or_obos         ; 0   1  1   1   1  0
        dd  offset FLAT:xgi2_buffer_or_obos         ; 0   1  1   1   1  1
        dd  offset FLAT:xgi1_scrn_g_xbos_1scrn      ; 1   0  0   0   0  0
        dd  offset FLAT:xgi2_scrn_g_xbos_1scrn      ; 1   0  0   0   0  1
        dd  offset FLAT:xgi1_scrn_g_xbms_1scrn      ; 1   0  0   0   1  0
        dd  offset FLAT:xgi2_scrn_g_xbms_1scrn      ; 1   0  0   0   1  1
        dd  offset FLAT:xgi1_scrn_g_obos_1scrn      ; 1   0  0   1   0  0
        dd  offset FLAT:xgi2_scrn_g_obos_1scrn      ; 1   0  0   1   0  1
        dd  offset FLAT:xgi1_scrn_g_obos_1scrn      ; 1   0  0   1   1  0
        dd  offset FLAT:xgi2_scrn_g_obos_1scrn      ; 1   0  0   1   1  1
        dd  offset FLAT:xgi1_scrn_gm_xbos_1scrn     ; 1   0  1   0   0  0
        dd  offset FLAT:xgi2_scrn_gm_xbos_1scrn     ; 1   0  1   0   0  1
        dd  offset FLAT:xgi1_scrn_gm_xbms_1scrn     ; 1   0  1   0   1  0
        dd  offset FLAT:xgi2_scrn_gm_xbms_1scrn     ; 1   0  1   0   1  1
        dd  offset FLAT:xgi1_scrn_gm_obos_1scrn     ; 1   0  1   1   0  0
        dd  offset FLAT:xgi2_scrn_gm_obos_1scrn     ; 1   0  1   1   0  1
        dd  offset FLAT:xgi1_scrn_gm_obos_1scrn     ; 1   0  1   1   1  0
        dd  offset FLAT:xgi2_scrn_gm_obos_1scrn     ; 1   0  1   1   1  1
        dd  offset FLAT:xgi1_buffer_over            ; 1   1  0   0   0  0
        dd  offset FLAT:xgi2_buffer_over            ; 1   1  0   0   0  1
        dd  offset FLAT:xgi1_buffer_over            ; 1   1  0   0   1  0
        dd  offset FLAT:xgi2_buffer_over            ; 1   1  0   0   1  1
        dd  offset FLAT:xgi1_buffer_over_obos       ; 1   1  0   1   0  0
        dd  offset FLAT:xgi2_buffer_over_obos       ; 1   1  0   1   0  1
        dd  offset FLAT:xgi1_buffer_over_obos       ; 1   1  0   1   1  0
        dd  offset FLAT:xgi2_buffer_over_obos       ; 1   1  0   1   1  1
        dd  offset FLAT:xgi1_buffer_over            ; 1   1  1   0   0  0
        dd  offset FLAT:xgi2_buffer_over            ; 1   1  1   0   0  1
        dd  offset FLAT:xgi1_buffer_over            ; 1   1  1   0   1  0
        dd  offset FLAT:xgi2_buffer_over            ; 1   1  1   0   1  1
        dd  offset FLAT:xgi1_buffer_over_obos       ; 1   1  1   1   0  0
        dd  offset FLAT:xgi2_buffer_over_obos       ; 1   1  1   1   0  1
;       dd  offset FLAT:xgi1_buffer_over_obos       ; 1   1  1   1   1  0
;       dd  offset FLAT:xgi2_buffer_over_obos       ; 1   1  1   1   1  1

xgi_1st_byte    equ this dword                      ;SB LMB OB  MS  2F
        dd  offset FLAT:xgi1_scrn_g_xbos            ; 0  0   0   0  0
        dd  offset FLAT:xgi2_scrn_g_xbos            ; 0  0   0   0  1
        dd  offset FLAT:xgi1_scrn_g_xbms            ; 0  0   0   1  0
        dd  offset FLAT:xgi2_scrn_g_xbms            ; 0  0   0   1  1
        dd  offset FLAT:xgi1_scrn_g_obos            ; 0  0   1   0  0
        dd  offset FLAT:xgi2_scrn_g_obos            ; 0  0   1   0  1
        dd  offset FLAT:xgi1_scrn_g_obos            ; 0  0   1   1  0
        dd  offset FLAT:xgi2_scrn_g_obos            ; 0  0   1   1  1
        dd  offset FLAT:xgi1_scrn_gmb_xbos          ; 0  1   0   0  0
        dd  offset FLAT:xgi2_scrn_gmb               ; 0  1   0   0  1
        dd  offset FLAT:xgi1_scrn_gmb_xbms          ; 0  1   0   1  0
        dd  offset FLAT:xgi2_scrn_gmb               ; 0  1   0   1  1
        dd  offset FLAT:xgi1_scrn_gmb_obos          ; 0  1   1   0  0
        dd  offset FLAT:xgi2_scrn_gmb               ; 0  1   1   0  1
        dd  offset FLAT:xgi1_scrn_gmb_obos          ; 0  1   1   1  0
        dd  offset FLAT:xgi2_scrn_gmb               ; 0  1   1   1  1
        dd  offset FLAT:xgi1_buffer_over            ; 1  0   0   0  0
        dd  offset FLAT:xgi2_buffer_over            ; 1  0   0   0  1
        dd  offset FLAT:xgi1_buffer_over            ; 1  0   0   1  0
        dd  offset FLAT:xgi2_buffer_over            ; 1  0   0   1  1
        dd  offset FLAT:xgi1_buffer_over_obos       ; 1  0   1   0  0
        dd  offset FLAT:xgi2_buffer_over_obos       ; 1  0   1   0  1
        dd  offset FLAT:xgi1_buffer_over_obos       ; 1  0   1   1  0
        dd  offset FLAT:xgi2_buffer_over_obos       ; 1  0   1   1  1
        dd  offset FLAT:xgi1_buffer_or              ; 1  1   0   0  0
        dd  offset FLAT:xgi2_buffer_or              ; 1  1   0   0  1
        dd  offset FLAT:xgi1_buffer_or              ; 1  1   0   1  0
        dd  offset FLAT:xgi2_buffer_or              ; 1  1   0   1  1
        dd  offset FLAT:xgi1_buffer_or_obos         ; 1  1   1   0  0
        dd  offset FLAT:xgi2_buffer_or_obos         ; 1  1   1   0  1
;       dd  offset FLAT:xgi1_buffer_or_obos         ; 1  1   1   1  0
;       dd  offset FLAT:xgi2_buffer_or_obos         ; 1  1   1   1  1

xgi_1st_byte_last_glyph equ this dword              ;LMB OB MS  2F
        dd  offset FLAT:xgi1_scrn_gm_xbos           ; 0   0  0  0
        dd  offset FLAT:xgi2_scrn_gm_xbos           ; 0   0  0  1
        dd  offset FLAT:xgi1_scrn_gm_xbms           ; 0   0  1  0
        dd  offset FLAT:xgi2_scrn_gm_xbms           ; 0   0  1  1
        dd  offset FLAT:xgi1_scrn_gm_obos_lg        ; 0   1  0  0
        dd  offset FLAT:xgi2_scrn_gm_obos_lg        ; 0   1  0  1
        dd  offset FLAT:xgi1_scrn_gm_obos_lg        ; 0   1  1  0
        dd  offset FLAT:xgi2_scrn_gm_obos_lg        ; 0   1  1  1
        dd  offset FLAT:xgi1_scrn_gmb_xbos          ; 1   0  0  0
        dd  offset FLAT:xgi2_scrn_gmb               ; 1   0  0  1
        dd  offset FLAT:xgi1_scrn_gmb_xbms          ; 1   0  1  0
        dd  offset FLAT:xgi2_scrn_gmb               ; 1   0  1  1
        dd  offset FLAT:xgi1_scrn_gmb_obos_lg       ; 1   1  0  0
        dd  offset FLAT:xgi2_scrn_gmb               ; 1   1  0  1
        dd  offset FLAT:xgi1_scrn_gmb_obos_lg       ; 1   1  1  0
;       dd  offset FLAT:xgi2_scrn_gmb               ; 1   1  1  1

xgi_middle_byte_table   equ this dword              ;OB  MS  2F
        dd  offset FLAT:xgi1_scrn_g_xbos            ; 0   0   0
        dd  offset FLAT:xgi2_scrn_g_xbos            ; 0   0   1
        dd  offset FLAT:xgi1_scrn_g_xbms            ; 0   1   0
        dd  offset FLAT:xgi2_scrn_g_xbms            ; 0   1   1
        dd  offset FLAT:xgi1_scrn_g_obos            ; 1   0   0
        dd  offset FLAT:xgi2_scrn_g_obos            ; 1   0   1
;       dd  offset FLAT:xgi1_scrn_g_obos            ; 1   1   0
;       dd  offset FLAT:xgi2_scrn_g_obos            ; 1   1   1

xgi_middle_bytes_table   equ this dword             ;OB  MS  2F
        dd  offset FLAT:xgi1_scrn_gs_xbos           ; 0   0   0
        dd  offset FLAT:xgi2_scrn_gs_xbos           ; 0   0   1
        dd  offset FLAT:xgi1_scrn_gs_xbms           ; 0   1   0
        dd  offset FLAT:xgi2_scrn_gs_xbms           ; 0   1   1
        dd  offset FLAT:xgi1_scrn_gs_obos           ; 1   0   0
        dd  offset FLAT:xgi2_scrn_gs_obos           ; 1   0   1
;       dd  offset FLAT:xgi1_scrn_gs_obos           ; 1   1   0
;       dd  offset FLAT:xgi2_scrn_gs_obos           ; 1   1   1

xgi_last_byte_table     equ this dword              ;OB  MS  2F
        dd  offset FLAT:xgi1_buffer_over            ; 0   0  0
        dd  offset FLAT:xgi2_buffer_over            ; 0   0  1
        dd  offset FLAT:xgi1_buffer_over            ; 0   1  0
        dd  offset FLAT:xgi2_buffer_over            ; 0   1  1
        dd  offset FLAT:xgi1_buffer_over_obos       ; 1   0  0
        dd  offset FLAT:xgi2_buffer_over_obos       ; 1   0  1
;       dd  offset FLAT:xgi1_buffer_over_obos       ; 1   1  0
;       dd  offset FLAT:xgi2_buffer_over_obos       ; 1   1  1

xgi_last_byte_last_glyph    equ this dword          ;LMB OB  MS  2F
        dd  offset FLAT:xgi1_scrn_gm_xbos           ; 0   0   0  0
        dd  offset FLAT:xgi2_scrn_gm_xbos           ; 0   0   0  1
        dd  offset FLAT:xgi1_scrn_gm_xbms           ; 0   0   1  0
        dd  offset FLAT:xgi2_scrn_gm_xbms           ; 0   0   1  1
        dd  offset FLAT:xgi1_scrn_gm_obos           ; 0   1   0  0
        dd  offset FLAT:xgi2_scrn_gm_obos           ; 0   1   0  1
        dd  offset FLAT:xgi1_scrn_gm_obos           ; 0   1   1  0
        dd  offset FLAT:xgi2_scrn_gm_obos           ; 0   1   1  1
        dd  offset FLAT:xgi1_scrn_gm_xbos           ; 1   0   0  0
        dd  offset FLAT:xgi2_scrn_gm_xbos           ; 1   0   0  1
        dd  offset FLAT:xgi1_scrn_gm_xbms           ; 1   0   1  0
        dd  offset FLAT:xgi2_scrn_gm_xbms           ; 1   0   1  1
        dd  offset FLAT:xgi1_scrn_gm_obos           ; 1   1   0  0
        dd  offset FLAT:xgi2_scrn_gm_obos           ; 1   1   0  1
;       dd  offset FLAT:xgi1_scrn_gm_obos           ; 1   1   1  0
;       dd  offset FLAT:xgi2_scrn_gm_obos           ; 1   1   1  1

xgi_buffer_offset   equ     this dword
        db      0
        db      3
        db      2
        db      1
        db      0
        db      3
        db      2
        db      1

loop_table      macro   name

        dd      offset FLAT:&name&_loop8
        dd      offset FLAT:&name&_loop1
        dd      offset FLAT:&name&_loop2
        dd      offset FLAT:&name&_loop3
        dd      offset FLAT:&name&_loop4
        dd      offset FLAT:&name&_loop5
        dd      offset FLAT:&name&_loop6
        dd      offset FLAT:&name&_loop7

        endm

xgi1_fob_over   equ this dword
        loop_table  xgi_fetch_one_byte_over

xgi2_ftb_over   equ this dword
        loop_table  xgi_fetch_two_bytes_over

xgi1_fob_or     equ this dword
        loop_table  xgi_fetch_one_byte_or

xgi2_ftb_or     equ this dword
        loop_table  xgi_fetch_two_bytes_or

xsg1_xbos_dispatch equ this dword
        loop_table  xsg1_xbos

xsgmb1_xbos_dispatch equ this dword
        loop_table  xsgmb1_xbos

xsg2_xbos_dispatch equ this dword
        loop_table  xsg2_xbos

xsg1_obos_dispatch equ this dword
        loop_table  xsg1_obos

xsgmb1_obos_dispatch equ this dword
        loop_table  xsgmb1_obos

xsg2_obos_dispatch equ this dword
        loop_table  xsg2_obos

IFDEF   GMB_2_FETCH_POSSIBLE

xsgmb2_xbos_dispatch equ this dword
        loop_table  xsgmb2_xbos

xsgmb2_obos_dispatch equ this dword
        loop_table  xsgmb2_obos_loop8

ENDIF   ;GMB_2_FETCH_POSSIBLE

xxxvStringBlt endp

public sblt_glyph_search
public sblt_glyph_search_last
public sblt_glyph_search_1st
public sblt_found_both
public sblt_calc_glyph_height
public sblt_accel_flag
public sblt_special_case
public sblt_init_jmp_table
;public sblt_call_vglyphblt
public sblt_1st_glyph_cell
public sblt_last_glyph_cell
public sblt_middle_fpa_glyph
public sblt_next_glyph_cell
public sblt_left_mask
public sblt_one_byte_mask
public sblt_inner_count
public sblt_last_mask
public sblt_glyph_info
public sblt_xfer_glyph_image
public sblt_exit
public xgi_inner_bytes
public xgi_last_byte
public fixed_pitch_aligned_sblt
public fixed_pitch_aligned_sblt_inner
public fpa_single_byte_glyph
public fpa_multi_byte_glyph
public fpa_exit
public xgi1_buffer_over
public xgi1_buffer_or
public xgi2_buffer_over
public xgi2_buffer_or
public xgi1_scrn_g_obos_1scrn
public xgi1_scrn_g_obos
public xgi1_scrn_g_xbos_1scrn
public xgi1_scrn_g_xbos
public xgi1_scrn_g_xbms_1scrn
public xgi1_scrn_g_xbms
public xgi1_scrn_gs_obos
public xgi1_scrn_gs_xbos
public xgi1_scrn_gs_xbms
public xgi1_scrn_gm_xbos_1scrn
public xgi1_scrn_gm_xbos
public xgi1_scrn_gm_xbms_1scrn
public xgi1_scrn_gm_xbms
public xgi1_scrn_gmb_obos_1scrn
public xgi1_scrn_gmb_obos
public xgi1_scrn_gmb_xbos_1scrn
public xgi1_scrn_gmb_xbos
public xgi1_scrn_gmb_xbms_1scrn
public xgi1_scrn_gmb_xbms
public xgi2_scrn_g_obos_1scrn
public xgi2_scrn_g_obos
public xgi2_scrn_g_xbos_1scrn
public xgi2_scrn_g_xbos
public xgi2_scrn_g_xbms_1scrn
public xgi2_scrn_g_xbms
public xgi2_scrn_gs_obos
public xgi2_scrn_gs_xbos
public xgi2_scrn_gs_xbms
public xgi2_scrn_gm_xbos_1scrn
public xgi2_scrn_gm_xbos
public xgi2_scrn_gm_xbms_1scrn
public xgi2_scrn_gm_xbms
public xgi1_scrn_gm_obos_1scrn
public xgi1_scrn_gm_obos_lg
public xgi1_scrn_gm_obos
public xgi2_scrn_gm_obos_1scrn
public xgi2_scrn_gm_obos_lg
public xgi2_scrn_gm_obos
public xgi1_scrn_gmb_obos_lg
public xgi_setup_opaque_bkgnd_edge
public xgi_set_opaque_bkgnd

IFDEF  GMB_2_FETCH_POSSIBLE
public xgi2_scrn_gmb_obos_1scrn
public xgi2_scrn_gmb_obos
public xgi2_scrn_gmb_xbos_1scrn
public xgi2_scrn_gmb_xbos
public xgi2_scrn_gmb_xbms_1scrn
public xgi2_scrn_gmb_xbms
public xgi2_scrn_gmb_obos_lg
ENDIF  ;GMB_2_FETCH_POSSIBLE

        end

