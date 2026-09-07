        page    ,132
        title   strnicmp - compare n chars of strings, ignore case
;***
;strnicmp.asm - compare n chars of strings, ignoring case
;
;       Copyright (c) 1985-1994, Microsoft Corporation. All rights reserved.
;
;Purpose:
;       defines _strnicmp() - Compares at most n characters of two strings,
;       without regard to case.
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
;int _strnicmp(first, last, count) - compares count char of strings, ignore case
;
;Purpose:
;       Compare the two strings for lexical order.  Stops the comparison
;       when the following occurs: (1) strings differ, (2) the end of the
;       strings is reached, or (3) count characters have been compared.
;       For the purposes of the comparison, upper case characters are
;       converted to lower case.
;
;       Algorithm:
;       int
;       _strncmpi (first, last, count)
;             char *first, *last;
;             unsigned int count;
;             {
;             int f,l;
;             int result = 0;
;
;             if (count) {
;                     do      {
;                             f = tolower(*first);
;                             l = tolower(*last);
;                             first++;
;                             last++;
;                             } while (--count && f && l && f == l);
;                     result = f - l;
;                     }
;             return(result);
;             }
;
;Entry:
;       char *first, *last - strings to compare
;       unsigned count - maximum number of characters to compare
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

        public  _strnicmp
_strnicmp proc \
        uses edi esi ebx, \
        first:ptr byte, \
        last:ptr byte, \
        count:IWORD

        mov     ecx,[count]     ; cx = byte count
        or      ecx,ecx
        jz      toend           ; if count = 0, we are done

        mov     esi,[first]     ; si = first string
        mov     edi,[last]      ; di = last string

        ; test locale

ifndef DLL_FOR_WIN32S
        lea     eax,__lc_handle
        cmp     [eax + LC_CTYPE],_CLOCALEHANDLE
else  ; DLL_FOR_WIN32S
        ; the following was obtained by compiling strnicmp.c and inspecting the
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
        mov     ah,[esi]        ; *first

        or      ah,ah           ; see if *first is null

        mov     al,[edi]        ; *last

        jz      short eject     ;   jump if *first is null

        or      al,al           ; see if *last is null
        jz      short eject     ;   jump if so

        inc     esi             ; first++
        inc     edi             ; last++

        cmp     ah,bh           ; 'A'
        jb      short skip1

        cmp     ah,bl           ; 'Z'
        ja      short skip1

        add     ah,dh           ; make lower case

skip1:
        cmp     al,bh           ; 'A'
        jb      short skip2

        cmp     al,bl           ; 'Z'
        ja      short skip2

        add     al,dh           ; make lower case

skip2:
        cmp     ah,al           ; *first == *last ??
        jne     short differ

        dec     ecx
        jnz     short lupe

eject:
        xor     ecx,ecx
        cmp     ah,al           ; compare the (possibly) differing bytes
        je      short toend     ; both zero; return 0

differ:
        mov     ecx,-1          ; assume last is bigger (* can't use 'or' *)
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
        mov     al,[esi]        ; *first

        or      eax,eax         ; see if *first is null

        mov     bl,[edi]        ; *last

        jz      short eject2    ;   jump if *first is null

        or      ebx,ebx         ; see if *last is null
        jz      short eject2    ;   jump if so

        inc     esi
        inc     edi

        push    ecx             ; save ecx (holds count)
        push    eax
        push    ebx

ifdef _MT
        call    _tolower_lk     ; convert *last
else  ; _MT
        call    tolower         ; convert *last
endif  ; _MT

        mov     ebx,eax
        add     esp,4

ifdef _MT
        call    _tolower_lk     ; convert *first
else  ; _MT
        call    tolower         ; convert *first
endif  ; _MT

        add     esp,4

        pop     ecx             ; restore ecx (count)

        cmp     eax,ebx
        jne     short differ2

        dec     ecx
        jnz     short lupe2

eject2:
        xor     ecx,ecx
        cmp     eax,ebx
        je      short toend2

differ2:
        mov     ecx,1
        jb      short toend2

        neg     ecx

toend2:

ifdef _MT
        mov     ebx,ecx         ; save return value in ebx
        push    _LC_CTYPE_LOCK
        call    _unlock
        add     esp, 4
        mov     ecx,ebx         ; restore return value to ecx
endif  ; _MT

toend:
        mov     eax,ecx

        ret                     ; _cdecl return

_strnicmp endp
         end
