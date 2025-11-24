        page    60, 132

;---------------------------------------------------------------------------;
;
;   assemf.asm
;
;   General Description:
;      Contains FIXED routines.
;
;---------------------------------------------------------------------------;
;
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1991 - 1995	Microsoft Corporation.	All Rights Reserved.
;
;---------------------------------------------------------------------------;


ifdef MASM6
        option oldmacros
        option oldstructs
endif

	.286

        ?DF=1                           ; No default segments
        ?PLM=1                          ; Pascal calling convention
        ?WIN=0                          ; NO! Windows prolog/epilog code

	.xlist
	include cmacros.inc
	include windows.inc
	include mmsystem.inc
        include mssndsys.inc
	include driver.inc
	.list

;===========================================================================;
;   segmentation
;===========================================================================;

IFNDEF SEGNAME
        SEGNAME equ <_TEXT>
ENDIF

createSeg %SEGNAME, CodeSeg, dword, public, CODE
createSeg _DATA, Data, word, public, DATA, DGROUP
defgrp DGROUP, Data

        .386p

;===========================================================================;
;   extrn declarations
;===========================================================================;

        externA __AHINCR                ; KERNEL

        externFP wodInterrupt           ; hardfix.c
        externFP widInterrupt           ; hardfix.c

        externD _glpVSNDSYSEntry        ; commona.asm

;===========================================================================;
;   public data
;===========================================================================;

sBegin Data

        public isr_Srv_Table
isr_Srv_Table label dword
	    .errnz INT_WAVEOUT - 1
	dd  wodInterrupt                ; INT_WAVEOUT   equ 1
	    .errnz INT_WAVEIN - 2
	dd  widInterrupt                ; INT_WAVEIN    equ 2
sEnd

;===========================================================================;
;   code segment
;===========================================================================;

sBegin CodeSeg

	assumes cs, CodeSeg
	assumes ds, Data

;---------------------------------------------------------------------------;
;
;   LPVOID FAR PASCAL NMCMemCopySrc(lpDst, lpSrc, wCount, wSramDataReg )
;
;   DESCRIPTION:
;       This function is used to copy data FROM a memory block that
;       is MAY contain segment boundaries TO a memory block that is
;       guaranteed to NOT contain segment boundaries.
;
;       This function handles segment crossings in the SOURCE *only*.
;
;   ENTRY:
;       LPVOID lpDst    :   FAR pointer to destination memory block that
;                           does NOT contain segment boundaries.
;
;       LPVOID lpSrc    :   FAR pointer to source memory block that
;                           may contain segment boundaries.
;
;       WORD wCount     :   Number of BYTES to copy.
;
;   EXIT:
;       DX:AX is the destination pointer advanced by wCount bytes.
;
;   USES:
;       FLAGS, AX, BX, CX, DX, ES
;
;---------------------------------------------------------------------------;

	assumes ds, nothing
	assumes es, nothing

cProc NMCMemCopySrc <FAR, PASCAL, PUBLIC> <ds, si, di>
        ParmD   lpSrc
        ParmW   wCount
        ParmW   wSramDataReg
cBegin

        lds     si, lpSrc             ; get source pointer
        mov     cx, wCount            ; cx is count of bytes
        jcxz    nmcmcs_Exit              ; copy no bytes--return current pointer...

        cld                           ; let's not assume this

        mov     ax, si                ; check for a segment cross in the source
        add     ax, cx
        sbb     bx, bx                ; if C BX=FFFF, NC BX=0000
        and     ax, bx                ;
        sub     cx, ax                ; CX contains amount to copy now, AX has
                                      ; ...amount to copy later

nmcmcs_Copy_It:

        jcxz    nmcmcs_Exit

        ;
        ; Optimize with dword rep movs?
        ;

        mov     dx, wSramDataReg

        shr     cx, 1                 ; copy the memory
        rep     outsw
        adc     cl, cl
        rep     outsb

        or      si, si                ; check for a segment wrap
        jnz     nmcmcs_Exit

        mov     cx, ax
        mov     ax, ds
        add     ax, __AHINCR

        ; watch out for those 64K boundaries
        jcxz    nmcmcs_64KExit                             ; if there's nothin left to copy and we're
        								; about to cross a segment boundary, fake it

        mov     ds, ax
        jmp     short nmcmcs_Copy_It

nmcmcs_64KExit:
		mov		dx, ax					; ax has the selector
                jmp             short nmcmcs_Exit2

nmcmcs_Exit:

        mov     dx, ds                ; DX:AX = advanced source pointer

nmcmcs_Exit2:
        mov     ax, si

cEnd


;---------------------------------------------------------------------------;
;
;   LPVOID FAR PASCAL NMCMemCopyDest(lpDst, lpSrc, wCount, wSramDataReg )
;
;   DESCRIPTION:
;       This function is used to copy data FROM a memory block that
;       is guaranteed to NOT cross a segment boundary TO a memory block
;       that CAN contain segment boundaries.
;
;       This function handles segment crossings in the DESTINATION *only*.
;
;   ENTRY:
;       LPVOID lpDst    :   FAR pointer to destination memory block that
;                           may contain segment boundaries.
;
;       WORD wCount     :   Number of BYTES to copy.
;
;   EXIT:
;       DX:AX is the destination pointer advanced by wCount bytes.
;
;   USES:
;       FLAGS, AX, BX, CX, DX, ES
;
;---------------------------------------------------------------------------;

	assumes ds, nothing
	assumes es, nothing

cProc NMCMemCopyDest <FAR, PASCAL, PUBLIC> <ds, si, di>
        ParmD   lpDst
        ParmW   wCount
        ParmW   wSramDataReg
cBegin

        cld                             ; let's not assume this

        les     di, lpDst               ; get dest pointer
        mov     cx, wCount              ; cx is count of bytes

        mov     ax, di                  ; check for a segment cross in the dest
        add     ax, cx
        sbb     bx, bx                  ; if C BX=FFFF, NC BX=0000
        and     ax, bx                  ;
        sub     cx, ax                  ; CX contains amount to copy now, AX has
                                        ; ...amount to copy later
nmcmcd_Copy_It:

        jcxz    nmcmcd_Exit

        ;
        ; Optimize with dword rep movs?
        ;

        mov     dx, wSramDataReg

        shr     cx, 1                   ; copy the memory
        ;rep     movsw
        rep     insw
        adc     cl, cl
        ;rep     movsb
        rep     insb

        or      di, di                  ; check for a segment wrap
        jnz     nmcmcd_Exit

        mov     cx, ax                  ; cross huge boundary
        mov     ax, es
        add     ax, __AHINCR

        ; Watch out for those 64K boundaries
        jcxz    nmcmcd_64KExit                             ; do something special if there's nothing
        								; left to copy AND we are gonna cross a boundary

        mov     es, ax
        jmp     short nmcmcd_Copy_It

nmcmcd_64KExit:
		mov		dx, ax					; ax contains the segment in this case
                jmp             short nmcmcd_Exit2

nmcmcd_Exit:

        mov     dx, es                  ; DX:AX = advanced dest pointer

nmcmcd_Exit2:
        mov     ax, di

cEnd


;---------------------------------------------------------------------------;
;
; @doc INTERNAL
;
; @asm void | NMCMemFillSilent | Fill memory with silence.
;
; @parm LPVOID | lpDst | Destination.
;
; @parm WORD | wCount | Number of bytes to copy.
;
; @parm WORD | wValue | Value to fill with
;
; @comm Doesn't check for segment crossing since we're only padding DMA buffer.
;     This is specific to 8-bit data since it fills 0x80 for silence.
;
;---------------------------------------------------------------------------;

	assumes ds, nothing
	assumes es, nothing

cProc NMCMemFillSilent <FAR, PASCAL, PUBLIC> <di>
	ParmW wCount
	ParmW wValue
        ParmW wSramDataReg
cBegin

	cld                             ; let's not assume this

	mov     cx, wCount              ; cx is count of bytes
        jcxz    nmcmfs_Exit                ; any bytes at all?

        ;
        ; Optimize with dword rep movs?
        ;

        ;shr     cx, 1                   ; copy the memory
	mov     ax, wValue              ; silence for 8/16-bit data
        ;rep     stosw
        mov     dx, wSramDataReg
nmcmfs_Loop:
        out     dx, al
        loop    nmcmfs_Loop

        ;adc     cl, cl                  ; is there a byte left over?
        ;rep     stosb

nmcmfs_Exit:

cEnd

;---------------------------------------------------------------------------;
;
;   LPVOID FAR PASCAL MemCopySrc(lpDst, lpSrc, wCount)
;
;   DESCRIPTION:
;       This function is used to copy data FROM a memory block that
;       is MAY contain segment boundaries TO a memory block that is
;       guaranteed to NOT contain segment boundaries.
;
;       This function handles segment crossings in the SOURCE *only*.
;
;   ENTRY:
;       LPVOID lpDst    :   FAR pointer to destination memory block that
;                           does NOT contain segment boundaries.
;
;       LPVOID lpSrc    :   FAR pointer to source memory block that
;                           may contain segment boundaries.
;
;       WORD wCount     :   Number of BYTES to copy.
;
;   EXIT:
;       DX:AX is the destination pointer advanced by wCount bytes.
;
;   USES:
;       FLAGS, AX, BX, CX, DX, ES
;
;---------------------------------------------------------------------------;

	assumes ds, nothing
	assumes es, nothing

cProc MemCopySrc <FAR, PASCAL, PUBLIC> <ds, si, di>
        ParmD   lpDst
        ParmD   lpSrc
        ParmW   wCount
cBegin

        lds     si, lpSrc             ; get source pointer
        mov     cx, wCount            ; cx is count of bytes
        jcxz    mcs_Exit              ; copy no bytes--return current pointer...

        cld                           ; let's not assume this
        les     di, lpDst             ; get dest pointer

        mov     ax, si                ; check for a segment cross in the source
        add     ax, cx
        sbb     bx, bx                ; if C BX=FFFF, NC BX=0000
        and     ax, bx                ;
        sub     cx, ax                ; CX contains amount to copy now, AX has
                                      ; ...amount to copy later

mcs_Copy_It:

        jcxz    mcs_Exit

        ;
        ; Optimize with dword rep movs?
        ;

        shr     cx, 1                 ; copy the memory
        rep     movsw
        adc     cl, cl
        rep     movsb

        or      si, si                ; check for a segment wrap
        jnz     mcs_Exit

        mov     cx, ax
        mov     ax, ds
        add     ax, __AHINCR

        ; watch out for those 64K boundaries
        jcxz	mcs_64KExit				; if there's nothin left to copy and we're
        								; about to cross a segment boundary, fake it

        mov     ds, ax
        jmp     short mcs_Copy_It

mcs_64KExit:
		mov		dx, ax					; ax has the selector
		jmp		short mcs_Exit2

mcs_Exit:

        mov     dx, ds                ; DX:AX = advanced source pointer

mcs_Exit2:
        mov     ax, si

cEnd


;---------------------------------------------------------------------------;
;
;   LPVOID FAR PASCAL MemCopyDest(lpDst, lpSrc, wCount)
;
;   DESCRIPTION:
;       This function is used to copy data FROM a memory block that
;       is guaranteed to NOT cross a segment boundary TO a memory block
;       that CAN contain segment boundaries.
;
;       This function handles segment crossings in the DESTINATION *only*.
;
;   ENTRY:
;       LPVOID lpDst    :   FAR pointer to destination memory block that
;                           may contain segment boundaries.
;
;       LPVOID lpSrc    :   FAR pointer to source memory block that
;                           does NOT contain segment boundaries.
;
;       WORD wCount     :   Number of BYTES to copy.
;
;   EXIT:
;       DX:AX is the destination pointer advanced by wCount bytes.
;
;   USES:
;       FLAGS, AX, BX, CX, DX, ES
;
;---------------------------------------------------------------------------;

	assumes ds, nothing
	assumes es, nothing

cProc MemCopyDest <FAR, PASCAL, PUBLIC> <ds, si, di>
        ParmD   lpDst
        ParmD   lpSrc
        ParmW   wCount
cBegin

        cld                             ; let's not assume this

        lds     si, lpSrc               ; get source pointer
        les     di, lpDst               ; get dest pointer
        mov     cx, wCount              ; cx is count of bytes

        mov     ax, di                  ; check for a segment cross in the dest
        add     ax, cx
        sbb     bx, bx                  ; if C BX=FFFF, NC BX=0000
        and     ax, bx                  ;
        sub     cx, ax                  ; CX contains amount to copy now, AX has
                                        ; ...amount to copy later
mcd_Copy_It:

        jcxz    mcd_Exit

        ;
        ; Optimize with dword rep movs?
        ;

        shr     cx, 1                   ; copy the memory
        rep     movsw
        adc     cl, cl
        rep     movsb

        or      di, di                  ; check for a segment wrap
        jnz     mcd_Exit

        mov     cx, ax                  ; cross huge boundary
        mov     ax, es
        add     ax, __AHINCR

        ; Watch out for those 64K boundaries
        jcxz	mcd_64KExit				; do something special if there's nothing
        								; left to copy AND we are gonna cross a boundary

        mov     es, ax
        jmp     short mcd_Copy_It

mcd_64KExit:
		mov		dx, ax					; ax contains the segment in this case
		jmp		short mcd_Exit2

mcd_Exit:

        mov     dx, es                  ; DX:AX = advanced dest pointer

mcd_Exit2:
        mov     ax, di

cEnd


;---------------------------------------------------------------------------;
;
; @doc INTERNAL
;
; @asm void | MemFillSilent | Fill memory with silence.
;
; @parm LPVOID | lpDst | Destination.
;
; @parm WORD | wCount | Number of bytes to copy.
;
; @parm WORD | wValue | Value to fill with
;
; @comm Doesn't check for segment crossing since we're only padding DMA buffer.
;     This is specific to 8-bit data since it fills 0x80 for silence.
;
;---------------------------------------------------------------------------;

	assumes ds, nothing
	assumes es, nothing

cProc MemFillSilent <FAR, PASCAL, PUBLIC> <di>
	ParmD lpDst
	ParmW wCount
	ParmW wValue
cBegin

	cld                             ; let's not assume this

	les     di, lpDst               ; get dest pointer
	mov     cx, wCount              ; cx is count of bytes
	jcxz    mfs_Exit                ; any bytes at all?

        ;
        ; Optimize with dword rep movs?
        ;

	shr     cx, 1                   ; copy the memory
	mov     ax, wValue              ; silence for 8/16-bit data
	rep     stosw
	adc     cl, cl                  ; is there a byte left over?
	rep     stosb

mfs_Exit:

cEnd

;---------------------------------------------------------------------------;
;
;   WORD FAR PASCAL MSSNDSYS_Get_DMA_Count( DWORD dn, WORD sf )
;
;   DESCRIPTION:
;       Calls the supporting VxD to retrieve the DMA count.  This
;       is the actual latched count of the DMA controller read at
;       ring 0 using the new VDMAD_Get_Phys_Count service.
;
;   ENTRY:
;
;   EXIT:
;       AX   :   DMA channel count
;
;   USES:
;       Flags, AX, DX
;
;---------------------------------------------------------------------------;

        assumes ds, nothing
        assumes es, nothing

cProc MSSNDSYS_Get_DMA_Count <FAR, PASCAL, PUBLIC> <>
        ParmD   dn
        ParmW   sf
cBegin

        push    bx
        push    ecx
        mov     bx, sf
        mov     ecx, dn 
        mov     dx, MSS_API_Get_DMA_Count
        call    [_glpVSNDSYSEntry]
        pop     ecx
        pop     bx
cEnd

sEnd CodeSeg

	end
