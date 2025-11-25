;**********************************************************************x
;chkstk.asm
;
;       Copyright (c) 1989-1994, Microsoft Corporation. All rights reserved.
;
;
; __chkstk
;
; Check if the stack pointer will exceed the max stack size if a new
; frame is created.
;**********************************************************************

;#define        FIXED_SEG

        .ppc
        .code
        align 4

        extern _LMGetApplLimit:near
        extern _DebugStr:near

        public pch

__chkstk proc public
        stwu  sp,-16(sp)
        stw r0,0(sp)
        stw r3,4(sp)
        stw r12,8(sp)
        mflr r12
        stw r12, 12(sp)

        bl _LMGetApplLimit
        nop
        cmplw 0,r3,sp
        blt _chkstk@2

        lwz r3,pch(r2)
        bl _DebugStr
        nop

_chkstk@2:
        lwz r0,0(sp)
        lwz r3,4(sp)
        lwz r12,12(sp)
        mtlr r12
        lwz r12,8(sp)
        addic sp, sp, 16
        ret
__chkstk endp

        .data
pch     db 24,"_chkstk: stack overflow!",0

        end

