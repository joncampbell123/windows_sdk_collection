        page    ,132
        title   memcmp - compare to blocks of memory
;***
;memcmp.asm - compare two blocks of memory
;
;       Copyright (c) 1985-1991, Microsoft Corporation. All rights reserved.
;
;Purpose:
;       defines memcmp() - compare two memory blocks lexically and
;       find their order.
;
;*******************************************************************************

        .xlist
        include cruntime.inc
        .list

page
;***
;int memcmp(buf1, buf2, count) - compare memory for lexical order
;
;Purpose:
;       Compares count bytes of memory starting at buf1 and buf2
;       and find if equal or which one is first in lexical order.
;
;       Algorithm:
;       int
;       memcmp (buf1, buf2, count)
;               char *buf1, *buf2;
;               unsigned count;
;       {
;               if (!count)
;                       return(0);
;               while (--count && *buf1 == *buf2)
;                       {
;                       buf1++;
;                       buf2++;
;                       }
;               return(*buf1 - *buf2);
;       }
;
;Entry:
;       char *buf1, *buf2 - pointers to memory sections to compare
;       unsigned count - length of sections to compare
;
;Exit:
;       returns -1 if buf1 < buf2
;       returns  0 if buf1 == buf2
;       returns +1 if buf1 > buf2
;
;Uses:
;
;Exceptions:
;
;*******************************************************************************

        CODESEG

        public  memcmp
memcmp  proc \
        uses edi esi, \
        buf1:ptr byte, \
        buf2:ptr byte, \
        count:IWORD

        mov     esi,buf1        ; si = buf1
        mov     edi,buf2        ; di = buf2

;
; choose ds:si=buf1 and es:di=buf2 so that the CARRY flag
; gets the right way by the REP CMPSB instruction below.
;
        xor     eax,eax
        mov     ecx,count
        jecxz   short done

        repe    cmpsb           ; compare while equal, at most "count" bytes
        je      short done      ; buf1 == buf2?  (AX = 0)
                                ;       buf1 < buf2     buf1 > buf2
        sbb     eax,eax         ;       AX=-1, CY=1     AX=0, CY=0
        sbb     eax,-1          ;       AX=-1           AX=1
done:

        ret                     ; _cdecl return

memcmp  endp
        end
