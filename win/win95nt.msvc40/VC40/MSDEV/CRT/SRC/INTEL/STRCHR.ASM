        page    ,132
        title   strchr - search string for given character
;***
;strchr.asm - search a string for a given character
;
;       Copyright (c) 1985-1994, Microsoft Corporation. All rights reserved.
;
;Purpose:
;       defines strchr() - search a string for a character
;
;*******************************************************************************

        .xlist
        include cruntime.inc
        .list

page
;***
;char *strchr(string, c) - search a string for a character
;
;Purpose:
;       Searches a string for a given character, which may be the
;       null character '\0'.
;
;       Algorithm:
;       char *
;       strchr (string, ch)
;             char *string, ch;
;             {
;             while (*string && *string != ch)
;                     string++;
;             if (*string == ch)
;                     return(string);
;             return((char *)0);
;             }
;
;Entry:
;       char *string - string to search in
;       char c - character to search for
;
;Exit:
;       returns pointer to the first occurence of c in string
;       returns NULL if c does not occur in string
;
;Uses:
;
;Exceptions:
;
;*******************************************************************************

        CODESEG

        public  strchr
strchr  proc

        .FPO    ( 0, 2, 0, 0, 0, 0 )

string  equ     [esp + 4]
chr     equ     [esp + 8]

        mov     eax,string
        mov     cl,chr

nextchar:
        mov     dl,[eax]
        cmp     cl,dl
        je      gotmatch
        or      dl,dl
        jz      endofstr
        inc     eax
        jmp     short nextchar

endofstr:
        xor     eax,eax

gotmatch:
        ret

strchr  endp
        end
