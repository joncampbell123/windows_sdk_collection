;---------------------------Module-Header------------------------------;
; Module Name: srccopy.asm
;
; Copyright (c) 1993 Microsoft Corporation
;-----------------------------------------------------------------------;

;-----------------------------------------------------------------------;
; VOID vSrcCopy8bpp(ppdev, psoSrc, prclDst, pptlSrc, lSrcDelta, pvSrcStart);
;
; Input:
;
;  ppdev      - screen pdev
;  psoSrc     - source surface
;  prcldest   - pointer to destination rectangle
;  pptlsrc    - pointer to source upper left corner
;  lSrcDelta  - offset from start of one scan to next in source
;  pvSrcStart - pointer to start of bitmap
;
; Performs 8bpp SRCCOPY memory-to-screen blts.
;
;-----------------------------------------------------------------------;
; NOTE: Assumes all rectangles have positive heights and widths. Will
; not work properly if this is not the case.
;-----------------------------------------------------------------------;

;-----------------------------------------------------------------------;
; Set LOOP_UNROLL_SHIFT to the log2 of the number of times you want loops in
; this module unrolled. For example, LOOP_UNROLL_SHIFT of 3 yields 2**3 = 8
; times unrolling. This is the only thing you need to change to control
; unrolling.

LOOP_UNROLL_SHIFT equ 2

;-----------------------------------------------------------------------;

        .386

        .model  small,c

        assume cs:FLAT,ds:FLAT,es:FLAT,ss:FLAT
        assume fs:nothing,gs:nothing

        .xlist
        include stdcall.inc             ;calling convention cmacros
        include i386\strucs.inc
        include i386\driver.inc
        include i386\unroll.inc
        include i386\egavga.inc

        .list

;-----------------------------------------------------------------------;

        .data

;-----------------------------------------------------------------------;

        .code

;-----------------------------------------------------------------------;

cProc   vSrcCopy8bpp,20,<       \
        uses esi edi ebx,       \
        ppdev:      ptr PDEV,   \
        prclDst:    ptr RECTL,  \
        pptlSrc:    ptr POINTL, \
        lSrcDelta:  dword,      \
        pvSrcStart: ptr         >

        local culMiddle:    dword ;# of dwords in middle
        local cyToGo:       dword ;# of scans to copy after the current bank
        local ppfnUnroll:   ptr   ;points to appropriate unroll loop table
        local pvSrc:        ptr   ;source pointer

        mov     esi,prclDst             ;esi = prclDest
        mov     ebx,ppdev               ;ebx = ppdev
        mov     edi,[esi].yTop

        cmp     edi,[ebx].pdev_rcl1WindowClip.yTop
        jl      short src8_map_init_bank

        mov     edx,[ebx].pdev_rcl1WindowClip.yBottom
                                        ;edx = ppdev->rcl1WindowClip.bottom
        cmp     edi,edx
        jl      short src8_init_bank_mapped

src8_map_init_bank:
        ptrCall <dword ptr [ebx].pdev_pfnBankControl>, \
                <ebx,edi,JustifyTop>

        mov     edx,[ebx].pdev_rcl1WindowClip.yBottom

src8_init_bank_mapped:
        mov     eax,[esi].yBottom
        sub     eax,edx
        mov     cyToGo,eax              ;eax = # scans to do after this bank
        sbb     ecx,ecx
        and     ecx,eax
        add     edx,ecx                 ;edx = min(prclDst->bottom,
                                        ;          ppdev->rcl1WindowClip.bottom)
        sub     edx,edi                 ;edx = # of scans to do in this bank

; ebx = ppdev
; edx = # of scans to do in this bank
; esi = prclDst

        mov     eax,[esi].xLeft
        add     eax,3
        and     eax,not 3               ;eax = xLeft aligned to next dword

        mov     edi,pptlSrc             ;edi = pptlSrc
        mov     ecx,lSrcDelta
        imul    ecx,[edi].ptl_y
        add     ecx,[edi].ptl_x
        add     ecx,eax
        sub     ecx,[esi].xLeft
        add     ecx,pvSrcStart
        mov     pvSrc,ecx               ;pvSrc = pptlSrc->y * lSrcDelta +
                                        ;        pptlSrc->x + dest alignment +
                                        ;        pvSrcStart

        mov     edi,[ebx].pdev_lNextScan
        imul    edi,[esi].yTop
        add     edi,eax
        add     edi,[ebx].pdev_pvBitmapStart
                                        ;edi = prclDst->top * ppdev->lNextScan +
                                        ; aligned left + ppdev->pvBitmapStart
                                        ; (the aligned destination address)

; eax = prclDst->left aligned to dword
; ebx = ppdev
; edx = # of scans to do in this bank
; esi = prclDst
; edi = destination address

        mov     ecx,[esi].xRight        ;esi = prclDst->right
        sub     ecx,eax                 ;ecx = length in bytes from first full
                                        ;      dword to last byte
        jl      short src8_one_dword    ;special case if the destination
                                        ; starts and ends in the same dword

        mov     eax,ecx
        and     ecx,not 3               ;ecx = length of middle dwords in bytes
        sub     eax,ecx                 ;eax = (right & 3)

        mov     esi,[esi].xLeft
        and     esi,3                   ;esi = (left & 3)
        shl     eax,2
        or      esi,eax                 ;esi = ((right & 3) << 2) | (left & 3)
                                        ; (look-up index for unroll loops)

        mov     ebx,[ebx].pdev_lNextScan
        sub     ebx,ecx                 ;ebx = ppdev->lNextScan
                                        ; - (culMiddle << 2)
                                        ; (destination delta)

        mov     eax,lSrcDelta
        sub     eax,ecx                 ;eax = lSrcDelta - (culMiddle << 2)
                                        ; (source delta)

        shr     ecx,2
        mov     culMiddle,ecx           ;culMiddle = number of middle dwords

; eax = source delta
; ebx = destination delta
; ecx =
; edx = # scans to do
; esi = flags
; edi = destination pointer

; Set up for loop unrolling:

        mov     ecx,edx
        add     edx,(1 shl LOOP_UNROLL_SHIFT)-1 ;round count up
        and     ecx,(1 shl LOOP_UNROLL_SHIFT)-1 ;fractional part of loop
        shr     edx,LOOP_UNROLL_SHIFT           ;number of whole loops

        shl     esi,LOOP_UNROLL_SHIFT+2 ;every unroll table is
                                        ; (1 << LOOP_UNROLL_SHIFT) dwords long
        add     esi,offset gapfnMasterCopyTable
        mov     ppfnUnroll,esi          ;save unroll table pointer for next bank

        mov     ecx,[esi+ecx*4]
        mov     esi,pvSrc
        jmp     ecx

;-----------------------------------------------------------------------;
; Here we handle cases where copy starts and ends in same dword:

        public  src8_one_dword
src8_one_dword:
        sub     eax,[esi].xLeft         ;eax = # of bytes from left edge to
                                        ; first dword
        add     ecx,eax                 ;ecx = # of bytes to do

        sub     edi,eax                 ;adjust back to start byte
        sub     pvSrc,eax               ;adjust accordingly

        mov     ebx,[ebx].pdev_lNextScan;ebx = ppdev->lNextScan
                                        ; (destination delta)

        mov     eax,lSrcDelta           ;eax = lSrcDelta
                                        ; (source delta)

; eax = source delta
; ebx = destination delta
; esi =
; edx = # scans to do
; ecx = flags
; edi = destination pointer

; Set up for loop unrolling:

        mov     esi,edx
        add     edx,(1 shl LOOP_UNROLL_SHIFT)-1 ;round count up
        and     esi,(1 shl LOOP_UNROLL_SHIFT)-1 ;fractional part of loop
        shr     edx,LOOP_UNROLL_SHIFT           ;number of whole loops

        dec     ecx                     ;adjust for table (no zero entry)
        shl     ecx,LOOP_UNROLL_SHIFT+2 ;every unroll table is
                                        ; (1 << LOOP_UNROLL_SHIFT) dwords long
        add     ecx,offset gapfnOneDwordCopyTable
        mov     ppfnUnroll,ecx          ;save unroll table pointer for next bank

        mov     ecx,[ecx+esi*4]
        mov     esi,pvSrc
        jmp     ecx

;-----------------------------------------------------------------------;
; We have following variables set before calling unrolled loops:
;
;   eax = source delta (from end of dwords to start of dwords on next scan)
;   ebx = destination delta
;   edx = # of unrolled loops
;   esi = source pointer
;   edi = destination pointer
;   culMiddle = number of dwords to copy

;-----------------------------------------------------------------------;
; See if done.  If not, get next bank.

        align   4
        public  src8_done
src8_done:
        cmp     cyToGo,0
        jg      short src8_next_bank
        cRet    vSrcCopy8bpp

src8_next_bank:
        push    esi
        push    ebx                     ;save some registers
        mov     ebx,ppdev
        push    eax

        mov     esi,[ebx].pdev_rcl1WindowClip.yBottom
        sub     edi,[ebx].pdev_pvBitmapStart

        ptrCall <dword ptr [ebx].pdev_pfnBankControl>, \
                <ebx,esi,JustifyTop>

        add     edi,[ebx].pdev_pvBitmapStart
        mov     edx,[ebx].pdev_rcl1WindowClip.yBottom
        sub     edx,esi                 ;edx = # scans can do in this bank

        mov     eax,cyToGo
        sub     eax,edx
        mov     cyToGo,eax              ;eax = # scans to do after this bank
        sbb     ecx,ecx
        and     ecx,eax
        add     edx,ecx                 ;edx = min(# of scans can do in bank,
                                        ;          # of scans to go)

        mov     ecx,edx
        mov     esi,ppfnUnroll          ;unroll table
        add     edx,(1 shl LOOP_UNROLL_SHIFT)-1 ;round count up
        and     ecx,(1 shl LOOP_UNROLL_SHIFT)-1 ;fractional part of loop
        shr     edx,LOOP_UNROLL_SHIFT           ;# of whole loops

        mov     ecx,[esi+ecx*4]

        pop     eax                     ;restore those registers
        pop     ebx
        pop     esi

        jmp     ecx

;-----------------------------------------------------------------------;
; We organize the unroll tables as follows so that it's easy to index
; into them:
;
;   Bits 2 and 3 = # of trailing bytes
;   Bits 0 and 1 = # of leading bytes to skip in first dword

        align   4
gapfnMasterCopyTable label dword
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW00Entry,W_00,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW30Entry,W_30,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW20Entry,W_20,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW10Entry,W_10,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW01Entry,W_01,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW31Entry,W_31,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW21Entry,W_21,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW11Entry,W_11,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW02Entry,W_02,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW32Entry,W_32,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW22Entry,W_22,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW12Entry,W_12,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW03Entry,W_03,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW33Entry,W_33,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW23Entry,W_23,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyWideW13Entry,W_13,LOOP_UNROLL_COUNT

        align   4
gapfnOneDwordCopyTable label dword
        UNROLL_LOOP_ENTRY_TABLE pfnCopyThin1Entry,T_1,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyThin2Entry,T_2,LOOP_UNROLL_COUNT
        UNROLL_LOOP_ENTRY_TABLE pfnCopyThin3Entry,T_3,LOOP_UNROLL_COUNT

;-----------------------------------------------------------------------;
; Copy n dwords, 0 leading bytes, 0 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_00 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ecx,culMiddle
        rep     movsd
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_00_loop:
        UNROLL_LOOP     COPY_WIDE_W_00,W_00,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_00_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 0 leading bytes, 1 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_01 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ecx,culMiddle
        rep     movsd
        mov     cl,[esi]
        mov     [edi],cl
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_01_loop:
        UNROLL_LOOP     COPY_WIDE_W_01,W_01,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_01_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 0 leading bytes, 2 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_02 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ecx,culMiddle
        rep     movsd
        mov     cx,[esi]
        mov     [edi],cx
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_02_loop:
        UNROLL_LOOP     COPY_WIDE_W_02,W_02,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_02_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 0 leading bytes, 3 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_03 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     ecx,culMiddle
        rep     movsd
        mov     cx,[esi]
        mov     [edi],cx
        mov     cl,[esi+2]
        mov     [edi+2],cl
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_03_loop:
        UNROLL_LOOP     COPY_WIDE_W_03,W_03,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_03_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 1 leading byte, 0 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_10 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cl,[esi-1]
        mov     [edi-1],cl
        mov     ecx,culMiddle
        rep     movsd
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_10_loop:
        UNROLL_LOOP     COPY_WIDE_W_10,W_10,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_10_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 1 leading byte, 1 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_11 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cl,[esi-1]
        mov     [edi-1],cl
        mov     ecx,culMiddle
        rep     movsd
        mov     cl,[esi]
        mov     [edi],cl
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_11_loop:
        UNROLL_LOOP     COPY_WIDE_W_11,W_11,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_11_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 1 leading byte, 2 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_12 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cl,[esi-1]
        mov     [edi-1],cl
        mov     ecx,culMiddle
        rep     movsd
        mov     cx,[esi]
        mov     [edi],cx
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_12_loop:
        UNROLL_LOOP     COPY_WIDE_W_12,W_12,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_12_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 1 leading byte, 3 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_13 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cl,[esi-1]
        mov     [edi-1],cl
        mov     ecx,culMiddle
        rep     movsd
        mov     cx,[esi]
        mov     [edi],cx
        mov     cl,[esi+2]
        mov     [edi+2],cl
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_13_loop:
        UNROLL_LOOP     COPY_WIDE_W_13,W_13,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_13_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 2 leading bytes, 0 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_20 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cx,[esi-2]
        mov     [edi-2],cx
        mov     ecx,culMiddle
        rep     movsd
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_20_loop:
        UNROLL_LOOP     COPY_WIDE_W_20,W_20,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_20_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 2 leading bytes, 1 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_21 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cx,[esi-2]
        mov     [edi-2],cx
        mov     ecx,culMiddle
        rep     movsd
        mov     cl,[esi]
        mov     [edi],cl
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_21_loop:
        UNROLL_LOOP     COPY_WIDE_W_21,W_21,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_21_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 2 leading bytes, 2 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_22 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cx,[esi-2]
        mov     [edi-2],cx
        mov     ecx,culMiddle
        rep     movsd
        mov     cx,[esi]
        mov     [edi],cx
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_22_loop:
        UNROLL_LOOP     COPY_WIDE_W_22,W_22,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_22_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 2 leading bytes, 3 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_23 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cx,[esi-2]
        mov     [edi-2],cx
        mov     ecx,culMiddle
        rep     movsd
        mov     cx,[esi]
        mov     [edi],cx
        mov     cl,[esi+2]
        mov     [edi+2],cl
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_23_loop:
        UNROLL_LOOP     COPY_WIDE_W_23,W_23,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_23_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 3 leading bytes, 0 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_30 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cl,[esi-3]
        mov     [edi-3],cl
        mov     cx,[esi-2]
        mov     [edi-2],cx
        mov     ecx,culMiddle
        rep     movsd
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_30_loop:
        UNROLL_LOOP     COPY_WIDE_W_30,W_30,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_30_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 3 leading bytes, 1 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_31 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cl,[esi-3]
        mov     [edi-3],cl
        mov     cx,[esi-2]
        mov     [edi-2],cx
        mov     ecx,culMiddle
        rep     movsd
        mov     cl,[esi]
        mov     [edi],cl
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_31_loop:
        UNROLL_LOOP     COPY_WIDE_W_31,W_31,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_31_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 3 leading bytes, 2 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_32 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cl,[esi-3]
        mov     [edi-3],cl
        mov     cx,[esi-2]
        mov     [edi-2],cx
        mov     ecx,culMiddle
        rep     movsd
        mov     cx,[esi]
        mov     [edi],cx
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_32_loop:
        UNROLL_LOOP     COPY_WIDE_W_32,W_32,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_32_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy n dwords, 3 leading bytes, 3 trailing bytes, then advance to next
; scan line.

COPY_WIDE_W_33 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cl,[esi-3]
        mov     [edi-3],cl
        mov     cx,[esi-2]
        mov     [edi-2],cx
        mov     ecx,culMiddle
        rep     movsd
        mov     cx,[esi]
        mov     [edi],cx
        mov     cl,[esi+2]
        mov     [edi+2],cl
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_wide_w_33_loop:
        UNROLL_LOOP     COPY_WIDE_W_33,W_33,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_wide_w_33_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy 1 byte, then advance to next scan line.

COPY_THIN_T_1 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cl,[esi]
        mov     [edi],cl
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_thin_t_1_loop:
        UNROLL_LOOP     COPY_THIN_T_1,T_1,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_thin_t_1_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy 2 bytes, then advance to next scan line.

COPY_THIN_T_2 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cx,[esi]
        mov     [edi],cx
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_thin_t_2_loop:
        UNROLL_LOOP     COPY_THIN_T_2,T_2,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_thin_t_2_loop
        jmp     src8_done

;-----------------------------------------------------------------------;
; Copy 3 bytes, then advance to next scan line.

COPY_THIN_T_3 macro ENTRY_LABEL,ENTRY_INDEX
&ENTRY_LABEL&ENTRY_INDEX&:
        mov     cx,[esi]
        mov     [edi],cx
        mov     cl,[esi+2]
        mov     [edi+2],cl
        add     esi,eax
        add     edi,ebx
        endm    ;--------------------------------;

        align   4
copy_thin_t_3_loop:
        UNROLL_LOOP     COPY_THIN_T_3,T_3,LOOP_UNROLL_COUNT
        dec     edx
        jnz     copy_thin_t_3_loop
        jmp     src8_done

public copy_wide_w_00_loop
public copy_wide_w_01_loop
public copy_wide_w_02_loop
public copy_wide_w_03_loop
public copy_wide_w_10_loop
public copy_wide_w_11_loop
public copy_wide_w_12_loop
public copy_wide_w_13_loop
public copy_wide_w_20_loop
public copy_wide_w_21_loop
public copy_wide_w_22_loop
public copy_wide_w_23_loop
public copy_wide_w_30_loop
public copy_wide_w_31_loop
public copy_wide_w_32_loop
public copy_wide_w_33_loop
public copy_thin_t_1_loop
public copy_thin_t_2_loop
public copy_thin_t_3_loop

endProc vSrcCopy8bpp

        end

