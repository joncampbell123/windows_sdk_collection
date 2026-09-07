        page    ,132
        title   strstr - search for one string inside another
;***
;strstr.asm - search for one string inside another
;
;       Copyright (c) 1985-1995, Microsoft Corporation. All rights reserved.
;
;Purpose:
;       defines strstr() - search for one string inside another
;
;*******************************************************************************

        .xlist
        include cruntime.inc
        .list

page
;***
;char *strstr(str1, str2) - search for string2 in string1
;
;Purpose:
;       finds the first occurrence of str2 in str1
;
;Entry:
;       char *str1 - string to search in
;       char *str2 - string to search for
;
;Exit:
;       returns a pointer to the first occurrence of string2 in
;       string1, or NULL if string2 does not occur in string1
;
;Uses:
;
;Exceptions:
;
;*******************************************************************************

        CODESEG

        public  strstr
strstr proc \
        uses esi edi ebx, \
        str1:ptr byte, \
        str2:ptr byte

        ; use two registers for each string pointer so we can save, and quickly
        ; recover, the positions at which we start comparing characters

        mov     esi,[str1]
        mov     ecx,esi
        mov     edi,[str2]
        mov     edx,edi

mainlupe:
        mov     al,[edi]
        or      al,al           ; if we've reached the end of str2, we've
        jz      short success   ; found the first matching substring

        mov     ah,[esi]
        or      ah,ah           ; if we've reached the end of str1, we've
        jz      short failure   ; failed

        cmp     al,ah           ; characters match?
        je      matchchar

        ; failed comparison. recover the pointer to the start of str2 and
        ; where we started the comparision in str1. increment the later
        ; pointer (to start comparing again)

        mov     edi,edx
        inc     ecx
        mov     esi,ecx

        jmp     short mainlupe

matchchar:
        inc     edi
        inc     esi
        jmp     short mainlupe


success:
        mov     eax,ecx
        jmp     short doret

failure:
        xor     eax,eax

doret:
        ret

strstr  endp
        end
