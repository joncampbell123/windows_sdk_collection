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
; Module Name:	SSWITCH.ASM
;
;   This module contains the functions:
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


	.286


	.xlist
	include cmacros.inc
	include macros.inc
	include	mflags.inc
	include debug.inc
	.list

	??_out	sswitch


	externFP ScreenSwitchEnable	;Imported from KEYBOARD.DRV
	externFP GetModuleHandle
	externFP GetProcAddress
	externNP dev_to_background	;Switch to background code
	externNP dev_to_foreground	;Switch to foreground code
	externNP dev_to_save_regs	;save device registers
	externNP dev_to_res_regs	;restore device registers
	externNP dev_initialization	;Other boot-time initialization
	externFP AllocCSToDSAlias	;get a DS alias for CS
	externFP FreeSelector		;frees the selector
	externFP GetProfileInt		;in KERNEL
	externFP GlobalSmartPageLock	;in KERNEL

SCREEN_SWITCH_OUT equ	4001h		;Moving 3xBox to background
SCREEN_SWITCH_IN  equ	4002h		;Moving 3xBox to foreground
;DOS_VERSION	  equ	1000h		;Earliest DOS we must support (10.00)
DOS_VERSION	  equ	0310h		;Earliest DOS we must support (03.10)
HOT_KEY_VERSION   equ	1000h		;Version with hot key support (10.00)


INT_MULT	equ	2Fh		;Multiplexed interrupt number
ENTER_CRIT_REG	equ	4003h		;int 2f code for enter critical reg
EXIT_CRIT_REG	equ	4004h		;int 2f code for leaving crit reg
SAVE_DEV_REGS   equ	4005h		;int 2f code to save registers
RES_DEV_REGS    equ     4006h		;int 2f code to restore registers

SCREEN_IN_FGND equ	0		;screen is in forground
SCREEN_IN_BGND equ	1		;screen is in background

;----------------------------------------------------------------------------;
; define the flag bit in __WinFlags					     ;
;----------------------------------------------------------------------------;
WINR	equ	00000000b		;real mode, 8086
WINP	equ	00000001b		;protected mode
WIN286	equ	00000010b		;running on a 286 machine
WIN386	equ	00000100b		;running on a 386 machine
WIN486	equ	00001000b		;running on a 486 machine

;----------------------------------------------------------------------------;
createSeg _PROTECT,pCode,word,public,CODE
sBegin pCode
sEnd pCode
createSeg _LINES,LineSeg,word,public,CODE
sBegin	LineSeg
sEnd LineSeg
createSeg _PIXEL,PixelSeg,word,public,CODE
sBegin	PixelSeg
sEnd	PixelSeg
createSeg _MODE,ModeSeg,word,public,CODE
sBegin	ModeSeg
sEnd	ModeSeg
createSeg _COMPOSE_AREA,Compose,word,public,DATA
sBegin	Compose
sEnd	Compose


sBegin	Data

	public		in_background

	externB		screen_busy	;screen semaphore
	externB		enabled_flag	;set 0 to disable all functions
	externW		is_protected	;LSB set to 1 in protected mode
	externW		Qsize
	externW		wMouseTrails	;cached win.ini setting.

in_background	db	0		;flag to say we are in background
old_screen_busy	db	?		;saved value of screen semaphore
IS_BUSY	equ	0			;should be public in cursors.asm!


pre_switch	label	word
		dw	pre_switch_to_background
		dw	pre_switch_to_foreground
		dw	0
		dw	0
		dw	pre_save_regs
		dw	pre_res_regs

post_switch	label	word
		dw	post_switch_to_background
		dw	post_switch_to_foreground
		dw	0
		dw	0
		dw	post_save_regs
		dw	post_res_regs

switch_table	label	word		;Screen switch dispatch table
		dw	dev_to_background
		dw	dev_to_foreground
		dw	0
		dw	0
		dw	dev_to_save_regs
		dw	dev_to_res_regs


switch_control	db	0		;Switch control flags
PREVENT_SWITCH	equ	10000000b	;Don't allow switch (DOS 3.x, 4.x)
DO_SWITCHING	equ	01000000b	;Have to do switching
INT_2F_HOOKED	equ	00000001b	;Have hooked int 2Fh
DISABLE_HOT_KEY equ	00000010b	;Set if keyboard disabling required

FLAGS_ON_STACK	equ	4		;iret --> offset(0), seg(2), flags(4)
CARRY_FLAG	equ	00000001b


REPAINT_EXPORT_INDEX	equ	275
repaint_disable	db	0h		;ok to call user to repaint
repaint_pending	db	0		;repaint is pending.
repaint_addr	dd	0
user_string	db	'USER',0

prev_int_2Fh	dd	0

sEnd	Data

sBegin	Code
assumes cs,Code

	externW		_cstods
page

;----------------------------------------------------------------------------;
; UserRepaintDisable:							     ;
;								             ;
; USER calls this function to tell the display driver whether it expects     ;
; the repaint call to be postponed or not.			             ;
;								             ;
; If RepaintDisable is TRUE then the display driver should not send          ;
; repaint requests to USER immediately but wait till it is enabled again.    ;
; USER will enable repaint by seting RepaintDisable to FALSE.		     ;
;----------------------------------------------------------------------------;

cProc	UserRepaintDisable,<FAR,PASCAL,PUBLIC>

	parmB	RepaintDisable		;TRUE to Disable/FALSE to enable

cBegin

	assumes	ds,Data

	mov	al,RepaintDisable	;get the value
	mov	repaint_disable,al	;save it
	or	al,al			;being enabled again
	jnz	@f			;no.
	cmp	repaint_pending,0ffh	;pending repaint ?
	jnz	@f			;no.
	call	pre_switch_to_foreground;call repaint
	mov	repaint_pending,0	;done with pending call
@@:

cEnd
;-------------------------Interrupt-Handler-----------------------------;
; screen_switch_hook
;
; Watches calls to the OS/2 multiplex interrupt chain, and traps calls
; to the driver to save or restore the state of the display hardware
; before a context switch.
;
; If a save/restore call is recognized, then it will be dispatched
; to the device dependent handler unless PREVENT_SWITCH is
; set in switch_control.
;
;   Currently under OS/2, we inform the keyboard driver that the
;   hot key is not to be passed along to DOS whenever the display
;   driver cannot save it's state (this currently only occurs when
;   we've already saved the EGA state).  Since OS/2 will only take
;   the screen away from us when the hot key is pressed, we should
;   never see the PREVENT_SWITCH bit set).
;
;   If we're not running under OS/2, it is possible a pop-up not
;   caused by a hot key could try and grab the screen away from us .
;   In this case, we'll return with 'C' set to show that it's a real
;   bad time and try again later (they may still grab it, though).
;   In this case, PREVENT_SWITCH could be set.
;
; Entry:
;	AH = multiplex number
;	AL = function code
; Returns:
;	'C' set if screen switch cannot occur (DOS's < 10.0)
;	'C' clear otherwise
; Registers Preserved:
;	AL,BX,CX,DX,SI,DI,BP,DS,ES,SS
; Registers Destroyed:
;	AH,FLAGS
; Calls:
;	dev_to_background
;	dev_to_foreground
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

screen_switch_hook proc far

	cmp	ax,SCREEN_SWITCH_IN
	je	screen_switch_occurring
	cmp	ax,SCREEN_SWITCH_OUT
	je	screen_switch_occurring
	cmp	ax,SAVE_DEV_REGS
	je	screen_switch_occurring
	cmp	ax,RES_DEV_REGS
	je	screen_switch_occurring

	push	ds
	mov	ds, [_cstods]			; set up ds for data
	assumes ds,Data
	push	word ptr [prev_int_2Fh][2]	; push address to jump to
	push	word ptr [prev_int_2Fh][0]
	push	bp
	mov	bp, sp
	mov	ds, [bp+6]			; restore ds
	pop	bp
	retf	2				; pass it along popping ds

screen_switch_occurring:
	push	bp			;set up frame for altering flags
	mov	bp,sp

	push	ds
	mov	ds,[_cstods]
	assumes	ds,Data

	mov	ah,switch_control
	add	ah,ah
	jc	exit_screen_switch_error
	errnz	PREVENT_SWITCH-10000000b



	pusha

	push	es
	and	ax,000fh		;just retain low nibble
	dec	ax			;make the nos zero based
	shl	ax,1			;will index into a word table
;	and	ax,00000010b		;Use D1 of function to index into
	xchg	ax,bx			;  the two word dispatch table
	call	pre_switch[bx]
	push	bx
	push	ds
	call	switch_table[bx]	;this guy only saved BP
	pop	ds
	assumes	ds,Data
	pop	bx
	call	post_switch[bx]
	pop	es
	assumes es,nothing

	popa

	pop	ds
	assumes ds,nothing


;	Note that BP is still on the stack (hence FLAGS_ON_STACK+2).

	and	wptr [bp][FLAGS_ON_STACK][2],not CARRY_FLAG
	pop	bp
	iret

exit_screen_switch_error:
	pop	ds
	assumes ds,nothing

	or	wptr [bp][FLAGS_ON_STACK][2],CARRY_FLAG
	pop	bp
	iret

screen_switch_hook	endp
page



;---------------------------Public-Routine-----------------------------;
; pre_switch_to_foreground
;
; This function is called when switching to the foreground, before
; any device-specific code has executed.
;
; Entry:
;	DS = Data
; Returns:
;	DS = Data
; Registers Preserved:
;	BX,SI,DI,BP,DS
; Registers Destroyed:
;	AX,CX,DX,ES,FLAGS
; Calls:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

pre_switch_to_foreground	proc	near

	push	bx			;Save dispatch table index
	mov	ax,wptr repaint_addr[0] ;We expect to always have the
	or	ax,wptr repaint_addr[2] ;  address, but let's be sure
	jz	pre_switch_to_fore_exit
	cmp	repaint_disable,0	;is repaint enabled ?
	jz	@f			;yes.
	mov	repaint_pending,0ffh	;repaint pending
	jmp	short pre_switch_to_fore_exit
@@:
	call	repaint_addr		;Force repaint of all windows

pre_switch_to_fore_exit:
	pop	bx

	ret

pre_switch_to_foreground	endp


;---------------------------Public-Routine-----------------------------;
; pre_switch_to_background
;
; This function is called when switching to the background, before
; any device-specific code has executed.
;
; Entry:
;	DS = Data
; Returns:
;	DS = Data
; Registers Preserved:
;	AH,BX,CX,DX,SI,DI,BP,DS,ES,FLAGS
; Registers Destroyed:
;	AL
; Calls:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

public pre_switch_to_background
pre_switch_to_background	proc	near

; set a flag to indicate we are in background

	mov	in_background,SCREEN_IN_BGND
	mov	al,IS_BUSY		;disable mouse cursor drawing code
	cli
	xchg	al,screen_busy		;say it's busy
	xchg	al,old_screen_busy	;store old flag semaphore value
	sti

	ret

pre_switch_to_background	endp


;---------------------------Public-Routine-----------------------------;
; post_switch_to_foreground
;
; This function is called when switching to the foreground, after
; any device-specific code has executed.
;
; Entry:
;	DS = Data
; Returns:
;	DS = Data
; Registers Preserved:
;	AH,BX,CX,DX,SI,DI,BP,DS,ES,FLAGS
; Registers Destroyed:
;	AL
; Calls:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

post_switch_to_foreground	proc	near

	cli
;----------------------------------------------------------------------------;
; at this point we will enable the enabled_flag to allow all functions to    ;
; access the display hardware. Also reenable the mouse cursor drawing code   ;
;----------------------------------------------------------------------------;

; reset the 'in_background' flag

	mov	in_background,SCREEN_IN_FGND
	mov 	enabled_flag,0ffh	

	mov	al,old_screen_busy	;reenable mouse cursor drawing code
	xchg	al,screen_busy		;set semaphore to old value
	sti

	ret

post_switch_to_foreground	endp


;---------------------------Public-Routine-----------------------------;
; post_switch_to_background
;
; This function is called when switching to the background, after
; any device-specific code has executed.
;
; Entry:
;	DS = Data
; Returns:
;	DS = Data
; Registers Preserved:
;	AX,BX,CX,DX,SI,DI,BP,DS,ES,FLAGS
; Registers Destroyed:
;	None
; Calls:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

post_switch_to_background	proc	near

;----------------------------------------------------------------------------;
; we simply set the enabled_flag to 0 so that none of the display functions  ;
; will access the display hardware.					     ;
;----------------------------------------------------------------------------;
	
	mov	enabled_flag,0		;disable
	ret

post_switch_to_background	endp

;----------------------------------------------------------------------------;
; we can expand yjese prelude and postelude functions if we need them at a   ;
; later date else not.						             ;
;----------------------------------------------------------------------------;

pre_save_regs:
pre_res_regs:
post_save_regs:
post_res_regs:
	ret



;---------------------------Public-Routine-----------------------------;
; disable_switching
;
; This function is called whenever we need to prevent a screen switch
; from occuring.
;
; Entry:
;	DS = Data
; Returns:
;	DS = Data
; Registers Preserved:
;	SI,DI,BP,DS
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	ScreenSwitchEnable in keybaord.drv
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

	public	disable_switching

disable_switching proc	near

;----------------------------------------------------------------------------;
; if we are in protected mode simply notifying the kernel is enough, else in ;
; real mode we lock out the keyboard hot key.			  	     ;
;----------------------------------------------------------------------------;

	test	is_protected,1		;are we in protected mode ?
	jz	enter_critical_in_real_mode;no!
	mov	ax,ENTER_CRIT_REG	;code for enter critical region
	int	2fh			;this does are work
	jmp	short show_switch_disabled
enter_critical_in_real_mode:
	mov	al,switch_control	;Must have correct DOS version
	test	al,DO_SWITCHING
	jz	disable_switching_exit
	test	al,DISABLE_HOT_KEY	;Only call keyboard driver if
	jz	show_switch_disabled	;  we need to
	xor	ax,ax
	cCall	ScreenSwitchEnable,<ax>

show_switch_disabled:
	or	switch_control,PREVENT_SWITCH

disable_switching_exit:
	ret

disable_switching endp
page

;---------------------------Public-Routine-----------------------------;
; enable_switching
;
; This function is called whenever we can allow a screen group switch.
;
; Entry:
;	DS = Data
; Returns:
;	DS = Data
; Registers Preserved:
;	SI,DI,BP,DS
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	ScreenSwitchEnable in keybaord.drv
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

	public	enable_switching

enable_switching proc	near

	mov	al,switch_control
	and	al,not PREVENT_SWITCH
	mov	switch_control,al

;----------------------------------------------------------------------------;
; in protected mode we can notify the kernel that we are leaving the critical;
; region.								     ;
;----------------------------------------------------------------------------;
	test	is_protected,1		;will be set in protected mode
	jz	leave_critical_in_real_mode ; no
	mov	ax,EXIT_CRIT_REG	;'end of critical region' code
	int	2fh
	jmp	short enable_switching_exit
leave_critical_in_real_mode:
	test	al,DISABLE_HOT_KEY
	jz	enable_switching_exit
	mov	ax,0FFFFh
	cCall	ScreenSwitchEnable,<ax>

enable_switching_exit:
	ret

enable_switching endp

sEnd	Code
page

createSeg _INIT,InitSeg,word,public,CODE
sBegin	InitSeg
assumes cs,InitSeg


;---------------------------Public-Routine-----------------------------;
; hook_int_2Fh
;
; Installs a link in the 2Fh multiplex interrupt chain to watch for
; calls to the driver to save or restore the state of the display
; hardware before a context switch.
;
; This function is called whenever the driver recieves an enable call.
;
; Entry:
;	DS = Data
; Returns:
;	DS = Data
; Registers Preserved:
;	BX,CX,DX,SI,DI,BP,DS,ES
; Registers Destroyed:
;	AX,flags
; Calls:
;	none
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

		public	hook_int_2Fh
hook_int_2Fh	proc	near

	push	bx
	push	cx
	push	dx
	push	ds
	push	es

;	See if we already have repaint_addr.
	mov	ax,wptr [repaint_addr][0]
	or	ax,wptr [repaint_addr][2]
	jnz	hook_int_2f_repaint_addr

;	Need module handle to pass to GetProcAddress.
	mov	ax,DataOFFSET user_string
	farPtr	module_name,ds,ax
	cCall	GetModuleHandle,<module_name>
	
;	Get the value of repaint_addr.
	xchg	ax,bx
	mov	ax,REPAINT_EXPORT_INDEX
	cwd
	farPtr	func_number,dx,ax
	cCall	GetProcAddress,<bx,func_number>
	mov	wptr [repaint_addr][0],ax
	mov	wptr [repaint_addr][2],dx

hook_int_2f_repaint_addr:

	mov	al,switch_control	;Only hook if we have the correct DOS
	xor	al,INT_2F_HOOKED
	test	al,DO_SWITCHING+INT_2F_HOOKED
	jz	hook_int_done		;Don't need to hook it


	cli	
	or	switch_control,INT_2F_HOOKED
	xor	ax,ax

	mov	ax,3500h+INT_MULT	; get the vector
	int	21h	
	
	mov	wptr prev_int_2Fh[0],bx
	mov	wptr prev_int_2Fh[2],es

	mov	dx,CodeOFFSET screen_switch_hook
	mov	ax,CodeBASE
	mov	ds,ax
	assumes ds,nothing

	mov	ax,2500h+INT_MULT	; set the vector
	int	21h
	assumes	ds,Data

	sti

hook_int_done:
	pop	es
	assumes es,nothing
	pop	ds
	assumes ds,nothing
	pop	dx
	pop	cx
	pop	bx
	ret

hook_int_2Fh	endp
page

;---------------------------Public-Routine-----------------------------;
; restore_int_2Fh
;
; If we installed ourselves into int 2Fh, we'll restore the previous
; vector.
;
; This function is called whenever the driver receives a disable call.
;
; Entry:
;	ES = Data
; Returns:
;	ES = Data
; Registers Preserved:
;	BX,CX,DX,SI,DI,BP,DS
; Registers Destroyed:
;	AX,ES,flags
; Calls:
;	none
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,Data

		public	restore_int_2Fh

restore_int_2Fh proc	near

	test	switch_control,INT_2F_HOOKED
	jz	restore_done
	cli	
	and	switch_control,not INT_2F_HOOKED

	push	es
	push	ds

	push	dx			; save
	lds	dx,prev_int_2Fh		; get the saved vector
	mov	ax,252fh		; set vector 2F
	int	21h
	pop	dx			; restore

	sti
	pop	ds
	assumes ds,nothing

	pop	es
	assumes	es,Data

restore_done:
	ret

restore_int_2Fh endp
page

;---------------------------Public-Routine-----------------------------;
; driver_initialization
;
; Windows display driver initialization.  All display drivers which
; require special support for screen group switching will have this
; as their load time entry point.  This function will perform its
; initialization, then call the device specific initialization code.
;
; The DOS version number is checked, and the internal flags for
; screen group switching are set.
;
; Entry:
;	CX = size of heap
;	DI = module handle
;	DS = automatic data segment
;	ES:SI = address of command line (not used)
; Returns:
;	DS = Data
; Registers Preserved:
;	SI,DI,BP,DS
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	none
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

szwindows	db	'windows',0
szMouseTrails	db	'MouseTrails',0

cProc	driver_initialization,<FAR,PUBLIC>,<si,di>
cBegin
	mov	ah,30h			;Check DOS version number
        int     21h
	xchg	al,ah			;Correct order for comparing
	xor	bl,bl			;Accumulate flags here
	cmp	ax,DOS_VERSION		;Earliest DOS we must support (10.0)			 ;10 or higher means OS/2
	jb	dont_support_switching
	or	bl,DO_SWITCHING 	;Have to handle screen group switches
	cmp	ax,HOT_KEY_VERSION
	jb	save_switch_control
	or	bl,DISABLE_HOT_KEY	;Can disable hot key

save_switch_control:
	mov	switch_control,bl

dont_support_switching:

        push    _INIT
        call    GlobalSmartPageLock
        push    _TEXT
        call    GlobalSmartPageLock
        push    _PROTECT
        call    GlobalSmartPageLock
        push    _LINES
        call    GlobalSmartPageLock
        push    _PIXEL
        call    GlobalSmartPageLock
        push    _DATA
        call    GlobalSmartPageLock
        push    _COMPOSE_AREA
        call    GlobalSmartPageLock
        push    _MODE
        call    GlobalSmartPageLock

	call	dev_initialization	;Device specific initialization

	mov	si,InitSegOFFSET szwindows
	mov	ax,InitSegOFFSET szMouseTrails
	cCall	GetProfileInt, <cs, si, cs, ax, -7> ;Default is zero.
	cmp	ax,7			; Bounds check. Can only be -7..-1,1..7
	jle	@f
	mov	ax,7
@@:	cmp	ax,-7
	jge	@f
	mov	ax,-7
@@:	mov	wMouseTrails,ax		;Set cache variable.
	or	ax,ax
	jg	short @f		;If negative (or zero), make Qsize = 1.
	mov	ax,1			;Otherwise, store directly into Qsize.
@@:	mov	Qsize,ax
	mov	ax,1			;initialization successful.
cEnd

sEnd	InitCode

end	driver_initialization
