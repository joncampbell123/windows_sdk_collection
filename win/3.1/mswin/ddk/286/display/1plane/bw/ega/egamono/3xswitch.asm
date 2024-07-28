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
	include	egamem.inc
	.list

	??_out	3xswitch

	public		dev_to_foreground
	public		dev_to_background
	public		dev_to_save_regs
	public		dev_to_res_regs

	externA		ScreenSelector


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
;	Any bits saved by SaveScreenBitmap in the upper page of EGA
;	memory have been invalidated by changing flags in
;	shadow_mem_status.
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

 
;----------------------------------------------------------------------------;
; procedure to restore th device registers.				     ;
;----------------------------------------------------------------------------;


dev_to_res_regs	proc	near

	ret

dev_to_res_regs	endp

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

;----------------------------------------------------------------------------;
; procedure called when device is asked to restore its registers             ;
;----------------------------------------------------------------------------;

dev_to_save_regs	proc	near

	ret

dev_to_save_regs	endp

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
;	Wed 30-Sep-1987 19:20:00 -by-  Bob Grudem [bobgru]
;	Converted for use with screen switching code.
;
;	Tue 18-Aug-1987 18:09:00 -by-  Walt Moore [waltm]
;	Added enabled_flag
;
;	Thu 26-Feb-1987 13:45:58 -by-  Walt Moore [waltm]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,Data
assumes es,nothing

screen_switch_help	proc near


;	Monochrome graphics mode was set up.  Now set up the EGA's
;	registers for the special mode used by this driver.  If
;	only 64K of memory is available, then the plane enable
;	registers will have to be set to allow access to both
;	planes C0 and C1 since they will be chained together.
;	If more than 64K is available, then planes C0 and C1
;	will not be chained so only C0 will be used.


	mov	ah,12h			;Inquire special EGA info
	mov	bl,10h
	int	10h

	mov	dx,cs			;--> 64K parameter table
	mov	ds,dx
	assumes ds,Code

	mov	si,CodeOFFSET ssh_ega64k
	mov	cx,SSH_COUNT_EGA_64K	;Set number of data bytes to output
	cld
	and	bl,00000011b		;See how may planes there are
	jz	screen_switch_help_10	;Only 64K of memory available
	mov	si,CodeOFFSET ssh_ega128k;--> >64K parameter table
	mov	cx,SSH_COUNT_EGA_128K 	;Set number of data bytes to output

screen_switch_help_10:
	lodsw				;Get port
	mov	dx,ax
	lodsb				;Get data
	out	dx,al
	loop	screen_switch_help_10	;Until all special stuff set

	ret

screen_switch_help	endp


;	EGA64K - EGA 64K Memory Parameters
;
;	The following parameters are output to the EGA to configure
;	it for 64K single plane operation.


ssh_ega64k	label	byte

	dw	3C4h			;Sequencer Address
	db	02h			;Sequencer Map Mask
	dw	3C5h			;Sequencer Data
	db	00000011b		;Access planes C0 and C1

SSH_COUNT_EGA_64K	= ($-ssh_ega64k)/3



;	EGA128K - EGA 128K Memory Parameters
;
;	The following parameters are output to the EGA to configure
;	it for 128K single plane operation.


ssh_ega128k label	byte

	dw	3C4h			;Sequencer Address
	db	02h			;Sequencer Map Mask
	dw	3C5h			;Sequencer Data
	db	00000001b		;Access plane C0


SSH_COUNT_EGA_128K	= ($-ssh_ega128k)/3


sEnd	Code
	end
