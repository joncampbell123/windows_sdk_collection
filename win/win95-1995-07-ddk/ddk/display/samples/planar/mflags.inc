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

;--------------------------------------------------
;MASMFLAGS definitions
;--------------------------------------------------
   EGA    =  0001h		;Build the EGA driver.
   VGA    =  0002h		;Build the VGA driver.
   SVGA   =  0004h		;Build the SuperVGA driver.
   PUBDEFS=  4000h		;Make symbols public.
   DEBUG  =  8000h		;Build a debug driver.
