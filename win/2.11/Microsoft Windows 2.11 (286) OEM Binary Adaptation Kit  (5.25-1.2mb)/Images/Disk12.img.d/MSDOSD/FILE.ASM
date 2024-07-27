;
; file.asm - primitves for manipulating MSDOS files
;

.xlist
include cmacros.inc
include disk.inc
include file.inc
if1                     ; This module also used by SETUP
ifndef ?SETUP
    ?SETUP1 = 0
else
    ?SETUP1 = 1
endif
endif
.list

sBegin CODE
assumes CS,CODE

externNP   <GetCurDisk,SetCurDisk>

ife ?SETUP1

cProc   GetCurDirectory,<PUBLIC,NEAR>,<si>
    parmW   pBuf
cBegin
        mov     si,pBuf
        cCall   GetCurDisk
        add     al,'A'
        mov     ah,':'
        mov     word ptr [si],ax
        inc     si
        inc     si
        mov     ax,'\'
        mov     word ptr [si],ax
        inc     si
        mov     ah,47h
        int     21h
        mov     ax,-1
        jc      gcddone
        not     ax
gcddone:
cEnd

endif

cProc   SetCurDirectory,<PUBLIC,NEAR>
    parmW   pBuf
cBegin
        mov     dx,pBuf
        mov     ah,3Bh
        int     21h
        mov     ax,-1
        jc      scddone
        not     ax
        mov     bx,dx
        cmp     byte ptr [bx+1],':'
        jne     scddone
        mov     al,[bx]
        sub     al,'A'
        cCall   SetCurDisk,<ax>
        xor     ax,ax
scddone:
cEnd


cProc   CreateDirectory,<PUBLIC,NEAR>
    parmW   pBuf
cBegin
        mov     dx,pBuf
        mov     ax,4300h
        int     21h
        jc      mdnew
        test    cl,A_DIR
        jnz     mdold
mdnew:
        mov     ah,39h
        int     21h
        jnc     mdold
mdfail:
        mov     ax,-1
        jmp     short mddone
mdold:
        xor     ax,ax
mddone:
cEnd


ife ?SETUP1

cProc   DeleteDirectory,<PUBLIC,NEAR>
    parmW   pBuf
cBegin
        mov     dx,pBuf
        mov     ah,3Ah
        int     21h
        mov     ax,-1
        jc      rddone
        not     ax
rddone:
cEnd

endif

;
; FindFile (name, attr, buf) - find a name in the file system with the
; appropriate attributes. returns 0 or -1
;

cProc   FindFile,<PUBLIC,NEAR>
    parmW pName
    parmW attr
    parmW pFindBuf
cBegin
        mov     dx,pFindBuf
        mov     ah,1Ah                  ; set dma to point to buffer
        int     21h
        xor     bx,bx
        mov     cx,attr
        mov     dx,pName
        mov     ah,4Eh
        int     21h
        mov     ax,bx
        jnc     ffexit
        dec     ax
ffexit:
cEnd

;
; FindNextFile (buf) - find next name in the file system with the
; appropriate attributes. returns 0 or -1
;

cProc   FindNextFile,<PUBLIC,NEAR>
    parmW pFindBuf
cBegin
        mov     dx,pFindBuf
        mov     ah,1Ah                  ; set dma to point to buffer
        int     21h
        xor     bx,bx
        mov     ah,4Fh
        int     21h
        mov     ax,bx
        jnc     fnexit
        dec     ax
fnexit:
cEnd

cProc OpenFile,<PUBLIC,NEAR>
    parmW pName
    parmB mode
cBegin
        mov     dx,pName
        mov     ah,3dh
        mov     al,mode
        int     21h
        jnc     opendone
        mov     ax,-1
opendone:
cEnd

cProc CreateFile,<PUBLIC,NEAR>
    parmW pname
    parmW attr
cBegin
        mov     dx,pname
        mov     ax,4300h
        int     21h
        jc      cf1
        and     cl,not (A_DIR+A_VID)
        mov     attr,cx
        xor     cx,cx
        mov     ax,4301h
        int     21h
cf1:
        mov     dx,pname
        mov     cx,attr
        test    cl,80h              ; If Novell shareable bit set
        jz      cf2
        and     cl,not 81h          ; then clear it and readonly bit
                                    ; as Novell wont allow writes if set
cf2:
        mov     ah,3ch
        int     21h
        jc      cffail
        mov     bx,ax               ; Do a zero length write
        xor     cx,cx
        mov     ah,40h
        int     21h
        mov     ax,bx
        jnc     cfdone
        mov     ah,3Eh
        int     21h
cffail:
        mov     ax,-1
cfdone:
cEnd

ife ?SETUP1
cProc CreateNewFile,<PUBLIC,NEAR>
    parmW pname
    parmW attr
cBegin
        mov     ah,30h
        int     21h
        mov     ah,5Bh
        mov     dx,pname
        cmp     al,3
        jae     cnfcreate
        mov     ax,4300h
        int     21h
        jnc     cnffail
        mov     ah,3ch
cnfcreate:
        mov     cx,attr
        int     21h
        jc      cnffail
        mov     bx,ax
        mov     ah,3Eh
        int     21h
        xor     ax,ax
        jmp     short cnfdone
cnffail:
        mov     ax,-1
cnfdone:
cEnd
endif

cProc DeleteFile,<PUBLIC,NEAR>
    parmW pName
cBegin
        mov     dx,pName
        xor     cx,cx
        mov     ax,4301h
        int     21h
        jc      deldone
        mov     dx,pName
        mov     ah,41h              ; delete directory entry
        int     21h
        jc      deldone
        xor     ax,ax
deldone:
cEnd

cProc MoveFilePtr,<PUBLIC,NEAR>
    parmW fh
    parmD pos
    parmB mode
cBegin
        mov     bx,fh
        mov     cx,SEG_pos
        mov     dx,OFF_pos
        mov     ah,42h
        mov     al,mode
        int     21h
        jnc     seekdone
        mov     ax,-1
        mov     dx,ax
seekdone:
cEnd

cProc ReadFile,<PUBLIC,NEAR>
    parmW fh
    parmD buf
    parmW count
cBegin
        push    ds
        mov     bx,fh
        lds     dx,buf
        mov     ah,3fh
        mov     cx,count
        int     21h
        jnc     rfdone
        xor     ax,ax
rfdone:
        pop     ds
cEnd

cProc WriteFile,<PUBLIC,NEAR>
    parmW   fh
    parmD   buf
    parmW   count
cBegin
        push    ds
        mov     bx,fh
        lds     dx,buf
        mov     ah,40h
        mov     cx,count
        int     21h
        jnc     wfdone
        xor     ax,ax
wfdone:
        pop     ds
cEnd

ife ?SETUP1
cProc TruncateFile,<PUBLIC,NEAR>
    parmW   fh
    parmD   xsize
cBegin
        mov     dx,OFF_xsize
        mov     cx,SEG_xsize
        mov     bx,fh
        mov     ax,4200h
        int     21h
        jc      tffail
        xor     cx,cx
        mov     ah,40h
        int     21h
        mov     ax,cx
        jnc     tfdone
tffail:
        mov     ax,-1
tfdone:
cEnd
endif

cProc CloseFile,<PUBLIC,NEAR>
    parmW fh
cBegin
        mov     bx,fh
        inc     fh
        jz      closedone
        mov     ah,3Eh
        int     21h
        jc      closedone
        mov     bx,-1
closedone:
        mov     ax,bx
cEnd

cProc   CopyFile,<PUBLIC,NEAR>,<si,di>
    parmW   pSrcName
    parmW   pDstName
    parmW   attr
    parmD   lpBuf
    parmW   cbBuf
cBegin
        mov     si,-1
        mov     di,si

        xor     ax,ax
        cCall   OpenFile,<pSrcName,ax>
        jc      cffail3
        mov     si,ax

        cCall   CreateFile,<pDstName,attr>
        jc      cffail3
        mov     di,ax

        push    ds
        lds     dx,lpBuf
cfloop:
        mov     cx,cbBuf
        mov     bx,si
        mov     ah,3Fh
        int     21h
        jc      cffail1
        mov     cx,ax
        jcxz    cffinish
        mov     bx,di
        mov     ah,40h
        int     21h
        jc      cffail1
        cmp     ax,cx
        je      cfloop
        jmp     short cffail1
cffinish:
        pop     ds
        mov     bx,si
        mov     ax,5700h
        int     21h
        jc      cffail2
        mov     bx,di
        mov     ax,5701h
        int     21h
        jc      cffail2
        xor     ax,ax
        jmp     short cfexit
cffail1:
        pop     ds
cffail2:
        cCall   CloseFile,<di>
        mov     di,ax
        cCall   DeleteFile,<pDstName>
cffail3:
        mov     ax,-1
cfexit:
        push    ax
        cCall   CloseFile,<si>
        cCall   CloseFile,<di>
        pop     ax
cEnd

cProc GetFileSize,<PUBLIC,NEAR>,<si,di>
    parmW fh
cBegin
        mov     bx,fh
        xor     cx,cx
        xor     dx,dx
        mov     ax,4201h
        int     21h
        jc      gfsfail
        mov     si,ax
        mov     di,dx
        xor     cx,cx
        xor     dx,dx
        mov     ax,4202h
        int     21h
        jc      gfsfail
        mov     cx,dx
        mov     dx,ax
        xchg    dx,si
        xchg    cx,di
        mov     ax,4200h
        int     21h
        jc      gfsfail
        mov     ax,si
        mov     dx,di
        jmp     short gfsdone
gfsfail:
        mov     ax,-1
        mov     dx,ax
gfsdone:
cEnd

ife ?SETUP1
cProc SetFileSize,<PUBLIC,NEAR>,<si,di>
    parmW fh
    parmD fsize
cBegin
        mov     bx,fh
        mov     ax,4200h
        mov     dx,OFF_fsize
        mov     cx,SEG_fsize
        int     21h
        jc      sfsfail
        mov     ah,40h
        xor     cx,cx
        int     21h
        jnc     sfsdone
sfsfail:
        mov     ax,-1
sfsdone:
cEnd

cProc GetFileDateTime,<PUBLIC,NEAR>
    parmW fh
cBegin
        mov     bx,fh
        mov     ax,5700h
        int     21h
        mov     ax,cx
        jnc     gfdtdone
        mov     ax,-1
        mov     dx,ax
gfdtdone:
cEnd

cProc SetFileDateTime,<PUBLIC,NEAR>
    parmW fh
    parmD datetime
cBegin
        mov     bx,fh
        mov     cx,OFF_datetime
        mov     dx,SEG_datetime
        mov     ax,5701h
        int     21h
        mov     ax,0
        jnc     sfdtdone
        mov     ax,-1
sfdtdone:
cEnd
endif


; GetFileAttr returns a file's attribute.
;
;  Inputs:     pName   pointer to file name
;  Returns:    attr    if success
;              -1      error
;  Side effects:       none
;
cProc   GetFileAttr,<PUBLIC,NEAR>
    parmW   pname
cBegin
        mov     ax,4300h
        mov     dx,pname
        int     21h
        mov     ax,cx
        jnc     gfadone
        mov     ax,-1
gfadone:
cEnd

ife ?SETUP1
; SetFileAttr changes a files attribute.  This is a replacement of attribute
; and not an augmentation.
;
;  Inputs:     pName   pointer to file name
;              attr    attribute to set.
;  Returns:    0       if success
;              -1      error
;  Side effects:       none
;
cProc   SetFileAttr,<PUBLIC,NEAR>
    parmW   pname
    parmW   attr
cBegin
        mov     ax,4301h
        mov     cx,attr
        mov     dx,pname
        int     21h
        jnc     sfadone
        mov     ax,-1
sfadone:
cEnd
endif

;
; RootPath
;
cProc   RootPath,<PUBLIC,NEAR>,<si,di>
    parmW   pSrc
    parmW   pDst
cBegin
        push    bp
        mov     si,pSrc
        mov     di,pDst
        push    ds
        pop     es
        cld
        xor     bp,bp                   ; count of slashes encountered
        cmp     byte ptr ds:[si+1],':'
        jne     nodrive
        lodsb
        inc     si
        or      al,20h                  ; convert to lower case
        sub     al,'a'                  ; convert to number
        jb      pf1
        cmp     al,'z'-'a'
        jbe     GotDrive
pf1:
        jmp     gpFail2
nodrive:
        mov     ah,19h
        int     21h
gotdrive:
        mov     dl,al
        inc     dl
        add     al,'A'                  ; convert to ascii
        mov     ah,':'
        stosw                           ; Store drive letter and colon

        mov     bx,'/' shl 8 + '\'
        mov     al,ds:[si]
        cmp     al,bh
        je      getpath
        cmp     al,bl
        je      getpath
        mov     al,bl
        stosb
        mov     cx,ds
        xchg    si,di
        mov     ax,es
        mov     ds,ax
        mov     ah,47h
        int     21h
        jc      gpfail
        mov     ds,cx
        xchg    si,di
        xor     al,al
        mov     cx,-1
        repnz   scasb
        dec     di
        mov     al,es:[di-1]
        cmp     al,bh
        je      getpath
        cmp     al,bl
        je      getpath
        mov     al,bl
        stosb
getpath:
        xor     cx,cx
        mov     dx,di
gp0:
        lodsb
        cmp     al,bl
        je      gp1
        cmp     al,bh
        jne     gp2
gp1:
        cmp     ds:[si],bl              ; if double slash, bad file name
        je      gp1e
        cmp     ds:[si],bh
        jne     gp1f
gp1e:
        dec     bp                      ; Only count double slash as one
        cmp     byte ptr es:[di-1],":"  ; if first slash, 2 are ok
        jne     gpfail
        cmp     byte ptr ds:[si+1],0
        je      gpfail                  ; unless double slash only
gp1f:
        inc     bp                      ; we found a slash
        cmp     cl,ch                   ; number of chars = number of '.'
        jne     gp1b                    ; nope....
        jcxz    gp2b                    ; first slash?

        cmp     cl,2                    ; if more than 2 '.'
        ja      gpFail                  ; then we are bogus

        dec     di
        dec     cl
        jz      getpath
        mov     di,dx
gp1a:
        dec     di
        mov     al,es:[di-1]
        cmp     al,bl
        je      getpath
        cmp     al,':'
        jne     gp1a
gpfail:
gpFail2:
        xor     ax,ax
        jmp     gpexit
gp1b:
        mov     al,bl
        stosb
        jmp     getpath
gp2:
        or      al,al
        jz      gpx
        cmp     al,' '
        jb      gpFail
        ja      gp20

gp2x:   lodsb                   ; if space encountered continue scanning...
        or      al,al           ; if end of string, all ok
        jz      gpx
        cmp     al,' '          ; if space, keep looking...
        jz      gp2x
        jmp     short gpFail    ; otherwise error

gp20:   cmp     al,';'
        jz      gpFail
        cmp     al,':'
        jz      gpFail
        cmp     al,','
        jz      gpFail
        cmp     al,'|'
        jz      gpFail
        cmp     al,'+'
        jz      gpFail
        cmp     al,'<'
        jz      gpFail
        cmp     al,'>'
        jz      gpFail
        cmp     al,'"'
        jz      gpFail
        cmp     al,'['
        jz      gpFail
        cmp     al,']'
        jz      gpFail
        cmp     al,'='
        jz      gpFail

        cmp     al,'a'
        jb      gp2a
        cmp     al,'z'
        ja      gp2a
        add     al,'A'-'a'
gp2a:
        inc     cl              ; one more char
        js      gpFail          ; Fail if 128 or more.
gp2b:
        stosb
        cmp     al,'.'
        jne     gp2c
        inc     ch
        mov     ah,cl
        dec     ah
gp2c:
        cmp     ch,0            ; did we find a . yet?
        jz      gpT1            ; no, max count is 8
        cmp     cl,12           ; otherwise check for 12 chars
        ja      gpFv            ; if more, no good
        mov     al,cl
        sub     al,ah
        cmp     al,3+1          ; more than 3 chars in extension?
        ja      gpFv
        jmp     gp0             ; continue

gpT1:   cmp     cl,8            ; did we find 8 chars between / and .
        ja      gpFv            ; if more, we fail
        jmp     gp0
gpx:
        cmp     ch,1
        je      gpx1
        ja      gpFv                    ; if more than 1 we fail
        mov     ah,cl                   ; No extension given
gpx1:
        mov     es:[di],al              ; Zero terminate destination
        xchg    al,ah                   ; Get length of name in AX
        or      ax,ax
        jnz     gpx2
        not     ax                      ; If zero and one slash
        cmp     bp,1
        je      gpexit                  ; return -1 to indicate root
        xor     ax,ax                   ; o.w. clobber trailing slash
        mov     es:[di-1],ah
        jmp     short gpx2
gpFv:   jmp     gpFail
gpx2:
        cmp     ax,8                    ; but no more than 8
        ja      gpFv
        mov     ax,1
gpexit:
        pop     bp
cEnd

sEnd    CODE

end
