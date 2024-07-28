        title   lmemcpy.asm

;***************************************************************
;*
;*  PSCRIPT.DRV
;*
;*  void FAR PASCAL lmemcpy(lpbDst, lpbSrc, cb);
;*
;****************************************************************

?WIN = 1

.xlist
include cmacros.inc
.list

        extrn DOS3Call : far

sBegin  CODE

assumes CS,CODE
assumes DS,DATA

cProc   lmemcpy,<PUBLIC,FAR>,<di,si,ds>
        parmD   lpDest
        parmD   lpSrc
        parmW   cnt
cBegin
        les     di,lpDest
        lds     si,lpSrc
        mov     cx,cnt
        cld
        repne   movsb
cEnd

cProc   lmemset,<PUBLIC,FAR>,<di,si>
        parmD   lpDest
        parmW   val
        parmW   cnt
cBegin
        les     di,lpDest
        mov     ax,val
        mov     cx,cnt
        cld
        repne   stosb
cEnd

;*--------------------------------------------------------------------------*
;*                                                                          *
;*  DosFindFirst() -                                                        *
;*                                                                          *
;*--------------------------------------------------------------------------*

; Get the first directory entry.

cProc DosFindFirst, <FAR, PUBLIC>

ParmD lpDest
ParmD szFileSpec
ParmW attrib

cBegin
            push    ds

            mov     ah,2fh
            call	DOS3Call
            push    es
            push    bx

            lds     dx,lpDest
            mov     ah,1Ah          ; Set DTA
            call	DOS3Call

            mov     cx,attrib       ; Find First File
            lds     dx,szFileSpec   ; Path = szFileSpec
            mov     ah,4Eh
            call	DOS3Call
            jc      ffdone
            xor     ax,ax

ffdone:
            pop     dx
            pop     ds
            push    ax
            mov     ah,1ah
            call	DOS3Call
            pop     ax

            pop     ds


cEnd


;*--------------------------------------------------------------------------*
;*                                                                          *
;*  DosFindNext() -                                                         *
;*                                                                          *
;*--------------------------------------------------------------------------*

cProc DosFindNext, <FAR, PUBLIC>

ParmD lpDest

cBegin
            push    ds

            mov     ah,2fh
            call	DOS3Call
            push    es
            push    bx

            lds     dx,lpDest
            mov     ah,1Ah          ; Set DTA
            call	DOS3Call

            les     bx,lpDest       ; ES:BX = lpDest
            mov     ah,4Fh          ; Find Next File
            call	DOS3Call
            jc      FNExit          ; Exit if error
            xor     ax,ax           ; Return TRUE

FNExit:

            pop     dx
            pop     ds
            push    ax
            mov     ah,1ah
            call	DOS3Call
            pop     ax

            pop     ds

cEnd

sEnd    CODE

end



