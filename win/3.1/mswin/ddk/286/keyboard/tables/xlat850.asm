; ****** XLAT850.ASM ************************************************
;	Contains XLAT850.INC
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

	include xlat850.inc

    xlatEnd label byte

include date.inc

sEnd CODE

end
