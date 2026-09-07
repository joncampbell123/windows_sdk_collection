        page    ,132
        title   strncpy - copy at most n characters of string
;***
;strncpy.asm - copy at most n characters of string
;
;       Copyright (c) 1985-1991, Microsoft Corporation. All rights reserved.
;
;Purpose:
;       defines strncpy() - copy at most n characters of string
;
;*******************************************************************************

        .xlist
        include cruntime.inc
        .list

page
;***
;char *strncpy(dest, source, count) - copy at most n characters
;
;Purpose:
;       Copies count characters from the source string to the
;       destination.  If count is less than the length of source,
;       NO NULL CHARACTER is put onto the end of the copied string.
;       If count is greater than the length of sources, dest is padded
;       with null characters to length count.
;
;       Algorithm:
;       char *
;       strncpy (dest, source, count)
;             char *dest, *source;
;             unsigned count;
;             {
;             char *start = dest;
;
;             while (count && (*dest++ = *source++))
;                     count--;
;             if (count)
;                     while (--count)
;                             *dest++ = '\0';
;             return(start);
;             }
;
;Entry:
;       char *dest - pointer to spot to copy source, enough space
;       is assumed.
;       char *source - source string for copy
;       unsigned count - characters to copy
;
;Exit:
;       returns dest, with the character copied there.
;
;Uses:
;
;Exceptions:
;
;*******************************************************************************

        CODESEG

        public  strncpy
strncpy proc \
        uses edi esi, \
        dest:ptr byte, \
        sorc:ptr byte, \
        count:IWORD


        mov     edi,[dest]      ; di=pointer to dest
        mov     esi,[sorc]      ; si=pointer to source

        mov     edx,edi         ; dx saves dest pointer
        mov     ecx,[count]     ; get the max char count
        jecxz   short toend     ; don't do loop if nothing to move

lupe:
        lodsb                   ; get byte into al and kick si
        or      al,al           ; see if we just moved a null
        jz      short outlupe   ; end of string

        stosb                   ; store byte from al and kick di
        loop    lupe            ; dec cx & jmp to lupe if nonzero
                                ; else drop out
outlupe:
        xor     al,al           ; null byte to store
rep     stosb                   ; store null for all cx>0

toend:
        mov     eax,edx         ; return value: dest addr

        ret                     ; _cdecl return

strncpy endp
        end
