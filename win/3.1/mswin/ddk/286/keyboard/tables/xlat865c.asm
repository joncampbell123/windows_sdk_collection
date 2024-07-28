; ****** XLATNO.ASM ************************************************
;	Contains XLATNO.INC
;
;	Copyright 1989-1990 Microsoft Corp.
; ******************************************************************

include cmacros.inc

sBegin CODE

assumes CS,CODE
; assumes DS,DATA

	public	xlatBeg, xlatSize

xlatSize	dw	CODEoffset xlatEnd - CODEoffset xlatBeg

    xlatBeg label byte

	include xlat865c.inc

    xlatEnd label byte

include date.inc

sEnd CODE

end
