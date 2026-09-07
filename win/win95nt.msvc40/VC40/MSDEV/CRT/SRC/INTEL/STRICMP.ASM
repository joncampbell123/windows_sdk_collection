        page    ,132
        title   stricmp
;***
;stricmp.asm - contains case-insensitive string comparision routine
;       _stricmp/_strcmpi
;
;       Copyright (c) 1985-1994, Microsoft Corporation. All rights reserved.
;
;Purpose:
;       contains _stricmpi(), also known as _strcmpi()
;
;*******************************************************************************

        .xlist
        include cruntime.inc
        .list


ifdef _MT

; Def and decls necessary to assert the lock for the LC_CTYPE locale category

_LC_CTYPE_LOCK  EQU     21

extrn   _lock:proc
extrn   _unlock:proc

endif  ; _MT

; Defs and decl necessary to test for the C locale.

_CLOCALEHANDLE  EQU     0
LC_CTYPE        EQU     2 * 4

ifndef DLL_FOR_WIN32S

extrn   __lc_handle:dword

else  ; DLL_FOR_WIN32S

extrn   _GetPPD:proc

endif  ; DLL_FOR_WIN32S

ifdef _MT

extrn   _tolower_lk:proc

else  ; _MT

extrn   tolower:proc

endif  ; _MT


page
;***
;int _stricmp(dst, src), _strcmpi(dst, src) - compare strings, ignore case
;
;Purpose:
;       _stricmp/_strcmpi perform a case-insensitive string comparision.
;       For differences, upper case letters are mapped to lower case.
;       Thus, "abc_" < "ABCD" since "_" < "d".
;
;       Algorithm:
;
;       int _strcmpi (char * dst, char * src)
;       {
;               int f,l;
;
;               do {
;                       f = tolower(*dst);
;                       l = tolower(*src);
;                       dst++;
;                       src++;
;               } while (f && f == l);
;
;               return(f - l);
;       }
;
;Entry:
;       char *dst, *src - strings to compare
;
;Exit:
;       AX = -1 if dst < src
;       AX =  0 if dst = src
;       AX = +1 if dst > src
;
;Uses:
;       CX, DX
;
;Exceptions:
;
;*******************************************************************************

        CODESEG

        public  _strcmpi        ; alternate entry point for compatibility
_strcmpi label   proc

        public  _stricmp
_stricmp proc \
        uses edi esi ebx, \
        dst:ptr, \
        src:ptr

        ; load up args

        mov     esi,[src]       ; esi = src
        mov     edi,[dst]       ; edi = dst

        ; test locale

ifndef DLL_FOR_WIN32S
        lea     eax,__lc_handle
        cmp     [eax + LC_CTYPE],_CLOCALEHANDLE
else  ; DLL_FOR_WIN32S
        ; the following was obtained by compiling stricmp.c and inspecting the
        ; code listing.
        call    _GetPPD
        ;
        ;       IMPORTANT NOTE:
        ;               stricmp.asm, strnicmp.asm, and memicmp.asm hard-code the offset
        ;               of the _lc_handle[2] field within the _CRTDLLPPD structure.
        ;
        ;               This field MUST be first in the _CRTDLLPPD structure (win32s.h).
        ;
        ;               Otherwise a debug assertion at Win32s DLL startup will be triggered (crtlib.c).
        ;
        cmp     dword ptr [eax + 8],0
endif  ; DLL_FOR_WIN32S

        jne     notclocale

        ; C locale

        mov     al,-1           ; fall into loop

        align   4

chk_null:
        or      al,al
        jz      short done

        mov     al,[esi]        ; al = next source byte
        inc     esi
        mov     ah,[edi]        ; ah = next dest byte
        inc     edi

        cmp     ah,al           ; first try case-sensitive comparision
        je      short chk_null  ; match

        sub     al,'A'
        cmp     al,'Z'-'A'+1
        sbb     cl,cl
        and     cl,'a'-'A'
        add     al,cl
        add     al,'A'          ; tolower(*dst)

        xchg    ah,al           ; operations on AL are shorter than AH

        sub     al,'A'
        cmp     al,'Z'-'A'+1
        sbb     cl,cl
        and     cl,'a'-'A'
        add     al,cl
        add     al,'A'          ; tolower(*src)

        cmp     al,ah           ; inverse of above comparison -- AL & AH are swapped
        je      short chk_null

                                ; dst < src     dst > src
        sbb     al,al           ; AL=-1, CY=1   AL=0, CY=0
        sbb     al,-1           ; AL=-1         AL=1
done:
        movsx   eax,al          ; extend al to eax

        jmp     short doret

notclocale:

        ; Not the C locale. Must call tolower/_tolower_lk to convert chars
        ; to lower case.

ifdef _MT
        push    _LC_CTYPE_LOCK
        call    _lock
        add     esp, 4
endif  ; _MT

        mov     eax,255
        xor     ebx,ebx

        align   4

chk_null2:
        or      al,al           ; not that if al == 0, then eax == 0!
        jz      short done2

        mov     al,[esi]        ; al = next src byte
        inc     esi
        mov     bl,[edi]        ; bl = next dst byte
        inc     edi

        cmp     al,bl           ; first try case-sensitive comparision
        je      short chk_null2 ; match

        push    eax
        push    ebx

ifdef _MT
        call    _tolower_lk     ; convert dst char to lower case
else  ; _MT
        call    tolower         ; convert dst char to lower case
endif  ; _MT

        mov     ebx,eax
        add     esp,4

ifdef _MT
        call    _tolower_lk     ; convert src char to lower case
else  ; _MT
        call    tolower         ; convert src char to lower case
endif  ; _MT

        add     esp,4

        cmp     bl,al
        je      chk_null2

        sbb     eax,eax
        sbb     eax,-1

done2:

ifdef _MT
        mov     ebx,eax         ; save return value in ebx
        push    _LC_CTYPE_LOCK
        call    _unlock
        add     esp, 4
        mov     eax,ebx         ; recover return value
endif  ; _MT

doret:
        ret

_stricmp endp
        end
