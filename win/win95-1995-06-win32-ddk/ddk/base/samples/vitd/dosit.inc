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

;----------------------------- M A C R O S ------------------------------
Writel	MACRO	addr
	    push	ax
	    push	bx
	    push	cx
	    push	dx

        mov     dx,offset &addr         ;Print
        mov     cx,&addr&l   
        mov     bx,1                    ;stdout
        mov     ah,40h                  ;write
        int     21h

	    pop	    dx
	    pop	    cx
	    pop	    bx
	    pop	    ax	
	    ENDM

cvt_nibble MACRO
        add     ax, '0'         ; convert to ASCII
        cmp     ax, '9'         ; is it 0-9?
        jbe     @f              ; yes, jump
        add     ax, 'A'-'9'-1   ; make it 'A-F'
@@:    
        ENDM

;----------------------------- E Q U A T E S -----------------------------

cr      equ     0dh
lf      equ     0ah
bs      equ     08h
