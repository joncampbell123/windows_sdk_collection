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

	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	BLKWHITE.ASM
;
; This file contains the definitions of the default colors to be used
; for all black and white display drivers.
;
; Exported Functions:	none
;
; Public Functions:	none
;
; Public Data:
;	      R_SCROLLBARBODY	    G_SCROLLBARBODY	 B_SCROLLBARBODY
;	      R_DESKTOP 	    G_DESKTOP		 B_DESKTOP
;	      R_ACTIVECAPTION	    G_ACTIVECAPTION	 B_ACTIVECAPTION
;	      R_INACTIVECAPTION     G_INACTIVECAPTION	 B_INACTIVECAPTION
;	      R_MENUBACKGROUND	    G_MENUBACKGROUND	 B_MENUBACKGROUND
;	      R_WINDOWBACKGROUNDD   G_WINDOWBACKGROUND	 B_WINDOWBACKGROUND
;	      R_CAPTION 	    G_CAPTION		 B_CAPTION
;	      R_MENUTEXT	    G_MENUTEXT		 B_MENUTEXT
;	      R_WINDOWTEXT	    G_WINDOWTEXT	 B_WINDOWTEXT
;	      R_CAPTIONTEXT	    G_CAPTIONTEXT	 B_CAPTIONTEXT
;
; General Description:
;
;	The default colors are defined as absolute values to be linked
;	into black/white only drivers.
;
; Restrictions:
;
;-----------------------------------------------------------------------;




;	def_RGB is a macro which will define the appropriate absolute
;	values to the linker.


def_RGB macro	Red,Green,Blue,name
	public	R_&name
	public	G_&name
	public	B_&name
R_&name equ	Red
G_&name equ	Green
B_&name equ	Blue
	endm



;	Define that actual values

;		 R    G    B

if 0
;	alpha/beta 1 colors
	def_RGB 0C0h,0C0h,0C0h,SCROLLBARBODY
	def_RGB 080h,080h,080h,DESKTOP
	def_RGB 000h,000h,000h,ACTIVECAPTION
	def_RGB 080h,080h,080h,INACTIVECAPTION
	def_RGB 0FFh,0FFh,0FFh,MENUBACKGROUND
	def_RGB 0FFh,0FFh,0FFh,WINDOWBACKGROUND
	def_RGB 000h,000h,000h,CAPTION
	def_RGB 000h,000h,000h,MENUTEXT
	def_RGB 000h,000h,000h,WINDOWTEXT
	def_RGB 0FFh,0FFh,0FFh,CAPTIONTEXT
endif

if 0
;	beta 2 colors
	def_RGB 0BFh,0BFh,0BFh,SCROLLBARBODY
	def_RGB 080h,080h,080h,DESKTOP
	def_RGB 000h,000h,000h,ACTIVECAPTION
	def_RGB 080h,080h,080h,INACTIVECAPTION
	def_RGB 0FFh,0FFh,0FFh,MENUBACKGROUND
	def_RGB 0FFh,0FFh,0FFh,WINDOWBACKGROUND
	def_RGB 000h,000h,000h,CAPTION
	def_RGB 000h,000h,000h,MENUTEXT
	def_RGB 000h,000h,000h,WINDOWTEXT
	def_RGB 0FFh,0FFh,0FFh,CAPTIONTEXT
endif

	def_RGB 03Fh,03Fh,03Fh,SCROLLBAR
	def_RGB 07Fh,07Fh,07Fh,BACKGROUND
	def_RGB 000h,000h,000h,ACTIVETITLE
	def_RGB 0FFh,0FFh,0FFh,INACTIVETITLE
	def_RGB 0FFh,0FFh,0FFh,MENU
	def_RGB 0FFh,0FFh,0FFh,WINDOW
	def_RGB 000h,000h,000h,WINDOWFRAME
	def_RGB 000h,000h,000h,MENUTEXT
	def_RGB 000h,000h,000h,WINDOWTEXT
	def_RGB 0FFh,0FFh,0FFh,TITLETEXT
	def_RGB 07Fh,07Fh,07Fh,ACTIVEBORDER
	def_RGB 07Fh,07Fh,07Fh,INACTIVEBORDER
	def_RGB 0BFh,0BFh,0BFh,APPWORKSPACE
end

