;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;******************************************************************************
;
;   Title:	OPTTEST.INC - Macros to optimize test instructions
;
;   Version:	1.00
;
;==============================================================================


TestMem MACRO MemPtr, M, N
IFNB <N>
	%out TestMem has too many parameters!
	.err
ENDIF
IFE (M) AND 0FFFFFF00h
	test	BYTE PTR [MemPtr], M
ELSE
IFE (M) AND 0FFFF00FFh
	test	BYTE PTR [MemPtr+1], ((M) SHR 8)
ELSE
IFE (M) AND 0FF00FFFFh
	test	BYTE PTR [MemPtr+2], ((M) SHR 16)
ELSE
IFE (M) AND 000FFFFFFh
	test	BYTE PTR [MemPtr+3], ((M) SHR 24)
ELSE
IFE (M) AND 0FFFF0000h
	test	WORD PTR [MemPtr], M
ELSE
IFE (M) AND 0FF0000FFh
	test	WORD PTR [MemPtr+1], ((M) SHR 8)
ELSE
IFE (M) AND 00000FFFFh
	test	WORD PTR [MemPtr+2], ((M) SHR 16)
ELSE
	test	DWORD PTR [MemPtr], M
ENDIF
ENDIF
ENDIF
ENDIF
ENDIF
ENDIF
ENDIF
	ENDM





TestReg MACRO R, Mask

_@TR MACRO Ltr
IFE (Mask) AND 0FFFFFF00h
	test	Ltr&&l, Mask
ELSE
IFE (Mask) AND 0FFFF00FFh
	test	Ltr&&h, ((Mask) SHR 8)
ELSE
IFE (Mask) AND 0FFFF0000h
	test	Ltr&&x, Mask
ELSE
	test	e&&Ltr&&x, Mask
ENDIF
ENDIF
ENDIF
	ENDM


IFIDNI <R>, <EAX>
	_@TR a
ELSE
IFIDNI <R>, <EBX>
	_@TR b
ELSE
IFIDNI <R>, <ECX>
	_@TR c
ELSE
IFIDNI <R>, <EDX>
	_@TR d
ELSE
IFIDNI <R>, <ESI>
IFE (Mask) AND 0FFFF0000h
	test	si, Mask
ELSE
	test	esi, Mask
ENDIF
ELSE
IFIDNI <R>, <EDI>
IFE (Mask) AND 0FFFF0000h
	test	di, Mask
ELSE
	test	edi, Mask
ENDIF
ELSE
%OUT ERROR:  Bad param to TestReg macro
.ERR
ENDIF
ENDIF
ENDIF
ENDIF
ENDIF
ENDIF
	ENDM




SetFlag MACRO MemPtr, M
IFE (M) AND 0FFFFFF00h
	or	BYTE PTR [MemPtr], M
ELSE
IFE (M) AND 0FFFF00FFh
	or	BYTE PTR [MemPtr+1], ((M) SHR 8)
ELSE
IFE (M) AND 0FF00FFFFh
	or	BYTE PTR [MemPtr+2], ((M) SHR 16)
ELSE
IFE (M) AND 000FFFFFFh
	or	BYTE PTR [MemPtr+3], ((M) SHR 24)
ELSE
IFE (M) AND 0FFFF0000h
	or	WORD PTR [MemPtr], M
ELSE
IFE (M) AND 0FF0000FFh
	or	WORD PTR [MemPtr+1], ((M) SHR 8)
ELSE
IFE (M) AND 00000FFFFh
	or	WORD PTR [MemPtr+2], ((M) SHR 16)
ELSE
	or	DWORD PTR [MemPtr], M
ENDIF
ENDIF
ENDIF
ENDIF
ENDIF
ENDIF
ENDIF
	ENDM


;
; BUGBUG -- ClrFlag uses BYTE overrides when a DWORD would do better
; on a 486 in 32-bit mode.
;
; NOTE: any future modifications of this macro must also clear carry
;
ClrFlag MACRO MemPtr, M
IFE (M) AND 0FFFFFF00h
	and	BYTE PTR [MemPtr], NOT (M)
ELSEIFE (M) AND 0FFFF00FFh
	and	BYTE PTR [MemPtr+1], NOT ((M) SHR 8)
ELSEIFE (M) AND 0FF00FFFFh
	and	BYTE PTR [MemPtr+2], NOT ((M) SHR 16)
ELSEIFE (M) AND 000FFFFFFh
	and	BYTE PTR [MemPtr+3], NOT ((M) SHR 24)
ELSEIFE (M) AND 0FFFF0000h
	and	WORD PTR [MemPtr], NOT (M)
ELSEIFE (M) AND 0FF0000FFh
	and	WORD PTR [MemPtr+1], NOT ((M) SHR 8)
ELSEIFE (M) AND 00000FFFFh
	and	WORD PTR [MemPtr+2], NOT ((M) SHR 16)
ELSE
	and	DWORD PTR [MemPtr], NOT (M)
ENDIF
	ENDM

;
; SetReg sets a bit in a register, analogous to TestReg.
;

;
; _@SRb is the SetReg helper macro for registers which are byte-addressible.
;
_@SRb macro reg:req, mask:req
    ife (mask) and 0ffffff00h
	    or	reg&l, mask
    elseife (mask) and 0ffff00ffh
	    or	reg&h, ((mask) shr 8)
    elseife (mask) and 0ffff0000h
	    or	reg&x, mask
    else
	    or	e&reg&x, mask
    endif

endm

;
; _@SRw is the SetReg helper macro for registers which are word-addressible.
;
; Note that the case where the bitmask is less than 128 is special, because
; we are better off using the short form, to take advantage of the CPU's
; special `byte operand' mode.  No such luck with Test, which has no `byte'
; mode.
;
_@SRw macro reg:req, mask:req
    ife (mask) and 0ffffff80h
	if  @WordSize EQ 2
	    or	reg, mask
	else
	    or	e&reg, mask
	endif
    elseife (mask) and 0ffff0000h
	    or	reg, mask
    else
	    or	e&reg&x, mask
    endif
endm


SetReg macro r:req, mask:req

    ifidni <r>, <eax>
	    _@SRb a, <mask>
    elseifidni <r>, <ebx>
	    _@SRb b, <mask>
    elseifidni <r>, <ecx>
	    _@SRb c, <mask>
    elseifidni <r>, <edx>
	    _@SRb d, <mask>
    elseifidni <r>, <esi>
	    _@SRw si, <mask>
    elseifidni <r>, <edi>
	    _@SRw di, <mask>
    else
	.err Bad param to SetReg macro
    .err
    endif
endm


;
; mov_b moves a small byte value into a 32-bit register or 32-bit memory
; location, sacrificing speed for size.  It may destroy flags.
;
mov_b	macro	regmem:req, val:req
    ??mov_b_done = 0
    ife ((val) LT 080h) OR ((val) GE 0ffffff80h)
	.err Second argument to mov_b out of range.
    endif
    if	(OPATTR regmem) and 10h		; If is a register
					; Then additional opt'z'ns possible
	if (val) EQ 0
	    xor	regmem, regmem		; Zero
	    ??mov_b_done = 1
	elseif (val) EQ 1
	    xor	regmem, regmem
	    inc	regmem			; One
	    ??mov_b_done = 1
	else
	    ifidni <regmem>, <eax>	; EAX has really cool stuff available
		if ((val) GE 0) and ((val) LE 0FFh)
	    xor	    eax, eax
	    mov	    al, val
		    ??mov_b_done = 1
		endif
	    endif
	endif
    endif

    ife ??mov_b_done
	if (VAL) eq 0
	    and	regmem, 0		; Zero
	elseif (VAL) eq 0FFFFFFFFh
	    or	regmem, -1		; Negative one
	else
	    push val			; Anything else
	    pop	regmem
	endif
    endif
endm

;
; movnz moves an arbitrary nonzero value into the specified register.
; It may destroy flags.  If fParanoid is non-empty, then we will also
; ensure that the low word of the register is nonzero.  (This is to
; cover for some people who forgot that they are in 32-bit land and
; just do a "test ax, ax" to check the return value.)  I.e., if you
; suspect your caller of being a bozo, you say
;
;	movnz r, BOZO
;
; instead of just
;
;	movnz r
;
movnz	macro	R:req, fParanoid
    IFIDNI <R>, <EAX>
	mov	al, 1		; 2 bytes
    ELSEIFIDNI <R>, <EBX>
	mov	bl, 1		; 2 bytes
    ELSEIFIDNI <R>, <ECX>
	mov	cl, 1		; 2 bytes
    ELSEIFIDNI <R>, <EDX>
	mov	dl, 1		; 2 bytes
    ELSEIFNB <fParanoid>
	or	R, 1		; 3 bytes
    ELSE
	mov	R, esp	    	; 2 bytes (at ring 0, esp is never zero)
    ENDIF
endm
