;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1994 - 1995  Microsoft Corporation.  All Rights Reserved.
;
;   File:       vjoydacc.asm
;   Content:    code to access VJOYD services
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	include	vjoydapi.inc
        .386p
_DATA segment WORD public 'DATA' use16
DGROUP group _DATA

vjoydAddr   	dd  0

_DATA ENDS
	
_TEXT segment para public 'CODE' use16

        assume cs:_TEXT
        assume ds:DGROUP

dataptr	equ	<dword ptr [bp+6]>
id   	equ	<word ptr [bp+10]>

public jsGetPos
jsGetPos PROC FAR
    push    bp
    mov	    bp,sp
    mov	    ax, VJOYDAPI_GetPos
    mov	    dx, id
    les	    bx, dataptr
    call    dword ptr [vjoydAddr]
    pop	    bp
    ret	    6
jsGetPos ENDP

public jsGetPosEx
jsGetPosEx PROC FAR
    push    bp
    mov	    bp,sp
    mov	    ax, VJOYDAPI_GetPosEx
    mov	    dx, id
    les	    bx, dataptr
    call    dword ptr [vjoydAddr]
    pop	    bp
    ret	    6
jsGetPosEx ENDP

public jsSetVJoyDData
jsSetVJoyDData PROC FAR
    push    bp
    mov	    bp,sp
    mov	    ax, VJOYDAPI_SetData
    les	    bx, dataptr
    call    dword ptr [vjoydAddr]
    pop	    bp
    ret	    4
jsSetVJoyDData ENDP

public jsGetHWCaps
jsGetHWCaps PROC FAR
    push    bp
    mov	    bp,sp
    mov	    ax, VJOYDAPI_GetHWCaps
    mov	    dx, id
    les	    bx, dataptr
    call    dword ptr [vjoydAddr]
    pop	    bp
    ret	    4
jsGetHWCaps ENDP

public jsInitVJoyD
jsInitVJoyD PROC FAR
    push    es
    push    di
    mov	    bx,VJOYD_Device_ID
    mov	    ax, 01684h
    int	    02fh			; get callback routine
    mov	    ax,es
    cmp	    ax,0			; selector 0?
    jne	    short @f			; nope, OK
    mov	    ax,0			; yes, fail
    jmp	    short done
@@: mov	    word ptr vjoydAddr+2,ax
    mov	    word ptr vjoydAddr,di
    mov	    ax, VJOYDAPI_GetVersion
    call    dword ptr [vjoydAddr]
    cmp	    ah, VJOYD_Ver_Major		; major version OK?
    jle	    short @f			; yep
    mov	    ax, 0			; no, fail 
    jmp	    short done
@@: mov	    ax,1
done:
    pop	    di
    pop	    es
    ret
jsInitVJoyD ENDP

_TEXT	ENDS

end
