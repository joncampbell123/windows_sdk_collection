TITLE Scale - signed 16 bit multiply and divide routine
	subttl	Copyright (C) 1986 Aldus Corporation.  All rights reserved.
	page	60,132
;
; abstract
;
; This module contains a c-language callable routine for a * b / c : A 16 bit
; signed and rounded multiply and divide with a 32 bit intermediate result.
;
; short Scale(a, b, c)
; short a;
; short b;
; short c;
;
;============================================================================

.xlist
	?PLM = 1				;no plm, use C calling convention
	?WIN = 0				;no, don't follow windows calling conventions

   include cmacros.inc				;formerly \include\cmacros.inc
.list

;============================================================================

sBegin	CODE
assumes	CS,CODE
assumes ds,DATA

;============================================================================
LabelFP <PUBLIC, Scale>
ife ?PLM
        mov     bx,sp
        mov     ax,ss:[bx+4]
        mov     dx,ss:[bx+6]
        mov     cx,ss:[bx+8]
        mov     bx,dx
else
        pop     dx                      ; pop return
        pop     es
        pop     cx                      ; c
        pop     bx                      ; b
        pop     ax                      ; a
        push    es
        push    dx                      ; push return
endif
        push    si
        mov     si,7fffh

        imul    bx                      ; get a * b

        push    cx                      ; save divisor

        or      cx,cx                   ; get absolute value of divisor
        jns     ab100
        neg     cx
        neg     si                      ; negate clip value
ab100:
        push    cx                      ; push positive divisor
        shr     cx,1                    ; now divide divisor by 2 for rouding
                                        ; factor

        or      dx,dx
        jns     ab200                   ; make rounding factor negative if
        neg     cx                      ; intermediate result is negative.
        neg     si                      ; negate clip value
ab200:

        or      cx,cx                   ; if we're dividing by 0, return
        jnz     ab240                   ; maxshort of sign ab.
        pop     cx
        pop     cx
        jmp     short ab260        

ab240:
	xchg	ax,cx
	mov	bx,dx
	cwd
	add	ax,cx
	adc	dx,bx	; add in 32 bit rounding number

        or      dx,dx
        jns     ab250

        xor     bx,0ffffh               ; make 32 bit number positive
        xor     cx,0ffffh
        add     cx,1
        adc     bx,0

ab250:
        rol     cx,1
        rcl     bx,1                    ; div 32 bit number by 8000h.

        pop     cx                      ; get positive divisor
        cmp     cx,bx
        pop     cx                      ; get real divisor
        ja      ab300
        
ab260:
        mov     ax,si
        pop     si
	db	0cbh	; retf

ab300:
        idiv    cx                      ; now div by our divisor
        pop     si
ab400:
        db      0cbh                    ; retf

sEnd
end
