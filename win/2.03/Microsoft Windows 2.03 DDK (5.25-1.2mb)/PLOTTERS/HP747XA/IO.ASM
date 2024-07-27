.xlist
include cmacros.inc
.list

; The following are a set of C callable DOS entries fot file and device IO.

; int         FAR PASCAL OpenPathname( LPSTR, int );
; int         FAR PASCAL DeletePathname( LPSTR );
; int         FAR PASCAL _lopen( LPSTR, int );
; int         FAR PASCAL _lclose( int );
; int         FAR PASCAL _lcreat( LPSTR, int );
; int         FAR PASCAL _ldelete( LPSTR );
; int         FAR PASCAL _ldup( int );
; long        FAR PASCAL _llseek( int, long, int );
; int         FAR PASCAL _lread( int, LPSTR, int );
; int         FAR PASCAL _lwrite( int, LPSTR, int );


Assumes cs,CODE
sBegin  CODE

cProc   _lopen <PUBLIC,FAR>
        parmD   lpFilename
        parmW   mode
cBegin
        push    ds
        lds     dx,lpFilename
        mov     ax,mode
        mov     ah,3Dh              ; DOS file open function
        int     21h
        jnc     lopen_ok
        mov     ax,-1
lopen_ok:
        pop     ds
cEnd

cProc   _lcreat <PUBLIC,FAR>
        parmD   lpFilename
        parmW   attributes
cBegin
        push    ds
        lds     dx,lpFilename
        mov     cx,attributes
        mov     ah,3Ch              ; DOS file create function
        int     21h
        jnc     lcreat_ok
        mov     ax,-1
lcreat_ok:
        pop     ds
cEnd


cProc   _lclose <PUBLIC,FAR>
        parmW   fd
cBegin
        mov     bx,fd
        mov     ah,3Eh              ; DOS file close function
        int     21h
        jnc     lclose_ok
        mov     ax,-1
        jmp     lclose_end
lclose_ok:
        xor     ax,ax
lclose_end:
cEnd

cProc   _ldelete <PUBLIC,FAR>
        parmD   lpFilename
cBegin
        push    ds
        lds     dx,lpFilename
        mov     ah,41h              ; DOS file delete function
        int     21h
        jnc     ldelete_ok
        mov     ax,-1
        jmp     ldelete_end
ldelete_ok:
        xor     ax,ax
ldelete_end:
        pop     ds
cEnd

cProc   _ldup <PUBLIC,FAR>
        parmW   fh              ; file handle to be duplicated
cBegin
        mov     ah,45h          ; DOS dup call
        mov     bx,fh
        int     21h
        jnc     dup_done        ; no carry means dup OK
        mov     ax,-1
dup_done:
cEnd                            ; file handle or error in ax


cProc   _lread  <PUBLIC,FAR>
        parmW   fd
        parmD   lpBuf
        parmW   bufsize
cBegin
        push    ds
        mov     bx,fd
        lds     dx,lpBuf
        mov     cx,bufsize
        mov     ah,3Fh
        int     21h
        jnc     lread_ok
        mov     ax,-1
lread_ok:
        pop     ds
cEnd


cProc   _lwrite <PUBLIC,FAR>
        parmW   fd
        parmD   lpBuf
        parmW   bufsize
cBegin
        push    ds
        mov     bx,fd
        lds     dx,lpBuf
        mov     cx,bufsize
        mov     ah,40h
        int     21h
        jnc     lwrite_ok
        mov     ax,-1
lwrite_ok:
        pop     ds
cEnd


cProc   _llseek  <PUBLIC,FAR>
        parmW   fh
        parmD   fileOffset
        parmW   mode
cBegin
        push    es
        mov     bx,fh
        les     dx,fileOffset
        mov     cx,es
        mov     ax,mode
        mov     ah,42h
        int     21h
        jnc     lseek_ok
        mov     ax,-1
        cwd                         ; must return a long
lseek_ok:
        pop     es
cEnd

sEnd    CODE
        END
