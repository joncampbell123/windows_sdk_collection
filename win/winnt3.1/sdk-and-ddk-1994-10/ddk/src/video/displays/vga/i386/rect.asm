;---------------------------Module-Header------------------------------;
; Module Name: rect.asm
;
; Rectangle Utilities
;
; Copyright (c) 1992 Microsoft Corporation
;-----------------------------------------------------------------------;
        .386

ifndef  DOS_PLATFORM
        .model  small,c
else
ifdef   STD_CALL
        .model  small,c
else
        .model  small,pascal
endif;  STD_CALL
endif;  DOS_PLATFORM

        assume cs:FLAT,ds:FLAT,es:FLAT,ss:FLAT
        assume fs:nothing,gs:nothing

        .xlist
        include stdcall.inc             ;calling convention cmacros
        include i386\strucs.inc
        .list

        .code

_TEXT$01   SEGMENT DWORD USE32 PUBLIC 'CODE'
           ASSUME  CS:FLAT, DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;---------------------------Public-Routine------------------------------;
; bIdenticalRect ( pRect1, pRect2 )
;
;   Compare two rectangles whether they are identical
;
;-----------------------------------------------------------------------;

cProc   bIdenticalRect,8,<    \
        uses      esi edi,    \
        pRect1:   ptr RECTL,  \
        pRect2:   ptr RECTL   >

        cld
        xor     eax,eax
        mov     ecx,(size RECTL/4)
        mov     esi,pRect1
        mov     edi,pRect2
        repe    cmpsd
        setz    al

bir_exit:
        cRet    bIdenticalRect

endProc bIdenticalRect

;---------------------------Public-Routine------------------------------;
; flClipRect ( pDst, pRect1, pRect2 )
;
;   Clip the first rectangle with the second one, and put the result
;   rectangle in the destination one.
;
; Return:
;       0 if Rect1 is not clipped by Rect2
;       1 if Rect1 is horizontally clipped only
;       2 if Rect1 is vertically clipped only
;       3 if Rect1 is both horizontally and vertically clipped.
;
;-----------------------------------------------------------------------;
 
cProc   flClipRect,12,<            \
        uses      ebx esi edi,     \
        pDstRect: ptr RECTL,       \
        pRect1:   ptr RECTL,       \
        pRect2:   ptr RECTL        >


        cld
        mov     edi,pDstRect
        mov     esi,pRect1
        mov     ebx,pRect2
        push    ebp
        xor     ebp,ebp             ; assume no clipping required

bisr_left:
        lodsd                       ; left of rect1
        mov     edx,[ebx].xLeft     ; left of rect2
        cmp     eax,edx             ; clipped if left of rect1 < left of rect2
        jge     short @F
        mov     eax,edx             ; clipped left
        or      ebp,1               ; indicate horizontally clipped
@@:
        stosd

bisr_Top:
        lodsd                       ; top of rect1
        mov     edx,[ebx].yTop      ; top of rect2
        cmp     eax,edx             ; clipped if top of rect1 < top of rect2
        jge     short @F
        mov     eax,edx             ; clipped top
        or      ebp,2               ; indicate vertically clipped
@@:
        stosd

bisr_Right:
        lodsd                       ; right of rect1
        mov     edx,[ebx].xRight    ; right of rect2
        cmp     edx,eax             ; clipped if right of rect1 > right of rect2
        jge     short @F
        mov     eax,edx             ; clipped top
        or      ebp,1               ; indicate horizontally clipped
@@:
        stosd

bisr_Bottom:
        lodsd                       ; bottom of rect1
        mov     edx,[ebx].yBottom   ; bottom of rect2
        cmp     edx,eax             ; clipped if bottom of rect1 > bottom of rect2
        jge     short @F
        mov     eax,edx             ; clipped bottom
        or      ebp,2               ; indicate vertically clipped
@@:
        stosd

bisr_exit:
        mov     eax,ebp             ; return value
        pop     ebp
        cRet    flClipRect

endProc flClipRect

_TEXT$01   ends

        end

