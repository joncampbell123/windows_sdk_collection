        TITLE   SPOOLAUX - assembler support routines for SPOOL

.xlist
include cmacros.inc
.list

externFP    <GlobalLock,GlobalUnlock>

sBegin  CODE

assumes CS,CODE

cProc   SetPortRaw, <PUBLIC, NEAR>
        parmW   handle
cBegin
        mov     ax,4400h                ;get device info
        mov     bx,handle
        int     21h                     ;get device info back in dx
        test    dx,80h                  ;a device?
        jz      exit
        xor     dh,dh
        or      dl,20h                  ;set raw mode bit
        mov     ax,4401h                ;
        int     21h                     ;set device info
exit:
cEnd

cProc   Copy, <NEAR, PUBLIC>, <si,di>
        parmd   dst             ;long destination pointer
        parmd   src             ;long source pointer
        parmw   cnt             ;cnt of bytes

cBegin  Copy
        mov     cx,cnt
        les     di,dst
        lds     si,src
        rep     movsb
cEnd    Copy


sEnd

end
