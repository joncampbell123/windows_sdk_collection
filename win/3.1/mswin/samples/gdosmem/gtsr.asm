        name    gtsr

;**************************************************************************
;
;       GTSR
;
;       This program is the companion TSR for the Windows program
;       "Gdosmem". The purpose of this TSR is to hook INT 60h and
;       wait for requests from the Windows program.
;
;       The Gdosmem program allocates memory using GlobalDosAlloc,
;       and then displays the contents using the returned selector
;       value. Then, it issues INT60's to this TSR program, passing
;       in the returned segment value of the allocated memory. This
;       TSR increments the word at that location, and the results
;       are subsequently displayed in the Windows APP.
;
;       This demonstrates a simple technique for communication between
;       a Windows app and a DOS TSR.
;
;       NOTE: The memory allocated using GlobalDosAlloc is local to
;       the Windows VM. Thus, other virtual machines will not have
;       access to this memory location. 
;
;--------------------------------------------------------------------------
;
;       GTSR Installation Check
;
;       entry:
;           AX = GTSR Identification #
;           BX = 0
;       exit
;           BX = GTSR Identification #
;
;--------------------------------------------------------------------------
;
;       GTSR Increment WORD in CX:DX
;
;       entry:
;           AX = GTSR Identification #
;           BX = 1
;           CX = Segment => WORD
;           DX = Offset  => WORD
;
;**************************************************************************

Vect_Num equ    60h
Signature equ   899ah

cr      equ     0dh
lf      equ     0ah


_TEXT   segment word public 'CODE'
        assume cs:_TEXT,ds:_DATA

;*-----------------------  TSR Data Area ---------------------*
count   dw      0
oldint  dd      0
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
        jmp     short return

chk1:
        cmp     bx, 1                   ; increment pointer?
        jnz     short chain             ; no, just forget it
        mov     ds, cx                  ; segment value here
        mov     bx, dx                  ; offset here
        inc     word ptr [bx]           ; add it in
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
        pop     dx
        pop     cx
        add     sp, 2       ;!!!! DON'T RESTORE BX !!!!!!
        pop     ax
        pop     ds
        pop     es
        pop     di
        pop     si
        iret
handle  endp


        ALIGN   16
init_fence:           ; All code following this label will be discarded

;*-------------------------- Initialization Code ----------------------*

gtsr    proc    far
        mov     ax,_DATA
        mov     ds,ax

        mov     ax,Vect_Num
        mov     bx,offset tsrmsgx
        call    hexasc

        mov     dx,offset tsrmsg        ;Print message
        mov     cx,tsrmsgl
        mov     bx,1                    ;stdout
        mov     ah,40h                  ;write
        int     21h

;get old vector
        mov     ah,35h
        mov     al,Vect_Num
        int     21h
        mov     WORD PTR cs:oldint,bx
        mov     WORD PTR cs:oldint+2,es

        push    ds
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
gtsr    endp

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



        end     gtsr          
