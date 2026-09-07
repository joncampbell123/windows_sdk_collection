        page    ,132
        title   memicmp - compare blocks of memory, ignore case
;***
;memicmp.asm - compare memory, ignore case
;
;       Copyright (c) 1985-1994, Microsoft Corporation. All rights reserved.
;
;Purpose:
;       defines _memicmp() - compare two blocks of memory for lexical
;       order.  Case is ignored in the comparison.
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
;int _memicmp(first, last, count) - compare two blocks of memory, ignore case
;
;Purpose:
;       Compares count bytes of the two blocks of memory stored at first
;       and last.  The characters are converted to lowercase before
;       comparing (not permanently), so case is ignored in the search.
;
;       Algorithm:
;       int
;       _memicmp (first, last, count)
;             char *first, *last;
;             unsigned count;
;             {
;             if (!count)
;                     return(0);
;             while (--count && tolower(*first) == tolower(*last))
;                     {
;                     first++;
;                     last++;
;                     }
;             return(tolower(*first) - tolower(*last));
;             }
;
;Entry:
;       char *first, *last - memory buffers to compare
;       unsigned count - maximum length to compare
;
;Exit:
;       returns <0 if first < last
;       returns 0 if first == last
;       returns >0 if first > last
;
;Uses:
;
;Exceptions:
;
;*******************************************************************************

        CODESEG

        public  _memicmp
_memicmp proc \
        uses edi esi ebx, \
        first:ptr byte, \
        last:ptr byte, \
        count:IWORD

        mov     ecx,[count]     ; cx = count
        or      ecx,ecx
        jz      toend           ; if count=0, nothing to do

        mov     esi,[first]     ; si = first
        mov     edi,[last]      ; di = last

        ; test locale

ifndef DLL_FOR_WIN32S
        lea     eax,__lc_handle
        cmp     [eax + LC_CTYPE],_CLOCALEHANDLE
else  ; DLL_FOR_WIN32S
        ; the following was obtained by compiling memicmp.c and inspecting the
        ; code listing.
        mov     ebx,ecx         ; save count in ebx
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
        mov     ecx,ebx         ; restore count to ecx
endif  ; DLL_FOR_WIN32S

        jne     notclocale

        ; C locale

        mov     bh,'A'
        mov     bl,'Z'
        mov     dh,'a'-'A'      ; add to cap to make lower

        align   4

lupe:
        mov     ah,[esi]        ; ah = *first
        inc     esi             ; first++
        mov     al,[edi]        ; al = *last
        inc     edi             ; last++

        cmp     ah,al           ; test for equality BEFORE converting case
        je      short dolupe

        cmp     ah,bh           ; ah < 'A' ??
        jb      short skip1

        cmp     ah,bl           ; ah > 'Z' ??
        ja      short skip1

        add     ah,dh           ; make lower case

skip1:
        cmp     al,bh           ; al < 'A' ??
        jb      short skip2

        cmp     al,bl           ; al > 'Z' ??
        ja      short skip2

        add     al,dh           ; make lower case

skip2:
        cmp     ah,al           ; *first == *last ??
        jne     short differ    ; nope, found mismatched chars

dolupe:
        dec     ecx
        jnz     short lupe

        jmp     short toend     ; cx = 0, return 0

differ:
        mov     ecx,-1          ; assume last is bigger
                                ; *** can't use "or ecx,-1" due to flags ***
        jb      short toend     ; last is, in fact, bigger (return -1)
        neg     ecx             ; first is bigger (return 1)

        jmp     short toend


notclocale:

        ; Not the C locale. Must call tolower/_tolower_lk to convert chars
        ; to lower case.

ifdef _MT
        mov     ebx,ecx         ; save count in ebx
        push    _LC_CTYPE_LOCK
        call    _lock
        add     esp, 4
        mov     ecx,ebx         ; restore count to ecx
endif  ; _MT

        xor     eax,eax
        xor     ebx,ebx

        align   4

lupe2:
        mov     al,[esi]        ; eax = *first
        inc     esi             ; first++
        mov     bl,[edi]        ; ebx = *last
        inc     edi             ; last++

        cmp     al,bl           ; test for equality BEFORE converting case
        je      short dolupe2

        push    ecx             ; save count

        push    eax
        push    ebx

ifdef _MT
        call    _tolower_lk     ; convert *last to lower case
else  ; _MT
        call    tolower         ; convert *last to lower case
endif  ; _MT

        mov     ebx,eax
        add     esp,4

ifdef _MT
        call    _tolower_lk     ; convert *first to lower case
else  ; _MT
        call    tolower         ; convert *first to lower case
endif  ; _MT

        add     esp,4

        pop     ecx             ; recover count

        cmp     al,bl           ; now equal?
        jne     short differ2

dolupe2:
        dec     ecx
        jnz     lupe2

        jmp     short toend2

differ2:
        mov     ecx,-1          ; return -1 if *first < *last
        jb      short toend2

        neg     ecx             ; return 1

toend2:

ifdef _MT
        mov     ebx,ecx         ; save return value in ebx
        push    _LC_CTYPE_LOCK
        call    _unlock
        add     esp, 4
        mov     ecx,ebx         ; restore return value to ecx
endif  ; _MT

toend:
        mov     eax,ecx         ; move return value to ax

        ret                     ; _cdecl return

_memicmp endp
        end
