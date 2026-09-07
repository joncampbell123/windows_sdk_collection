;***
;mbsdup.asm - duplicate a string in malloc'd memory
;
;       Copyright (c) 1985-1991, Microsoft Corporation. All rights reserved.
;
;Purpose:
;       defines _mbsdup() - grab new memory, and duplicate the string into it.
;
;*******************************************************************************

file    TEXTEQU <ntel\str>
path    TEXTEQU <i>

_strdup EQU <_mbsdup>

%        include &path&&file&dup.asm
        end
