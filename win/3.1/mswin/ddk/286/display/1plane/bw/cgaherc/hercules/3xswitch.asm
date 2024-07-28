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


sBegin	Data
	externB		enabled_flag
sEnd	Data

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
;	The mouse cursor drawing code has been disabled by way of the
;	screen_busy semaphore.
; Returns:
;	The mouse cursor drawing code has been restored to its former
;	state by way of the screen_busy semaphore.
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

	call	screen_switch_help
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
;	The mouse cursor drawing code has been disabled by way of the
;	screen_busy semaphore.
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


dev_to_save_regs	proc	near

	ret

dev_to_save_regs		endp

dev_to_res_regs		proc	near

	ret

dev_to_res_regs		endp



;---------------------------Public-Routine------------------------------;
; screen_switch_help
;
; Entry:
;	None
; Returns:
;	None
; Registers Preserved:
;	BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,ES,DS,FLAGS
; Calls:
;	INT 10h
; History:
;	Thu 01-Oct-1987 13:45:58 -by-  Bob Grudem [bobgru]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,Data
assumes es,nothing

screen_switch_help	proc near

	push	ds
	mov	ax,cs
	mov	ds,ax
	assumes ds,Code

	mov	dx,3BFh			;enable graphics and allow
	mov	al,00000011b		;  page two
	out	dx,al

	mov	al,82h			;select graphics, page two
	mov	si,CodeOFFSET herc_graph_table_2
	call	reset_mode		;set graphics mode

	pop	ds
	assumes	ds,Data

	mov	ax,1
	mov	enabled_flag,al 	;Show enabled

	ret

screen_switch_help	endp


;---------------------------Private-Routine-----------------------------;
; reset_mode
;
; Set Hercules graphics card mode
;
; The Hercules graphics card CRTC parameters will be set for
; graphics.
;
;
; Entry:
;	SI --> parameter table
;	AL  =  mode byte
;	DS  =  CS
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	AX,BX,CX,DS
; Registers Destroyed:
;	DX,SI,DI,ES,FLAGS
; Calls:
;	INT 10h
; History:
;	Thu 01-Oct-1987 13:45:58 -by-  Bob Grudem [bobgru]
;	Converted from physical_enable support routine for use here.
;	Thu 26-Feb-1987 13:45:58 -by-  Walt Moore [waltm]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	public	reset_mode		;Public for debugging

assumes ds,Code
assumes es,nothing

reset_mode	proc	near

	push	ax
	mov	dx,3B8h
	out	dx,al			;change mode with screen off
	mov	dx,3B4h
	mov	cx,12			;12 parameters to be output
	xor	ah,ah
reset_mode_parms:
	mov	al,ah
	out	dx,al
	inc	dx
	lods	herc_graph_table_2
	out	dx,al
	inc	ah
	dec	dx
	loop	reset_mode_parms

	mov	dx,3B8h			;control register
	pop	ax
	or	al,8
	out	dx,al			;turn screen on

	ret

reset_mode	endp


;	Hercules 6845 CRTC parameters for high-res graphics

herc_graph_table_2	label	byte

	db	35h
	db	2Dh
	db	2Eh
	db	07h
	db	5Bh
	db	02h
	db	57h
	db	57h
	db	02h
	db	03h
	db	00h
	db	00h

sEnd	Code
	end
