        name    ttsr
;**************************************************************************
;
;       TTSR
;
;       This program is the companion TSR for the Windows program
;       "Tdosmem". The purpose of this TSR is to hook INT 60h and
;       wait for requests from the Windows program.
;
;       The Tdosmem Windows application accesses a buffer allocated
;       by this TSR using Windows LDT functions. 
;
;       This demonstrates a simple technique for communication between
;       a Windows app and a DOS TSR. Note that this may also be used
;       as a crude method for communication between virtual machines.
;       Since this buffer is allocated by the TSR, it is global to
;       all virtual machines.
;
;--------------------------------------------------------------------------
;
;       TTSR Installation Check
;
;       entry:
;           AX = TTSR Identification #
;           BX = 0
;       exit
;           BX = GTSR Identification #
;           CX = Segment => local WORD
;           DX = Offset  => local WORD
;
;--------------------------------------------------------------------------
;
;       TTSR Increment WORD in local buffer
;
;       entry:
;           AX = TTSR Identification #
;           BX = 1
;
;**************************************************************************
;
;
;

Vect_Num equ    60h
Signature equ   899bh

cr      equ     0dh
lf      equ     0ah


_TEXT   segment word public 'CODE'
        assume cs:_TEXT,ds:_DATA

;*-----------------------  TSR Data Area ---------------------*
count   dw      0                       ; local buffer
oldint  dd      0                       ; old interrupt handler address
;*-----------------------  TSR Code --------------------------*
handle  proc

        push    si
        push    di
        push    es
        push    ds
        push    ax
        push    bx
        push    cx
        push    dx

        cmp     ax, Signature           ; look for our signature
        jnz     short chain             ; no, just chain
        cmp     bx, 0                   ; installation check?
        jnz     short chk1
        mov     bx, ax                  ; show that we're here
        mov     dx, OFFSET count
        push    cs
        pop     cx                      
        jmp     short return

chk1:
        cmp     bx, 1                   ; increment pointer?
        jnz     short chain             ; no, just forget it
        inc     cs:[count]              ; add it in
        jmp     short return
        
chain:                                  ; chain to previous handler
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        pop     ds
        pop     es
        pop     di
        pop     si

        jmp     DWORD PTR cs:oldint

return:                                 ; return to caller
        add     sp, 2       ;!!!! DON'T RESTORE DX !!!!!!
        add     sp, 2       ;!!!! DON'T RESTORE CX !!!!!!
        add     sp, 2       ;!!!! DON'T RESTORE BX !!!!!!
        pop     ax
        pop     ds
        pop     es
        pop     di
        pop     si
        iret
handle  endp


        ALIGN   16
init_fence:

;*-------------------------- Initialization Code ----------------------*

ttsr    proc    far
        mov     ax,_DATA
        mov     ds,ax

        mov     ax,Vect_Num             
        mov     bx,offset tsrmsgx
        call    hexasc

        mov     dx,offset tsrmsg        ; Print message
        mov     cx,tsrmsgl
        mov     bx,1                    ; stdout
        mov     ah,40h                  ; write
        int     21h

;get old vector
        mov     ah,35h
        mov     al,Vect_Num
        int     21h
        mov     WORD PTR cs:oldint,bx
        mov     WORD PTR cs:oldint+2,es

        push    ds                      ; install handler
        mov     dx, offset handle
        push    cs
        pop     ds
        mov     ah,25h
        mov     al,Vect_Num
        int     21h
        pop     ds


        
        mov     ax,3100h        ;TSR
        mov     dx,offset init_fence
        add     dx,15
        shr     dx,1
        shr     dx,1
        shr     dx,1
        shr     dx,1
        add     dx,16
        int     21h
ttsr    endp


;***********************************************************************

hexasc  proc    near            ; converts word to hex ASCII
                                ; call with AX = value,
                                ; DS:BX = address for string
                                ; returns AX, BX destroyed

        push    cx              ; save registers
        push    dx

        mov     dx,4            ; initialize character counter
hexasc1:
        mov     cx,4            ; isolate next four bits
        rol     ax,cl
        mov     cx,ax
        and     cx,0fh
        add     cx,'0'          ; convert to ASCII
        cmp     cx,'9'          ; is it 0-9?
        jbe     hexasc2         ; yes, jump
        add     cx,'A'-'9'-1    ; add fudge factor for A-F

hexasc2:                        ; store this character
        mov     [bx],cl
        inc     bx              ; bump string pointer

        dec     dx              ; count characters converted
        jnz     hexasc1         ; loop, not four yet

        pop     dx              ; restore registers
        pop     cx
        ret                     ; back to caller

hexasc  endp

        

_TEXT   ends


;*--------------------------------------------------------------------*

_DATA   segment word public 'DATA'
tsrmsg  db      'hooking interrupt '
tsrmsgx dd      ?
        db      cr,lf
tsrmsgl equ     $-tsrmsg

_DATA   ends



        end     ttsr          
