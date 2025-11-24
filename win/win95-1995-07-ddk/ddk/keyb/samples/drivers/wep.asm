;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

; WEP.ASM
;
; ========================================================================
;
; WEP()
;
; This function is called when the DLL is loaded and unloaded.
; The parameter determines whether the DLL is being loaded or unloaded:
;
; ========================================================================

include cmacros.inc

sBegin code

assumes cs, code

cProc	WEP, <FAR, PUBLIC, LOADDS, PASCAL>

;; wParam fExit

cBegin	nogen

	ret	2			; move SP past parameter

cEnd	nogen

sEnd code

end
