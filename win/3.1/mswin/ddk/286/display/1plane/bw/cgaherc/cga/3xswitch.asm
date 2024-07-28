	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	3XSWITCH.ASM
;
;   This module contains the functions:
;
;	dev_to_background
;	dev_to_foreground
;
;   These functions support screen switching in and out of the OS/2
;   compatibility box.
;
; Created: 16-Sep-1987
; Author:  Bob Grudem [bobgru]
;
; Copyright (c) 1984-1987 Microsoft Corporation
;
; Exported Functions:	none
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
; Restrictions:
;
;-----------------------------------------------------------------------;


	.xlist
	include cmacros.inc
	include macros.mac
	.list

	??_out	3xswitch

	public		dev_to_foreground
	public		dev_to_background
	public		dev_to_save_regs
	public		dev_to_res_regs

sBegin	Code
assumes cs,Code
	page


;---------------------------Public-Routine-----------------------------;
; dev_to_foreground
;
; Performs any action necessary to restore the state of the display
; hardware prior to becoming the active context.
;
; Entry:
;	nothing
; Returns:
;	nothing
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	none
; History:
;	Wed 16-Sep-1987 20:17:08 -by-  Bob Grudem [bobgru]
;	Wrote it.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


dev_to_foreground	proc	near

	ret

dev_to_foreground	endp
	page

;---------------------------Public-Routine-----------------------------;
; dev_to_background
;
; Performs any action necessary to save the state of the display
; hardware prior to becoming an inactive context.
;
; Entry:
;	nothing
; Returns:
;	nothing
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	none
; History:
;	Wed 16-Sep-1987 20:17:08 -by-  Bob Grudem [bobgru]
;	Wrote it.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


dev_to_background	proc	near

	ret

dev_to_background	endp

dev_to_save_regs proc near

	ret

dev_to_save_regs endp

dev_to_res_regs	proc near

	ret

dev_to_res_regs	  endp


	

sEnd	Code
	end
