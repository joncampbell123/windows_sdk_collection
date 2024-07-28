;****************************************************************************
;*                                                                          *
;*  DOS.ASM -								    *
;*                                                                          *
;*	File I/O utilities						    *
;*                                                                          *
;****************************************************************************
ifdef DLL
extrn  DOS3CALL 	 :far
Y
endif

?PLM=1      ; PASCAL Calling convention is DEFAULT
?WIN=1      ; Windows calling convention
?386=0      ; Use 386 code?

include cmacros.inc

; The following structure should be used to access high and low
; words of a DWORD.  This means that "word ptr foo[2]" -> "foo.hi".

LONG    struc
lo      dw      ?
hi      dw      ?
LONG    ends

FARPOINTER      struc
off     dw      ?
sel     dw      ?
FARPOINTER      ends

ifndef SEGNAME
    SEGNAME equ <TEXT>
endif

if ?386
    createSeg _%SEGNAME, CodeSeg, word, use16, CODE
else
    createSeg _%SEGNAME, CodeSeg, word, public, CODE
endif

NOT_SUPPORTED     =  2h      ; Return code from IsDeviceRemote function.
TRUE              =  1h      ; TRUE Definition
FALSE             =  0h      ; False Definition.

;=============================================================================

sBegin DATA


sEnd   DATA

;=============================================================================

sBegin CodeSeg

assumes CS,CodeSeg
assumes DS,DATA

;*--------------------------------------------------------------------------*
;*                                                                          *
;*  GetCurrentDrive() - Determines letter of current drive		    *
;*                                                                          *
;*--------------------------------------------------------------------------*

cProc GetCurrentDrive, <FAR, PUBLIC>

cBegin
	mov	ah,19h		    ; Get Current Drive
ifdef DLL
	DOS3CALL
else
	int	21h
endif
        sub     ah,ah               ; Zero out AH
cEnd

;*--------------------------------------------------------------------------*
;*                                                                          *
;*  DosCwd() -	Returns current working directory			    *
;*                                                                          *
;*--------------------------------------------------------------------------*

cProc DosCwd, <FAR, PUBLIC>, <SI, DI>

ParmD lpDest

cBegin
            push    ds                  ; Preserve DS

            call    GetCurrentDrive
            mov     si,ax               ; SI = Current drive

            les     di,lpDest           ; ES:DI = lpDest
            push    es
            pop     ds                  ; DS:DI = lpDest
            cld
            mov     ax,si               ; AX = Current drive
            inc     al                  ; Convert to logical drive number
            mov     dl,al               ; DL = Logical Drive Number
            add     al,'@'              ; Convert to ASCII drive letter
            stosb
            mov     al,':'
            stosb
            mov     al,'\'              ; Start string with a backslash
            stosb
            mov     byte ptr es:[di],0  ; Null terminate in case of error
            mov     si,di               ; DS:SI = lpDest[1]
            mov     ah,47h              ; Get Current Directory
ifdef DLL
	    DOS3CALL
else
	    int     21h
endif
            jc      CDExit              ; Skip if error
            xor     ax,ax               ; Return FALSE if no error
CDExit:
            pop     ds                  ; Restore DS
cEnd

;*--------------------------------------------------------------------------*
;*                                                                          *
;*  FileExists() - Determines if the input filename exists		    *
;*                                                                          *
;*--------------------------------------------------------------------------*

cProc FileExists, <FAR, PUBLIC>

ParmD lpszPath

cBegin
            push    ds                  ; Preserve DS
            lds     dx,lpszPath         ; DS:DI = lpszPath
            mov     ax,4300h            ; Get File Attributes
ifdef DLL
	    DOS3CALL
else
	    int     21h
endif
            jc      GFAErr
            mov     ax,cx               ; AX = attribute
	    jmp short	 GFAExit	; Return attribute
GFAErr:     mov     ah,80h              ; Return negative error code
GFAExit:
            pop     ds                  ; Restore DS
cEnd

;*--------------------------------------------------------------------------*
;*                                                                          *
;*  SetCurrentDrive() - Sets the current drive				    *
;*                                                                          *
;*--------------------------------------------------------------------------*

; Returns the number of drives in AX.

cProc SetCurrentDrive, <FAR, PUBLIC>

ParmW Drive

cBegin
            mov     dx,Drive
            mov     ah,0Eh              ; Set Current Drive
ifdef DLL
	    DOS3CALL
else
	    int     21h
endif
            sub     ah,ah               ; Zero out AH
cEnd

;*--------------------------------------------------------------------------*
;*                                                                          *
;*  DosChDir() - Changes current working dir. to specified dir. 	    *
;*                                                                          *
;*--------------------------------------------------------------------------*

cProc DosChDir, <FAR, PUBLIC>

ParmD lpDirName

cBegin
            push    ds                  ; Preserve DS
            lds     dx,lpDirName        ; DS:DX = lpDirName

            mov     bx,dx
            mov     ax,ds:[bx]
            cmp     ah,':'
            jnz     cdnodrive

            ;
            ;       Convert drive letter to drive index
            ;
            or      al,20h
            sub     al,'a'
            xor     ah,ah

            push    dx
            cCall   SetCurrentDrive,<ax>
            pop     dx
cdnodrive:
            mov     ah,3Bh              ; Change Current Directory
ifdef DLL
	    DOS3CALL
else
	    int     21h
endif
            jc      SCDExit             ; Skip on error
            xor     ax,ax               ; Return FALSE if successful
SCDExit:
            pop     ds                  ; Restore DS
cEnd

;*--------------------------------------------------------------------------*
;*                                                                          *
;*  DosValidDir() - Determines if input directory name is valid 	    *
;*                                                                          *
;*--------------------------------------------------------------------------*

cProc DosValidDir, <FAR, PUBLIC>, <SI, DI>
ParmD       szDir
LocalV      szCwd, 128
cBegin
    lea     si,szCwd
    cCall   DosCwd,<ss,si>
    push    szDir.sel
    push    szDir.off
    call    DosChDir
    or      ax,ax
    pushf
    cCall   DosChDir,<ss,si>
    ;
    ;   return TRUE if DosChdir returns 0, FALSE otherwise
    ;
    xor     ax,ax                ; don't care about this return val.
    popf
    jnz     vdexit
    inc     ax
vdexit:

cEnd
;*--------------------------------------------------------------------------*
;*                                                                          *
;*  DosDelete(szFile);                                                      *
;*                                                                          *
;*  Delete file                                                             *
;*                                                                          *
;*--------------------------------------------------------------------------*

cProc DosDelete, <FAR, PUBLIC>, <DS>
ParmD   szFile
cBegin
        lds     dx,szFile
        mov     ah,41h
ifdef DLL
	DOS3CALL
else
	int	21h
endif
        jc      dexit
        xor     ax,ax
dexit:
cEnd
sEnd CodeSeg


end

