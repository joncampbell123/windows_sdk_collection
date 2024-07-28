; WEP.ASM
; ========================================================================
; Copyright (C) 1989-1990 Microsoft Corporation.  All rights reserved.
; ========================================================================
; WEP()
;
; This function is called when the DLL is loaded and unloaded.
; The parameter determines whether the DLL is being loaded or unloaded:
;
; ========================================================================

include cmacros.inc

sBegin code

assumes cs, code

cProc	WEP, <FAR, PUBLIC>

;; wParam fExit

cBegin	nogen

	ret	2			; move SP past parameter

cEnd	nogen

sEnd code

end
