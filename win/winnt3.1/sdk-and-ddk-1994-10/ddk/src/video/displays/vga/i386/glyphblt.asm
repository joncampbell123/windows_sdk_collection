;---------------------------Module-Header------------------------------;
; Module Name: glyphblt.asm
;
; Output a glyph onto VGA screen
;
; Copyright (c) 1992 Microsoft Corporation
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
        .list

        include i386\strblt.inc

        .code

;---------------------------Public-Routine------------------------------;
; vGlyphBlt
;
;   Draw a glyph to the VGA screen
;
;-----------------------------------------------------------------------;

_TEXT$01   SEGMENT DWORD USE32 PUBLIC 'CODE'
           ASSUME  CS:FLAT, DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

ProcName    xxxvGlyphBlt,vGlyphBlt,32

xxxvGlyphBlt proc uses    esi edi ebx,  \
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
        local   cScan       :dword      ; visible glyph height
        local   cPels       :dword      ; visible glyph width
        local   cjBytes     :dword      ; glyph scan size
        local   cTmp        :dword      ; temporary scan count
        local   cInnerBytes :dword
        local   ulNextScan  :dword      ;offset from one screen scan to next
        local   cfBits      :byte
        local   cfLeft      :byte
        local   fjMask      :byte
        local   fjLastMask  :byte
        local   cfLastBits  :byte

; Initialize stuff

        cld
        mov     edi,pdsurf
        mov     esi,prcl

; Set the offset from one scan to the next

        mov     eax,[edi].dsurf_lNextScan
        mov     ulNextScan,eax

; Calculate number of scans for bltting cell

gblt_calc_dest_addr:
        mov     eax,[esi].yTop
        mov     edx,[esi].yBottom
        sub     edx,eax
        mov     cScan,edx               ; Number of scans

; Can we assume positive value?

        mov     edx,ulNextScan
        mul     edx                     ; EAX = byte offset to scan

; Calculate cell width in pels

        mov     ebx,[esi].xRight
        mov     ecx,[esi].xLeft
        sub     ebx,ecx
        mov     cPels,ebx               ; Width in pels

; Calculate pointer to the 1st screen byte to blt

        mov     edx,ecx
        shr     edx,3
        add     eax,edx
        add     eax,[edi].dsurf_pvBitmapStart
        mov     pjScreen,eax            ; Base address of blit

; Calculate the destination left edge mask

gblt_left_mask:
        xor     eax,eax
        cdq
        mov     cInnerBytes,eax         ; Assume glyph fits one byte
        mov     fjLastMask,al
        and     cl,7
        mov     dl,8
        sub     dl,cl                   ; DL = bits in mask

        dec     al
        shr     al,cl                   ; AL = left mask

; Assume cell is at least one byte wide

        mov     fjMask,al               ; then we have this left mask
        mov     cfBits,dl               ; along with 1 bits count
        sub     ebx,edx                 ; EBX = cPels, EDX = bits in left mask
        jg      short gblt_inner_count  ; but if the assume is true?
        je      short gblt_glyph_info

; It is less than one byte wide, calculate the right edge mask

        mov     cl,bl
        neg     cl
        inc     ah                      ; AH = 1
        shl     ah,cl
        neg     ah                      ; AH = right mask
        and     al,ah                   ; AL = composite mask
        mov     fjMask,al
        jmp     short gblt_glyph_info   ; no middle or last bytes

; Calculate the inner count

gblt_inner_count:
        mov     ecx,ebx                 ; EBX = # bits left in cell
        shr     ebx,3
        mov     cInnerBytes,ebx         ; inner byte count

; Calculate the last byte mask

gblt_last_mask:
        and     cl,7                    ; CL = # of trailing bits
        mov     cfLastBits,cl
        dec     ah                      ; AH = 0FFh
        shr     ah,cl
        not     ah
        mov     fjLastMask,ah

; Now get the glyph information

gblt_glyph_info:
        mov     edi,pgp
        mov     ebx,[edi].gp_pgdf
        mov     ebx,[ebx].gdf_pgb       ; ptr to glyph bits

; Calculate the glyph scan size

gblt_glyph_scan:
        mov     eax,[ebx].gb_cx         ; Glyph width in pels
        add     eax,7
;        and     al,0e0h                 ; DWORD aligned
        shr     eax,3
        mov     cjBytes,eax             ; Count to move to next line of glyph

; Assume the glyph is not clipped

        lea     edx,[ebx].gb_aj
        mov     pjGlyph,edx             ; Start of glyph
        mov     dl,8
        mov     cfLeft,dl               ; Bits left in glyph byte
        test    flOption,VGB_VERT_CLIPPED_GLYPH or VGB_HORIZ_CLIPPED_GLYPH
        jz      short gblt_draw_glyph

; Calculate byte offset of the 1st scan in glyph

gblt_glyph_offset:
        mov     edx,[esi].yTop
        sub     edx,[edi].gp_y          ; Vertical offset into glyph
        mul     edx                     ; Can we assume positive value?

; Calculate offset to the 1st byte in Glyph

        mov     edx,[esi].xLeft
        sub     edx,[edi].gp_x          ; Horizontal offset into glyph
        mov     ecx,edx
        shr     edx,3
        add     eax,edx                 ; Offset into glyph
        add     pjGlyph,eax             ; Start of glyph

; Calculate glyph's left mask

gblt_glyph_left_mask:
        and     cl,7
        sub     cfLeft,cl               ; Bits left in glyph byte

; Setup the drawing parameters

gblt_draw_glyph:
        mov     eax,ulMode              ; determine the write mode
        call    offset FLAT:vgblt_table.[eax*4]

; Everything is done, go home

glyph_blt_exit:

        cRet    vGlyphBlt

vgblt_table     equ     this dword
        dd      offset FLAT:vgblt_mix_glyph
        dd      offset FLAT:vgblt_opaque_glyph

        .errnz  VGB_MIX_STRING
        .errnz  VGB_OPAQUE_STRING-1

;---------------------------Public-Routine------------------------------;
; vgblt_mix_glyph
;
;   Draw a glyph to the VGA screen using the write mode 0
;
;-----------------------------------------------------------------------;

vgblt_mix_glyph::

        mov     dx,VGA_BASE + GRAF_ADDR ; Leave this in a register
        mov     eax,cScan
        mov     cTmp,eax                ; Scans to draw
        mov     esi,pjGlyph             ; Read glyph from here
        mov     edi,pjScreen            ; Write glyph to there
        mov     bh,fjMask               ; Current mask
        mov     cl,cfLeft               ; Bits available in current byte

        sub     cl,cfBits
        jl      short vmg_fetch_two_bytes

;
; Fetching one byte means we are at the left edge of the glyph, or
; by some miracle we are aligned with the VGA memory.  Either way
; we will still mask the data with fjMask.
;

vmg_fetch_one_byte:
        mov     al,GRAF_BIT_MASK        ; leave it in AL

vmg_fetch_one_byte_next:
        mov     ah,[esi]                ; AH = glyph data

        shr     ah,cl                   ; Put usable glyph data in place

        and     ah,bh                   ; AH = masked glyph data
        jz      short @F                ; Don't waste time

        out     dx,ax
        xchg    ah,[edi]                ; Set the bits

@@:
        add     edi,ulNextScan          ; Next scan on VGA
        add     esi,cjBytes             ; Next line of glyph
        dec     cTmp
        jnz     short vmg_fetch_one_byte_next ; finish column

        neg     cl
        and     cl,7                    ; CL = # usable bits in next byte
        jnz     short vmg_advance_screen_ptr
        jmp     short vmg_advance_glyph_ptr

; The current screen byte covers two glyph bytes

vmg_fetch_two_bytes:
        mov     bl,GRAF_BIT_MASK        ; Set bitmask for altered bits
        neg     cl                      ; CL = # bits required from next byte
        add     cl,8                    ; we will have to swap AH and AL

vmg_fetch_two_bytes_next:
        mov     ax,[esi]                ; AL = current byte, AH = next byte
        rol     ax,cl                   ; AH = glyph data

        and     ah,bh                   ; AH = masked glyph data
        jz      short @F                ; Don't waste time

        mov     al,bl
        out     dx,ax
        xchg    al,[edi]                ; Set the bits

@@:
        add     edi,ulNextScan          ; Next scan on VGA
        add     esi,cjBytes             ; Next line of glyph
        dec     cTmp
        jnz     short vmg_fetch_two_bytes_next ; finish column
        sub     cl,8                    ; CL = # bits required from next byte

; We will start the next screen column byte. Recalculate parameters.

vmg_advance_glyph_ptr:
        inc     pjGlyph                 ; increment glyph pointer

vmg_advance_screen_ptr:
        inc     pjScreen                ; Increment screen pointer

vmg_inner_bytes:
        mov     ebx,cInnerBytes         ; do we have inner bytes?
        or      ebx,ebx
        jz      short vmg_last_byte

        mov     eax,cScan
        mov     cTmp,eax                ; Scans to draw
        mov     esi,pjGlyph             ; Read glyph from here
        mov     edi,pjScreen            ; Write glyph to there
        add     pjGlyph,ebx             ; advance pointer
        add     pjScreen,ebx
        sub     cjBytes,ebx             ; glyph scan incremental
        or      cl,cl
        jnz     short vmg_inner_fetch_two_bytes

        mov     al,GRAF_BIT_MASK        ; leave it in AL

vmg_inner_fetch_one_byte:
        mov     ah,[esi]                ; AH = glyph data
        or      ah,ah
        jz      short @F                ; Don't waste time

        out     dx,ax
        xchg    ah,[edi]                ; Set the bits

@@:
        inc     esi
        inc     edi
        dec     ebx
        jnz     short vmg_inner_fetch_one_byte

        mov     ebx,cInnerBytes
        add     edi,ulNextScan          ; Next scan on VGA
        add     esi,cjBytes             ; Next line of glyph
        sub     edi,ebx                 ; adjust for previous increment
        dec     cTmp
        jnz     short vmg_inner_fetch_one_byte ; finish column

        jmp     short vmg_inner_byte_end

; The current screen byte covers two glyph bytes

vmg_inner_fetch_two_bytes:
        add     cl,8                    ; we will have to swap AH and AL

vmg_inner_fetch_two_bytes_next:
        mov     ax,[esi]                ; AL = curr byte, AH = next byte
        rol     ax,cl                   ; AH = glyph data
        jz      short @F                ; Don't waste time

        mov     al,GRAF_BIT_MASK
        out     dx,ax
        xchg    al,[edi]                ; Set the bits
@@:
        inc     esi
        inc     edi
        dec     ebx
        jnz     short vmg_inner_fetch_two_bytes_next

        mov     ebx,cInnerBytes
        add     edi,ulNextScan          ; Next scan on VGA
        add     esi,cjBytes             ; Next line of glyph
        sub     edi,ebx                 ; adjust for previous increment
        dec     cTmp
        jnz     short vmg_inner_fetch_two_bytes_next ; finish column
        sub     cl,8

vmg_inner_byte_end:
        add     cjBytes,ebx             ; restore its actual value

vmg_last_byte:
        mov     bl,fjLastMask           ; BL = last byte mask
        or      bl,bl
        jz      short vmg_exit

        mov     esi,pjGlyph             ; Read glyph from here
        mov     edi,pjScreen            ; Write glyph to there
        mov     ch,8
        sub     ch,cl                   ; CH = # usable bits in current byte
        cmp     ch,cfLastBits           ; do we need the next byte?
        jl      short vmg_last_fetch_two_bytes

        mov     al,GRAF_BIT_MASK

vmg_last_fetch_one_byte:
        mov     ah,[esi]                ; AH = glyph data
        shl     ah,cl
        and     ah,bl
        jz      short @F                ; Don't waste time

        out     dx,ax
        xchg    ah,[edi]                ; Set the bits

@@:
        add     edi,ulNextScan          ; Next scan on VGA
        add     esi,cjBytes             ; Next line of glyph
        dec     cScan
        jnz     short vmg_last_fetch_one_byte ; finish column

        jmp     short vmg_exit

; The current screen byte covers two glyph bytes

vmg_last_fetch_two_bytes:
        add     cl,8                    ; we will swap AH and AL
        mov     bh,GRAF_BIT_MASK

vmg_last_fetch_two_bytes_next:
        mov     ax,[esi]
        rol     ax,cl                   ; AH = glyph data
        and     ah,bl
        jz      short @F                ; Don't waste time

        mov     al,bh
        out     dx,ax
        xchg    al,[edi]                ; Set the bits

@@:
        add     edi,ulNextScan          ; Next scan on VGA
        add     esi,cjBytes             ; Next line of glyph
        dec     cScan
        jnz     short vmg_last_fetch_two_bytes_next ; finish column

vmg_exit:
        retn

;---------------------------Public-Routine------------------------------;
; vgblt_opaque_glyph
;
;   Draw a glyph to the VGA screen using the write mode 3
;
;-----------------------------------------------------------------------;

vgblt_opaque_glyph:

        mov     eax,cScan
        mov     cTmp,eax                ; Scans to draw
        mov     esi,pjGlyph             ; Read glyph from here
        mov     edi,pjScreen            ; Write glyph to there
        mov     bh,fjMask               ; Current mask
        mov     edx,ulNextScan

        mov     cl,cfLeft               ; Bits available in current byte
        sub     cl,cfBits
        jl      short vog_fetch_two_bytes

;
; Fetching one byte means we are at the left edge of the glyph, or
; by some miracle we are aligned with the VGA memory.  Either way
; we will still mask the data with fjMask.
;

vog_fetch_one_byte:
        mov     ah,[esi]                ; AH = glyph data
        shr     ah,cl                   ; Put usable glyph data in place
        and     ah,bh
        jz      short @F                ; don't waste time
        xchg    ah,[edi]                ; Set the bits
@@:
        add     edi,edx                 ; Next scan on VGA
        add     esi,cjBytes             ; Next line of glyph
        dec     cTmp
        jnz     short vog_fetch_one_byte ; finish column

        neg     cl
        and     cl,7                    ; CL = # usable bits in next byte
        jnz     short vog_advance_screen_ptr
        jmp     short vog_advance_glyph_ptr

; The current screen byte covers two glyph bytes

vog_fetch_two_bytes:
        neg     cl                      ; CL = cfBits - cfLeft
        add     cl,8                    ; we will swap AH and AL

vog_fetch_two_bytes_next:
        mov     ax,[esi]                ; AL = curr byte, AH = next byte
        rol     ax,cl                   ; AH = glyph data
        and     ah,bh
        jz      short @F                ; don't waste time
        xchg    ah,[edi]                ; Set the bits
@@:
        add     edi,edx                 ; Next scan on VGA
        add     esi,cjBytes             ; Next line of glyph
        dec     cTmp
        jnz     short vog_fetch_two_bytes_next ; finish column
        sub     cl,8

; We will start the next screen column byte. Recalculate parameters.

vog_advance_glyph_ptr:
        inc     pjGlyph

vog_advance_screen_ptr:
        inc     pjScreen                ; Increment screen pointer

vog_inner_bytes:
        mov     ebx,cInnerBytes         ; do we have inner bytes?
        or      ebx,ebx
        jz      short vog_last_byte

        mov     eax,cScan
        mov     cTmp,eax                ; Scans to draw
        mov     esi,pjGlyph             ; Read glyph from here
        mov     edi,pjScreen            ; Write glyph to there
        add     pjGlyph,ebx             ; advance pointer
        add     pjScreen,ebx
        sub     cjBytes,ebx             ; glyph scan size
        sub     edx,ebx                 ; screen scan size
        or      cl,cl
        jnz     short vog_inner_fetch_two_bytes

vog_inner_fetch_one_byte:
        mov     ecx,ebx

vog_inner_loop:
        lodsb
        or      al,al
        jz      short @F
        xchg    al,[edi]
@@:
        inc     edi
        loop    short vog_inner_loop

        add     edi,edx                 ; Next scan on VGA
        add     esi,cjBytes             ; Next line of glyph
        dec     cTmp
        jnz     short vog_inner_fetch_one_byte ; finish column

        jmp     short vog_inner_byte_end

; The current screen byte covers two glyph bytes

vog_inner_fetch_two_bytes:
        add     cl,8

vog_inner_fetch_two_bytes_next:
        mov     ax,[esi]                ; AL = curr byte, AH = next byte
        rol     ax,cl                   ; AH = glyph data
        or      ah,ah
        jz      short @F
        xchg    ah,[edi]                ; Set the bits
@@:
        inc     esi
        inc     edi
        dec     ebx
        jnz     short vog_inner_fetch_two_bytes_next

        mov     ebx,cInnerBytes
        add     edi,edx                 ; Next scan on VGA
        add     esi,cjBytes             ; Next line of glyph
        dec     cTmp
        jnz     short vog_inner_fetch_two_bytes_next ; finish column
        sub     cl,8

vog_inner_byte_end:
        add     edx,ebx                 ; restore its original value
        add     cjBytes,ebx             ; restore its original value

vog_last_byte:
        mov     bl,fjLastMask           ; BL = last byte mask
        or      bl,bl
        jz      short vog_exit

        mov     esi,pjGlyph             ; Read glyph from here
        mov     edi,pjScreen            ; Write glyph to there
        mov     ch,8
        sub     ch,cl                   ; CH = # usable bits in current byte
        cmp     ch,cfLastBits           ; do we need the next byte?
        jl      short vog_last_fetch_two_bytes


vog_last_fetch_one_byte:
        mov     ah,[esi]                ; AH = glyph data
        shl     ah,cl
        and     ah,bl
        jz      short @F
        xchg    ah,[edi]                ; Set the bits
@@:
        add     edi,edx                 ; Next scan on VGA
        add     esi,cjBytes             ; Next line of glyph
        dec     cScan
        jnz     short vog_last_fetch_one_byte ; finish column

        jmp     short vog_exit

; The current screen byte covers two glyph bytes

vog_last_fetch_two_bytes:
        add     cl,8                    ; we will swap AH and AL

vog_last_fetch_two_bytes_next:
        mov     ax,[esi]
        rol     ax,cl                   ; AH = glyph data
        and     ah,bl
        jz      short @F
        xchg    ah,[edi]                ; Set the bits
@@:
        add     edi,edx                 ; Next scan on VGA
        add     esi,cjBytes             ; Next line of glyph
        dec     cScan
        jnz     short vog_last_fetch_two_bytes_next ; finish column

vog_exit:
        retn

xxxvGlyphBlt endp


;---------------------------Public-Routine------------------------------;
; ulSetXParentRegs
;
;   Set the VGA control registers for the specified Rop4 and foreground
;   color when background is transparent.
;
;-----------------------------------------------------------------------;

cProc   ulSetXParentRegs,12,<           \
        uses    ebx edi,                \
        rop4:   dword,                  \
        pclr:   ptr,                    \
        bSet:   dword                   >

        mov     edi,VGB_OPAQUE_STRING
        mov     ebx,rop4
        xor     ecx,ecx                 ; Color = 00, mode = set
        or      ebx,ebx
        jz      short vsv_set_vga_mode

        dec     ch                      ; Color = FF, mode = set
        inc     bx
        jz      short vsv_set_vga_mode

        mov     eax,pclr
        mov     ch,byte ptr [eax]       ; Color = passed color, mode = set
        cmp     bx,0f0f0h + 1
        je      short vsv_set_vga_mode

        not     ch                      ; ~passed color
        cmp     bx,00f0fh + 1
        je      short vsv_set_vga_mode

        mov     cx,0FF00h + DR_XOR      ; Color = FF, mode = xor
        mov     edi,VGB_MIX_STRING      ; We will actually do mode 0

vsv_set_vga_mode:
        mov     eax,bSet                ; set flag
        or      eax,eax
        jz      short vsv_exit

        mov     dx,VGA_BASE + GRAF_ADDR
        mov     al,GRAF_SET_RESET
        mov     ah,ch
        out     dx,ax
        mov     ax,0f00h + GRAF_ENAB_SR
        out     dx,ax
        mov     ah,cl
        mov     al,GRAF_DATA_ROT
        out     dx,ax

        or      edi,edi
        .errnz  VGB_MIX_STRING
        jz      short vsv_exit
        mov     ax,(M_AND_WRITE shl 8)+GRAF_MODE
        out     dx,ax

vsv_exit:
        mov     eax,edi                 ; return VGA mode
        mov     edi,pclr
        mov     byte ptr [edi],ch       ; CH = actual foreground color
        cRet    ulSetXParentRegs

endProc ulSetXParentRegs

;---------------------------Public-Routine------------------------------;
; vResetVGARegs
;
;   Reset the VGA control registers to a default state.
;
;-----------------------------------------------------------------------;

cProc   vResetVGARegs

        mov     dx,VGA_BASE + GRAF_ADDR
        mov     ax,0ff00h + GRAF_BIT_MASK
        out     dx,ax
        mov     ax,0000h + GRAF_SET_RESET
        out     dx,ax
        mov     ax,0000h + GRAF_ENAB_SR
        out     dx,ax
        mov     ax,(DR_SET shl 8) + GRAF_DATA_ROT
        out     dx,ax
        mov     ax,((M_PROC_WRITE or M_DATA_READ) shl 8) + GRAF_MODE
        out     dx,ax

        cRet    vResetVGARegs

endProc vResetVGARegs

public gblt_calc_dest_addr
public gblt_left_mask
public gblt_inner_count
public gblt_last_mask
public gblt_glyph_info
public gblt_glyph_offset
public gblt_draw_glyph
public glyph_blt_exit
public vgblt_mix_glyph
public vmg_fetch_one_byte
public vmg_fetch_two_bytes
public vmg_advance_glyph_ptr
public vmg_advance_screen_ptr
public vmg_inner_bytes
public vmg_inner_fetch_one_byte
public vmg_inner_fetch_two_bytes
public vmg_last_byte
public vmg_last_fetch_one_byte
public vmg_last_fetch_two_bytes
public vmg_exit
public vgblt_opaque_glyph
public vog_fetch_one_byte
public vog_fetch_two_bytes
public vog_advance_glyph_ptr
public vog_advance_screen_ptr
public vog_inner_bytes
public vog_inner_fetch_one_byte
public vog_inner_fetch_two_bytes
public vog_last_byte
public vog_last_fetch_one_byte
public vog_last_fetch_two_bytes
public vog_exit

_TEXT$01   ends

        end

