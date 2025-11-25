        page    ,132
        title   strdup - duplicate string in malloc'd memory
;***
;strdup.asm - duplicate a string in malloc'd memory
;
;       Copyright (c) 1985-1991, Microsoft Corporation. All rights reserved.
;
;Purpose:
;       defines _strdup() - grab new memory, and duplicate the string into it.
;
;*******************************************************************************

        .xlist
        include cruntime.inc
        .list

        extrn   strlen:proc
        extrn   strcpy:proc
        extrn   malloc:proc


page
;***
;char *_strdup(string) - duplicate string into malloc'd memory
;
;Purpose:
;       Allocates enough storage via malloc() for a copy of the
;       string, copies the string into the new memory, and returns
;       a pointer to it.
;
;       Algorithm:
;       char *
;       _strdup (string)
;             char *string;
;             {
;             char *memory;
;
;             if (!string)
;                     return(NULL);
;             if (memory = malloc(strlen(string) + 1))
;                     return(strcpy(memory,string));
;             return(NULL);
;             }
;
;Entry:
;       char *string - string to copy into new memory
;
;Exit:
;       returns a pointer to the newly allocated storage with the
;       string in it.
;       returns NULL if enough memory could not be allocated, or
;       string was NULL.
;
;Uses:
;       eax, edx
;
;Exceptions:
;
;*******************************************************************************

        CODESEG

%       public  _strdup
_strdup proc \
        uses edi, \
        string:ptr byte


        mov     edi,[string]    ; edi=pointer to string
        push    edi             ; stack parameter: string pointer
        call    strlen          ; eax = string length

        pop     edx             ; caller cleans stack (_cdecl)
;else
                                ; callee cleaned stack (_stdcall)

        inc     eax             ; need space for null byte too
        push    eax             ; stack parameter: string length (with null)
        call    malloc          ; eax = pointer to space

        pop     edx             ; caller cleans stack (_cdecl)
;else
                                ; callee cleaned stack (_stdcall)

        or      eax,eax         ; offset == NULL ??
        jz      short toend     ; error -- couldn't malloc space

okay:
        push    edi             ; push address of original string
        push    eax             ; push address of dest string
                                ; source string addr is still on stack
        call    strcpy          ; duplicate the string

        pop     edx             ; caller cleans stack (_cdecl)
        pop     edx
;else
                                ; callee cleaned stack (_stdcall)

                                ; pointer to duplicate is in eax

toend:                          ; eax = return value

        ret                     ; _cdecl return

_strdup endp
        end
