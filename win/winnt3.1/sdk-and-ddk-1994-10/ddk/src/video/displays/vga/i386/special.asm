                page    ,132
;---------------------------Module-Header------------------------------;
; Module Name: special.asm
;
; Copyright (c) 1992 Microsoft Corporation
;-----------------------------------------------------------------------;
        title   Special
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

        .data

;BUGBUG this should be obtained from the surface

        extrn   ulNextScan_global:dword

        .code

_TEXT$01   SEGMENT DWORD USE32 PUBLIC 'CODE'
           ASSUME  CS:FLAT, DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        .xlist
        include stdcall.inc             ;calling convention cmacros
        include i386\cmacFLAT.inc       ; FLATland cmacros
        include i386\display.inc        ; Display specific structures
        include i386\ppc.inc            ; Pack pel conversion structure
        include i386\bitblt.inc         ; General definitions
        include i386\ropdefs.inc        ; Rop definitions
        include i386\egavga.inc         ; EGA register definitions
        include i386\strucs.inc
        .list

; Rops we use in this code

ROP_P           equ     0F0h
ROP_Pn          equ      0Fh
ROP_S           equ     0CCh
ROP_DDx         equ       0
ROP_DDxn        equ     0FFh
ROP_Dn          equ     055h
ROP_DPx         equ     05Ah

; Other values used in this code

fr              equ     [ebp]           ; Access to bitblt frame
bptr            equ     byte ptr

;----------------------------Public Routine----------------------------;
; check_device_special_cases
;
; Check for fast special cases of BLT.
;
; Determine if the BLT is a special case which can be performed with
; static code as opposed to code compiled on the stack, and, if so,
; dispatch to the proper static code.
;
; The parameters needed for the BLT (phase alignment, directions of
; movement, ...) have been computed and saved.  These parameters will
; now be interpreted and a BLT created on the stack.
;
; If the raster op is source copy, both devices are the screen, and the
; phase alignment is 0, then the copy can be performed by the static
; code using the EGA's write mode 1.
;
; If the rasterop is P, Pn, DDx (0), DDxn (1), and the brush is solid
; or grey (for P and Pn), and the destination device is the screen,
; then the operation can be performed by the static code using the EGA's
; write mode 2 (write mode 0 for greys).
;
; Entry:
;       EDI --> pointer to target surface
;       EBP --> frame of BitBLT local variables
;       EGA registers in default state
; Returns:
;       Carry set if BLT was performed with static code.
; Error Returns:
;       Carry clear if BLT was not a special case.
; Registers Destroyed:
;       AX,BX,CX,DX,SI,DI,DS,ES,flags
; Registers Preserved:
;       EBP
; Calls:
;-----------------------------------------------------------------------;

cProc   check_device_special_cases

        xor     cx,cx                       ;This is used, what is it????!!!
        mov     dh,fr.the_flags             ;Keep the flags in DH for a while
        test    dh,F0_DEST_IS_DEV           ;Is the destination a device?
        jz      cdsc_blt_not_special_cased  ;Not device, cannot special case it
        mov     edi,fr.dest.next_scan       ;Special case code expects this
        mov     al,byte ptr fr.Rop[0]       ;Get the raster op
        cmp     al,ROP_S                    ;Is it src copy?
        je      cdsc_its_src_copy     ;  Yes, go check it out
        cmp     al,ROP_P
        je      short cdsc_its_patblt

cdsc_not_s_or_p:
        xor     bl,bl                       ;Color for 0 fill
        cmp     al,ROP_DDx
        je      short cdsc_its_1_or_0
        cmp     al,ROP_Pn
        je      short cdsc_its_inverse_patblt
        cmp     al,ROP_Dn
        je      short cdsc_its_dn
        cmp     al,ROP_DPx
        je      short cdsc_its_dpx
        cmp     al,ROP_DDxn
        jne     cdsc_blt_not_special_cased
        mov     bl,0FFh                     ;Color for 1 fill

; It's "1" (DDxn) or "0" (DDx)
cdsc_its_1_or_0:
        mov     fr.brush_accel,SOLID_BRUSH  ;(no brush given for DDx or DDxn)
        call    ega_solid_pat
        jmp     cdsc_exit

cdsc_its_patblt:
        mov     bl,fr.brush_accel       ;Ccolor in lower bits, flags in upper
        test    bl,SOLID_BRUSH
        jnz     short cdsc_its_a_solid_color
        test    bl,GREY_SCALE
        jz      short cdsc_not_solid_nor_grey
        xor     bl,bl                   ;It's grey.  !!! what is this flag
        call    ega_solid_pat
        jmp     cdsc_exit

cdsc_not_solid_nor_grey:
        call    do_wes_patblt
        jmp     cdsc_exit

cdsc_its_inverse_solid_color:
        not     bl
cdsc_its_a_solid_color:
        mov     cx,2
        call    do_solid_patcopy        ;color and accl. flags already in BL
        jmp     short cdsc_exit

cdsc_its_inverse_patblt:
        mov     bl,fr.brush_accel       ;color in lower bits, flags in upper
        test    bl,SOLID_BRUSH
        jnz     short cdsc_its_inverse_solid_color
        test    bl,GREY_SCALE
        jz      short cdsc_blt_not_special_cased
        mov     bl,-1                   ;It's grey.  !!! What is this flag
        call    ega_solid_pat
        jmp     short cdsc_exit

cdsc_its_dn:
        call    do_wes_invert
        jmp     short cdsc_exit

cdsc_its_dpx:
        mov     ah,fr.brush_accel       ;color in lower bits, flags in upper
        test    ah,SOLID_BRUSH
        jnz     cdsc_its_solid_dpx      ;dpx with solid brush
        test    ah,GREY_SCALE           ;Otherwise must be a grey brush
        jz      short cdsc_blt_not_special_cased
        call    do_grey_dpx
        jmp     short cdsc_exit

cdsc_its_solid_dpx:
        call    do_wes_dpx_solidpat     ;solid color => can special case
        jmp     short cdsc_exit


;-----------------------------------------------------------------------;
; This is a source copy.  The phase must be zero to special case the
; source copy, and both devices must be the screen.
;
;       errnz      F0_SRC_IS_DEV - 00001000b
;       errnz    F0_SRC_IS_COLOR - 00000100b
;
; its_src_copy:
;       and     dh,F0_SRC_IS_DEV + F0_SRC_IS_COLOR
;       shiftr  dh,2
;       cmp     fr.phase_h,1            ; Gives CF if horizontal phase = zero
;       rcl     dh,1
; Now we have the needed flags in the lower 3 bits of DH
;                                       ; Src=EGA  Src=Color  Phase0
;
;       dw      do_wes_mono_trick       ;    0        0         0
;       dw      blt_not_a_special_case  ;    0        0         1
;       dw      blt_not_a_special_case  ;    0        1         0
;       dw      blt_not_a_special_case  ;    0        1         1
;       dw      do_wes_mono_trick       ;    1        0         0
;       dw      blt_not_a_special_case  ;    1        0         1
;       dw      blt_not_a_special_case  ;    1        1         0
;       dw      ega_src_copy            ;    1        1         1
;-----------------------------------------------------------------------;

cdsc_its_src_copy:
        test    dh,F0_SRC_IS_DEV
        jnz     short cdsc_exit

cdsc_source_is_memory:
        test    dh,F0_SRC_IS_COLOR          ;mono-mem to color-EGA conversion?
        jnz     cdsc_blt_not_special_cased  ;color to color, cannot special case it
        call    do_wes_mono_trick
        jmp     short cdsc_exit

cdsc_blt_not_special_cased:
        clc
        cRet    check_device_special_cases

cdsc_exit:
        stc
        cRet    check_device_special_cases


;----------------------------Private-Routine----------------------------;
; calc_parms
;
; Calculate the parameters needed for the output functions contained
; in this file.
;
; To avoid conditional jumps we will use some sick optimizations.
; Remember this:
;       adc     ax,-1           ; DEC AX if carry clear
;       sbb     ax,0            ; DEC AX if carry set
;       sbb     ax,-1           ; INC AX if carry clear
;       adc     ax,0            ; INC AX if carry set
;
; Entry:
;       EBP --> BitBLT local frame
; Returns:
;       ESI set to upper left of bitmap or pattern
;       EDI set to upper left
;       EDX = src bitmap width  (if present)
;       ECX = fr.yExt
;       EBX = offset into pattern (if pat present)
;       sets dest_right_edge
;       sets start_mask[0]
;       sets last_mask[0]
;       sets inner_loop_count
; Registers Destroyed:
;       EAX,flags
; Registers Preserved:
;       EBP
; Alters:
;
; Calls:
;       None
;-----------------------------------------------------------------------;

        .errnz  SIZE_PATTERN - 8                ; any power of 2 will work

        align   4
calc_parms:

; Left edge.

        movzx   edi,fr.DestxOrg         ;X origin in pixels, must be positive
        mov     ebx,edi
        mov     cl,7
        and     ecx,edi                 ;Save lower 3 bits
        mov     fr.phase_h,cl
        shr     edi,3                   ;EDI set for left edge
        mov     al,0FFh
        shr     al,cl
        mov     byte ptr fr.start_mask[0],al

; Right edge.

        movzx   eax,fr.xExt
        add     ebx,eax                 ;Right edge in pixels
        mov     cl,7
        and     cl,bl                   ;save lower 3 bits
        shr     ebx,3                   ;convert to bytes
        mov     fr.start_fl,ebx         ;dest_right_edge (reuse stk variable)
        mov     al,0FFh
        shr     al,cl
        not     al

; Check if the BLT does not cross any byte boundaries.

        sub     ebx,edi                 ;make EBX # bytes including left edge
        jnz     crosses_byte_boundary
        and     byte ptr fr.start_mask[0],al
        xor     al,al

; There are 2 cases where we get zero for fr.inner_loop_count:
; When the start and end bytes are adjacent and when they are
; the same byte.  In the latter case we get -1 for
; fr.inner_loop_count so INC BX now so it will be zero.

        inc     ebx
crosses_byte_boundary:

        cmp     al,0FFh
        sbb     al,-1                   ;AL=FF -> AL=0 (put in innerloop)
        mov     byte ptr fr.last_mask[0],al

; Inner loop  --  combine edge bytes into inner loop if they are
; full bytes.

        mov     fr.end_fl,ebx           ;src_right_edge (reuse stk variable)
        mov     al,byte ptr fr.start_mask[0]
        cmp     al,0FFh

; If gl_start_mask = FF the carry is clear, otherwise carry is set.
; We want to DEC BX if carry set because we have already included
; the left edge byte in BX, but we shouldn't have included it if
; it's only a partial byte.

        sbb     ebx,0
        cmp     al,0FFh

; If gl_start_mask = FF the carry is clear, otherwise carry is set.
; We want to INC AL (zero it) if it is FF (carry clear) because we
; will do this edge as part of the innerloop.

        sbb     al,-1
        mov     byte ptr fr.start_mask[0],al
        mov     fr.inner_loop_count,ebx
        movzx   eax,fr.DestyOrg
        imul    eax,ulNextScan_global
        add     edi,eax
        mov     esi,fr.pdsurfDst
        add     edi,[esi].dsurf_pvBitmapStart

; The source.

        test    fr.the_flags,F0_SRC_PRESENT
        jz      no_source
        mov     esi,fr.pdsurfSrc
        mov     ecx,[esi].dsurf_lNextScan   ;!!! will this be correct ??
        mov     esi,[esi].dsurf_pvBitmapStart

; Left edge.

        movzx   ebx,fr.SrcxOrg
        mov     dl,7
        and     dl,bl                   ;get lower 3 bits ( Src Mod 8 )
        sub     fr.phase_h,dl           ;phase def'd as Mod8[gl_dest]-
        shr     ebx,3                   ;               Mod8[gl_src]
        add     esi,ebx                 ;ESI set for left edge
        movzx   eax,fr.SrcyOrg
        mul     ecx
        add     esi,eax
        add     fr.end_fl,esi           ;src_right_edge (reuse stack variable)
        mov     edx,ecx
        jmp     short   no_pattern
no_source:
        test    fr.the_flags,F0_PAT_PRESENT ; assuming P or S but not both
        jz      no_pattern
        mov     esi,fr.lpPBrush
        movzx   ebx,fr.DestyOrg
        and     ebx,SIZE_PATTERN - 1
no_pattern:
        movzx   ecx,fr.yExt
        PLAIN_RET


;----------------------------Private-Routine----------------------------;
; ega_solid_pat
;
; EGA special case for solid color pattern copy.
;
; The following routine is invoked instead of generating code for a
; pattern copy.  The actual time involved in executing the pattern
; copy as static code as compared to compiled code is a win.
;
; This code can only be used if the pattern is a solid color or a grey,
; and the operation is to the screen.  In this case, the three bits of
; color stored in the accelerator byte of the brush will be used, or the
; bits of the grey brush.
;
; The logic operations which will invoke this routine are:
;
;       P
;       Pn
;       DDx
;       DDxn
;
; Entry:
;       BL = color to write or xor value for a grey pattern
;       CX = Mode register value (sort of)
;       EBP = BitBLT local variable frame
; Returns:
;       Nothing
; Registers Destroyed:
;       ALL except EBP
; Registers Preserved:
;       EBP
; Calls:
;       None
;-----------------------------------------------------------------------;

        align   4
ega_solid_pat:

; Instead of pushing and popping the destination pointer and adding in
; the fr.dest.Incr, the bias needed for adjusting the pointer at the
; end of a scan line will be computed and used.
;
; Since this is a pattern copy, the fr.dest.Incr will be positive.

;       mov     esi,edi                 ;Get destination increment
;       sub     esi,1                   ;Adjust for first byte
        lea     esi,-1[edi]
        sub     esi,fr.inner_loop_count ;Compute number of bytes to copy

; Put color in Set/Reset if it is a solid color.

        mov     dx,EGA_BASE + GRAF_ADDR
        or      cx,cx
        jz      not_solid_color
        mov     ax,MM_ALL * 256 + GRAF_ENAB_SR
        out     dx,ax
        mov     ah,bl
        mov     al,GRAF_SET_RESET
        out     dx,ax
not_solid_color:
        mov     al,GRAF_BIT_MASK        ;Leave graphics controller pointing
        out     dx,al                   ;  to the bitmask register, which
        inc     dx                      ;  is where cursor leaves it too

; Set up for the loop.

        mov     edi,fr.dest.lp_bits     ;--> destination
        mov     fr.phase_h,bl           ;Save color to write or grey XOR mask

ega_solid_pat_20:
        mov     al,fr.phase_h               ;Get the color to write
        test    fr.brush_accel,SOLID_BRUSH  ;Grey scale brush?
        jnz     ega_solid_pat_30            ;  No, a solid color
        mov     bl,fr.pat_row               ;Get scan of brush
        inc     bl                          ;  and update brush pointer
        mov     fr.pat_row,bl
        dec     bl
        and     ebx,00000111b
        add     ebx,fr.lpPBrush
        xor     al,bptr [ebx]               ;Invert if needed

ega_solid_pat_30:
        mov     bl,al
        mov     al,bptr fr.start_mask[1]    ;Set bitmask for first byte
        out     dx,al
        mov     al,bl
        xchg    al,[edi]                    ;xchg to load EGA's latches first
        inc     edi                         ;PAT_COPY step +X always!

        mov     ecx,fr.inner_loop_count     ;Set count for innerloop
        jecxz   ega_solid_pat_40            ;No innerloop or last byte
        mov     al,0FFh                     ;Inner loop alters all bits
        out     dx,al
        mov     al,bl
        rep     stosb

ega_solid_pat_40:
        mov     al,bptr fr.last_mask[1]     ;Last byte?
        or      al,al
        jz      ega_solid_pat_50            ;No last byte
        out     dx,al
        xchg    bl,[edi]

ega_solid_pat_50:
        add     edi,esi                     ;--> next destination
        dec     fr.yExt                     ;Any more scans to process?
        jnz     ega_solid_pat_20            ;  Yes
        PLAIN_RET


;----------------------------Private-Routine----------------------------;
; do_wes_invert
; do_wes_dpx_solidpat
;
; Entry:
;       EBP --> BitBLT local variable frame
;       AH = color of solid-pat.
; Returns:
;       Nothing
; Registers Destroyed:
;       ALL but EBP
; Registers Preserved:
;       EBP
; Calls:
;       calc_parms
;       edge_invert
;       invert
;-----------------------------------------------------------------------;

        align   4
do_wes_invert:

        mov     ah,0Fh                  ; black

do_wes_dpx_solidpat     label near

; Setup SET_RESET.

        mov     dx,EGA_BASE + GRAF_ADDR
        mov     al,GRAF_SET_RESET
        out     dx,ax
        mov     ax,0F00h + GRAF_ENAB_SR ; enable all planes
        out     dx,ax

; Go to XOR mode.

        mov     ax,GRAF_DATA_ROT + 256 * DR_XOR
        out     dx,ax

        call    calc_parms
        mov     ah,byte ptr fr.start_mask[0]
        or      ah,ah
        jz      no_left_invert_edge
        push    edi
        call    edge_invert
        pop     edi
        inc     edi

no_left_invert_edge:
        mov     ebx,fr.inner_loop_count
        or      ebx,ebx
        jz      no_inner_invert_loop
        movzx   ecx,fr.yExt
        push    edi
        call    invert
        pop     edi
        add     edi,fr.inner_loop_count

no_inner_invert_loop:
        mov     ah,byte ptr fr.last_mask[0]
        or      ah,ah
        jz      no_last_invert_edge
        movzx   ecx,fr.yExt
        call    edge_invert

no_last_invert_edge:
        PLAIN_RET


;----------------------------Private-Routine----------------------------;
;do_solid_patcopy() is called to copy (PatCopy) a solid brush directly to
;the screen.
;
;Entry:
;   BL      color to write with
;   EDI     fr.dest.next_scan which is equal to width_b
;   EBP     local varible frame
;
;-----------------------------------------------------------------------;

        align   4
do_solid_patcopy:

        cmp     fr.inner_loop_count,0   ;if zero, let ega_solid_pat do the work
        jne     dsp_do_it
        call    ega_solid_pat
        PLAIN_RET

dsp_do_it:
        sub     eax,eax                 ;accumulate flags in AL
        mov     esi,edi                 ;SI: fr.dest.next_scan (must be > 0)
        mov     edi,fr.dest.lp_bits     ;EDI-->first byte to write to
        mov     bh,bptr fr.start_mask[1]
        cmp     ah,bh                   ;is there a left edge?
        rcl     al,1                    ;CY if there is an edge to draw
        sub     bh,0ffh                 ;is the left edge an entire byte wide?
        neg     bh                      ;CY if less than a byte
        rcl     al,1                    ;accumulate flag into AL
        mov     bh,bptr fr.last_mask[1]
        cmp     ah,bh                   ;is there a right edge?
        rcl     al,1                    ;CY if there is an edge to draw
        sub     bh,0ffh                 ;is right edge an entire byte wide?
        neg     bh                      ;CY if less than a byte
        rcl     al,1                    ;accumulate flag into AL
        test    al,04h                  ;set all pixels in left byte?
        jnz     dsp_keep_left_edge      ;no. Do normal stuff
        inc     fr.inner_loop_count     ;left edge is an entire byte. INC inner
        and     al,0f7h                 ;loop cnt and draw it the fast way

dsp_keep_left_edge:
        test    al,01h                  ;set all pixels in right byte?
        jnz     dsp_keep_right_edge     ;no. Do normal stuff
        inc     fr.inner_loop_count     ;need to set all pixels in right byte.
        and     al,0fdh                 ;inc inner loop cnt do it the fast way

dsp_keep_right_edge:
        test    al,0ah                  ;any edges to draw?
        jz      dsp_draw_core_piece     ;no, just do the main chunk

dsp_draw_edges:
        mov     bh,al                   ;save flags in BH
        mov     dx,EGA_BASE + GRAF_ADDR
        mov     ax,MM_ALL * 256 + GRAF_ENAB_SR
        out     dx,ax                   ;enable writing to all planes at once
        mov     ah,bl
        mov     al,GRAF_SET_RESET
        out     dx,ax                   ;program to color value to write
        mov     al,GRAF_BIT_MASK        ;Leave graphics controller pointing
        out     dx,al                   ;  to the bitmask register, which
        inc     dx                      ;  is where cursor leaves it too

        test    bh,08h                  ;need to draw the left edge?
        jz      dsp_draw_right_edge

        push    edi                     ;save destination offset
        mov     al,bptr fr.start_mask[1]
        out     dx,al                   ;get it to the board
        movzx   ecx,fr.yExt

dsp_left_edge_draw_loop:
        mov     al,bl                   ;copy color index into AL
        xchg    al,[edi]                ;load latches, copy color index
        add     edi,esi
        loop    dsp_left_edge_draw_loop
        pop     edi                     ;restore dest offset
        inc     edi                     ;update to new draw position


dsp_draw_right_edge:
        test    bh,02h                  ;is there a right edge to draw?
        jz      dsp_reset_registers     ;no.  Restore default settings
        push    edi                     ;save updated dest offset
        add     edi,fr.inner_loop_count ;go to the right hand edge
        mov     al,bptr fr.last_mask[1]
        out     dx,al                   ;get it to the board
        movzx   ecx,fr.yExt

dsp_right_edge_draw_loop:
        mov     al,bl                   ;copy color index into AL
        xchg    al,[edi]                ;load latches, copy color index
        add     edi,esi
        loop    dsp_right_edge_draw_loop
        pop     edi                     ;restore dest offset


dsp_reset_registers:
        mov     al,0ffh                 ;allow writing to all bits in the byte
        out     dx,al                   ;this is the default value

dsp_draw_core_piece:
        sub     esi,fr.inner_loop_count ;account for EDI being incr. by stosb
        mov     dx,EGA_BASE+SEQ_DATA    ;time to copy pattern to board  DX=3C5h
        mov     al,01h
        mov     ecx,4

dsp_load_latches_loop:
        out     dx,al                   ;select the next plane to write to
        shr     bl,1                    ;move plane bit into carry
        sbb     ah,ah                   ;expand into AH
        mov     [edi],ah                ;copy it to the bit plane
        shl     al,1                    ;update plane selector
        loop    dsp_load_latches_loop   ;do all 4 planes

        mov     al,MM_ALL               ;to enable all four planes
        out     dx,al                   ;enable all planes
        mov     dx,EGA_BASE+GRAF_ADDR   ;DX=3CEh
        mov     ax,GRAF_BIT_MASK        ;AH=0 ie., copy data from latches, AL=8
        out     dx,ax                   ;ignore CPU data on write to board

        mov     al,[edi]                ;load the latches
        movzx   eax,fr.yExt             ;initialize loop counter
        mov     ebx,fr.inner_loop_count ;initialize rep counter value

        align   4
dsp_pat_blt_loop:
        mov     ecx,ebx                 ;ECX: repeat count
        test    edi,1
        jz      short dsp_pat_blt_aligned2
        stosb
        dec     ecx
dsp_pat_blt_aligned2:
        shr     ecx,1
        rep     stosw
        adc     ecx,ecx
        rep     stosb
        add     edi,esi                 ;point to next scanline
        dec     eax
        jnz     dsp_pat_blt_loop

        PLAIN_RET


;----------------------------Private-Routine----------------------------;
; do_wes_patblt
;
; Entry:
;       EBP --> BitBLT local variable frame
; Returns:
;       Nothing
; Registers Destroyed:
;       All but EBP
; Registers Preserved:
;       EBP
; Calls:
;       calc_parms
;       edge_pat_blt
;       pat_blt
;-----------------------------------------------------------------------;

        align   4
do_wes_patblt:

        call    calc_parms
        mov     ah,byte ptr fr.start_mask[0]
        or      ah,ah
        jz      no_left_pat_edge
        push    esi
        push    ebx
        call    edge_pat_blt            ; preserves DI
        pop     ebx
        pop     esi
        inc     edi

no_left_pat_edge:
        mov     edx,fr.inner_loop_count
        or      edx,edx
        jz      no_inner_pat_loop
        movzx   ecx,fr.yExt
        push    edi
        push    esi
        push    ebx
        call    pat_blt
        pop     ebx
        pop     esi
        pop     edi
        add     edi,fr.inner_loop_count

no_inner_pat_loop:
        mov     ah,byte ptr fr.last_mask[0]
        or      ah,ah
        jz      no_last_pat_edge
        movzx   ecx,fr.yExt
        call    edge_pat_blt

no_last_pat_edge:
        PLAIN_RET


;----------------------------Private-Routine----------------------------;
; do_wes_mono_trick
;
; Entry:
;       EBP --> BitBLT local variable frame
; Returns:
;       Nothing
; Registers Preserved:
;       EBP
; Registers Destroyed:
;       All but EBP
; Calls:
;       calc_parms
;       left_edge_mono_to_color_blt
;       right_edge_mono_to_color_blt
;       mono_to_color_blt
;-----------------------------------------------------------------------;

        align   4
do_wes_mono_trick:

        call    calc_parms
        push    edx
        mov     ah,byte ptr fr.start_mask[0]
        or      ah,ah
        jz      no_left_edge
        push    edi
        push    esi
        push    edx
        mov     al,fr.phase_h
        mov     bx,fr.both_colors
;-      mov     ecx,fr.yExt
        call    left_edge_mono_to_color_blt
        pop     edx
        pop     esi
        pop     edi
        inc     esi
        inc     edi
no_left_edge:
        mov     ebx,edx
        mov     edx,fr.inner_loop_count
        mov     ecx,edi                   ; compute/save the right-hand edge
        add     ecx,edx
        push    ecx
        or      edx,edx
        jz      no_inner_loop
        sub     ebx,edx
        movzx   ecx,fr.yExt
        mov     al,fr.phase_h
        cbw
        push    ebp
        mov     bp,fr.both_colors
        xchg    bp,ax
        call    mono_to_color_blt
        pop     ebp

no_inner_loop:
        pop     edi
        pop     edx
        mov     ah,byte ptr fr.last_mask[0]
        or      ah,ah
        jz      no_last_edge

        mov     ecx,fr.inner_loop_count
        mov     esi,fr.end_fl           ; src_right_edge (reuse stk variable)
        movzx   ecx,fr.yExt
        mov     bx,fr.both_colors
        mov     al,fr.phase_h
        call    right_edge_mono_to_color_blt

no_last_edge:
        PLAIN_RET


;----------------------------Private-Routine----------------------------;
; mono_to_color_blt
;
; This does phase-0, byte-aligned, mem-mono to ega-color blt.
;
; The Problem: copy to the ega a bitmap where "0"s in the bitmap mean
; color1 and "1"s in the bitmap mean color2, where color1 and color2
; are arbitrary colors.
;
; The solution:
;
;                       plane0  plane1  plane2  plane3
;
; color1                1       1       0       0
; color2                1       0       1       0
; SetResetEnable        1       0       0       1
; SetReset              0       x       x       0
; latches               1       1       0       0       (=color1)
;
; Now with datarot = XOR we get
;
;  when databit=0       1       1       0       0       (=color1)
;  when databit=1       1       0       1       0       (=color2)
;
;
; Entry:
;       BP  = phase ( -7 to 7)   (high byte ignored)
;       AL  = background color ( "1" bits in mono-bitmap )
;       AH  = foreground color
;       EBX = SI wrap
;       ESI = Mono Bitmap first byte
;       EDI = First EGA Byte
;       ECX = Number of scan lines
;       EDX = bytes per scan line
;       GRAF_DATA_ROT = DR_SET
;       All Planes Enabled
; Returns:
;       Nothing
; Registers Destroyed:
;       ALL
; Registers Preserved:
;       None
; Alters:
;       GRAF_SET_RESET
;       GRAF_ENAB_SR
;       GRAF_BIT_MASK
; Calls:
;       None
;-----------------------------------------------------------------------;

        align   4
mono_to_color_blt:

        push    ebp                     ; phase
        push    edx                     ; bytes per scan line
        push    ebx                     ; wrap for SI
        mov     ebx,eax                 ; colors

; First we put the foreground color into the latches.  We do this
; by putting this color into SET_RESET, writing it, then reading it.
; The memory location we will use is the first byte where we will blt.

        mov     dx,EGA_BASE + GRAF_ADDR
        mov     al,GRAF_SET_RESET
        out     dx,ax
        mov     ax,0F00h + GRAF_ENAB_SR
        out     dx,ax

; Set bit mask = FF.
        mov     ax,0FF00h + GRAF_BIT_MASK
        out     dx,ax

; Fill the latches.
        mov     [edi],al                ; color in SetReset is written, not AL
        mov     al,[edi]                ; read to fill latches

; Go to XOR mode.
        mov     ax,GRAF_DATA_ROT + 256 * DR_XOR
        out     dx,ax

; Now setup SET_RESET.

        mov     eax,ebx                 ; restore colors
        xor     ah,al                   ; gives 0 where colors match
        mov     al,GRAF_SET_RESET
        out     dx,ax
        not     ah
        mov     al,GRAF_ENAB_SR
        out     dx,ax                   ; enable Set/Reset where colors match

        pop     ebp                     ; wrap for ESI
        pop     edx                     ; bytes per scan
        mov     ebx,ulNextScan_global
        sub     ebx,edx                 ; BX = wrap

        mov     eax,ecx                 ; loop count
        pop     ecx                     ; phase
        or      cl,cl
        js      phase_neg
        jz      phase_zero
        dec     esi
;*      dec     ebp
pmono_to_color_loop:
        push    eax
        push    edx
pnext_byte:
        lodsw
        dec     esi
        xchg    al,ah
        shr     ax,cl
;+      shl     ax,cl
        stosb
        dec     edx
        jnz     pnext_byte
        pop     edx
        pop     eax
        add     edi,ebx
        add     esi,ebp
        dec     eax
        jnz     pmono_to_color_loop
        jmp     leave_in_set_mode

phase_zero:
zmono_to_color_loop:
        mov     ecx,edx
        shr     ecx,1
        rep     movsw
        rcl     ecx,1
        rep     movsb
        add     edi,ebx
        add     esi,ebp
        dec     eax
        jnz     zmono_to_color_loop
        jmp     leave_in_set_mode

phase_neg:
        neg     cl                      ; make CX = abs phase
nmono_to_color_loop:
        push    eax
        push    edx
nnext_byte:
        lodsw
        dec     esi
        rol     ax,cl
;+      shr     ax,cl
        stosb
        dec     edx
        jnz     nnext_byte
        pop     edx
        pop     eax
        add     edi,ebx
        add     esi,ebp
        dec     eax
        jnz     nmono_to_color_loop

leave_in_set_mode:
        mov     dx,EGA_BASE + GRAF_ADDR
        mov     ax,GRAF_DATA_ROT + 256 * DR_SET
        out     dx,ax
        PLAIN_RET


;----------------------------Private-Routine----------------------------;
; left_edge_mono_to_color_blt
; right_edge_mono_to_color_blt
;
; This problem here is the same as in mono_to_color_blt, except it
; is complicated by the need to preserve what is already in EGA memory
; for part of the byte which we are writing.
;
; We will set the BIT MASK to preserve these bytes.  We will then read
; the data from memory, and write it to the EGA using an XCHG so the
; latches are filled before the write -- so the appropriate EGA bits
; are preserved.
;
; The method for writing the data involves two passes.  The first pass
; writes the data to some of the planes, the second pass writes NOT the
; data to the other planes.  Depending on the two colors involved we
; may be able to skip one of the two passes.
;
; Define BkColor = the color corresponding to "1" bits in the data.
; Define TextColor = the color corresponding to "0" bits in the data.
;
; We will use the Set/Reset register to take care of the planes where
; the colors match.  These planes will be ignored in the rest of this
; comment block.
;
; The first pass writes "1"s where the data is "1".  Therefore, the
; condition for doing the first pass is that the BkColor has a "1"
; somewhere (ignoring those planes taken care of by Set/Reset).
; The second pass does whatever planes remain.  We can skip this pass
; if no planes remain.  To maximize to likelihood of this we make sure
; that all "Set/Reset" planes are enabled on the first pass (if the
; first pass occurs).
;
; Entry:
;       AH  = bitmask
;       AL  = phase (-7 to +7)
;       BH  = foreground color
;       BL  = background color
;       EDX = src bitmap width in bytes
;       ESI = Mono Bitmap first byte
;       EDI = First EGA Byte
;       ECX = Number of scan lines
;       DATA_ROT = DR_SET
; Returns:
;       Nothing
; Registers Destroyed:
;       ALL but EBP
; Registers Preserved:
;       EBP
; Alters:
;       GRAF_SET_RESET
;       GRAF_BIT_MASK
;       GRAF_ENAB_SR
; Calls:
;       None
;-----------------------------------------------------------------------;

; Does left edge, which never requires more than 1 byte, and can fault with
; the 2-byte approach used by the right edge.

        align   4
left_edge_mono_to_color_blt:

        push    ebp
        push    eax              ; AL = phase
        mov     ebp,edx

; Set bit mask.

        mov     dx,EGA_BASE + GRAF_ADDR
        mov     al,GRAF_BIT_MASK
        out     dx,ax

; Put foreground color in Set/Reset and enable planes where colors
; match.

        mov     ah,bh
        mov     al,GRAF_SET_RESET
        out     dx,ax
        xor     ah,bl
        not     ah                      ; gives 1 where colors match
        mov     al,GRAF_ENAB_SR
        out     dx,ax
        mov     dx,EGA_BASE + SEQ_DATA  ; The rest of the OUTs are here.
        mov     al,ah
        not     ah                      ; Gives 1 where colors mismatch.

; The following AND leaves 1 bits in AH for the planes which
; CANNOT be done on the second pass.  So if this is zero we can
; skip the first pass.

        and     ah,bl                   ; BL = BkColor = color where data is 1
        or      ah,bl                   ; planes to enable
        mov     ebx,ecx                 ; we're done with the colors in BX
        pop     ecx                     ; phase
        jz      short left_skip_first_pass
        or      al,ah                   ; Include "Set/Reset" planes.
        out     dx,al                   ; Enable planes for first pass.

        push    ecx
        push    esi
        push    edi
        push    eax
        push    ebx
        or      cl,cl
        js      short left_phase_is_negative1
left_pfirst_pass:
        mov     ah,[esi]
        shr     ah,cl
        xchg    ah,[edi]
        add     esi,ebp
        add     edi,ulNextScan_global
        dec     ebx
        jnz     left_pfirst_pass
        jmp     short left_end_pass_one

left_phase_is_negative1:
        neg     cl                      ; make CL = abs phase
left_nfirst_pass:
        mov     ax,[esi]
        rol     ax,cl
        xchg    al,[edi]
        add     esi,ebp
        add     edi,ulNextScan_global
        dec     ebx
        jnz     left_nfirst_pass
left_end_pass_one:
        pop     ebx
        pop     eax
        pop     edi
        pop     esi
        pop     ecx

left_skip_first_pass:

; Enable the other planes.

        not     ah
        and     ah,MM_ALL
        jz      short left_no_planes_left
        mov     al,ah
        out     dx,al
        or      cl,cl
        js      short left_phase_is_negative2
left_psecond_pass:
        mov     ah,[esi]
        not     ah
        shr     ah,cl
        xchg    ah,[edi]
        add     esi,ebp
        add     edi,ulNextScan_global
        dec     ebx
        jnz     left_psecond_pass
        jmp     short left_no_planes_left

left_phase_is_negative2:
        neg     cl                      ; make CL = abs phase
left_nsecond_pass:
        mov     ax,[esi]
        not     ax
        rol     ax,cl
        xchg    al,[edi]
        add     esi,ebp
        add     edi,ulNextScan_global
        dec     ebx
        jnz     left_nsecond_pass

left_no_planes_left:
        mov     al,MM_ALL
        out     dx,al
        pop     ebp
        PLAIN_RET

; Does right edge, which may require 2 bytes. 2 bytes are always available,
; because if there was only 1 byte across, the left edge would handle it,
; so we don't have to worry about faulting.

        align   4
right_edge_mono_to_color_blt:

        push    ebp
        push    eax              ; AL = phase
        mov     ebp,edx

; Set bit mask.

        mov     dx,EGA_BASE + GRAF_ADDR
        mov     al,GRAF_BIT_MASK
        out     dx,ax

; Put foreground color in Set/Reset and enable planes where colors
; match.

        mov     ah,bh
        mov     al,GRAF_SET_RESET
        out     dx,ax
        xor     ah,bl
        not     ah                      ; gives 1 where colors match
        mov     al,GRAF_ENAB_SR
        out     dx,ax
        mov     dx,EGA_BASE + SEQ_DATA  ; The rest of the OUTs are here.
        mov     al,ah
        not     ah                      ; Gives 1 where colors mismatch.

; The following AND leaves 1 bits in AH for the planes which
; CANNOT be done on the second pass.  So if this is zero we can
; skip the first pass.

        and     ah,bl                   ; BL = BkColor = color where data is 1
        or      ah,bl                   ; planes to enable
        mov     ebx,ecx                 ; we're done with the colors in BX
        pop     ecx                     ; phase
        jz      short right_skip_first_pass
        or      al,ah                   ; Include "Set/Reset" planes.
        out     dx,al                   ; Enable planes for first pass.

        push    ecx
        push    esi
        push    edi
        push    eax
        push    ebx
        or      cl,cl
        js      short right_phase_is_negative1
        dec     esi
right_pfirst_pass:
        mov     ax,[esi]
        ror     ax,cl
        xchg    ah,[edi]
        add     esi,ebp
        add     edi,ulNextScan_global
        dec     ebx
        jnz     right_pfirst_pass
        jmp     short right_end_pass_one

right_phase_is_negative1:
        neg     cl                      ; make CL = abs phase
right_nfirst_pass:
        mov     ax,[esi]
        rol     ax,cl
        xchg    al,[edi]
        add     esi,ebp
        add     edi,ulNextScan_global
        dec     ebx
        jnz     right_nfirst_pass
right_end_pass_one:
        pop     ebx
        pop     eax
        pop     edi
        pop     esi
        pop     ecx

right_skip_first_pass:

; Enable the other planes.

        not     ah
        and     ah,MM_ALL
        jz      short right_no_planes_left
        mov     al,ah
        out     dx,al
        or      cl,cl
        js      short right_phase_is_negative2
        dec     esi
right_psecond_pass:
        mov     ax,[esi]
        not     ax
        ror     ax,cl
        xchg    ah,[edi]
        add     esi,ebp
        add     edi,ulNextScan_global
        dec     ebx
        jnz     right_psecond_pass
        jmp     short right_no_planes_left

right_phase_is_negative2:
        neg     cl                      ; make CL = abs phase
right_nsecond_pass:
        mov     ax,[esi]
        not     ax
        rol     ax,cl
        xchg    al,[edi]
        add     esi,ebp
        add     edi,ulNextScan_global
        dec     ebx
        jnz     right_nsecond_pass

right_no_planes_left:
        mov     al,MM_ALL
        out     dx,al
        pop     ebp
        PLAIN_RET


;----------------------------Private-Routine----------------------------;
; pat_blt
;                       XOR mode with data = FF for Pn?
;
; This BLTs an arbitrary 8x8 bit pattern (3 or 4 planes deep) to EGA.
;
; The method is simple.  Load the latches with the pattern for a
; particular scan line, then REP STOS this with the BIT MASK = 0
; so that only the latches get written.  Before putting the pattern
; for the next scan line into the latches we will do all other scan
; lines with the same pattern.
;
; Entry:
;       ESI = pattern bytes
;       EDI = First EGA Byte
;       ECX = Number of scan lines (yExt)
;       EBX = offset into pattern
;       EDX = bytes per scan line (scan_len)
;       GRAF_DATA_ROT = DR_SET
;       BIT_MASK = FF
; Returns:
;       Nothing
; Registers Destroyed:
;       ALL but EBP
; Registers Preserved:
;       EBP
; Alters:
;       GRAF_BIT_MASK   (leaves it 00)
; Calls:
;       None
;-----------------------------------------------------------------------;

        .errnz  SIZE_PATTERN - 8        ; actually any power of 2 is okay.

        align   4
pat_blt:

        push    ebp
        push    edx                     ; scan_len
        push    ecx                     ; yExt
        mov     ah,11h                  ; left nibble gives carry to end loop

; Set EBP = min(yExt, scans/pattern).

        sub     ecx,SIZE_PATTERN        ; SIZE_PATTERN = 8 = yExt of pattern
        sbb     ebp,ebp
        and     ebp,ecx
        add     ebp,SIZE_PATTERN

        mov     dx,EGA_BASE + SEQ_DATA
set_next_plane:
        push    ebx
        push    edi

; Enable next plane.

        mov     al,MM_ALL
        and     al,ah
        out     dx,al
        mov     ecx,ebp

hit_next_byte:
        mov     al,[esi][ebx]           ; Next pattern byte
        inc     ebx
        and     ebx,SIZE_PATTERN - 1
        mov     [edi],al
        add     edi,ulNextScan_global
        loop    hit_next_byte
        add     esi,SIZE_PATTERN
        pop     edi
        pop     ebx
        shl     ah,1
        jnc     set_next_plane

; Set bit mask = 00.

        mov     dx,EGA_BASE + GRAF_ADDR
        mov     ax,0000h + GRAF_BIT_MASK
        out     dx,ax

; Enable all planes.

        mov     al,MM_ALL
        mov     dx,EGA_BASE + SEQ_DATA
        out     dx,al

        mov     ecx,ebp                 ; MIN(yExt,SIZE_PATTERN)
        pop     ebp                     ; yExt
        pop     eax                     ; scan_len
        mov     esi,ulNextScan_global   ; ESI = scan_width

        .errnz  (SIZE_PATTERN - 8)
        ;------------------------------------------------;
        ;       mov     ebx,(SIZE_PATTERN - 1) * scan_width
        ;------------------------------------------------;
        lea     ebx,[esi*2+esi]         ; scan_width * 3
        lea     ebx,[ebx+esi*4]         ; scan_width * 7

        sub     esi,eax                 ; ESI = scan_width
        add     ebx,esi                 ; EBX = (scan_width * 7) + next_scan

pat_blt_next_scan:
        push    ecx
        mov     esi,edi                 ; save ESI
        mov     edx,ebp                 ; save yExt
        mov     cl,[edi]                ; load latches

pat_blt_loop:
        mov     ecx,eax                 ; EAX = scan_len
        rep     stosb
        add     edi,ebx                 ; EBX = (scan_width * 7) + next_scan
        sub     ebp,SIZE_PATTERN
        jg      pat_blt_loop

        mov     ebp,edx
        dec     ebp
        mov     edi,esi
        add     edi,ulNextScan_global
        pop     ecx
        loop    pat_blt_next_scan

        pop     ebp
        PLAIN_RET


;----------------------------Private-Routine----------------------------;
; edge_pat_blt
;
; Entry:
;       AH  = bitmask
;       ESI = pattern bytes
;       EDI = First EGA Byte
;       ECX = Number of scan lines (yExt)
;       EBX = offset into pattern
;       DATA_ROT = DR_SET
; Returns:
;       Nothing
; Registers Destroyed:
;       EAX,ECX,EDX,ESI,flags
; Registers Preserved:
;       EBX,EDI,EBP
; Alters:
;       GRAF_BIT_MASK   (leaves it FF)
; Calls:
;       None
;-----------------------------------------------------------------------;

        align   4
edge_pat_blt:

        push    ebp

; Set bit mask.

        mov     dx,EGA_BASE + GRAF_ADDR
        mov     al,GRAF_BIT_MASK
        out     dx,ax
        mov     ah,11h                  ; left nibble gives carry to end loop
        mov     dx,EGA_BASE + SEQ_DATA
        sub     si,SIZE_PATTERN
        mov     ebp,ecx

enable_next_plane:
        push    ebx
        push    edi
        mov     ecx,ebp                 ; yExt
        mov     al,MM_ALL
        and     al,ah
        out     dx,al
        add     esi,SIZE_PATTERN

over_scans:
        mov     al,[ebx][esi]           ; pattern fetch
        inc     ebx
        and     ebx,SIZE_PATTERN - 1    ; 7
        .errnz  SIZE_PATTERN - 8        ; any power of 2 works
        xchg    [edi],al
        add     edi,ulNextScan_global
        loop    over_scans

        pop     edi
        pop     ebx
        shl     ah,1
        jnc     enable_next_plane

; Restore bitmask to default.

        mov     dx,EGA_BASE + GRAF_ADDR
        mov     ax,0FF00h + GRAF_BIT_MASK
        out     dx,ax

        pop     ebp
        PLAIN_RET


;----------------------------Private-Routine----------------------------;
; invert
;
; Inverts pixels in a rectangle on the display, by simply writing the
; memory to itself, letting the EGA hardware perform the XORing.
;
; Entry:
;       EDI = First EGA Byte
;       ECX = Number of scan lines (yExt)
;       EBX = scan line length in bytes
;       DATA_ROT = DR_XOR
;       GRAF_SET_RESET = color to xor DEST with
;       GRAF_SR_ENAB = MM_ALL
; Returns:
;       Nothing
; Registers Destroyed:
;       EAX,ECX,EDX,ESI,EDI,flags
; Registers Preserved:
;       EBX,EBP
; Alters:
;       GRAF_BIT_MASK   (leaves it FF)
; Calls:
;       None
;-----------------------------------------------------------------------;

        align   4
invert:

; Set bit mask.

        mov     dx,EGA_BASE + GRAF_ADDR
        mov     ax,0FF00h + GRAF_BIT_MASK
        out     dx,ax
        mov     edx,ulNextScan_global
        sub     edx,ebx
        mov     eax,ecx                 ; save height

invert_next_scan:
        mov     esi,edi
        mov     ecx,ebx                 ; scan len in bytes
        rep     movsb
        add     edi,edx
        dec     eax
        jnz     invert_next_scan
        PLAIN_RET


;----------------------------Private-Routine----------------------------;
; edge_invert
;
; Inverts one byte on each scan line vertically according to the mask
; in AH.
;
; Entry:
;       AH  = bitmask
;       EDI = First EGA Byte
;       ECX = Number of scan lines (yExt)
;       DATA_ROT = DR_XOR
;       GRAF_SET_RESET = color to xor DEST with
;       GRAF_ENAB_SR = MM_ALL
; Returns:
;       Nothing
; Registers Destroyed:
;       AL,ECX,EDX,EDI,flags
; Registers Preserved:
;       AH,EBX,ESI,EBP
; Alters:
;       GRAF_BIT_MASK
; Calls:
;       None
;-----------------------------------------------------------------------;

        align   4
edge_invert:

; Set bit mask.

        mov     dx,EGA_BASE + GRAF_ADDR
        mov     al,GRAF_BIT_MASK
        out     dx,ax

edge_invert_next_scan:
        xchg    [edi],al
        add     edi,ulNextScan_global
        loop    edge_invert_next_scan
        PLAIN_RET


;----------------------------Private-Routine----------------------------;
; edge_grey_dpx
;
; Inverts one or two bytes on each scan line vertically according
; to the grey pattern given, under the passed clipping mask.
;
; Entry:
;       EBX    =  brush index (0-7)
;       ECX    =  number of scan lines (cyExt)
;       DL     =  lhs clipping mask
;       DH     =  rhs clipping mask
;       ESI   --> base address of brush
;       EDI   --> rhs EGA
;       EBP   --> lhs EGA
; Returns:
;       Nothing
; Registers Destroyed:
;       All
; Registers Preserved:
;       None
; Alters:
;       None
; Calls:
;       None
;-----------------------------------------------------------------------;

        align   4
edge_grey_dpx:

        sub     ebp,edi                 ;Compute delta to lhs
        and     ebx,00000111b           ;Make sure brush is valid
        .errnz  SIZE_PATTERN - 8
        or      dh,dh                   ;Dispatch based on one or two edges
        jz      edge_grey_dpx_one_loop

edge_grey_dpx_both_loop:
        mov     al,[esi][ebx]           ;Get next byte of brush
        mov     ah,al
        and     al,dl                   ;Mask with lhs clipping mask
        xchg    al,[edi]                ;Invert necessary bits
        inc     ebx                     ;--> next brush byte
        and     bl,00000111b            ;Handle any wrap
        .errnz  SIZE_PATTERN - 8
        and     ah,dh                   ;Mask with rhs clipping mask
        xchg    ah,[edi][ebp]           ;Invert necessary bits
        add     edi,ulNextScan_global   ;--> next destination byte
        loop    edge_grey_dpx_both_loop
        PLAIN_RET

edge_grey_dpx_one_loop:
        mov     al,[esi][ebx]           ;Get next byte of brush
        and     al,dl                   ;Mask with lhs clipping mask
        xchg    al,[edi]                ;Invert necessary bits
        inc     ebx                     ;--> next brush byte
        and     bl,00000111b            ;Handle any wrap
        .errnz  SIZE_PATTERN - 8
        add     edi,ulNextScan_global   ;--> next destination byte
        loop    edge_grey_dpx_one_loop
        PLAIN_RET


;----------------------------Private-Routine----------------------------;
; middle_grey_dpx
;
; Inverts a rectangle on the display using the passed grey pattern.
;
; Entry:
;       EBX     =  brush index (0-7)
;       ECX     =  # byte to invert on the scan
;       EDX     =  EGA_BASE + GRAF_ADDR
;       ESI    --> base address of brush
;       EDI    --> starting byte
;       EBP     =  number of scan lines (cyExt)
;       DATA_ROT       = DR_XOR
;       GRAF_SET_RESET = All 1
;       GRAF_ENAB_SR   = MM_ALL
; Returns:
;       Nothing
; Registers Destroyed:
;       EAX,ECX,EDX,ESI,EDI,flags
; Registers Preserved:
;       EBX,EBP
; Alters:
;       GRAF_BIT_MASK   (leaves it FF)
; Calls:
;       None
;-----------------------------------------------------------------------;

        align   4
middle_grey_dpx:

;       mov     dx,EGA_BASE + GRAF_ADDR ; Leave the Graphics controller
        mov     al,GRAF_BIT_MASK        ;   address register pointing to
        out     dx,al                   ;   the bitmask register
        inc     edx                     ; --> Graphics controller data register
        .errnz  GRAF_DATA - GRAF_ADDR - 1

        mov     ah,bl                   ; Keep brush index here
        mov     ebx,ecx                 ; Save a copy of inner loop count here

middle_grey_dpx_loop:
        xchg    eax,ebx
        xchg    bl,bh
        and     ebx,00000111b
        errnz   SIZE_PATTERN-8
        mov     bh,[esi][ebx]           ; Get next byte of brush
        inc     bl                      ; --> byte of the brush
        xchg    bh,bl
        xchg    eax,ebx
        out     dx,al
        mov     ecx,ebx
        push    esi
        mov     esi,edi
        rep     movsb
        pop     esi
        sub     edi,ebx
        add     edi,ulNextScan_global   ;next scan on screen
        dec     ebp
        jnz     middle_grey_dpx_loop
        PLAIN_RET


;----------------------------Private-Routine----------------------------;
; do_grey_dpx
;
; This is EGA special cased code for the dpx raster op in the case where
; the pattern (p) is grey (the same on all planes).  We also come here
; for the special graying pattern rop.  This is not a normal P,S, and D rop,
; but a hack for pmwin.  It allows graying of things on the screen, meaning
; the background color is stuffed everywhere the pattern has a "1" bit.
;
; Entry:
;       EBP --> BitBLT local variable frame
; Returns:
;       Nothing
; Registers Destroyed:
;       ALL but EBP
; Registers Preserved:
;       EBP
; Calls:
;       calc_parms
;       edge_grey_dpx
;       middle_grey_dpx
;-----------------------------------------------------------------------;

        align   4
do_grey_dpx:

        call    calc_parms
        mov     dx,EGA_BASE + GRAF_ADDR
        mov     ax,DR_XOR shl 8 + GRAF_DATA_ROT ; XOR mode for grey dpx
        out     dx,ax
        mov     dl,byte ptr fr.start_mask[0]
        mov     dh,byte ptr fr.last_mask[0]
        or      dx,dx
        jz      do_grey_dpx_middle      ;Only middle bytes exist

        mov     eax,edi                 ;Assume we have a left edge
        inc     eax                     ;+1 to get to start of middle bytes
        or      dl,dl                   ;Is there really a left edge?
        jnz     do_grey_dpx_have_lhs    ;  Yes, AX = correct middle byte start
        dec     eax                     ;  No, restore middle byte start
        xchg    dl,dh                   ;  Pretend only a lhs edge
        add     edi,fr.inner_loop_count ;    but make it be the rhs

do_grey_dpx_have_lhs:
        push    eax                     ;Save middle bytes start
        push    ebx
        push    ecx
        push    esi
        push    ebp
        add     eax,fr.inner_loop_count ;--> possible rhs
        xchg    eax,ebp
        call    edge_grey_dpx
did_gray_pat_edge:
        pop     ebp
        pop     esi
        pop     ecx
        pop     ebx
        pop     edi                     ;Restore middle bytes start

do_grey_dpx_middle:
        mov     eax,fr.inner_loop_count
        or      eax,eax
        jz      do_grey_dpx_exit
        push    ebp
        mov     ebp,ecx
        xchg    eax,ecx
        mov     dx,EGA_BASE + GRAF_ADDR         ;
        mov     ax,MM_ALL shl 8 + GRAF_ENAB_SR  ; Set Set/Reset to all "1" bits
        out     dx,ax                           ; and Enable all planes (for
        .errnz  MM_ALL - 0Fh                    ; grey_dpx)
        mov     al,GRAF_SET_RESET               ;
        out     dx,ax                           ;

gray_pat:
        call    middle_grey_dpx
        pop     ebp

do_grey_dpx_exit:
        PLAIN_RET


endProc check_device_special_cases

_TEXT$01   ends

        end

