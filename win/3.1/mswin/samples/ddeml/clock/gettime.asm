; get the time more efficiently than cmerge and DOSCALL() do

?PLM=0
include cmacros.inc

time    struc
        hour    dw  ?
        minute  dw  ?
        second  dw  ?
time    ends

assumes CS,CODE
assumes DS,DATA

sBegin   CODE

cProc   GetTime, <PUBLIC, NEAR>
        parmW   pTime               ; pointer to the structure to fill

cBegin
        mov     ax, 2c00h           ; get time
        int     21h
        mov     bx, pTime
        cmp     ch, 12                  ; if hour <12
        jl      lt12                    ; we're ok
        sub     ch,12                   ; else adjust it
lt12:
        xor     ax,ax
        mov     al,ch
        mov     [bx].hour, ax
        mov     al,cl
        mov     [bx].minute, ax
        mov     al,dh
        mov     [bx].second, ax
cEnd



cProc   SetTime, <PUBLIC, NEAR>
        parmW   pTime               ; pointer to the structure to use

cBegin
        mov     bx, pTime
        mov     ax, [bx].hour
        mov     ch, al
        mov     ax, [bx].minute
        mov     cl, al
        mov     ax, [bx].second
        mov     dh, al
        mov     ax, 2d00h           ; set time
        mov     dl, al
        int     21h
cEnd



sEnd    CODE
        END
